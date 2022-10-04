#include "common.h"
#include "SearchSaleProduct.h"

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(SearchProduct, wxDialog)

END_EVENT_TABLE()


///////////////////////////////////////////////////////////////////////////

SearchProduct::SearchProduct(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
	CreateToolBar();





	m_mgr.Update();
	this->Centre(wxBOTH);
}

SearchProduct::~SearchProduct()
{
	m_mgr.UnInit();

}

void SearchProduct::CreateToolBar()
{
	SearchProductBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_OVERFLOW);
	SearchProductBar->SetToolBitmapSize(wxSize(16, 16));
	SearchCtrl = new wxSearchCtrl(SearchProductBar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
#ifndef __WXMAC__
	SearchCtrl->ShowSearchButton(true);
#endif
	SearchCtrl->ShowCancelButton(true);
	SearchCtrl->SetToolTip(wxT("Search for product to sell"));

	SearchProductBar->AddControl(SearchCtrl);
	m_tool1 = SearchProductBar->AddTool(ID_SEARCH_BUTTON, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

	SearchProductBar->Realize();
	m_mgr.AddPane(SearchProductBar, wxAuiPaneInfo().Name(wxT("SearchToolBar")).Top().CloseButton(false).PaneBorder(false).Dock().Resizable().FloatingSize(wxDefaultSize).BottomDockable(false).LeftDockable(false).RightDockable(false).Floatable(false).Layer(10).ToolbarPane());

}

void SearchProduct::CreateSearchView()
{
	m_panel1 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_mgr.AddPane(m_panel1, wxAuiPaneInfo().Center().CaptionVisible(false).CloseButton(false).PaneBorder(false).Movable(false).Dock().Resizable().FloatingSize(wxDefaultSize).BottomDockable(false).TopDockable(false).LeftDockable(false).RightDockable(false).Floatable(false).CentrePane());

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	SearchData = new wxDataViewCtrl(m_panel1, ID_SEARCH_VIEW, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES | wxDV_ROW_LINES);
	ProductIdCol = SearchData->AppendTextColumn(wxT("Product ID"), 0, wxDATAVIEW_CELL_INERT, 120, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	ProductName = SearchData->AppendTextColumn(wxT("Product Name"), 1, wxDATAVIEW_CELL_INERT, 150, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	PriceCol = SearchData->AppendTextColumn(wxT("Price"), 2, wxDATAVIEW_CELL_INERT, 120, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	bSizer1->Add(SearchData, 1, wxALL | wxEXPAND, 0);


	m_panel1->SetSizer(bSizer1);
	m_panel1->Layout();
	bSizer1->Fit(m_panel1);
}

void SearchProduct::OnItemSelected(wxDataViewEvent& evt)
{
}

void SearchProduct::OnActivated(wxDataViewEvent& evt)
{
}
