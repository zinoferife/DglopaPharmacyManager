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
#include <wx/tglbtn.h>
#include <wx/editlbox.h>


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
#include "ListDisplayDialog.h"


#include <nl_uuid.h>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <tuple_json.h>

#include <fstream>
#include <unordered_map>
#include <set>
#include <bitset>
#include <memory>
#include <random>
#include <functional>


#include "DropTarget.h"

namespace js = nlohmann;

class ProductView : public wxPanel
{
	constexpr static char all_categories[] = "ALL CATEGORIES";
public:
	virtual ~ProductView();

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
		ID_ADD_CATEGORY,
		ID_REMOVE_GROUP_BY,
		ID_QUICK_SORT_TEST,
		ID_INVENTORY_VIEW,
		ID_INVENTORY_VIEW_TOOL_ADD,
		ID_INVENTORY_VIEW_TOOL_REMOVE,
		ID_INVENTORY_PRODUCT_NAME,
		ID_CATEGORY_LIST_CONTROL,
		ID_PRODUCT_CONTEXT_EDIT,
		ID_PRODUCT_CONTEXT_REMOVE,
		ID_PRODUCT_CONTEXT_EXPORT,
		ID_PRODUCT_CONTEXT_EXPORT_JSON,
		ID_PRODUCT_CONTEXT_EDIT_BOX,
		ID_EXPIRY_VIEW,
		ID_SELECT_MULTIPLE,
		ID_UNSELECT_MULTIPLE,
		ID_TO_JSON,
		ID_FILE
	};


	void ImportProductsFromJson(std::fstream& file);
	std::set<size_t> mSelectedProductIndex;
private: 
	void CreateToolBar();
	void CreateInentoryToolBar();
	void CreateDataView();
	void CreateSpecialColumnHandlers();
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
	void OnFile(wxAuiToolBarEvent& evt);
	void OnCategory(wxCommandEvent& evt);

	
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
	void OnProductContextMenu(wxDataViewEvent& evt);
	void OnProductDetailView(wxCommandEvent& evt);
	void OnExpiryView(wxCommandEvent& evt);
	void OnSelectMultiple(wxCommandEvent& evt);
	void OnUnSelectMultiple(wxCommandEvent& evt);
	void OnPaneClose(wxAuiManagerEvent& evt);
	void OnProductExportJson(wxCommandEvent& evt);

	void DoSearch(const std::string& searchString);
	void DoCategorySelect(const std::string& selected_category);

	void RegisterNotification();
	void UnregisterNotification();
	void OnInventoryAddNotification(const Inventories::table_t& table, const Inventories::notification_data& data);
	void OnUsersNotification(const Users::table_t& table, const Users::notification_data& data);
	void OnCategoryAddNotification(const Categories::table_t& table, const Categories::notification_data& data);
	void OnProductUpdateNotification(const Products::table_t& table, const Products::notification_data& data);
	void OnProductDetailUpdateNotification(const ProductDetails::table_t&, const ProductDetails::notification_data& data);
	
	//Drap and drop
	void OnDragStart(wxDataViewEvent& evt);


	//JSON test
	void OnToJson(wxCommandEvent& evt);
	typedef nl::tuple_json<Products::row_t> product_json_converter;
	void InitJsonConverter();
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
	std::unique_ptr<wxEditableListBox> mEditBox;

	DataModel<Products>* mModel;
	wxAuiToolBar* bar;
	wxSearchCtrl* search;
	wxChoice* categories;
	//product view item attributes
	std::shared_ptr<wxDataViewItemAttr> mInStock;
	std::shared_ptr<wxDataViewItemAttr> mExpired;
	std::shared_ptr<wxDataViewItemAttr> mModified;
	std::shared_ptr<wxDataViewItemAttr> mHeaderAttributes;

	wxAuiToolBarItem* mInventoryProductName;

	DECLARE_EVENT_TABLE()
};

