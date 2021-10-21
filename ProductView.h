#pragma once
#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/dataview.h>
#include <wx/artprov.h>
#include <wx/srchctrl.h>


#include "Tables.h"
#include "DataModelBase.h"
#include "Instances.h"
#include "SearchAutoComplete.h"
#include "InventoryView.h"
#include "ProductEntryDialog.h"
#include "Searcher.h"
#include "DatabaseManger.h"

#include <nl_uuid.h>

#include <spdlog/spdlog.h>

#include <unordered_map>
#include <bitset>
#include <memory>
#include <random>
#include <functional>
class ProductView : public wxPanel
{
public:
	~ProductView();
	ProductView();
	ProductView(wxWindow* parent, wxWindowID id, const wxPoint& position = wxDefaultPosition, const wxSize size = wxDefaultSize);
	//toobar id
	enum
	{
		ID_ADD_PRODUCT = wxID_HIGHEST + 2000,
		ID_REMOVE_PRODUCT,
		ID_BACK,
		ID_FORWARD,
		ID_DATA_VIEW,
		ID_SEARCH,
		ID_SEARCH_BY_CATEGORY,
		ID_SEARCH_BY_PRICE,
		ID_SEARCH_BY_NAME,
		ID_GROUP_BY,
		ID_REMOVE_GROUP_BY,
		ID_QUICK_SORT_TEST,
		ID_INVENTORY_VIEW,
		ID_INVENTORY_VIEW_TOOL_ADD,
		ID_INVENTORY_VIEW_TOOL_REMOVE,
		ID_INVENTORY_PRODUCT_NAME
	};


private: 
	void CreateToolBar();
	void CreateInentoryToolBar();
	void CreateDataView();
	void CreateItemAttr();
	//test
	void CreateInventoryList();
	void CreateDatabaseMgr();

	void CreateInventory(std::uint64_t product_id);
	void SetDefaultArt();
	void ShowInventoryToolBar(const Products::row_t& row);
	void HideInventoryToolBar();
//event table
private:
	void OnAddProduct(wxCommandEvent& evt);
	void OnRemoveProduct(wxCommandEvent& evt);
	void OnCheckInStock(wxCommandEvent& evt);
	void OnResetAttributes(wxCommandEvent& evt);
	void OnSearchProduct(wxCommandEvent& evt);
	void OnSearchCleared(wxCommandEvent& evt);
	void OnEraseBackground(wxEraseEvent& evt);
	void OnQuickSortTest(wxCommandEvent& evt);
	void OnBack(wxCommandEvent& evt);

	//evts for inventory view
	void OnInventoryViewColClick(wxListEvent& evt);
	void OnInventoryAddTool(wxCommandEvent& evt);
	void OnInventoryRemoveTool(wxCommandEvent& evt);
	//search
	void OnSearchFlag(wxCommandEvent& evt);
	void OnSearchByName(const std::string& SearchString);
	void OnSearchByCategory(const std::string& SearchString);
	void OnSearchByPrice(const std::string& SearchString);

	//data view events
	void OnProductItemSelected(wxDataViewEvent& evt);
	void OnProductItemActivated(wxDataViewEvent& evt);
	void OnColumnHeaderClick(wxDataViewEvent& evt);


	void DoSearch(const std::string& searchString);


	void RegisterNotification();
	void UnregisterNotification();
	void OnInventoryAddNotification(const Inventories::table_t& table, const Inventories::notification_data& data);
	void OnUsersNotification(const Users::table_t& table, const Users::notification_data& data);
private:
	//for test 
	static int gen_random();
	std::bitset<3> mSearchFlags;
	std::unique_ptr<wxAuiManager> mPanelManager;
	std::unique_ptr<wxDataViewCtrl> mDataView;
	std::unique_ptr<InventoryView> mInventoryView;
	std::unique_ptr<DatabaseManger<Products>> mDatabaseMgr;
	DataModel<Products>* mModel;
	wxAuiToolBar* bar;
	wxSearchCtrl* search;
	//product view item attributes
	std::shared_ptr<wxDataViewItemAttr> mInStock;
	std::shared_ptr<wxDataViewItemAttr> mExpired;
	wxAuiToolBarItem* mInventoryProductName;
	Products mTempProductTable;

	DECLARE_EVENT_TABLE()
};

