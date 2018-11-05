#include <iostream>
#include <chrono>
#include <app.hpp>
#include <db.hpp>
#include <packet_handler.hpp>
#include <util/termcolor.hpp>
#include <TcpLayer.h>
#include <UdpLayer.h>
#include <IPv4Layer.h>
#include <IPv6Layer.h>
#include <PcapFileDevice.h>
#include <PcapLiveDeviceList.h>
#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/iostreams/stream.hpp>

nanocap::packet_handler::packet_handler(nanocap::app& app) : app (app)
{
}

nanocap::packet_handler::~packet_handler()
{
}

static void on_packet(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie)
{
	pcpp::Packet parsed_packet(packet);
	nanocap::packet_handler* handler = static_cast<nanocap::packet_handler*>(cookie);
	handler->consume(parsed_packet);
}

void nanocap::packet_handler::consume(pcpp::Packet& packet)
{
	if (packet.isPacketOfType(pcpp::Ethernet))
		ethPacketCount++;
	if (packet.isPacketOfType(pcpp::IPv4))
		ipv4PacketCount++;
	if (packet.isPacketOfType(pcpp::IPv6))
		ipv6PacketCount++;
	if (packet.isPacketOfType(pcpp::TCP))
		tcpPacketCount++;
	if (packet.isPacketOfType(pcpp::UDP))
	{
		handle_udp(packet);
		udpPacketCount++;
	}
	if (packet.isPacketOfType(pcpp::HTTP))
		httpPacketCount++;
	if (packet.isPacketOfType(pcpp::SSL))
		sslPacketCount++;
}

void nanocap::packet_handler::analyze_pcap_file(std::string path)
{
	importing = true;
	std::cout << "Analyzing " << path << "..." << std::endl;
	pcpp::IFileReaderDevice* reader = pcpp::IFileReaderDevice::getReader(path.c_str());
	if (!reader->open())
	{
		delete reader;
		std::cout << "Error opening input pcap file" << std::endl;
	}
	else
	{
		if (app.get_config().capture.filter.size() > 0)
		{
			if (!reader->setFilter(app.get_config().capture.filter))
			{
				std::cerr << "Could not set filter: " << app.get_config().capture.filter << std::endl;
				reader->close();
				std::exit(0);
			}
		}
		
		pcpp::RawPacket rawPacket;
		int count = 0;
		bool output_progress = !app.get_config().web.enabled;
		while (reader->getNextPacket(rawPacket))
		{
			on_packet(&rawPacket, nullptr, this);
			count++;
			if (count % 10000 == 0)
			{
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(1ms);
				if (output_progress)
				{
					std::cout << "Processed " << count << " packets" << std::endl;
				}
				
				if (app.get_db().max_reached)
				{
					std::cout << termcolor::yellow << termcolor::bold << "Captured stopped: reached maximum database size." << termcolor::reset << std::endl;
					break;
				}
			}
		}
		
		reader->close();
	}
	
	importing = false;
}

void nanocap::packet_handler::start_capture()
{
	capturing = true;
	dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(app.get_config().capture.device_ip.c_str());
	if (dev == NULL)
	{
		std::cerr << termcolor::red << "ERROR: " << termcolor::reset << termcolor::bold << "Could not find interface with IP address: " << app.get_config().capture.device_ip << ". Please update nanocap.config." << termcolor::reset << std::endl;
		std::exit(1);
	}
	else if (!dev->open())
	{
		std::cout << "Cannot open device for IP address: " << app.get_config().capture.device_ip << std::endl;
		std::exit(1);
	}
	else
	{
		print_devices();
		
		// Use Berkeley Packet Filter?
		if (app.get_config().capture.filter.size() > 0)
		{
			if (!dev->setFilter(app.get_config().capture.filter))
			{
				std::cerr << "Could not set filter: " << app.get_config().capture.filter << std::endl;
				dev->close();
				std::exit(0);
			}
		}

		std::cout << "Starting packet capture..." << std::endl;
		dev->startCapture(on_packet, this);
	}
}

void nanocap::packet_handler::stop_capture()
{
	capturing = false;
	if (dev)
	{
		dev->stopCapture();
		dev->close();
		dev = nullptr;
	}
}

