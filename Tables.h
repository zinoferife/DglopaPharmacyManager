#pragma once
#include "common.h"
#include <table.h>
//holds all the tables in the system for now

/*--------------------------------------------------------------- Products -------------------------------------------------------------------------*/
class Products : public nl::vector_table<std::uint64_t, //product serial no
	std::string, //product name
	std::uint32_t, // product package size
	std::uint32_t, // product stock count
	std::string, // product price
	std::uint64_t> // category_id 
{
public:
	BEGIN_COL_NAME("Products") 
		COL_NAME("id")
		COL_NAME("name")
		COL_NAME("package_size")
		COL_NAME("stock_count")
		COL_NAME("unit_price")
		COL_NAME("category_id")
	END_COL_NAME()
	IMPLEMENT_GET_COL_NAME()
	enum
	{
		id = 0,
		name,
		package_size,
		stock_count,
		unit_price,
		category_id
	};
};
/*--------------------------------------------------------------- categories -------------------------------------------------------------------------*/

class Categories : public nl::vector_table<std::uint64_t, //category_id
	std::string> //category name
{
public:
	BEGIN_COL_NAME("Categories")
		COL_NAME("id")
		COL_NAME("name")
	END_COL_NAME()
	IMPLEMENT_GET_COL_NAME()
	enum
	{
		id = 0,
		name
	};

};
/*--------------------------------------------------------------- Product details -------------------------------------------------------------------------*/
class ProductDetails : public nl::vector_table < std::uint64_t, //prouct id
	std::string, // product active ingredent
	std::string, // product descriptions
	std::string, // product direction for use
	std::string, // product class
	std::vector<std::string>> // health conditions 
{
	BEGIN_COL_NAME("ProductDetails")
		COL_NAME("product_id")
		COL_NAME("product_active_ingredent")
		COL_NAME("product_description")
		COL_NAME("product_directions_for_use")
		COL_NAME("product_class")
		COL_NAME("product_health_conditions")
	END_COL_NAME()
	IMPLEMENT_GET_COL_NAME()
	enum
	{
		id = 0,
		active_ing,
		description,
		dir_for_use,
		p_class,
		health_conditions
	};
};

/*--------------------------------------------------------------- custormers -------------------------------------------------------------------------*/
class Customers : public nl::vector_table<std::uint64_t,
	std::string, //name
	std::string, //address
	std::string, //email address
	std::string> //phone number
{
public:
	BEGIN_COL_NAME("Customers")
	COL_NAME("id")
	COL_NAME("name")
	COL_NAME("address")
	COL_NAME("email_address")
	COL_NAME("phone_no")
	END_COL_NAME()
	IMPLEMENT_GET_COL_NAME()
	enum
	{
		id = 0,
		name,
		address,
		email_address,
		phone_no
	};
};

/*--------------------------------------------------------------- Sales -------------------------------------------------------------------------*/
class Sales : public nl::vector_table<std::uint64_t, // sale id
	std::uint64_t, //product id
	std::uint64_t, //customer id,
	std::uint64_t, //user id,
	std::uint32_t, //amount
	std::string, //date
	std::string> //price
{
public:
	BEGIN_COL_NAME("Sales")
		COL_NAME("id")
		COL_NAME("product_id")
		COL_NAME("customer_id")
		COL_NAME("user_id")
		COL_NAME("amount")
		COL_NAME("date")
		COL_NAME("price")
	END_COL_NAME()
	IMPLEMENT_GET_COL_NAME()
	enum
	{
		id = 0,
		product_id,
		customer_id,
		user_id,
		amount,
		date,
		price
	};
};

/*--------------------------------------------------------------- Inventories-------------------------------------------------------------------------*/
class Inventories : public nl::vector_table<std::uint64_t, //inventory id
	std::uint64_t, //product_id
	std::string, //date
	std::uint64_t, //invoice way bill no
	std::uint32_t, // quantity in
	std::uint32_t, // quantity out
	std::uint32_t, // balance
	std::uint64_t, //user issued
	std::uint64_t> //checked by
{
public:
	BEGIN_COL_NAME("Inventories")
		COL_NAME("id")
		COL_NAME("product_id")
		COL_NAME("date")
		COL_NAME("invoice_way_bill_no")
		COL_NAME("quantity_in")
		COL_NAME("quantity_out")
		COL_NAME("balance")
		COL_NAME("user_issued")
		COL_NAME("user_checked")
	END_COL_NAME()
	IMPLEMENT_GET_COL_NAME()
	enum
	{
		id = 0,
		product_id,
		date,
		invoice_way_bill_no,
		quantity_in,
		quantity_out,
		balance,
		user_issued,
		user_checked
	};
};
/*--------------------------------------------------------------- Users-------------------------------------------------------------------------*/
class Users : public nl::vector_table<std::uint64_t, //user_id
	std::string, //surname
	std::string, // name
	std::string, // phone number
	std::uint64_t> // password hash
{
public:
	BEGIN_COL_NAME("users")
		COL_NAME("id")
		COL_NAME("surname")
		COL_NAME("name")
		COL_NAME("phone_number")
		COL_NAME("password_hash")
	END_COL_NAME()
	IMPLEMENT_GET_COL_NAME()
	enum
	{
		id = 0,
		surname,
		name,
		phone_number,
		password_hash
	};
};