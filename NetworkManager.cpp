#include "common.h"
#include "NetworkManager.h"

NetworkManager::NetworkManager(const std::string& hostname, std::uint16_t port)
{
	

}

NetworkManager::~NetworkManager()
{
	if (!mStopCondition.load()) {
		mStopCondition.store(true);
	}
}

void NetworkManager::Stop()
{
	if (!mStopCondition.load()) {
		mStopCondition.store(true);
	}
}

void NetworkManager::Addervice(const std::string& service_name, const std::string& service_addr, std::uint16_t service_port)
{
}

bool NetworkManager::RemoveService(const std::string& service_name)
{
	return false;
}



net::ip::tcp::endpoint NetworkManager::ToTcpEndpoint(const std::string& service) const
{
	auto iter = mServices.find(service);
	if (iter != mServices.end()) {
		auto& Addr = iter->second;
		return net::ip::tcp::endpoint(net::ip::address::from_string(Addr.first), Addr.second);
	}
	return net::ip::tcp::endpoint{};


}

net::ip::udp::endpoint NetworkManager::ToUdpEndpoint(const std::string& service_name) const
{
	auto iter = mServices.find(service_name);
	if (iter != mServices.end()) {
		auto& Addr = iter->second;
		return net::ip::udp::endpoint(net::ip::address::from_string(Addr.first), Addr.second);
	}
	return net::ip::udp::endpoint{};
}

void NetworkManager::Run()
{
	spdlog::get("log")->info("Starting network thread");
	if (!mNetThread.joinable()) {
		//thread is empty. Default constructed
		mNetThread = std::thread { [this]() { mIoContext.run(); } };
	}
}

void NetworkManager::Join()
{
	if(mNetThread.joinable()){
		mNetThread.join();
	}
}
