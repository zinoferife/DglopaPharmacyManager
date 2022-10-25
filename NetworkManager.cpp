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
		return iter->second.first;
	}
	else {
		return {};
	}
}

std::string_view NetworkManager::GetSerivePort(const std::string& serv_name) const
{
	auto iter = mServices.find(serv_name);
	if (iter != mServices.end()) {
		return iter->second.second;
	}
	else {
		//what would be returned from here
		return {};
	}
}

const NetworkManager::ServiceAddr_t& NetworkManager::GetService(const std::string& serv_name) const
{
	auto iter = mServices.find(serv_name);
	if (iter == mServices.end()) {
		throw std::logic_error("Invalid service name in Network manager");
	}
	else {
		return iter->second;
	}
}

void NetworkManager::GetSettings()
{
	wxConfig& config = AppSettingsInsntance::instance();
	std::string_view NetPath = "/Network/Services";
	if (config.Exists(NetPath.data())) {
		auto oldPath = config.GetPath();
		config.SetPath(NetPath.data());

		//get the services names
		wxString names;
		long dummy = 0;

		bool cont = config.GetFirstEntry(names, dummy);
		while (cont) {
			//names cant be empty becasue cont would be false;
			wxString value;
			config.Read(names, &value);
			try {
				mServices.insert({ names.ToStdString(), ParseServiceAddr(value.ToStdString()) });
			}catch(const std::logic_error& e){
				spdlog::get("log")->critical(e.what());
			}
			cont = config.GetNextEntry(names, dummy);
		}
		config.SetPath(oldPath);

	}
	else {
		//no services in the settings; ask user for services or what ??
	}

}

void NetworkManager::SetSettings()
{
	wxConfig& config = AppSettingsInsntance::instance();
	auto oldPath = config.GetPath();
	config.SetPath("/Network/Services");
	for (auto& service : mServices) {
		ServiceAddr_t addy = service.second;
		config.Write(service.first, wxString(fmt::format("{}%{}", addy.first, addy.second)));
	}
	config.SetPath(oldPath);

	//flush
	config.Flush();
}

NetworkManager::ServiceAddr_t NetworkManager::ParseServiceAddr(const std::string& str) const
{
	auto pos = str.find_first_of("%");
	if (pos == std::string::npos) {
		throw std::logic_error("ServiceAddr_t format in this entry is not formarted correctly");
	}
	std::string s_1(str.begin(), std::next(str.begin(), pos));
	std::string s_2(str.begin() + pos, str.end());

	ServiceAddr_t addy{ std::move(s_1), std::move(s_2) };

	return addy;
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
