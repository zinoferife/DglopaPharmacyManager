#include "common.h"
#include "SearchSaleProduct.h"

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE! or else ?? lol
///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(SearchProduct, wxDialog)
EVT_DATAVIEW_ITEM_ACTIVATED(SearchProduct::ID_SEARCH_VIEW, SearchProduct::OnActivated)
EVT_SEARCH(SearchProduct::ID_SEARCH_CTRL, SearchProduct::OnSearch)
EVT_CLOSE(SearchProduct::OnClose)
EVT_SEARCH_CANCEL(SearchProduct::ID_SEARCH_CTRL, SearchProduct::OnSearchCancelled)
EVT_TEXT(SearchProduct::ID_SEARCH_CTRL, SearchProduct::OnSearch)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////////////////////

SearchProduct::SearchProduct(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : 
	wxDialog(parent, id, title, pos, size, style), mSelectedProduct(-1)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
	CreateViewManagerDefaultArt();
	CreateToolBar();
	CreateSearchView();

	//load the whole product relation
	SetUpView();

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
	SearchCtrl = new wxSearchCtrl(SearchProductBar, ID_SEARCH_CTRL, wxEmptyString, wxDefaultPosition, wxSize(300, -1), wxWANTS_CHARS);
#ifndef __WXMAC__
	SearchCtrl->ShowSearchButton(true);
#endif
	SearchCtrl->ShowCancelButton(true);
	SearchCtrl->SetToolTip(wxT("Search for product to sell"));
	SearchCtrl->SetDescriptiveText("Search products by name");

	SearchProductBar->AddControl(SearchCtrl);
	m_tool1 = SearchProductBar->AddTool(ID_SEARCH_BUTTON, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

	SearchProductBar->Realize();
	m_mgr.AddPane(SearchProductBar, wxAuiPaneInfo().Name(wxT("SearchToolBar")).Top().MinSize(-1, 30).DockFixed().CloseButton(false).PaneBorder(false).Dock().Resizable().BottomDockable(false).LeftDockable(false).RightDockable(false).Floatable(false).Layer(10).ToolbarPane());

}

void SearchProduct::CreateSearchView()
{
	m_panel1 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_mgr.AddPane(m_panel1, wxAuiPaneInfo().Center().CaptionVisible(false).CloseButton(false).PaneBorder(false).Movable(false).Dock().Resizable().FloatingSize(wxDefaultSize).BottomDockable(false).TopDockable(false).LeftDockable(false).RightDockable(false).Floatable(false).CentrePane());

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	mModel = new DataModel<view_t>(ProductView);

	SearchData = new wxDataViewCtrl(m_panel1, ID_SEARCH_VIEW, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES | wxDV_ROW_LINES);
	ProductIdCol = SearchData->AppendTextColumn(wxT("Product ID"), 0, wxDATAVIEW_CELL_INERT, 120, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	ProductName = SearchData->AppendTextColumn(wxT("Product Name"), 1, wxDATAVIEW_CELL_INERT, 300, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	PriceCol = SearchData->AppendTextColumn(wxT("Price"), 2, wxDATAVIEW_CELL_INERT, 120, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	bSizer1->Add(SearchData, 1, wxALL | wxEXPAND, 0);

	wxDataViewModel* ProductModel = mModel;
	SearchData->AssociateModel(ProductModel);
	ProductModel->DecRef();

	

	m_panel1->SetSizer(bSizer1);
	m_panel1->Layout();
	bSizer1->Fit(m_panel1);
}

void SearchProduct::CreateViewManagerDefaultArt()
{
	wxColour colour = wxTheColourDatabase->Find("Aqua");
	wxAuiDockArt* art = m_mgr.GetArtProvider();
	art->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 24);
	art->SetMetric(wxAUI_DOCKART_GRIPPER_SIZE, 5);
	art->SetMetric(wxAUI_DOCKART_SASH_SIZE, 5);
	art->SetColour(wxAUI_DOCKART_SASH_COLOUR, *wxWHITE);
	art->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, colour);
	art->SetColour(wxAUI_DOCKART_BORDER_COLOUR, *wxWHITE);
	art->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	art->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_HORIZONTAL);
	m_mgr.SetFlags(m_mgr.GetFlags() | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_VENETIAN_BLINDS_HINT);
}

void SearchProduct::SetUpView()
{
	ProductView = std::move(view_t::make_view<Products>(ProductInstance::instance().begin(),
		ProductInstance::instance().end(),
		std::index_sequence<Products::id, Products::name, Products::unit_price>{}));
	mModel->ReloadIndices();
}

void SearchProduct::SetUpView(std::vector<size_t> idx)
{
	ProductView = std::move(view_t::make_view<Products>(ProductInstance::instance(), idx,
		std::index_sequence<Products::id, Products::name, Products::unit_price>{}));
	mModel->ReloadIndices();
}

void SearchProduct::OnItemSelected(wxDataViewEvent& evt)
{

}

void SearchProduct::OnActivated(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (item.IsOk()) {
		mSelectedProduct = mModel->ItemToIdx(item);
		if (mSelectedProduct > ProductView.size()) return;
		//close the search 
		if (IsModal()) {
			EndModal(wxID_OK);
		}
		else {
			SetReturnCode(wxID_OK);
			this->Show(false);
		}
	}
}

void SearchProduct::OnSearch(wxCommandEvent& evt) {
	std::string search = evt.GetString().ToStdString();
	DoSearch(search);
}

void SearchProduct::OnSearchCancelled(wxCommandEvent& evt)
{
	spdlog::get("log")->info(evt.GetString().ToStdString());
	SetUpView();
}

void SearchProduct::OnClose(wxCloseEvent& evt)
{

	SearchCtrl->Destroy();
	

	SetReturnCode(wxID_CANCEL);
	evt.Skip();
}

void SearchProduct::DoSearch(const std::string& search_for)
{
	if (search_for.empty()) return;
	SetUpView(Searcher<Products::name, Products>{ProductInstance::instance()}(search_for));
}
