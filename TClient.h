#pragma once


#pragma once
#include <asio.hpp>
#include <spdlog/spdlog.h>
#include <memory>
#include "NetMessage.h"

#include <deque>
#include <unordered_map>
#include <functional>
#include <shared_mutex>
#include <fstream>


#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>



class TClient : public std::enable_shared_from_this<TClient>
	{
	public:
		TClient(asio::io_context& contex);
		virtual ~TClient();
		void Connect(const asio::ip::tcp::endpoint& endpoint);
		void Connect(const asio::ip::tcp::resolver::results_type& endpoints);
		bool SyncConnect(const asio::ip::tcp::endpoint& endpoint);
		void Open();
		bool IsOpen();
		void Cancel();
		void Shutdown();
		void GenerateID();
		virtual void Read();
		virtual void Write(const NetMessage& message);
		virtual void Init() = 0;

		inline const std::string& GetName() const { return mName; }
		inline const boost::uuids::uuid& GetID() const { return mID; }
		inline std::string GetIDAsString() const { return boost::uuids::to_string(mID); }
		inline void SetName(const std::string& name) { mName = name; }
		inline void SetId(const boost::uuids::uuid& id) { mID = id; }
		inline constexpr bool IsInitialised() const { return minitialised; }
		static void InitMessageHandlers();

		void SetClientData(const js::json& ClientData);
		void DeliverErrorMessage(size_t code, const std::string& message);
		void AppendToReadBuffer(const NetMessage& message);
		void ReadClientDataFromFile(const std::filesystem::path& file_path);
		void SaveClientDataToFile(const std::filesystem::path& file_path);

		//client data would be different for the publisher and the subscriber
		inline js::json& GetClientData() { return mClientData; }
		inline const js::json& GetClientData() const { return mClientData; }
		//statics 
	public:
		static void AddMessageHandler(const std::string& MesType, const std::function<void(const NetMessage&, std::shared_ptr<TClient> ptr)>& function);

	protected:
		virtual void OnConnected(const asio::error_code& ec) = 0;
		virtual void OnRead(const asio::error_code& ec, size_t bytes) = 0;
		virtual void OnWrite(const asio::error_code& ec, size_t bytes) = 0;
		virtual void OnInit(const NetMessage& message) = 0;
		virtual void OnError(const NetMessage& message) = 0;
	protected:
		void DoWrite();
		void DoRead();

	protected:
		boost::uuids::uuid mID;
		std::string mName;
		bool minitialised;
		asio::io_context& mContext;
		std::shared_ptr<asio::ip::tcp::socket> mSocket;
		NetMessage mReadMessage;
		std::deque<std::string> mMessageStrings;

		//client data from the server
		js::json mClientData;
	public:
		std::shared_timed_mutex mReadBufferMutex;
		std::deque<NetMessage> mReadBuffer;



	protected:
		static std::unordered_map<std::string, std::function<void(const NetMessage&, std::shared_ptr<TClient> ptr)>> mMessageHandlers;
};

typedef std::shared_ptr<TClient> TClientPtr;

