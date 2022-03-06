#pragma once
#include <nlohmann/json.hpp>
#include <thread>
#include <shared_mutex>
#include <asio.hpp>
#include <vector>


#include "NetMessage.h"
#include "NetSubscriber.h"


namespace js = nlohmann;
class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();

	std::shared_ptr<TClientPtr> CreateSubScriber(const std::string& SubName, bool GenerateID);
	void DestorySubScriber(std::shared_ptr<TClientPtr> Sub);



private:
	asio::io_context mIOContext;
	std::vector<TClientPtr> mSubscribers;
};

