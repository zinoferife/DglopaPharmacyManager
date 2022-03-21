#include "common.h"
#include "NetworkManager.h"

NetworkManager::NetworkManager()
{
	mWork.reset(new asio::io_context::work(mIOContext));
	NetSubscriber::InitMessageHandlers();
}

NetworkManager::~NetworkManager()
{
	StopContext();
}

void NetworkManager::RunContext()
{
	if (!mNetworkThread) {
		mNetworkThread = std::make_unique<std::thread>([this]() { mIOContext.run(); });
	}
}

void NetworkManager::StopContext()
{
	delete mWork.release();
	mIOContext.stop();

	if (mNetworkThread) {
		mNetworkThread->join();
	}
}

NetSubscriberPtr NetworkManager::CreateSubScriber(const std::string& SubName, bool GenerateID)
{
	auto ptr = std::make_shared<NetSubscriber>(mIOContext);
	if (ptr) {
		ptr->SetName(SubName);
		if (GenerateID) {
			ptr->GenerateID();
		}
		if (!ptr->SyncConnect(mServerSubscriberEndpoint)) {
			return nullptr;
		}
		mSubscribers.push_back(ptr);
		return ptr;
	}
	else {
		spdlog::get("log")->error("Could not create New Subscriber");
		return nullptr;
	}
}

void NetworkManager::DestorySubScriber(TClientPtr Sub)
{
	auto found = std::find(mSubscribers.begin(), mSubscribers.end(), Sub);
	if (found != mSubscribers.end()) {
		mSubscribers.erase(found);
	}
}

void NetworkManager::ClearSubscriberList()
{
	//clears the list
	mSubscribers.clear();
}

std::string NetworkManager::GetServerSubscriberEndpointAddressAsString() const noexcept
{
	// TODO: insert return statement here
	return mServerSubscriberEndpoint.address().to_string();
}
