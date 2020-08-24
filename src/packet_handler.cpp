#include <iostream>
#include <chrono>
#include <istream>
#include <app.hpp>
#include <db.hpp>
#include <packet_handler.hpp>
#include <util/termcolor.hpp>
#include <util/hex.hpp>
#include <util/nanoutil.hpp>
#include <TcpLayer.h>
#include <TcpReassembly.h>
#include <UdpLayer.h>
#include <IPv4Layer.h>
#include <IPv6Layer.h>
#include <PcapFileDevice.h>
#include <PcapLiveDeviceList.h>
#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/iostreams/stream.hpp>

namespace
{
	/**
	 * Called when new data on a connection is available. To prevent pcap from dropping packets, we
	 * copy the needed connection data and payload. This is then put on a queue which is processed
	 * by a separate thread.
	 */
	void tcpReassemblyMsgReadyCallback(int sideIndex, const pcpp::TcpStreamData& tcpData, void* userCookie)
	{
		auto handler = static_cast<nanocap::packet_handler*> (userCookie);
		auto flow_data = handler->connMgr.find(tcpData.getConnectionData().flowKey);

		// In case tcpReassemblyConnectionStartCallback has not been called
		if (flow_data == handler->connMgr.end())
		{
			handler->connMgr.insert(std::make_pair(tcpData.getConnectionData().flowKey, std::make_shared<nanocap::tcp_flow_data>()));
			flow_data = handler->connMgr.find(tcpData.getConnectionData().flowKey);
		}
		
		std::unique_lock<std::mutex> lk(handler->packet_queue_mutex);
		handler->packet_queue.emplace_back();
		nanocap::nano_packet& info = handler->packet_queue.back();
		info.src_ip = tcpData.getConnectionData().srcIP->toString();
		info.dst_ip = tcpData.getConnectionData().dstIP->toString();
		info.dst_port = tcpData.getConnectionData().dstPort;
		info.src_port = tcpData.getConnectionData().srcPort;
		info.timestamp = tcpData.getConnectionData().endTime.tv_sec;
		info.timestamp_usec = tcpData.getConnectionData().endTime.tv_usec;
		info.ip_version = tcpData.getConnectionData().srcIP->getType() == pcpp::IPAddress::AddressType::IPv4AddressType ? 4 : 6;
		
		// Copy data for later processing on a separate thread
		info.data = std::string(reinterpret_cast<const char*>(tcpData.getData()), tcpData.getDataLength());
		info.side = sideIndex;
		info.flow_data = flow_data->second;

		// Notify processing thread
		lk.unlock();
		handler->packet_queue_cond.notify_all();
	}

	void tcpReassemblyConnectionStartCallback(const pcpp::ConnectionData& connectionData, void* userCookie)
	{
		auto handler = static_cast<nanocap::packet_handler*> (userCookie);
		auto data = handler->connMgr.find(connectionData.flowKey);

		int ipv = connectionData.srcIP->getType() == pcpp::IPAddress::AddressType::IPv4AddressType ? 4 : 6;
		handler->get_app().get_db().put_connection(ipv, connectionData.srcIP->toString(), connectionData.srcPort, connectionData.dstIP->toString(), connectionData.dstPort, false);

		if (data == handler->connMgr.end())
		{
			handler->connMgr.insert(std::make_pair(connectionData.flowKey, std::make_shared<nanocap::tcp_flow_data>()));
		}
	}

	void tcpReassemblyConnectionEndCallback(const pcpp::ConnectionData& connectionData, pcpp::TcpReassembly::ConnectionEndReason reason, void* userCookie)
	{
		auto handler = static_cast<nanocap::packet_handler*> (userCookie);
		auto data = handler->connMgr.find(connectionData.flowKey);
		
		int ipv = connectionData.srcIP->getType() == pcpp::IPAddress::AddressType::IPv4AddressType ? 4 : 6;
		handler->get_app().get_db().put_connection(ipv, connectionData.srcIP->toString(), connectionData.srcPort, connectionData.dstIP->toString(), connectionData.dstPort, true);

		if (data != handler->connMgr.end())
		{
			handler->connMgr.erase(data);
		}
	}

