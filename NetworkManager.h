#pragma once
#include <nlohmann/json.hpp>
#include <thread>
#include <shared_mutex>
#include <asio.hpp>
#include <vector>
#include <algorithm>


#include "NetMessage.h"
#include "NetSubscriber.h"


namespace js = nlohmann;
class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();
	void RunContext();
	void StopContext();
	NetSubscriberPtr CreateSubScriber(const std::string& SubName, bool GenerateID = true);
	void DestorySubScriber(TClientPtr Sub);
	void ClearSubscriberList();


	//server endpoint functions
	inline void SetServerSubscriberEndpoint(const asio::ip::tcp::endpoint& endpoint) noexcept { mServerSubscriberEndpoint = endpoint; }
	inline const asio::ip::tcp::endpoint& GetServerSubscriberEndpoint() const noexcept{ return mServerSubscriberEndpoint; }
	std::string GetServerSubscriberEndpointAddressAsString() const noexcept;

private:
	std::unique_ptr<std::thread> mNetworkThread;
	std::unique_ptr<asio::io_context::work> mWork;
	asio::io_context mIOContext;
	std::vector<NetSubscriberPtr> mSubscribers;
	asio::ip::tcp::endpoint mServerSubscriberEndpoint;
};

