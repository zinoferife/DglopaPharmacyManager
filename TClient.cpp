#include "common.h"
#include "TClient.h"

//static message handlers
std::unordered_map<std::string, std::function<void(const NetMessage&, std::shared_ptr<TClient> ptr)>> TClient::mMessageHandlers{};


TClient::TClient(asio::io_context& contex)
	: mContext(contex), minitialised(false) {
	//create a tcp socket for the client
	mSocket = std::make_shared<asio::ip::tcp::socket>(contex);
}

TClient::~TClient()
{
	mSocket->close();
}

void TClient::Connect(const asio::ip::tcp::endpoint& endpoint)
{
	asio::error_code ec;
	mSocket->open(endpoint.protocol(), ec);
	if (!ec) {
		mSocket->async_connect(endpoint, [this](const asio::error_code& ec) {
			OnConnected(ec);
			});
	}
	else {
		spdlog::get("log")->error("Cannot open client socket, {}: {:d}", ec.message(), ec.value());
	}
}

void TClient::Connect(const asio::ip::tcp::resolver::results_type& endpoints)
{
	asio::async_connect(*mSocket, endpoints, [this](const asio::error_code& ec, asio::ip::tcp::endpoint) {
		OnConnected(ec);
	});
}

bool TClient::SyncConnect(const asio::ip::tcp::endpoint& endpoint)
{
	asio::error_code ec;
	mSocket->connect(endpoint, ec);
	OnConnected(ec);
	return (!(bool)ec);
}

void TClient::Open()
{
	asio::error_code ec;
	mSocket->open(asio::ip::tcp::v4(), ec);
	if (ec) {
		spdlog::get("log")->error("Could not open network socket.");
	}
}

bool TClient::IsOpen()
{
	return mSocket->is_open();
}

void TClient::Cancel()
{
	asio::error_code ec;
	mSocket->cancel(ec);
	if (ec) {
		spdlog::get("log")->error("Cannot cancel operation, {}: {:d}", ec.message(), ec.value());
	}
}

void TClient::Shutdown()
{
	mSocket->cancel();
	mSocket->shutdown(asio::ip::tcp::socket::shutdown_both);
	mSocket->close();
}

void TClient::GenerateID()
{
	mID = boost::uuids::random_generator_mt19937()();
}

void TClient::Read()
{
	DoRead();
}

void TClient::Write(const NetMessage& message)
{
	std::string data = message.GetJsonAsString();
	if (data.empty()) return;
	data += "\r\n";
	bool write_in_progress = !mMessageStrings.empty();
	mMessageStrings.push_back(data);
	if (!write_in_progress) {
		DoWrite();
	}
}

void TClient::AddMessageHandler(const std::string& MesType, const std::function<void(const NetMessage&, std::shared_ptr<TClient> ptr)>& function)
{
	auto [iter, inserted] = mMessageHandlers.insert({ MesType, function });
	if (!inserted && iter != mMessageHandlers.end()) {
		//assume we are changing the funtion;
		iter->second = function;
	}
}

void TClient::InitMessageHandlers()
{
	AddMessageHandler("Init", [](const NetMessage& message, std::shared_ptr<TClient> ptr) {
		ptr->OnInit(message);
	});

	AddMessageHandler("Error", [](const NetMessage& message, std::shared_ptr<TClient> ptr) {
		ptr->OnError(message);
	});
}

void TClient::SetClientData(const js::json& ClientData)
{
	if (ClientData.is_null()) return;
	mClientData = ClientData;
}

void TClient::DeliverErrorMessage(size_t code, const std::string& message)
{
	NetMessage Mess;
	Mess.GetJsonMessage() = {
		{"Type", "Error"},
		{"Code", code},
		{"Message", message}
	};
	Write(Mess);
}

void TClient::AppendToReadBuffer(const NetMessage& message)
{
	std::unique_lock<std::shared_timed_mutex> lock(mReadBufferMutex);
	mReadBuffer.emplace_back(message);
}

void TClient::ReadClientDataFromFile(const std::filesystem::path& file_path)
{
	std::fstream in(file_path, std::ios::in);
	if (!in.is_open()) {
		spdlog::error("Could not open file for reading, path: {}", file_path.string());
		return;
	}
	in >> mClientData;
	if (mClientData.is_null()) {
		spdlog::error("Invalid Client data in {} file", file_path.string());
		return;
	}
	spdlog::get("log")->info("Read client data from {}", file_path.string());
}

void TClient::SaveClientDataToFile(const std::filesystem::path& file_path)
{
	std::fstream out(file_path, std::ios::out);
	if (!out.is_open()) {
		spdlog::error("Cannot open file for writing, path: {}", file_path.string());
		return;
	}
	out << mClientData;
	spdlog::get("log")->info("Client {} data saved to file", GetIDAsString());
}

void TClient::DoWrite()
{
	asio::async_write(*mSocket, asio::buffer(mMessageStrings.front().data(), mMessageStrings.front().length()),
		[this](const asio::error_code& ec, size_t bytes) {
			OnWrite(ec, bytes);
		});
}

void TClient::DoRead()
{
	asio::async_read_until(*mSocket, mReadMessage.GetStreamBuffer(), "\r\n", [this](const asio::error_code& ec, size_t bytes) {
		OnRead(ec, bytes);
	});
}
