#pragma once
//the main pos system 
//products are bought and the inventory is updated
//the view of the system

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/artprov.h>
#include <wx/srchctrl.h>
#include <wx/combobox.h>
#include <wx/progdlg.h>
#include <wx/tglbtn.h>
#include <wx/grid.h>
#include <wx/dataview.h>


#include "Tables.h"
#include "Instances.h"
#include "SearchAutoComplete.h"
#include "Searcher.h"
#include "DatabaseManger.h"
#include "ListDisplayDialog.h"
#include "DataModel.h"
#include "SalesOutput.h"


#include <nl_uuid.h>
#include <array>
#include <utility>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>


#include "DropTarget.h"
#include "SearchSaleProduct.h"


class SalesView : public wxPanel
{
public:
	virtual ~SalesView();
	SalesView(wxWindow* parent,
		wxWindowID id,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize);
	//ids 
	enum {
		ID_DATA_VIEW = wxID_HIGHEST + 2000,
		ID_CHECKOUT,
		ID_SETTINGS,
		ID_SALES_OUTPUT,
		ID_ADD_PRODUCT,
		ID_SAVE_LIST

	};

	void CreateTopToolBar();
	void CreateBottomToolBar();

	void CreateDataView();
	void CreateSpecialColHandlers();
	void CreateSalesOutput();
	void CreateInventoryDatabaseManger();

	void SetDefaultAuiArt();
	void SetSpecicalColumns(); //set for writing the quanity that can be editited.

public:
	void OnCheckOut(wxCommandEvent& evnt);
	void OnReturn(wxCommandEvent& evnt);
	void OnAddProduct(wxCommandEvent& evnt);
	void OnSalesSave(wxCommandEvent& evt);
	void OnSaveListSelected(wxCommandEvent& evt);

	void OnEditingStarted(wxDataViewEvent& evt);
	void OnEditingStarting(wxDataViewEvent& evt);
	void OnEditingDone(wxDataViewEvent& evt);

	const std::string& GetProductNameByID(Sales::elem_t<Sales::product_id> id) const;

	void DropData(const Products::row_t& row);
	bool CheckInStock(const Products::row_t& row) const;
	bool CheckInStock(const SearchProduct::view_t::row_t& row) const;
	bool CheckProductClass(const Products::row_t& row) const;
	bool CheckProductClass(const SearchProduct::view_t::row_t& row) const;
	void GetDataFromProductSearch(const SearchProduct::view_t::row_t& SelectedRow);
	void StoreEditedValue(const wxVariant& data, const wxDataViewItem& item, 
			size_t col);
	void UpdateTotal();
	void DoCheckOut();
	bool CheckIfAdded(Products::elem_t<Products::id> id);
	void StoreDataInInventory();
	Inventories::row_t MostRecentInventoryEntry(Products::elem_t<Products::id> id) const;


	void DoSave();
	void DoRestore();


	void DoSwap();
private:
	std::unique_ptr<wxAuiManager> mViewManager;
	wxChoice* mSaveList;

	//this would come from settings
	bool mPOMwarningMessage = false;

	//so i cam return empty strings
	static std::string mEmptyString;

	//dataview and model
	std::unique_ptr<wxDataViewCtrl> mDataView;
	std::unique_ptr<DataModel<Sales>> mModel;
	std::unique_ptr<SalesOutput> mSalesOutput;
	std::unique_ptr<DatabaseManager<Inventories>> mInvetoryDatabasemagr;
	

	std::array<Sales, 5> mSalesTables;
	Sales mSalesTable;

	//so the the database manager can have where to read or write data
	Inventories  mTempInventoryRel;


	DECLARE_EVENT_TABLE()

};