void nanocap::packet_handler::handle_udp(pcpp::Packet& packet)
{
	static long consumed = 0;
	try
	{
		if (++consumed % 10000 == 0)
		{
			if (dev && app.get_db().max_reached)
			{
				std::cout << termcolor::yellow << termcolor::bold << "Captured stopped: reached maximum database size." << termcolor::reset << std::endl;
				dev->stopCapture();
			}
		}

		using bufferstream = boost::iostreams::stream<boost::iostreams::array_source>;

		udp_packet info;
		auto udp = packet.getLayerOfType<pcpp::UdpLayer>();
		std::string srcIp, dstIp;
		if (packet.isPacketOfType(pcpp::IPv4))
		{
			info.ip_version = 4;
			auto ip = (pcpp::IPv4Layer*) udp->getPrevLayer();
			info.src_ip = ip->getSrcIpAddress().toString();
			info.dst_ip = ip->getDstIpAddress().toString();
		}
		else
		{
			info.ip_version = 6;
			auto ip = (pcpp::IPv6Layer*) udp->getPrevLayer();
			info.src_ip = ip->getSrcIpAddress().toString();
			info.dst_ip = ip->getDstIpAddress().toString();
		}
		
		info.dst_port = boost::endian::big_to_native(udp->getUdpHeader()->portDst);
		info.src_port = boost::endian::big_to_native(udp->getUdpHeader()->portSrc);
		info.timestamp = packet.getRawPacketReadOnly()->getPacketTimeStamp().tv_sec;
		info.timestamp_usec = packet.getRawPacketReadOnly()->getPacketTimeStamp().tv_usec;

		auto data = packet.getLastLayer()->getData();
		auto len = packet.getLastLayer()->getDataLen();
		std::string str (reinterpret_cast<char*>(data), len);
		bufferstream bs(reinterpret_cast<char*>(data), len);

		kaitai::kstream ks (&bs);
		nano::protocol::nano_t proto (&ks);

		switch (proto.header()->message_type ())
		{
			case nano::protocol::nano_t::ENUM_MSGTYPE_INVALID: {
				// TODO: add stat
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_NOT_A_TYPE: {
				// TODO: add stat
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_KEEPALIVE:
			{
				nano::protocol::nano_t::msg_keepalive_t* msg = static_cast<nano::protocol::nano_t::msg_keepalive_t*> (proto.body ());
				this->handle_message (*msg, info);
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_CONFIRM_ACK:
			{
				nano::protocol::nano_t::msg_confirm_ack_t* msg = static_cast<nano::protocol::nano_t::msg_confirm_ack_t*> (proto.body ());
				this->handle_message (*msg, info);
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_CONFIRM_REQ:
			{
				nano::protocol::nano_t::msg_confirm_req_t* msg = static_cast<nano::protocol::nano_t::msg_confirm_req_t*> (proto.body ());
				this->handle_message (*msg, info);
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_PUBLISH: {
				nano::protocol::nano_t::msg_publish_t* msg = static_cast<nano::protocol::nano_t::msg_publish_t*> (proto.body ());
				this->handle_message (*msg, info);
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_NODE_ID_HANDSHAKE: {
				nano::protocol::nano_t::msg_node_id_handshake_t* msg = static_cast<nano::protocol::nano_t::msg_node_id_handshake_t*> (proto.body ());
				this->handle_message (*msg, info);
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_BULK_PULL: {
				
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_BULK_PUSH: {
				
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_FRONTIER_REQ: {
				
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_BULK_PULL_BLOCKS: {
				
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_BULK_PULL_ACCOUNT: {
				
				break;
			}
		}
	}
	catch (std::runtime_error & ex)
	{
		std::cerr << "Exception while parsing payload: " << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown exception" << std::endl;
	}
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_keepalive_t & msg, nanocap::udp_packet& info)
{
	app.get_db().put(msg, info);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_confirm_ack_t & msg, nanocap::udp_packet& info)
{
	app.get_db().put(msg, info);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_confirm_req_t & msg, nanocap::udp_packet& info)
{
	app.get_db().put(msg, info);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_publish_t & msg, nanocap::udp_packet& info)
{
	app.get_db().put(msg, info);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_node_id_handshake_t & msg, nanocap::udp_packet& info)
{
	app.get_db().put(msg, info);
}

void nanocap::packet_handler::print_devices()
{
	const std::vector<pcpp::PcapLiveDevice*>& devList = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();
	
	printf("\nNetwork interfaces:\n");
	for (std::vector<pcpp::PcapLiveDevice*>::const_iterator iter = devList.begin(); iter != devList.end(); iter++)
	{
		printf("    Name: '%s' IP: %s\n", (*iter)->getName(), (*iter)->getIPv4Address().toString().c_str());
	}
}

void nanocap::packet_handler::print_device_info(std::ostream & ostream)
{
	std::cout << "Interface info: " << std::endl;
	std::cout << "   Interface name:        " << dev->getName() << std::endl;
	std::cout << "   Interface description: " << dev->getDesc() << std::endl;
	std::cout << "   MAC address:           " << dev->getMacAddress().toString().c_str() << std::endl;
	std::cout << "   Default gateway:       " << dev->getDefaultGateway().toString().c_str() << std::endl;
	std::cout << "   Interface MTU:         %d " << dev->getMtu() << std::endl;
	if (dev->getDnsServers().size() > 0)
	{
		std::cout << "   DNS server:            " << dev->getDnsServers().at(0).toString().c_str() << std::endl;
	}
}

void nanocap::packet_handler::print_stats(std::ostream & ostream)
{
	ostream << "Ethernet packet count: " << ethPacketCount << std::endl;
	ostream << "IPv4 packet count:     " << ipv4PacketCount << std::endl;
	ostream << "IPv6 packet count:     " << ipv6PacketCount << std::endl;
	ostream << "TCP packet count:      " << tcpPacketCount << std::endl;
	ostream << "UDP packet count:      " << udpPacketCount << std::endl;
	ostream << "DNS packet count:      " << dnsPacketCount << std::endl;
	ostream << "HTTP packet count:     " << httpPacketCount << std::endl;
	ostream << "SSL packet count:      " << sslPacketCount << std::endl;
}
