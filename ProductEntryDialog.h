#pragma once
#include <wx/wx.h>
#include "Instances.h"
#include <wx/statline.h>
#include <wx/collpane.h>
#include <wx/spinctrl.h>
#include <wx/valnum.h>
#include "SearchAutoComplete.h"
#include <algorithm>
#include <sstream>


class ProductEntryDialog : public wxDialog
{
public:
	typedef  nl::vector_table<std::string> search_table;
	ProductEntryDialog(wxWindow* window);
	virtual bool TransferDataFromWindow() override;
	virtual bool TransferDataToWindow() override;
	Products::row_t product;
	ProductDetails::row_t productDetail;

	enum{
		ID_ERROR
	};

private:
	void SizeDialog();
	void CreateDialog();

	wxComboBox* mProductCategoryCtrl;
	wxChoice* mProductClassCtrl;
	wxTextCtrl* mProductNameCtrl;
	wxTextCtrl* mProductActiveIngredentCtrl;
	wxTextCtrl* mProductDescCtrl;
	wxTextCtrl* mProductDirForUseCtrl;
	wxTextCtrl* mProductUnitPriceCtrl;
	wxSpinCtrl* mProductPackageSizeCtrl;
	wxStaticText* descp[10];
	wxButton* mOkButton;
	wxButton* mCancelButton;
	wxWindow* wPane;
	wxCollapsiblePane* pane;
	float validate_float;
	wxArrayString choices;

	void OnOk(wxCommandEvent& evt);
	void OnCancel(wxCommandEvent& evt);


	static search_table mSearchTable;
	DECLARE_CLASS(ProductEntryDialog)
	DECLARE_EVENT_TABLE()
};

