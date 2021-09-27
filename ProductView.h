#pragma once
#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/dataview.h>
#include <wx/artprov.h>
#include <wx/srchctrl.h>


#include "Tables.h"
#include "DataModel.h"
#include "Instances.h"

#include <nl_uuid.h>

#include <spdlog/spdlog.h>

#include <unordered_map>
#include <bitset>
#include <memory>

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
		ID_DATA_VIEW,
		ID_SEARCH,
		ID_SEARCH_BY_CATEGORY,
		ID_SEARCH_BY_PRICE,
		ID_SEARCH_BY_NAME
	};


private: 
	void CreateToolBar();
	void CreateDataView();
	void SetDefaultArt();
//event table
private:
	void OnAddProduct(wxCommandEvent& evt);
	void OnRemoveProduct(wxCommandEvent& evt);
	void OnSearchProduct(wxCommandEvent& evt);
	void OnEraseBackground(wxEraseEvent& evt);
	
	//search
	void OnSearchFlag(wxCommandEvent& evt);
	void OnSearchByName(const std::string& SearchString);
	void OnSearchByCategory(const std::string& SearchString);
	void OnSearchByPrice(const std::string& SearchString);

private:
	std::bitset<3> mSearchFlags;
	std::unique_ptr<wxAuiManager> mPanelManager;
	std::unique_ptr<wxDataViewCtrl> mDataView;
	DECLARE_EVENT_TABLE()
};