	void on_packet(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie)
	{
		pcpp::Packet parsed_packet(packet);
		nanocap::packet_handler* handler = static_cast<nanocap::packet_handler*>(cookie);

		if (parsed_packet.isPacketOfType(pcpp::UDP))
		{
			handler->handle_udp(parsed_packet);
		}
		else if (parsed_packet.isPacketOfType(pcpp::TCP))
		{
			handler->tcpReassembly->reassemblePacket(packet);
		}
	}
}// ns

nanocap::packet_handler::packet_handler(nanocap::app& app) : app (app), packet_processing_thread([this](){
	this->run_packet_processing_loop();
})
{
	tcpReassembly = std::make_unique<pcpp::TcpReassembly> (tcpReassemblyMsgReadyCallback, this, tcpReassemblyConnectionStartCallback, tcpReassemblyConnectionEndCallback);
}

nanocap::packet_handler::~packet_handler()
{
	print_stats();
	stopped = true;
	packet_processing_thread.join();
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
	dev = nullptr;
	if (!app.get_config().capture.device_ip.empty())
	{
		dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(app.get_config().capture.device_ip.c_str());
	}

	if (app.get_config().capture.device_ip.empty() || dev == nullptr)
	{
		const std::vector<pcpp::PcapLiveDevice*>& devList = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();
		for (std::vector<pcpp::PcapLiveDevice*>::const_iterator iter = devList.begin(); iter != devList.end(); iter++)
		{
			auto ip = (*iter)->getIPv4Address().toString();
			if (ip != "0.0.0.0" && ip != "127.0.0.1")
			{
				printf("No IP set. Using first device. Name: '%s' IP: %s\nIf this is incorrect, set \"device_ip\" in nanocap.config\n", (*iter)->getName(), ip.c_str());
				dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(ip.c_str());
				break;
			}
		}
	}

	if (dev == nullptr)
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

		// Apply a Berkeley Packet Filter?
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

		print_stats();
		dev->close();
		dev = nullptr;
	}
}

void nanocap::packet_handler::run_packet_processing_loop()
{
	while(!stopped)
	{
		std::unique_lock<std::mutex> locker(packet_queue_mutex);
		packet_queue_cond.wait(locker, [this](){return packet_queue.size() > 0;});

		auto info = packet_queue.back();
		packet_queue.pop_back();

		handle_tcp(info);
	}
}

