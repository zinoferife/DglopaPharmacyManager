#pragma once
#include "Instances.h"
#include <tuple_loop.h>


#include <filesystem>
#include <vector>
#include <unordered_map>
#include <spdlog/spdlog.h>

//load, add, remove, update, delete select -> names of queries that the manager expects
//database mangers should not respond to notifications from tables 
//the should expose functions that the views/models call to update their tables when they recieve notifications


template<typename table>
class DatabaseManager
{
	static_assert(nl::detail::is_relation_v<table>, "Database manager only manages relations");
	template<size_t I>
	class loop
	{
	public:
		inline static nl::query& create_query(nl::query& q) {
			nl::query& q_ = loop<I - 1>::create_query(q);
			if constexpr (table::id == I)
				q_.column<typename table::template elem_t<I>, nl::unique>(table::template get_col_name<I>());
			else {
				q_.column<typename table::template elem_t<I>, nl::no_policy>(table::template get_col_name<I>());
			}
			return q_;
		}
	};


	template<>
	class loop<0>
	{
	public:
		inline static nl::query& create_query(nl::query& q){
			if constexpr (table::id == 0){
				q.column<typename table::template elem_t<0>, nl::unique>(table::template get_col_name<0>());
			}
			else{
				q.column<typename table::template elem_t<0>, nl::no_policy>(table::template get_col_name<0>());
			}
			return q;
		}
	};

public:
	DatabaseManager(table& table, nl::database& database): mTable(table), mDatabase(database){
		RegisterNotifications();
	}
	~DatabaseManager()
	{
		UnregisterNotifications();
	}

	bool AddQuery(std::string&& query_name, const nl::query& query)
	{
		auto statement = mDatabase.prepare_query(query);
		if (statement == nl::database::BADSTMT) return false;
		auto [iter, inserted] = m_statements.insert({ std::forward<std::string>(query_name), statement });
		return inserted;
	}

	bool ChangeQuery(const std::string& query_name, const nl::query& query)
	{
		auto iter = m_statements.find(query_name);
		if (iter == m_statements.end()) return false;
		nl::database::statement_index statement = mDatabase.prepare_query(query);
		if (statement == nl::database::BADSTMT) return false;
		iter->second = statement;
		return true;
	}

	bool RemoveQuery(const std::string& query_name)
	{
		auto iter = m_statements.find(query_name);
		if (iter == m_statements.end()) return false;
		mDatabase.remove_statement(iter->second);
		m_statements.erase(iter);
		return true;
	}

//database operations
	void CreateTable(){
		auto create_statement = m_statements.find("create");
		if (create_statement == m_statements.end())
		{
			nl::query q;
			q.create_table(table::table_name);
			loop<table::column_count - 1>::create_query(q);
			q.end_col();
			spdlog::get("log")->info("{}", q.get_query());
			auto statement = mDatabase.prepare_query(q);
			if (statement == nl::database::BADSTMT) {
				spdlog::get("log")->error("Invalid query {}", q.get_query());
				return;
			}
			auto [iter, inserted] = m_statements.insert({ "create", statement });
			if (!inserted){
				spdlog::get("log")->error("Could not insert create statement in cache");
				return;
			}
			create_statement = iter;
		}

		if (!mDatabase.exec_once(create_statement->second)) {
				spdlog::get("log")->error("{}", mDatabase.get_error_msg());
		}
	}
	
	void DropTable(){
		nl::query q;
		q.drop_table(table::table_name);
		auto statement = mDatabase.prepare_query(q);
		if (statement == nl::database::BADSTMT) {
			spdlog::get("log")->error("Invalid query {}", q.get_query());
			return;
		}
		if (!mDatabase.exec_once(statement)) {
			spdlog::get("log")->error("{}: {}", q.get_query(), mDatabase.get_error_msg());
		}
	}

	void LoadTable()
	{
		auto iter = m_statements.find("load");
		if (iter == m_statements.end())
		{
			//make default
			nl::query q;
			q.select("*").from(table::table_name);
			auto statement = mDatabase.prepare_query(q);
			spdlog::get("log")->info("{}", q.get_query());
			if (statement == nl::database::BADSTMT){
				spdlog::get("log")->error(" invalid query {}", q.get_query());
				return;
			}
			auto [iter_insert, inserted] = m_statements.insert({ "load", statement });
			if (!inserted){
				spdlog::get("log")->error("Failed to insert query in cache");
				return;
			}
			iter = iter_insert;
		}
		static_cast<typename table::relation_t&>(mTable) = std::move(mDatabase.retrive<typename table::relation_t>(iter->second));
		if (!mDatabase.get_error_msg().empty()) spdlog::get("log")->error("{}", mDatabase.get_error_msg());

		typename table::notification_data data;
		data.count = mTable.size();
		mTable.notify<nl::notifications::load>(data);
	}

