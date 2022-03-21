#include "common.h"
#include "NetSubscriber.h"

NetSubscriber::NetSubscriber(asio::io_context& context)
	:TClient(context), mPublisherID(boost::uuids::nil_uuid())
{
	SetName("Default Name for Subscriber");
	
}

NetSubscriber::~NetSubscriber()
{
}

void NetSubscriber::HandleError(const asio::error_code& ec)
{
	spdlog::get("log")->error("{}, {:d}", ec.message(), ec.value());
}

void NetSubscriber::Init()
{
	//if we have saved data, use save data, if not get data from server
	NetMessage message;
	if (mClientData.is_null()) {
		message.GetJsonMessage() = {
			{"Type", "Init"},
			{"Sub_id", boost::uuids::to_string(boost::uuids::nil_uuid())},
			{"Name", GetName()},
			{"Publisher_id", boost::uuids::to_string(boost::uuids::nil_uuid())}
		};
	}
	else {
		message.GetJsonMessage() = {
			{"Type", "Init"},
			{"Sub_id", GetIDAsString()},
			{"Name", GetName()},
			{"Publisher_id", std::string(mClientData["Pub_id"])}
		};
	}
	Write(message);
	Read();
}

void NetSubscriber::InitMessageHandlers()
{
	AddMessageHandler("Publist", [](const NetMessage& messge, TClientPtr ptr) {
		auto SubPtr = std::dynamic_pointer_cast<NetSubscriber, TClient>(ptr);
		SubPtr->OnPubList(messge);
	});

	AddMessageHandler("WELCOMESUB", [](const NetMessage& message, TClientPtr ptr) {
		auto SubPtr = std::dynamic_pointer_cast<NetSubscriber, TClient>(ptr);
		SubPtr->OnWelcomSub(message);
	});

	AddMessageHandler("ShutDown", [](const NetMessage& message, TClientPtr ptr) {
		auto SubPtr = std::dynamic_pointer_cast<NetSubscriber, TClient>(ptr);
		SubPtr->OnPublisherShutDown(message);
	});

	AddMessageHandler("UnSubscribed", [](const NetMessage& message, TClientPtr ptr) {
		auto SubPtr = std::dynamic_pointer_cast<NetSubscriber, TClient>(ptr);
		SubPtr->OnUnsubscribed(message);
	});
}

void NetSubscriber::OnConnected(const asio::error_code& ec)
{
	if (!ec) {
		spdlog::get("log")->info("Connected to {} on port {:d}", mSocket->remote_endpoint().address().to_string(),
			mSocket->remote_endpoint().port());
		Init();
	}
	else {
		HandleError(ec);
	}
}

void NetSubscriber::OnRead(const asio::error_code& ec, size_t bytes)
{
	if (ec) {
		HandleError(ec);
		return;
	}
	if (mReadMessage.ParseStreamBuffer()) {
		auto& json = mReadMessage.GetJsonMessage();
		if (json.is_null()) {
			DeliverErrorMessage(111, "Json format error, Json was not in the correct schema");
		}
		else {
			auto iter = mMessageHandlers.find(json["Type"]);
			if (iter != mMessageHandlers.end()) {
				iter->second(mReadMessage, shared_from_this());
			}
		}
	}
	Read();
}

void NetSubscriber::OnWrite(const asio::error_code& ec, size_t bytes)
{
	if (ec) {
		HandleError(ec);
		return;
	}else{
		spdlog::get("log")->info("Message delived, wrote {:d} bytes", bytes);
		mMessageStrings.pop_front();
		if (!mMessageStrings.empty()) {
			DoWrite();
		}
	}
}

void NetSubscriber::OnInit(const NetMessage& message)
{
	auto& json = message.GetJsonMessage();
	if (mClientData.is_null()) {
		mID = boost::uuids::string_generator()(std::string(json["ID"]));
		if (mID == boost::uuids::nil_uuid()) {
			//send an error message back
			spdlog::get("log")->error("Invaid subscriber ID from the server");
			//close subscriber... invalid means its over, i think
			DeliverErrorMessage(112, "Invalid subscriber ID from server");
			Shutdown();
			return;
		}
	}
	else {
		if (GetID() != boost::uuids::string_generator()(std::string(json["ID"]))) {
			spdlog::get("log")->error("Initalisation failed, Subscriber ID mismatch, shutting down");
		}
	}
	minitialised = true;
}

