#include "common.h"
#include "ProductView.h"
BEGIN_EVENT_TABLE(ProductView, wxPanel)
	EVT_TOOL(ProductView::ID_ADD_PRODUCT, ProductView::OnAddProduct)
	EVT_TOOL(ProductView::ID_REMOVE_PRODUCT, ProductView::OnRemoveProduct)
	EVT_SEARCH(ProductView::ID_SEARCH, ProductView::OnSearchProduct)
	EVT_MENU(ProductView::ID_SEARCH_BY_NAME, ProductView::OnSearchFlag)
	EVT_MENU(ProductView::ID_SEARCH_BY_CATEGORY, ProductView::OnSearchFlag)
	EVT_MENU(ProductView::ID_SEARCH_BY_PRICE, ProductView::OnSearchFlag)
	EVT_ERASE_BACKGROUND(ProductView::OnEraseBackground)
END_EVENT_TABLE()

ProductView::ProductView()
{
}

ProductView::~ProductView()
{
	mDataView.release();
}

ProductView::ProductView(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize size)
:wxPanel(parent, id, position, size){
	mPanelManager = std::make_unique<wxAuiManager>(this);
	SetDefaultArt();
	CreateToolBar();
	CreateDataView();
	mPanelManager->Update();
}

void ProductView::CreateToolBar()
{
	wxAuiToolBar* bar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW);
	bar->SetToolBitmapSize(wxSize(16, 16));
	bar->AddTool(wxID_ANY, wxEmptyString, wxArtProvider::GetBitmap("arrow_back"));
	bar->AddTool(wxID_ANY, wxEmptyString, wxArtProvider::GetBitmap("arrow_next"));

	//search bar
	wxSearchCtrl* search = new wxSearchCtrl(bar, ID_SEARCH, wxEmptyString, wxDefaultPosition, wxSize(300, -1), wxWANTS_CHARS);
	search->SetDescriptiveText("Search products");
	search->ShowCancelButton(true);
	wxMenu* menu = new wxMenu;
	menu->AppendRadioItem(ID_SEARCH_BY_NAME, "Search by name");
	menu->AppendRadioItem(ID_SEARCH_BY_CATEGORY, "Search by category");
	menu->AppendRadioItem(ID_SEARCH_BY_PRICE, "Search by price");
	menu->Check(ID_SEARCH_BY_NAME, true);
	search->SetMenu(menu);
	bar->AddControl(search);
	
	
	bar->AddStretchSpacer();
	bar->AddTool(ID_ADD_PRODUCT, wxEmptyString, wxArtProvider::GetBitmap("action_add"));
	bar->AddTool(ID_REMOVE_PRODUCT, wxEmptyString, wxArtProvider::GetBitmap("action_remove"));

	//realise and add to manager
	bar->Realize();
	mPanelManager->AddPane(bar, wxAuiPaneInfo().Name(wxT("Tool")).Caption(wxT("Tool bar")).ToolbarPane().Top()
		.Resizable().MinSize(wxSize(-1, 30)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void ProductView::CreateDataView()
{
	ProductInstance::instance().as<Products::id>("Serial number");
	ProductInstance::instance().as<Products::name>("Name");
	ProductInstance::instance().as<Products::package_size>("Package size");
	ProductInstance::instance().as<Products::stock_count>("Stock count");
	ProductInstance::instance().as<Products::unit_price>("Unit price");
	ProductInstance::instance().as<Products::category_id>("Category id");

	mDataView  = std::make_unique<wxDataViewCtrl>(this,ID_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES | wxNO_BORDER);
	wxDataViewModel* ProductModel = new DataModel<Products>(ProductInstance::instance()); 
	mDataView->AssociateModel(ProductModel);
	ProductModel->DecRef();
	//add columns
	/*
	for (size_t i = 0; i < Products::column_count - 1; i++)
	{
		mDataView->AppendTextColumn(ProductInstance::instance().get_name(i), i);
	}
	*/

	mDataView->AppendTextColumn(ProductInstance::instance().get_name(0), 0);
	mDataView->AppendTextColumn(ProductInstance::instance().get_name(2), 2);
	mDataView->AppendTextColumn(ProductInstance::instance().get_name(5), 5);

	//load test
	Products::set_default_row(100, "test", 0, 0, "0.00", 9);
	for (int i = 0; i < 3; i++)
	{
		ProductInstance::instance().add(i, nl::to_string_date(nl::clock::now()), 100, 123, "now", 2345);
	}
	ProductInstance::instance().notify(nl::notifications::add_multiple, ProductInstance::instance().size());
	mPanelManager->AddPane(mDataView.get(), wxAuiPaneInfo().Name("DataView").Caption("ProductView").CenterPane());
}

void ProductView::SetDefaultArt()
{
	wxColour colour = wxTheColourDatabase->Find("Aqua");
	wxAuiDockArt* art = mPanelManager->GetArtProvider();
	art->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 24);
	art->SetMetric(wxAUI_DOCKART_GRIPPER_SIZE, 5);
	art->SetMetric(wxAUI_DOCKART_SASH_SIZE, 5);
	art->SetColour(wxAUI_DOCKART_SASH_COLOUR, *wxWHITE);
	art->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, colour);
	art->SetColour(wxAUI_DOCKART_BORDER_COLOUR, *wxWHITE);
	art->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	art->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_HORIZONTAL);
	mPanelManager->SetFlags(mPanelManager->GetFlags() | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_VENETIAN_BLINDS_HINT);
}

void ProductView::OnAddProduct(wxCommandEvent& evt)
{
	nl::uuid id;
	id.generate();
	
	//std::string out = fmt::format("{:q}", id);
	spdlog::get("log")->info("{:q}", id);
}

void ProductView::OnRemoveProduct(wxCommandEvent& evt)
{
	spdlog::get("log")->info("OnRemoveProduct clicked");
}

void ProductView::OnSearchProduct(wxCommandEvent& evt)
{
	spdlog::get("log")->info("{}", evt.GetString().ToStdString());
	if (mSearchFlags.test(0)){
		spdlog::get("log")->info("name: {}", evt.GetString().ToStdString());
	}
	else if (mSearchFlags.test(1)){
		spdlog::get("log")->info("category: {}", evt.GetString().ToStdString());
	}else if (mSearchFlags.test(2)){
		spdlog::get("log")->info("price: {}", evt.GetString().ToStdString());
	}
}

void ProductView::OnEraseBackground(wxEraseEvent& evt)
{
	evt.Skip();
}

void ProductView::OnSearchFlag(wxCommandEvent& evt)
{
	mSearchFlags.reset();
	switch (evt.GetId())
	{
	case ID_SEARCH_BY_NAME:
		mSearchFlags.set(0, true);
		break;
	case ID_SEARCH_BY_CATEGORY:
		mSearchFlags.set(1, true);
		break;
	case ID_SEARCH_BY_PRICE:
		mSearchFlags.set(2, true);
		break;
	default:
		break;
	}

}

void ProductView::OnSearchByName(const std::string& SearchString)
{

}

void ProductView::OnSearchByCategory(const std::string& SearchString)
{

}

void ProductView::OnSearchByPrice(const std::string& SearchString)
{

}
