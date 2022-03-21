#pragma once
#include "TClient.h"
class NetSubscriber : public TClient
{
public:
	NetSubscriber(asio::io_context& context);
	virtual ~NetSubscriber();
	void HandleError(const asio::error_code& ec);
	virtual void Init() override;
	static void InitMessageHandlers();
protected:
	virtual void OnConnected(const asio::error_code& ec) override;
	virtual void OnRead(const asio::error_code& ec, size_t bytes) override;
	virtual void OnWrite(const asio::error_code& ec, size_t bytes) override;
	virtual void OnInit(const NetMessage& message) override;
	virtual void OnError(const NetMessage& message) override;
public:
	void OnPubList(const NetMessage& message);
	void OnWelcomSub(const NetMessage& message);
	void OnPublisherShutDown(const NetMessage& message);
	void OnUnsubscribed(const NetMessage& message);

	//server list of publishers functions
	inline bool IsServerPubliserListPresent() const { return !mPublishers.empty(); }
	inline const std::vector<boost::uuids::uuid>& GetServerPublisherList() const { return mPublishers; }
	inline void ClearServerPublisherList() { mPublishers.clear(); }
	void RefreashPublisherList();
	
	//publisher subscribed to functions;
	inline const boost::uuids::uuid& GetPublisherID() const noexcept { return mPublisherID; }
	inline const std::string& GetPublisherName() const noexcept { return mPublisherName; }
	inline const js::json& GetPublisherInfo() const noexcept { return mPublisherInfo; }
	inline std::string GetPublishersIDAsString() const { return boost::uuids::to_string(mPublisherID); }
	//
	void UnSubscribe();
	inline bool IsSubscribed() const noexcept { return (mPublisherID == boost::uuids::nil_uuid()); }
private:
	//tempoary list from the server
	std::vector<boost::uuids::uuid> mPublishers;	
	void ResendInitWithPubID(const boost::uuids::uuid& publiserID);
	boost::uuids::uuid mPublisherID;
	std::string mPublisherName;
	js::json mPublisherInfo;
};

typedef std::shared_ptr<NetSubscriber> NetSubscriberPtr;