void NetSubscriber::OnError(const NetMessage& message)
{
	auto& json = message.GetJsonMessage();
	spdlog::get("log")->error("Server error: {:d}, message {}",
		static_cast<int>(json["Code"]), std::string(json["Message"]));
}

void NetSubscriber::OnPubList(const NetMessage& message)
{
	auto& json = message.GetJsonMessage();
	if (json.is_null()) {
		spdlog::get("log")->error("Invalid Json format from server, PubList Message is invalid");
		DeliverErrorMessage(111, "Invalid Json format message");
		return;
	}
	try{
		auto json_array = json["Publishers"];
		if (json_array.is_null()) {
			spdlog::get("log")->error("No list of publishes sent from the server");
			return;
		}
		//need to find a way to send this to the main thread and display
		for (auto iter = json_array.begin(); iter != json_array.end(); (void)iter++) {
			auto pub = iter.value();
			if (pub.is_null()) {
				spdlog::get("log")->error("Invalid Publisher in PubList");
				continue;
			}
			boost::uuids::uuid PubId = 
				boost::uuids::string_generator()(std::string(pub["Publisher_id"]));
			if (PubId == boost::uuids::nil_uuid()) {
				spdlog::get("log")->error("Publisher with a null id is skiped");
				continue;
			}
			else {
				mPublishers.push_back(PubId);
			}	
		}

	}
	catch (js::json::type_error& error) {
		spdlog::get("log")->error("Invalid Json format from server, PubList Message is invalid");
		DeliverErrorMessage(111, "Invalid Json format message");
		return;
	}
}

void NetSubscriber::OnWelcomSub(const NetMessage& message)
{
	auto& json = message.GetJsonMessage();
	if (json.is_null()) {
		spdlog::get("log")->error("Welcome sub message is null");
		return;
	}
	try {
		mPublisherID = boost::uuids::string_generator()(std::string(json["Pub_id"]));
		mPublisherInfo = json["PubInfo"];
		mPublisherName = json["Name"];
	}
	catch (js::json::type_error& error) {
		spdlog::get("log")->critical("invalid json format in Welcome sub");
	}
}

void NetSubscriber::OnPublisherShutDown(const NetMessage& message)
{
	//UI needs to ne aware that the publisher is offline
	mPublisherName.clear();
	mPublisherInfo = {};
	mPublisherID = boost::uuids::nil_uuid();
}

void NetSubscriber::OnUnsubscribed(const NetMessage& message)
{
	auto& json = message.GetJsonMessage();
	if (json.is_null()) {
		spdlog::get("log")->error("Invalid Json format in Unsubscribe message");
		return;
	}
	if ("UnSubed" == std::string(json["Status"])) {
		//do sub 
		mPublisherID = boost::uuids::nil_uuid();
		mPublisherName.clear();
		mPublisherInfo = {};
		ClearServerPublisherList();
	}
}

void NetSubscriber::RefreashPublisherList()
{
	NetMessage message;
	message.GetJsonMessage() = {
		{"Type", "PubList"}
	};
	Write(message);
}

void NetSubscriber::UnSubscribe()
{
	NetMessage message;
	message.GetJsonMessage() = {
		{"Type", "UnSub"},
		{"Sub_id", GetIDAsString()}
	};
	Write(message);
}

void NetSubscriber::ResendInitWithPubID(const boost::uuids::uuid& publiserID)
{
	NetMessage message;
	message.GetJsonMessage() = {
		{"Type", "Init"},
		{"Sub_id", boost::uuids::to_string(boost::uuids::nil_uuid())},
		{"Name", GetName()},
		{"Publisher_id", boost::uuids::to_string(publiserID)}
	};
	Write(message);
}