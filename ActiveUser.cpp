#include "common.h"
#include "ActiveUser.h"

ActiveUser::ActiveUser(Users& table)
	: mUserTable(table), mSignedIn{false}{
	mUserTable.sink<nl::notifications::evt>().add_listener<ActiveUser, &ActiveUser::OnSignOut>(this);
}

ActiveUser::~ActiveUser()
{
	mUserTable.sink<nl::notifications::evt>().remove_listener<ActiveUser, & ActiveUser::OnSignOut>(this);
}

bool ActiveUser::Authenticate(const std::string& password, std::string& username)
{
	//case insenti
	std::transform(username.begin(), username.end(), username.begin(), [&](char& c) -> char {
		return std::tolower(c);
	});
	auto end = std::remove(username.begin(), username.end(), ' ');
	username.erase(end, username.end());
	auto iter = mUserTable.find_on<Users::username>(username);
	//do password authentication here??
	if (iter == mUserTable.end()) return false;
	mActiveUser = *iter;
	mSignedIn = true;
	return true;
}

void ActiveUser::OnEditProfile(const Users::table_t& table, const Users::notification_data& data)
{
	if (data.row_iterator != table.end()) mActiveUser = *(data.row_iterator);
}

void ActiveUser::OnSignOut(const Users::table_t& table, const Users::notification_data& data)
{
	if(data.event_type == 0) mSignedIn = false;
}

const Users::row_t& ActiveUser::GetActiveUser() const
{
	return mActiveUser;
}
