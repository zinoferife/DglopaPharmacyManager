#pragma once
#include <Singleton.h>
#include "Tables.h"
#include <Database.h>

typedef nl::singleton_holder<Products> ProductInstance;
typedef nl::singleton_holder<Categories> CategoriesInstance;
typedef nl::singleton_holder<Sales> SalesInstance;
typedef nl::singleton_holder<nl::database_connection> DatabaseInstance;
