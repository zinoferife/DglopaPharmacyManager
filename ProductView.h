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
		ID_INVENTORY_VIEW
	};


private: 
	void CreateToolBar();
	void CreateDataView();
	void CreateItemAttr();
	void CreateInventoryList();

	void SetDefaultArt();
//event table
private:
	void OnAddProduct(wxCommandEvent& evt);
	void OnRemoveProduct(wxCommandEvent& evt);
	void OnCheckInStock(wxCommandEvent& evt);
	void OnResetAttributes(wxCommandEvent& evt);
	void OnSearchProduct(wxCommandEvent& evt);
	void OnEraseBackground(wxEraseEvent& evt);
	void OnQuickSortTest(wxCommandEvent& evt);
	void OnBack(wxCommandEvent& evt);

	//search
	void OnSearchFlag(wxCommandEvent& evt);
	void OnSearchByName(const std::string& SearchString);
	void OnSearchByCategory(const std::string& SearchString);
	void OnSearchByPrice(const std::string& SearchString);

	//data view events
	void OnProductItemSelected(wxDataViewEvent& evt);
	void OnProductItemActivated(wxDataViewEvent& evt);
	void OnColumnHeaderClick(wxDataViewEvent& evt);

private:
	//for test 
	static int gen_random();
	std::bitset<3> mSearchFlags;
	std::unique_ptr<wxAuiManager> mPanelManager;
	std::unique_ptr<wxDataViewCtrl> mDataView;
	std::unique_ptr<InventoryView> mInventoryView;
	DataModel<Products>* mModel;

	//product view item attributes
	std::shared_ptr<wxDataViewItemAttr> mInStock;
	std::shared_ptr<wxDataViewItemAttr> mExpired;

	DECLARE_EVENT_TABLE()
};

