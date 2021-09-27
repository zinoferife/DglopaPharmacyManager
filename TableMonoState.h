#pragma once
//monostate class for the creations of tables, associating table to names and loading
#include "common.h"
#include "Instances.h"
#include "Tables.h"

class TableMonoState
{
public:
	typedef nl::database::statement_index statement;
	static void CreateProductTable();
	static void CreateSalesTable();
	static void CreateCategoryTable();
	static void CreateCustomersTable();
	static void CreateInventoriesTable();
	static void CreateProductDetailsTable();
	static void CreateUsersTable();
	static void CreatePrescriptionsTable();
	static void CreateAllTables();
	//
};

