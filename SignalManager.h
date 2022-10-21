#pragma once
#include <boost/signals2/signal.hpp>
#include <type_traits>
#include <functional>

//signals slot on a change to the underlining table in the database
//this is different from the notification system in table_relation, that signals changes to the table_relation structure
//might remove the notification per table_relation to this notification per table in the database

template<typename Table, typename PrimaryKey = std::uint64_t>
class DatabaseSignalManager {
public:
	//CURD + insert lol
	enum class DSM_FUNC{
		DSM_UPDATE,
		DSM_CREATE,
		DSM_INSERT,
		DSN_READ,
		DSM_DELETE
	};

	using Table_t = Table;
	using PrimaryKey_t = PrimaryKey;
	using Signal_t = boost::signals2::signal<void(PrimaryKey_t,DSM_FUNC, size_t)>;
	using Function_t = typename Signal_t::slot_type; //std::function<void(PrimaryKey_t, DSM_FUNC, size_t)>;
	//calls the slot
	static void Signal(PrimaryKey_t key, DSM_FUNC func, size_t col) {
		mSignalObject(key, func, col);
	}
	
	static boost::signals2::connection Connect(Function_t&& function) {
		return mSignalObject.connect(function);
	}

	static boost::signals2::connection Connect(int group, Function_t&& function){
		return mSignalObject.connect(group, function);
	}

	static void Disconnect(Function_t&& func){
		mSignalObject.disconnect(func);
	}

	static void Disconnect(const boost::signals2::connection& conn){
		mSignalObject.disconnect(conn);
	}

private:
	static Signal_t mSignalObject;

};

template<typename Table, typename PrimaryKey>
typename DatabaseSignalManager<Table, PrimaryKey>::Signal_t DatabaseSignalManager<Table, PrimaryKey>::mSignalObject{};