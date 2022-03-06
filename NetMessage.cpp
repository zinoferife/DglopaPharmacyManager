#include "common.h"
#include "NetMessage.h"


bool NetMessage::ParseStreamBuffer()
{
	std::stringstream ss;
	ss << &mStreamBuffer;
	auto data = ss.str();
	if (data.empty()) {
		return false;
	}
	//remove delimeter
	auto pos = data.find_first_of("\r\n");
	if (pos == std::string::npos) {
		return false;
	}
	auto json_string = data.substr(0, pos);
	std::stringstream ss2(json_string, std::ios::in);
	ss2 >> mJsonMessage;
	return true;
}

const std::string NetMessage::GetJsonAsString() const
{
	return mJsonMessage.dump();
}

void NetMessage::Clear()
{
	mJsonMessage = {};
}

bool NetMessage::Empty()
{
	return (mStreamBuffer.size() == 0);
}

NetMessage& NetMessage::operator=(const NetMessage& message)
{
	// TODO: insert return statement here
	this->mJsonMessage = message.mJsonMessage;
	return (*this);
}

NetMessage::NetMessage(const NetMessage&& message) noexcept
{
	this->mJsonMessage = std::move(message.mJsonMessage);
}

NetMessage::NetMessage(const NetMessage& message)
{
	this->mJsonMessage = message.mJsonMessage;
}
