#pragma once
#include <wx/wx.h>
#include <wx/grid.h>

#include "Tables.h"
#include "Searcher.h"
#include "DatabaseManger.h"

class SalesTable : public wxGridTableBase
{
public:

	virtual ~SalesTable();
	
	//from the base class
	virtual int GetNumberRows() override;
	virtual int GetNumberCols() override;
	virtual bool IsEmptyCell(int row, int col) override;
	virtual wxString GetValue(int row, int col) override;
	virtual wxString GetTypeName(int row, int col) override;
	virtual bool CanGetValueAs(int rows, int col, const wxString& TypeName) override;
	virtual bool CanSetValueAs(int row, int col, const wxString& TypeName) override;

	virtual long GetValueAsLong(int row, int col) override;
	virtual double GetValueAsDouble(int row, int col) override;
	virtual bool GetValueAsBool(int row, int col) override;
	
	virtual void SetValue(int row, int col, const wxString& value) override;
	virtual void SetValueAsLong(int row, int col, long value) override;
	virtual void SetValueAsDouble(int row, int col, double value) override;
	virtual void SetValueAsBool(int row, int col, bool value);

	virtual void Clear();
	
	inline const Sales& GetSalesTable() const { return mSalesTable; }
private:
	Sales mSalesTable;
	const std::string& GetProductNameByID(Sales::elem_t<Sales::product_id> id) const;

	template<typename T>
	T GetValueTemplate(int row, int col) {
		auto& row_tuple = mSalesTable[row];
		T value;
		if constexpr (nl::detail::index_of<Sales::row_t, T>::value != -1) {
			nl::detail::loop<Sales::column_count - 1>::get_in(row_tuple, value, col);
			return value;
		}
		else {
			return T{};
		}
	}

	template<typename T>
	bool SetValueTemplate(int row, int col, T& value) {
		auto& row_t = mSalesTable[row];
		if constexpr (nl::detail::index_of<Sales::row_t, T>::value != -1) {
			nl::detail::loop<Sales::column_count - 1>::put_value_in(row_t, value, col);
			return true;
		}
		//ignore the write
		return false;
	}
};

