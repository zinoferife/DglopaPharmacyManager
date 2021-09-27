#pragma once
//base class of 
#include "DataModelBase.h"

class ProductDataModel : public DataModel<Products>
{
public:
	ProductDataModel(Products& product);
	virtual ~ProductDataModel();
//wxDataViewModel functions 
public:
	virtual bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const;
	virtual unsigned int GetColumnCount() const;
	virtual bool SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col);
};

//Sales data model, deals with the sales table
class SalesDataModel : public DataModel<Sales>
{
public:
	SalesDataModel(Sales& sale);
	virtual ~SalesDataModel();

public:
	virtual bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const;
	virtual unsigned int GetColumnCount() const;
	virtual bool SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col);

};

