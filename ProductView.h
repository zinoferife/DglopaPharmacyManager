#pragma once
#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/dataview.h>
#include <wx/artprov.h>
#include <wx/srchctrl.h>
#include <wx/combobox.h>
#include <wx/progdlg.h>



#include "Tables.h"
#include "DataModelBase.h"
#include "Instances.h"
#include "SearchAutoComplete.h"
#include "InventoryView.h"
#include "ProductEntryDialog.h"
#include "Searcher.h"
#include "DatabaseManger.h"
#include "DetailView.h"
#include "ExpiryView.h"

#include <nl_uuid.h>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <fstream>
#include <unordered_map>
#include <bitset>
#include <memory>
#include <random>
#include <functional>


namespace js = nlohmann;

class ProductView : public wxPanel
{
	constexpr static char all_categories[] = "All categories";
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
		ID_INVENTORY_PRODUCT_NAME,
		ID_CATEGORY_LIST_CONTROL,
		ID_PRODUCT_CONTEXT_EDIT,
		ID_EXPIRY_VIEW
	};


	void ImportProductsFromJson(std::fstream& file);
private: 
	void CreateToolBar();
	void CreateInentoryToolBar();
	void CreateDataView();
	void LoadCategoryToChoiceBox();
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
	void OnCategorySelected(wxCommandEvent& evt);
	//data view events
	void OnProductItemSelected(wxDataViewEvent& evt);
	void OnProductItemActivated(wxDataViewEvent& evt);
	void OnColumnHeaderClick(wxDataViewEvent& evt);
	void OnProductContextMenu(wxDataViewEvent& evt);
	void OnProductDetailView(wxCommandEvent& evt);
	void OnExpiryView(wxCommandEvent& evt);

	void DoSearch(const std::string& searchString);
	void DoCategorySelect(const std::string& selected_category);

	void RegisterNotification();
	void UnregisterNotification();
	void OnInventoryAddNotification(const Inventories::table_t& table, const Inventories::notification_data& data);
	void OnUsersNotification(const Users::table_t& table, const Users::notification_data& data);
	void OnCategoryAddNotification(const Categories::table_t& table, const Categories::notification_data& data);
	void OnProductUpdateNotification(const Products::table_t& table, const Products::notification_data& data);
	void OnProductDetailUpdateNotification(const ProductDetails::table_t&, const ProductDetails::notification_data& data);
	
private:
	//for test 
	std::bitset<3> mSearchFlags;
	std::unique_ptr<wxAuiManager> mPanelManager;
	std::unique_ptr<wxDataViewCtrl> mDataView;
	std::unique_ptr<InventoryView> mInventoryView;
	std::unique_ptr<DatabaseManager<Products>> mDatabaseMgr;
	std::unique_ptr<DatabaseManager<Categories>> mDatabaseCatgoryMgr;
	std::unique_ptr<DatabaseManager<ProductDetails>> mDatabaseDetailMgr;
	std::unique_ptr<DetailView> mDetailView;


	DataModel<Products>* mModel;
	wxAuiToolBar* bar;
	wxSearchCtrl* search;
	wxChoice* categories;
	//product view item attributes
	std::shared_ptr<wxDataViewItemAttr> mInStock;
	std::shared_ptr<wxDataViewItemAttr> mExpired;
	std::shared_ptr<wxDataViewItemAttr> mModified;

	wxAuiToolBarItem* mInventoryProductName;

	DECLARE_EVENT_TABLE()
};

