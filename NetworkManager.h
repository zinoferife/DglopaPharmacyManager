#pragma once
//serves as a data structure to hold all things network

#include <spdlog/spdlog.h>
#include <fmt/format.h>

#include <thread>
#include <nl_http.h>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <atomic>

#include <map>
#include <utility>

namespace fs = std::filesystem;
namespace js = nlohmann;
namespace net = boost::asio;


class NetworkManager
{

public:
	typedef std::pair<std::string, std::uint16_t>  service_addr_t;


	NetworkManager(const std::string& hostname, std::uint16_t port);

	~NetworkManager(); 



	inline net::io_context& GetIoContex() { return mIoContext; }


	//returns an empty endpoint if there is no such service
	net::ip::tcp::endpoint ToTcpEndpoint(const std::string& service_name) const;
	net::ip::udp::endpoint ToUdpEndpoint(const std::string& service_name) const;


	void Run();
	void Join();
	void Stop();

	void Addervice(const std::string& service_name, const std::string& service_addr,
		std::uint16_t service_port);
	bool RemoveService(const std::string& service_name);


private:
	//the host of our remote serrvice
	net::io_context mIoContext;
	std::thread mNetThread;

	
	std::atomic_bool mStopCondition;

	//holds the address of the services that PharmaOffice can connect to
	std::map<std::string, service_addr_t> mServices;

	//things for staticstics 

};

