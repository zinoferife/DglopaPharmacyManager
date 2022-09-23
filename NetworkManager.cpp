#include "common.h"
#include "NetworkManager.h"

NetworkManager::NetworkManager()
{
	

}

NetworkManager::~NetworkManager()
{
	if (mNetThread.joinable()) {
		m_work.reset(nullptr);
		mNetThread.join();
	}
}

void NetworkManager::Stop() 
{
	if (!mStopCondition.load()) {
		mStopCondition.store(true);
	}
}

bool NetworkManager::AddService(const std::string& service_name, const std::string& service_addr, const std::string& service_port)
{
	auto [iter, insert] = mServices.insert({ service_name, {service_addr, service_port} });
	return insert;
}

bool NetworkManager::RemoveService(const std::string& service_name)
{
	return(mServices.erase(service_name) != 0);
}

std::string_view NetworkManager::GetServiceAddress(const std::string& serv_name) const
{
	auto iter = mServices.find(serv_name);
	if (iter != mServices.end()) {
		return iter->second.first.c_str();
	}
	else {
		return {};
	}
}

std::string_view NetworkManager::GetSerivePort(const std::string& serv_name) const
{
	auto iter = mServices.find(serv_name);
	if (iter != mServices.end()) {
		return iter->second.second.c_str();
	}
	else {
		//what would be returned from here
		return {};
	}
}


void NetworkManager::Run()
{
	spdlog::get("log")->info("Starting network thread");
	if (!mNetThread.joinable()) {
		//thread is empty. Default constructed
		m_work = std::make_unique<net::io_context::work>(mIoContext);
		mNetThread = std::thread { [this]() { mIoContext.run(); } };
	}
}

void NetworkManager::Join()
{
	if(mNetThread.joinable()){
		m_work.reset(nullptr);
		mNetThread.join();
	}
}
