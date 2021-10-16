#pragma once
#include "common.h"
#include <table.h>
#include <nl_time.h>
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
	std::string> // health conditions 
{
public:
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
	nl::uuid,
	std::string, //name
	std::string, //address
	std::string, //email address
	std::string> //phone number
{
public:
	BEGIN_COL_NAME("Customers")
	COL_NAME("id")
	COL_NAME("uuid")
	COL_NAME("name")
	COL_NAME("address")
	COL_NAME("email_address")
	COL_NAME("phone_no")
	END_COL_NAME()
	IMPLEMENT_GET_COL_NAME()
	enum
	{
		id = 0,
		uuid,
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
	nl::date_time_t, //date
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
class Inventories : public nl::vector_table < std::uint64_t, //inventory id
	std::uint64_t, //product_id
	nl::date_time_t , //date
	nl::date_time_t , //date expiry
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
		COL_NAME("date_issued")
		COL_NAME("date_expiry")
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
		date_issued,
		date_expiry,
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
	std::uint32_t, //User level
	std::string,
	nl::uuid, //user_uuid
	nl::blob_t> // password hash
{
public:
	BEGIN_COL_NAME("users")
		COL_NAME("id")
		COL_NAME("surname")
		COL_NAME("name")
		COL_NAME("phone_number")
		COL_NAME("user_level")
		COL_NAME("uuid")
		COL_NAME("password_hash")
	END_COL_NAME()
	IMPLEMENT_GET_COL_NAME()
	enum
	{
		id = 0,
		surname,
		name,
		phone_number,
		user_level,
		username,
		uuid,
		password_hash
	};
};
/*---------------------------------------------------------------Invoice-------------------------------------------------------------------------*/
class Invoice : public nl::vector_table < std::uint64_t, //invoice id
	std::uint64_t, //invoice number
	nl::date_time_t, //invoice date 
	std::string, //supplier name
	std::string, //suppiler address
	std::string, //recipient name
	std::string> //recipient address
{
public:
	BEGIN_COL_NAME("invoice")
	COL_NAME("id")
	COL_NAME("invoice_number")
	COL_NAME("invoice_date")
	COL_NAME("supplier_name")
	COL_NAME("supplier_address")
	COL_NAME("recipient_name")
	COL_NAME("recipient_address")
	END_COL_NAME()
	IMPLEMENT_GET_COL_NAME()
	enum
	{
		id = 0,
		invoice_number,
		invoice_date,
		supplier_name,
		supplier_address,
		recipient_name,
		recipient_address
	};
};

/*---------------------------------------------------------------track list-------------------------------------------------------------------------*/
class TrackList : public nl::vector_table<std::uint64_t> //productId
{
public:
	BEGIN_COL_NAME("track_list")
	COL_NAME("tracked_product")
	END_COL_NAME()
	IMPLEMENT_GET_COL_NAME()
	enum
	{
		tracked_product = 0
	};

};
/*---------------------------------------------------------------Prescriptions-------------------------------------------------------------------------*/
class Prescriptions : public nl::vector_table<std::uint64_t, //prescription id
	nl::uuid, //universal identifier
	nl::date_time_t, //date issued
	std::string, //medication
	std::string, //dosage form
	std::string, //strength
	std::uint32_t, //repeat count status (if 0, its dispenced all the repeats, else reduce by one everytime it is dispenced)
	std::string, //instructions to take
	std::uint32_t, //supply (count of dosage forms e.g 28 tablets)
	std::string, //patient name
	std::string, //patient address
	std::string, //prescriber name e.g (Dr vivienne ferife)
	std::string, //prescribers health center
	std::string> // prescriber licence number
{
public:
	BEGIN_COL_NAME("prescriptions")
		COL_NAME("id")
		COL_NAME("uuid")
		COL_NAME("date_issued")
		COL_NAME("medication")
		COL_NAME("dosage_form")
		COL_NAME("strength")
		COL_NAME("repeat_count_status")
		COL_NAME("instructions_to_take")
		COL_NAME("supply")
		COL_NAME("patient_name")
		COL_NAME("patient_address")
		COL_NAME("prescriber_name")
		COL_NAME("prescriber_health_center")
		COL_NAME("presriber_licence_number")
	END_COL_NAME()
	IMPLEMENT_GET_COL_NAME()
	enum
	{
		id = 0,
		uuid,
		date_issued,
		medication,
		dosage_form,
		strength,
		repeat_count_status,
		instructions_to_take,
		supply,
		patient_name,
		patient_address,
		prescriber_name,
		prescriber_health_center,
		prescriber_licence_number
	};
};

/*---------------------------------------------------------------Pharmacy credentials-------------------------------------------------------------------------*/
class PharmacyCredentials : nl::vector_table < std::uint64_t,
	nl::uuid, //universal user id, issued by Afrobug on download
	nl::date_time_t,
	std::string, //pharmacy name
	std::string, //address line
	std::string, //address city,
	std::string, //state 
	std::string> //country
{
public:
	BEGIN_COL_NAME("pharmacy_credentials")
		COL_NAME("id")
		COL_NAME("date_created")
		COL_NAME("name")
		COL_NAME("address_line")
		COL_NAME("address_city")
		COL_NAME("address_state")
		COL_NAME("address_country")
	END_COL_NAME()
	IMPLEMENT_GET_COL_NAME()
	enum 
	{
		id = 0,
		date_created,
		name,
		address_line,
		address_city,
		address_state,
		address_country
	};
};
