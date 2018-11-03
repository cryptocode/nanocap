#include <iostream>
#include <chrono>
#include <app.hpp>
#include <db.hpp>
#include <packet_handler.hpp>
#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>
#include "PcapFileDevice.h"

static boost::asio::thread_pool pool(std::thread::hardware_concurrency());

static void on_packet(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie)
{
	pcpp::Packet parsed_packet(packet);
	nanocap::packet_handler* handler = static_cast<nanocap::packet_handler*>(cookie);
	handler->consume(parsed_packet);
}

nanocap::packet_handler::packet_handler(nanocap::app& app) : app (app)
{
}

nanocap::packet_handler::~packet_handler()
{
	pool.join();
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
		//printf("Yay, got udp\n");
		handle_udp(packet);
		udpPacketCount++;
	}
	if (packet.isPacketOfType(pcpp::HTTP))
		httpPacketCount++;
	if (packet.isPacketOfType(pcpp::SSL))
		sslPacketCount++;
}

void nanocap::packet_handler::start_capture()
{
	pcpp::IFileReaderDevice* reader = pcpp::IFileReaderDevice::getReader("pcap/rep.pcap");
	if (!reader->open())
	{
		delete reader;
		std::cout << "Error opening input pcap file" << std::endl;
	}
	else
	{
		if (app.get_config().filter.size() > 0)
		{
			if (!reader->setFilter(app.get_config().filter))
			{
				std::cerr << "Could not set filter: " << app.get_config().filter << std::endl;
				reader->close();
				std::exit(0);
			}
		}

		pcpp::RawPacket rawPacket;
		int count = 0;
		while (reader->getNextPacket(rawPacket))
		{
			on_packet(&rawPacket, nullptr, this);
			count++;
			if (count % 1000 == 0)
			{
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(1ms);
				std::cout << std::this_thread::get_id() << ": Processed " << count << " packets" << std::endl;
			}
		}
		
		reader->close();
	}
	
if (true) return;
	
	dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(app.get_config().ip.c_str());
	if (dev == NULL)
	{
		std::cerr << "Could not find interface with IP address: " <<  app.get_config().ip.c_str() << std::endl;
		std::exit(1);
	}
	else if (!dev->open())
	{
		std::cout << "Cannot open device for IP address: " << app.get_config().ip.c_str() << std::endl;
		std::exit(1);
	}
	else
	{
		print_devices();
		
		// Use Berkeley Packet Filter?
		if (app.get_config().filter.size() > 0)
		{
			if (!dev->setFilter(app.get_config().filter))
			{
				std::cerr << "Could not set filter: " << app.get_config().filter << std::endl;
				dev->close();
				std::exit(0);
			}
		}
		else
		{
			pcpp::PortFilter portFilter(7075, pcpp::SRC_OR_DST);
			
			pcpp::AndFilter andFilter;
			andFilter.addFilter(&portFilter);

			// Uncomment for TCP only
			//pcpp::ProtoFilter protocolFilter(pcpp::TCP);
			//andFilter.addFilter(&protocolFilter);
			
			// set the filter on the device
			dev->setFilter(andFilter);
		}

		std::cout << "Starting packet capture..." << std::endl;
		dev->startCapture(on_packet, this);
	}
}

void nanocap::packet_handler::stop_capture()
{
	if (dev)
	{
		dev->stopCapture();
		dev->close();
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

void nanocap::packet_handler::print_devices()
{
	const std::vector<pcpp::PcapLiveDevice*>& devList = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();
	
	printf("\nNetwork interfaces:\n");
	for (std::vector<pcpp::PcapLiveDevice*>::const_iterator iter = devList.begin(); iter != devList.end(); iter++)
	{
		printf("    Name: '%s' IP: %s\n", (*iter)->getName(), (*iter)->getIPv4Address().toString().c_str());
	}
}
#include <boost/iostreams/stream.hpp>
//using bufferstream = boost::iostreams::stream_buffer<boost::iostreams::basic_array_source<uint8_t>>;
using bufferstream = boost::iostreams::stream<boost::iostreams::array_source>;
void nanocap::packet_handler::handle_udp(pcpp::Packet& packet)
{
	try
	{
		auto udp = packet.getLayerOfType<pcpp::UdpLayer>();
		std::string srcIp, dstIp;
		bool ipv6 = false;
		if (packet.isPacketOfType(pcpp::IPv4))
		{
			auto ip = (pcpp::IPv4Layer*) udp->getPrevLayer();
			current_src_ip = ip->getSrcIpAddress().toString();
			current_dst_ip = ip->getDstIpAddress().toString();
		}
		else
		{
			ipv6 = true;
			auto ip = (pcpp::IPv6Layer*) udp->getPrevLayer();
			current_src_ip = ip->getSrcIpAddress().toString();
			current_dst_ip = ip->getDstIpAddress().toString();
		}
		
		current_dst_port = boost::endian::big_to_native(udp->getUdpHeader()->portDst);
		current_src_port = boost::endian::big_to_native(udp->getUdpHeader()->portSrc);
		
		//std::cout << ipv6 << ":UDP from/to: " << srcIp << ":" << srcPort << " / " << dstIp << ":" << dstPort << std::endl;
		
		current_inbound = (current_dst_ip == app.get_config().ip && current_dst_port == app.get_config().port);
		current_outbound = (current_src_ip == app.get_config().ip && current_src_port == app.get_config().port);
		
		auto data = packet.getLastLayer()->getData();
		auto len = packet.getLastLayer()->getDataLen();
		std::string str (reinterpret_cast<char*>(data), len);
		/*queue_mutex.lock();
		queue.push_back(str);
		queue_mutex.unlock();*/

		/*boost::asio::post(pool,
						  [this]()
		{
			queue_mutex.lock();
			auto str = queue.front();
			queue.pop_front();
			queue_mutex.unlock();
			*/
			kaitai::kstream ks (str);
			
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
					nano::protocol::nano_t::msg_keepalive_t * msg = static_cast<nano::protocol::nano_t::msg_keepalive_t *> (proto.body ());
					this->handle_message (*msg);
					break;
				}
				case nano::protocol::nano_t::ENUM_MSGTYPE_CONFIRM_ACK:
				{
					nano::protocol::nano_t::msg_confirm_ack_t * msg = static_cast<nano::protocol::nano_t::msg_confirm_ack_t *> (proto.body ());
					this->handle_message (*msg);
					break;
				}
				case nano::protocol::nano_t::ENUM_MSGTYPE_CONFIRM_REQ:
				{
					nano::protocol::nano_t::msg_confirm_req_t * msg = static_cast<nano::protocol::nano_t::msg_confirm_req_t *> (proto.body ());
					this->handle_message (*msg);
					break;
				}
				case nano::protocol::nano_t::ENUM_MSGTYPE_PUBLISH: {
					
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
				case nano::protocol::nano_t::ENUM_MSGTYPE_NODE_ID_HANDSHAKE: {
					
					break;
				}
				case nano::protocol::nano_t::ENUM_MSGTYPE_BULK_PULL_ACCOUNT: {
					
					break;
				}
			}
							  
		//});

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

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_keepalive_t & msg)
{
	if (current_inbound)
		in_keepalive++;
	else if (current_outbound)
		out_keepalive++;
	
	// Save it
	app.get_db().put(msg);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_confirm_ack_t & msg)
{
	app.get_db().put(msg);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_confirm_req_t & msg)
{
	app.get_db().put(msg);
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
	
	ostream << "Keepalive inbound:     " << in_keepalive << std::endl;
	ostream << "Keepalive outbound:    " << out_keepalive << std::endl;
}
