#pragma once
#include <Singleton.h>
#include "Tables.h"
#include <Database.h>

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