	void RegisterNotifications(){
		mTable.sink<nl::notifications::add>().add_listener<DatabaseManager, & DatabaseManager::OnAddNotification>(this);
	}
	void UnregisterNotifications(){
		mTable.sink<nl::notifications::add>().remove_listener<DatabaseManager, & DatabaseManager::OnAddNotification>(this);
	}

	template<typename... column_names, typename... values>
	void UpdateTable(typename table::template elem_t<table::id> id, const std::tuple<values...>& data, const column_names& ... names){
		using names_t = std::tuple<column_names...>;
		using first_name_t = std::tuple_element_t<0, names_t>;
		static_assert(std::is_convertible_v<first_name_t, std::string> && std::conjunction_v<std::is_same<first_name_t, column_names>...>,
			"column names should have a string type and they all should have the same type");
			
		nl::query q;
		q.update(table::table_name);
		q.set(names...);
		q.where(fmt::format("{} = \'{:d}\'", table:: template get_col_name<table::id>(), id));
		spdlog::get("log")->info("update query {}", q.get_query());
		auto statement = mDatabase.prepare_query(q);
		if (statement == nl::database::BADSTMT){
			spdlog::get("log")->error("invalid query: {}", q.get_query());
			return;
		}
		bool ret = mDatabase.update(statement, data, names...);
		if (!ret){
			spdlog::get("log")->error("Could not update {}, error {}", table::table_name, mDatabase.get_error_msg());
		}
		mDatabase.remove_statement(statement);
	}
	template<typename Value>
	void InsertTable(const Value& tab)
	{
		static_assert(std::disjunction_v<nl::detail::is_relation<Value>, nl::detail::is_relation_row<Value, table>>, "Not a valid Value type");
		auto insert_iter = m_statements.find("add");
		if (insert_iter == m_statements.end()){
			nl::query q;
			q.insert(table::table_name);
			values(std::make_index_sequence<table::column_count>{}, q);
			auto statement = mDatabase.prepare_query(q);
			if (statement == nl::database::BADSTMT) {
				spdlog::get("log")->error("invalid query {}", q.get_query());
				return;
			}
			auto [iter, inserted] = m_statements.insert({ "add", statement });
			if (!inserted) {
				spdlog::get("log")->error("Could not insert into m_statement cache");
				return;
			}
			insert_iter = iter;
		}
		bool ret = insert(nl::select_all<table>{}, mDatabase, tab, insert_iter->second);
		if (!ret){
			spdlog::get("log")->error("insert error, {}", mDatabase.get_error_msg());
			return;
		}
		else {
			spdlog::get("log")->info("Successfully inserted into the table {}", table::table_name);
		}
	}

//table events
public:
	template<size_t... I>
	void values(std::index_sequence<I...>, nl::query& q){
		q.values(table:: template get_col_name<I>()...);
	}

	template<typename Value, size_t... I>
	bool insert(std::index_sequence<I...>, nl::database& database, const Value& value,  nl::database::statement_index index)
	{
		return database.insert<table>(index, value, table:: template get_col_name<I>()...);
	}

	template<size_t... I>
	void select(std::index_sequence<I...>, nl::query& q)
	{
		q.select(table:: template get_col_name<I>()...);
	}

	template<typename Value, size_t... I>
	void update_query(std::index_sequence<I...>, nl::query& q, const Value& col_names)
	{
		q.update(table::table_name);
		q.set(col_names[I].get()...);
	}

	template<typename row_t, typename Value, size_t... I>
	bool update(std::index_sequence<I...>, nl::database::statement_index index,  row_t& row, const Value& col_names)
	{
		return mDatabase.update(index, row, col_names[I].get()...);
	}

	void OnAddNotification(const typename table::table_t& table, const typename table::notification_data& data)
	{
		auto add_statement_iter = m_statements.find("add");
		if (add_statement_iter == m_statements.end()){
			nl::query q;
			q.insert(table::table_name);
			values(std::make_index_sequence<table::column_count>{}, q);
			auto statement = mDatabase.prepare_query(q);
			if (statement == nl::database::BADSTMT){
				spdlog::get("log")->error("invalid query {}", q.get_query());
				return;
			}
			auto [iter, inserted] = m_statements.insert({ "add", statement });
			if (!inserted) {
				spdlog::get("log")->error("Could not insert into m_statement cache");
				return;
			}
			add_statement_iter = iter;
		}
		bool ret = insert(std::make_index_sequence<table::column_count>{}, mDatabase, (*data.row_iterator), add_statement_iter->second);
		if (!ret){
			spdlog::get("log")->error("Cannot insert into table {}, {}", table::table_name, mDatabase.get_error_msg());
		}
		else{
			spdlog::get("log")->info("Successfully inserted into the table {}", table::table_name);
		}
	}

protected:
	std::unordered_map<std::string,nl::database::statement_index > m_statements;
	table& mTable;
	nl::database& mDatabase;
};

