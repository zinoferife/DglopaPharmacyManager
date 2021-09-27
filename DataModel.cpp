#include "common.h"
#include "DataModel.h"

static std::int64_t to_stdInt64(wxLongLong value)
{
	std::int64_t result = value.GetHi();
	result <<= 32;
	result |= value.GetLo();
	return result;
}


ProductDataModel::ProductDataModel(Products& product)
:DataModel<Products>(product){}

ProductDataModel::~ProductDataModel()
{}

bool ProductDataModel::GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const
{
	return false;
}

bool ProductDataModel::SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col)
{
	auto indexIter = mIndexMap.find(item);
	if (indexIter == mIndexMap.end()) return false;
	size_t index = indexIter->second;
	if (item.IsOk())
	{
		switch (col)
		{
		case Products::id:
			mData.update_row<Products::id>(index, to_stdInt64(variant.GetLongLong()));
			break;
		case Products::name:
			mData.update_row<Products::name>(index, variant.GetString().ToStdString());
			break;
		case Products::package_size:
			mData.update_row<Products::package_size>(index, variant.GetLong());
			break;
		case Products::stock_count:
			mData.update_row<Products::stock_count>(index, variant.GetLong());
			break;
		case Products::unit_price:
			mData.update_row<Products::unit_price>(index, variant.GetString().ToStdString());
			break;
		default:
			return false;
			break;
		}
		mData.notify(nl::notifications::update, col, index);
		return true;
	}
	return false;
}

unsigned int ProductDataModel::GetColumnCount() const
{
	//all but the category id
	return Products::column_count - 1;
}



SalesDataModel::SalesDataModel(Sales& sale)
:DataModel<Sales>(sale){}

SalesDataModel::~SalesDataModel()
{}

bool SalesDataModel::GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const
{
	return false;
}

unsigned int SalesDataModel::GetColumnCount() const
{
	return mData.get_column_count();
}

bool SalesDataModel::SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col)
{
	return false;
}

