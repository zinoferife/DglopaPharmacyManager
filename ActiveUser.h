#pragma once
#include "Tables.h"
#include "Instances.h"
#include <algorithm>
#include <string>

class ActiveUser 
{
public:
	ActiveUser(Users& table);
	~ActiveUser();
	bool Authenticate(const std::string& password, std::string& username);

	//edit profile


	void OnEditProfile(const Users::table_t& table, const Users::notification_data& data);
	void OnSignOut(const Users::table_t& table,  const Users::notification_data& data);
	const Users::row_t& GetActiveUser() const;

	inline constexpr bool IsSignedIn() const { return mSignedIn; }
private:
	bool mSignedIn;
	Users& mUserTable;
	Users::row_t mActiveUser;

};

