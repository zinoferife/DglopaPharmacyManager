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
#include <string_view>


namespace fs = std::filesystem;
namespace js = nlohmann;
namespace net = boost::asio;


class NetworkManager
{

public:
	typedef std::pair<std::string, std::string>  service_addr_t;


	NetworkManager();

	~NetworkManager(); 



	inline net::io_context& GetIoContex() { return mIoContext; }



	void Run();
	void Join();
	void Stop();

	bool AddService(const std::string& service_name, const std::string& service_addr,
		const std::string& service_port);
	bool RemoveService(const std::string& service_name);

	std::string_view GetServiceAddress(const std::string& serv_name) const;
	std::string_view GetSerivePort(const std::string& serv_name) const;



private:
	//the host of our remote serrvice
	net::io_context mIoContext;
	std::thread mNetThread;
	std::unique_ptr<net::io_context::work> m_work;


	
	std::atomic_bool mStopCondition;

	//holds the address of the services that PharmaOffice can connect to
	std::map<std::string, service_addr_t> mServices;

	//things for staticstics 

};

