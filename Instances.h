#pragma once
#include <Singleton.h>
#include "Tables.h"
#include <Database.h>

#include <random>
#include <functional>


typedef nl::singleton_holder<Products> ProductInstance;
typedef nl::singleton_holder<Categories> CategoriesInstance;
typedef nl::singleton_holder<Sales> SalesInstance;
typedef nl::singleton_holder<Customers> CustomersInstance;
typedef nl::singleton_holder<Inventories> InventoryInstance;
typedef nl::singleton_holder<ProductDetails> ProductDetailsInstance;
typedef nl::singleton_holder<Users> UsersInstance;
typedef nl::singleton_holder<Invoice> InvoiceInstance;
typedef nl::singleton_holder<Prescriptions> PrescriptionInstance;
typedef nl::singleton_holder<nl::database> DatabaseInstance;


//generates random Ids between one millon and 10 millon, They do not need to be universally unique
//just unique inthe context of the table that the ids in

template<typename T = std::uint64_t, std::enable_if_t<std::is_integral_v<T>, int> = 0>
int GenRandomId()
{
	static std::mt19937_64 engine(std::random_device{}());
	static std::uniform_int_distribution<T> dist(1000000, 100000000);
	static auto random = std::bind(dist, engine);

	return random();
}