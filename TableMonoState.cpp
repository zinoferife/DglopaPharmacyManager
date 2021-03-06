#include "common.h"
#include "TableMonoState.h"

void TableMonoState::CreateProductTable()
{
	ProductInstance::instance().as<Products::id>("Serial number");
	ProductInstance::instance().as<Products::name>("Name");
	ProductInstance::instance().as<Products::package_size>("Package size");
	ProductInstance::instance().as<Products::stock_count>("Stock count");
	ProductInstance::instance().as<Products::unit_price>("Unit price");
	ProductInstance::instance().as<Products::category_id>("Category id");
}

void TableMonoState::CreateSalesTable()
{
	SalesInstance::instance().as<Sales::id>("Sale number");
	SalesInstance::instance().as<Sales::product_id>("product number");
	SalesInstance::instance().as<Sales::customer_id>("customer number");
	SalesInstance::instance().as<Sales::user_id>("user number");
	SalesInstance::instance().as<Sales::amount>("amount");
	SalesInstance::instance().as<Sales::date>("date");
	SalesInstance::instance().as<Sales::price>("price");
}

void TableMonoState::CreateCategoryTable()
{
	CategoriesInstance::instance().as<Categories::id>("Category number");
	CategoriesInstance::instance().as<Categories::name>("Category name");
}

void TableMonoState::CreateCustomersTable()
{
	CustomersInstance::instance().as<Customers::id>("Customer number");
	CustomersInstance::instance().as<Customers::name>("Customer name");
	CustomersInstance::instance().as<Customers::address>("Customer address");
	CustomersInstance::instance().as<Customers::email_address>("Customer email address");
	CustomersInstance::instance().as<Customers::phone_no>("Customer phone number");
}

void TableMonoState::CreateInventoriesTable()
{
	InventoryInstance::instance().as<Inventories::id>("Inventory id");
	InventoryInstance::instance().as<Inventories::product_id>("Product id");
	InventoryInstance::instance().as<Inventories::date_issued>("Date issued");
	InventoryInstance::instance().as<Inventories::date_expiry>("Date expired");
	InventoryInstance::instance().as<Inventories::invoice_way_bill_no>("Invoice way bill No");
	InventoryInstance::instance().as<Inventories::quantity_in>("Quantity In");
	InventoryInstance::instance().as<Inventories::quantity_out>("Quantity Out");
	InventoryInstance::instance().as<Inventories::balance>("Balance");
	InventoryInstance::instance().as<Inventories::user_issued>("User issued");
	InventoryInstance::instance().as<Inventories::user_checked>("User checked");

}

void TableMonoState::CreateProductDetailsTable()
{
	ProductDetailsInstance::instance().as<ProductDetails::id>("Product details number");
	ProductDetailsInstance::instance().as<ProductDetails::active_ing>("Product active ingredient");
	ProductDetailsInstance::instance().as<ProductDetails::description>("Product descriptions");
	ProductDetailsInstance::instance().as<ProductDetails::dir_for_use>("Product Direction for use");
	ProductDetailsInstance::instance().as<ProductDetails::p_class>("Product class");
	ProductDetailsInstance::instance().as<ProductDetails::health_conditions>("Product associated health conditions");
}

void TableMonoState::CreateUsersTable()
{
	UsersInstance::instance().as<Users::id>("ID");
	UsersInstance::instance().as<Users::surname>("Last name");
	UsersInstance::instance().as<Users::name>("Name");
	UsersInstance::instance().as<Users::phone_number>("Phone number");
	UsersInstance::instance().as<Users::username>("Username");
}

void TableMonoState::CreatePrescriptionsTable()
{
	PrescriptionInstance::instance().as<Prescriptions::id>("Prescription number");
	PrescriptionInstance::instance().as<Prescriptions::uuid>("Prescription ID");
	PrescriptionInstance::instance().as<Prescriptions::medication>("Medication");
	PrescriptionInstance::instance().as<Prescriptions::patient_name>("Patient name");
	PrescriptionInstance::instance().as<Prescriptions::patient_address>("Patient address");
	PrescriptionInstance::instance().as<Prescriptions::prescriber_name>("Prescribers name");
	PrescriptionInstance::instance().as<Prescriptions::prescriber_health_center>("Prescribers health center");
	PrescriptionInstance::instance().as<Prescriptions::prescriber_licence_number>("Prescribers licenece number");
}

void TableMonoState::CreateAllTables()
{
	CreateProductTable();
	CreateCustomersTable();
	CreateProductDetailsTable();
	CreatePrescriptionsTable();
	CreateInventoriesTable();
	CreateSalesTable();
	CreateUsersTable();
}
