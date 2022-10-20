#pragma once
///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/srchctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/dataview.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/frame.h>

#include <wx/dialog.h>
///////////////////////////////////////////////////////////////////////////
#include <rel_view.h>
#include "Instances.h"
#include "Searcher.h"
#include "DataModelBase.h"

///////////////////////////////////////////////////////////////////////////////
/// Class SearchProduct
///////////////////////////////////////////////////////////////////////////////
class SearchProduct : public wxDialog
{
public:
	typedef nl::rel_view<std::uint64_t, std::string, std::string> view_t;

protected:
	wxAuiToolBar* SearchProductBar;
	wxSearchCtrl* SearchCtrl;
	wxAuiToolBarItem* m_tool1;
	wxPanel* m_panel1;
	wxDataViewCtrl* SearchData;
	wxDataViewColumn* ProductIdCol;
	wxDataViewColumn* ProductName;
	wxDataViewColumn* PriceCol;

	

	//id, name, price
	view_t ProductView;
	DataModel<view_t>* mModel;
	size_t mSelectedProduct;

public:
	enum {
		ID_SEARCH_BUTTON = wxID_HIGHEST + 3000,
		ID_SEARCH_CTRL,
		ID_SEARCH_VIEW
	};


	SearchProduct(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Search Product"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1057, 642), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
	wxAuiManager m_mgr;

	~SearchProduct();

	void CreateToolBar();
	void CreateSearchView();
	void CreateViewManagerDefaultArt();

	void SetUpView();
	void SetUpView(std::vector<size_t> idx);



	inline view_t::row_t GetSelectedProduct() const	{
		return ProductView[mSelectedProduct];
	}
protected:
	void OnItemSelected(wxDataViewEvent& evt);
	void OnActivated(wxDataViewEvent& evt);
	void OnSearch(wxCommandEvent& evt);
	void OnSearchCancelled(wxCommandEvent& evt);
	void OnClose(wxCloseEvent& evt);

	void DoSearch(const std::string& search_for);

	DECLARE_EVENT_TABLE()
};



