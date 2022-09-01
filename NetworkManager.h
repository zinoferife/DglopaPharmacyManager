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

namespace fs = std::filesystem;
namespace js = nlohmann;
namespace net = boost::asio;


class NetworkManager
{

public:


	NetworkManager(const std::string& hostname, std::uint16_t port);

	~NetworkManager(); 



	inline const std::string& GetHostName() const { return mHostName; }
	inline void SetHostName(const std::string& hostname) { mHostName = hostname;  }
	inline std::uint16_t GetPortNum() const { return mPortNum; }
	inline void SetPortNum(std::uint16_t port) { mPortNum = port;  }

	inline net::io_context& GetIoContex() { return mIoContext; }

	inline net::ip::tcp::endpoint ToTcpEndpoint() const;

	void Run();
	void Join();
	void Stop();
private:
	//the host of our remote serrvice 
	std::uint16_t mPortNum;
	std::string mHostName;
	net::io_context mIoContext;
	std::thread mNetThread;

	std::atomic_bool mStopCondition;


	//things for staticstics 

};