void nanocap::packet_handler::handle_tcp(nanocap::nano_packet& info)
{
	assert (info.flow_data->accumulated_data.capacity() == 4096);

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
		
		// First, check for bootstrap responses. These are not Nano messages, but a stream of data representing replies.
		// NOTE: currently, only frontier responses are handled
		if (app.get_config().capture.bootstrap_details && info.data.size() > 0 &&
			info.flow_data->current_msg_type == nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_FRONTIER_REQ)
		{
			std::copy(info.data.begin(), info.data.end(), std::back_inserter(info.flow_data->accumulated_data));
			
			// Read all available entries in the circular buffer.
			// If the last entry is partially available, we'll wait for more data.
			while (info.flow_data->accumulated_data.size() >= 64)
			{
				auto contiguous = info.flow_data->accumulated_data.linearize();
				bufferstream bs(reinterpret_cast<const char*>(contiguous), 64);
				kaitai::kstream ks (&bs);

				try
				{
					nano::protocol::nano_t::frontier_response_t::frontier_entry_t entry(&ks);
					bool all_zero = entry.account().find_first_not_of((char)0) == std::string::npos;
					if (all_zero)
					{
						// Enable new types of messages on the same connection
						info.flow_data->current_msg_type = nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_INVALID;
						info.flow_data->accumulated_data.clear();
						break;
					}
					
					this->get_app().get_db().put(entry, info);

					info.flow_data->accumulated_data.erase(info.flow_data->accumulated_data.begin(), info.flow_data->accumulated_data.begin() + ks.pos());
				}
				catch (std::runtime_error & ex)
				{
					std::cerr << "Could not read req response. Accumulated size: "<<info.flow_data->accumulated_data.size()<<" , Data size is: " << info.data.size() << std::endl;
					break;
				}
			}
		}
		else
		{
			kaitai::kstream ks (info.data);
			nano::protocol::nano_t proto (&ks);

			switch (proto.header()->message_type ())
			{
				case nano::protocol::nano_t::ENUM_MSGTYPE_INVALID:
				{
					break;
				}
				case nano::protocol::nano_t::ENUM_MSGTYPE_NOT_A_TYPE:
				{
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
				case nano::protocol::nano_t::ENUM_MSGTYPE_BULK_PULL:
				{
					nano::protocol::nano_t::msg_bulk_pull_t* msg = static_cast<nano::protocol::nano_t::msg_bulk_pull_t*> (proto.body ());
					this->handle_message (*msg, info);
					break;
				}
				case nano::protocol::nano_t::ENUM_MSGTYPE_BULK_PUSH:
				{
					nano::protocol::nano_t::msg_bulk_push_t* msg = static_cast<nano::protocol::nano_t::msg_bulk_push_t*> (proto.body ());
					this->handle_message (*msg, info);
					break;
				}
				case nano::protocol::nano_t::ENUM_MSGTYPE_FRONTIER_REQ:
				{
					nano::protocol::nano_t::msg_frontier_req_t* msg = static_cast<nano::protocol::nano_t::msg_frontier_req_t*> (proto.body ());
					this->handle_message (*msg, info);
					break;
				}
				case nano::protocol::nano_t::ENUM_MSGTYPE_BULK_PULL_BLOCKS:
				{
					nano::protocol::nano_t::msg_bulk_pull_blocks_t* msg = static_cast<nano::protocol::nano_t::msg_bulk_pull_blocks_t*> (proto.body ());
					this->handle_message (*msg, info);
					break;
				}
				case nano::protocol::nano_t::ENUM_MSGTYPE_BULK_PULL_ACCOUNT:
				{
					nano::protocol::nano_t::msg_bulk_pull_account_t* msg = static_cast<nano::protocol::nano_t::msg_bulk_pull_account_t*> (proto.body ());
					this->handle_message (*msg, info);
					break;
				}
				case nano::protocol::nano_t::ENUM_MSGTYPE_TELEMETRY_REQ:
				{
					nano::protocol::nano_t::msg_telemetry_req_t* msg = static_cast<nano::protocol::nano_t::msg_telemetry_req_t*> (proto.body ());
					this->handle_message (*msg, info);
					break;
				}
				case nano::protocol::nano_t::ENUM_MSGTYPE_TELEMETRY_ACK:
				{
					nano::protocol::nano_t::msg_telemetry_ack_t* msg = static_cast<nano::protocol::nano_t::msg_telemetry_ack_t*> (proto.body ());
					this->handle_message (*msg, info);
					break;
				}
			}
		}
	}
	catch (std::runtime_error & ex)
	{
		// Invalid payloads are to be expected
	}
	catch (...)
	{
		std::cerr << "Unknown exception" << std::endl;
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

		nano_packet info;
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
			case nano::protocol::nano_t::ENUM_MSGTYPE_INVALID:
			{
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_NOT_A_TYPE:
			{
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
			case nano::protocol::nano_t::ENUM_MSGTYPE_BULK_PULL:
			{
				nano::protocol::nano_t::msg_bulk_pull_t* msg = static_cast<nano::protocol::nano_t::msg_bulk_pull_t*> (proto.body ());
				this->handle_message (*msg, info);
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_BULK_PUSH:
			{
				nano::protocol::nano_t::msg_bulk_push_t* msg = static_cast<nano::protocol::nano_t::msg_bulk_push_t*> (proto.body ());
				this->handle_message (*msg, info);
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_FRONTIER_REQ:
			{
				nano::protocol::nano_t::msg_frontier_req_t* msg = static_cast<nano::protocol::nano_t::msg_frontier_req_t*> (proto.body ());
				this->handle_message (*msg, info);
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_BULK_PULL_BLOCKS:
			{
				nano::protocol::nano_t::msg_bulk_pull_blocks_t* msg = static_cast<nano::protocol::nano_t::msg_bulk_pull_blocks_t*> (proto.body ());
				this->handle_message (*msg, info);
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_BULK_PULL_ACCOUNT:
			{
				nano::protocol::nano_t::msg_bulk_pull_account_t* msg = static_cast<nano::protocol::nano_t::msg_bulk_pull_account_t*> (proto.body ());
				this->handle_message (*msg, info);
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_TELEMETRY_REQ:
			{
				nano::protocol::nano_t::msg_telemetry_req_t* msg = static_cast<nano::protocol::nano_t::msg_telemetry_req_t*> (proto.body ());
				this->handle_message (*msg, info);
				break;
			}
			case nano::protocol::nano_t::ENUM_MSGTYPE_TELEMETRY_ACK:
			{
				nano::protocol::nano_t::msg_telemetry_ack_t* msg = static_cast<nano::protocol::nano_t::msg_telemetry_ack_t*> (proto.body ());
				this->handle_message (*msg, info);
				break;
			}
		}
	}
	catch (std::runtime_error & ex)
	{
		std::cerr << "Error while parsing payload: " << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "An unknown error occurred while parsing payload" << std::endl;
	}
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_keepalive_t & msg, nanocap::nano_packet& info)
{
	if (info.flow_data)
	{
		info.flow_data->current_msg_type = nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_KEEPALIVE;
		info.flow_data->accumulated_data.clear();
	}

	app.get_db().put(msg, info);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_confirm_ack_t & msg, nanocap::nano_packet& info)
{
	if (info.flow_data)
	{
		info.flow_data->current_msg_type = nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_CONFIRM_ACK;
	}

	app.get_db().put(msg, info);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_confirm_req_t & msg, nanocap::nano_packet& info)
{
	if (info.flow_data)
	{
		info.flow_data->current_msg_type = nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_CONFIRM_REQ;
	}

	app.get_db().put(msg, info);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_publish_t & msg, nanocap::nano_packet& info)
{
	if (info.flow_data)
	{
		info.flow_data->current_msg_type = nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_PUBLISH;
	}

	app.get_db().put(msg, info);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_node_id_handshake_t & msg, nanocap::nano_packet& info)
{
	if (info.flow_data)
	{
		info.flow_data->current_msg_type = nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_NODE_ID_HANDSHAKE;
	}

	app.get_db().put(msg, info);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_frontier_req_t & msg, nano_packet& info)
{
	if (info.flow_data)
	{
		info.flow_data->current_msg_type = nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_FRONTIER_REQ;
		info.flow_data->requesting_side = info.side;
	}
	app.get_db().put(msg, info);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_telemetry_req_t & msg, nano_packet& info)
{
	if (info.flow_data)
	{
		info.flow_data->current_msg_type = nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_TELEMETRY_REQ;
	}

	app.get_db().put(msg, info);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_telemetry_ack_t & msg, nano_packet& info)
{
	if (info.flow_data)
	{
		info.flow_data->current_msg_type = nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_TELEMETRY_ACK;
	}

	app.get_db().put(msg, info);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_bulk_pull_t & msg, nano_packet& info)
{
	if (info.flow_data)
	{
		info.flow_data->current_msg_type = nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_BULK_PULL;
	}

	app.get_db().put(msg, info);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_bulk_pull_blocks_t & msg, nano_packet& info)
{
	if (info.flow_data)
	{
		info.flow_data->current_msg_type = nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_BULK_PULL_BLOCKS;
	}

	app.get_db().put(msg, info);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_bulk_push_t & msg, nano_packet& info)
{
	if (info.flow_data)
	{
		info.flow_data->current_msg_type = nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_BULK_PUSH;
	}

	app.get_db().put(msg, info);
}

void nanocap::packet_handler::handle_message (nano::protocol::nano_t::msg_bulk_pull_account_t & msg, nano_packet& info)
{
	if (info.flow_data)
	{
		info.flow_data->current_msg_type = nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_BULK_PULL_ACCOUNT;
	}

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

void nanocap::packet_handler::print_stats()
{
	if (dev)
	{
		pcap_stat stats;
		dev->getStatistics(stats);
		std::cout << "-----------Statistics------------\n";
		std::cout << "Received by pcap     : " << stats.ps_recv << std::endl;
		std::cout << "Dropped by pcap      : " << stats.ps_drop << std::endl;
		std::cout << "Dropped by interface : " << stats.ps_ifdrop << std::endl;
		std::cout << "----------------------------------\n";
	}
}
