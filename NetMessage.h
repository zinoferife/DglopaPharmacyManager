#pragma once
#include <asio.hpp>
#include <nlohmann/json.hpp>
#include <string>


namespace js = nlohmann;
//all messages passed in is just a json object
class NetMessage
{
public:
	NetMessage() {}
	inline asio::streambuf& GetStreamBuffer() { return mStreamBuffer; }
	inline js::json& GetJsonMessage() { return mJsonMessage; }
	inline const asio::streambuf& GetStreamBuffer() const { return mStreamBuffer; }
	inline const js::json& GetJsonMessage() const { return mJsonMessage; }
	bool ParseStreamBuffer();
	const std::string GetJsonAsString() const;
	void Clear();
	bool Empty();
	//value symantics
	NetMessage& operator=(const NetMessage& message);
	NetMessage(const NetMessage&& message) noexcept;
	NetMessage(const NetMessage& message);
private:
	//read until \r\n 
	asio::streambuf mStreamBuffer;
	js::json mJsonMessage;
};
