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

net::ip::tcp::endpoint NetworkManager::ToTcpEndpoint() const
{
	return net::ip::tcp::endpoint(net::ip::address::from_string(mHostName), 
			mPortNum);
}

void NetworkManager::Run()
{
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
