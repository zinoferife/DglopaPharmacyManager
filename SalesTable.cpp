#include "common.h"
#include "SalesTable.h"

SalesTable::~SalesTable()
{
}

int SalesTable::GetNumberRows()
{
	return mSalesTable.size();
}

int SalesTable::GetNumberCols()
{
	return Sales::column_count;
}

bool SalesTable::IsEmptyCell(int row, int col)
{
	if (mSalesTable.empty() && mSalesTable.size() <= row) {
		return false;
	}
	auto value = mSalesTable.get_as_string(row, col);
	if (value.empty()) {
		return false;
	}
	return true;
}

wxString SalesTable::GetValue(int row, int col)
{
	auto value = mSalesTable.get_as_string(row, col);
	return wxString(value);
}

wxString SalesTable::GetTypeName(int row, int col)
{
	(void)row;
	auto value = mSalesTable.get_column_type_name(col);
	return wxString(value);
}

bool SalesTable::CanGetValueAs(int row, int col, const wxString& TypeName)
{
	if (mSalesTable.empty() && mSalesTable.size() <= row) {
		return false;
	}
	else if (TypeName.ToStdString() == mSalesTable.get_column_type_name(col)){
		return true;
	}
	else {
		return false;
	}

}

bool SalesTable::CanSetValueAs(int row, int col, const wxString& TypeName)
{
	return CanGetValueAs(row, col, TypeName);
}

long SalesTable::GetValueAsLong(int row, int col)
{
	return GetValueTemplate<long>(row, col);
}

double SalesTable::GetValueAsDouble(int row, int col)
{
	return GetValueTemplate<double>(row, col);
}

bool SalesTable::GetValueAsBool(int row, int col)
{
	return GetValueTemplate<bool>(row, col);
}

void SalesTable::SetValue(int row, int col, const wxString& value)
{
	//this might be though
}

void SalesTable::SetValueAsLong(int row, int col, long value)
{
	SetValueTemplate<long>(row, col, value);
}

void SalesTable::SetValueAsDouble(int row, int col, double value)
{
	SetValueTemplate<double>(row, col, value);
}

void SalesTable::SetValueAsBool(int row, int col, bool value)
{
	SetValueTemplate<bool>(row, col, value);
}

void SalesTable::Clear()
{
	mSalesTable.clear();
}

const std::string& SalesTable::GetProductNameByID(Sales::elem_t<Sales::product_id> id) const
{
	// // O: insert return statement here
	auto& p = ProductInstance::instance();
	auto iter = p.find_on<Products::id>(id);
	if (iter == p.end()) {
		return std::string{};
	}
	else{
		return nl::row_value<Products::name>(*iter);
	}
}