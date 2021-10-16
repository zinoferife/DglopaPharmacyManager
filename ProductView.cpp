#include "common.h"
#include "ProductView.h"
BEGIN_EVENT_TABLE(ProductView, wxPanel)
	EVT_TOOL(ProductView::ID_ADD_PRODUCT, ProductView::OnAddProduct)
	EVT_TOOL(ProductView::ID_REMOVE_PRODUCT, ProductView::OnRemoveProduct)
	EVT_TOOL(ProductView::ID_GROUP_BY, ProductView::OnCheckInStock)
	EVT_TOOL(ProductView::ID_REMOVE_GROUP_BY, ProductView::OnResetAttributes)
	EVT_TOOL(ProductView::ID_BACK, ProductView::OnBack)
	EVT_TOOL(ProductView::ID_QUICK_SORT_TEST, ProductView::OnQuickSortTest)
	EVT_SEARCH(ProductView::ID_SEARCH, ProductView::OnSearchProduct)
	EVT_SEARCH_CANCEL(ProductView::ID_SEARCH, ProductView::OnSearchCleared)
	EVT_TEXT(ProductView::ID_SEARCH, ProductView::OnSearchProduct)
	EVT_MENU(ProductView::ID_SEARCH_BY_NAME, ProductView::OnSearchFlag)
	EVT_MENU(ProductView::ID_SEARCH_BY_CATEGORY, ProductView::OnSearchFlag)
	EVT_MENU(ProductView::ID_SEARCH_BY_PRICE, ProductView::OnSearchFlag)
	EVT_DATAVIEW_ITEM_ACTIVATED(ProductView::ID_DATA_VIEW, ProductView::OnProductItemActivated)
	EVT_DATAVIEW_COLUMN_HEADER_CLICK(ProductView::ID_DATA_VIEW, ProductView::OnColumnHeaderClick)
	EVT_ERASE_BACKGROUND(ProductView::OnEraseBackground)
	EVT_LIST_COL_CLICK(ProductView::ID_INVENTORY_VIEW, ProductView::OnInventoryViewColClick)
	EVT_TOOL(ProductView::ID_INVENTORY_VIEW_TOOL_ADD, ProductView::OnInventoryAddTool)
END_EVENT_TABLE()

ProductView::ProductView()
{
}

ProductView::~ProductView()
{
	UnregisterNotification();
	mDataView.release();
	mInventoryView.release();
}

ProductView::ProductView(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize size)
:wxPanel(parent, id, position, size){
	mPanelManager = std::make_unique<wxAuiManager>(this);
	RegisterNotification();
	CreateItemAttr();
	SetDefaultArt();
	CreateToolBar();
	CreateInventoryList();
	CreateDataView();
	mPanelManager->Update();
}

void ProductView::CreateToolBar()
{
	bar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW);
	bar->SetToolBitmapSize(wxSize(16, 16));
	bar->AddTool(ID_BACK, wxEmptyString, wxArtProvider::GetBitmap("arrow_back"));
	bar->AddTool(ID_FORWARD, wxEmptyString, wxArtProvider::GetBitmap("arrow_next"));

	//search bar
	search = new wxSearchCtrl(bar, ID_SEARCH, wxEmptyString, wxDefaultPosition, wxSize(300, -1), wxWANTS_CHARS);
	search->SetDescriptiveText("Search products by name");
//	search->AutoComplete(new SearchAutoComplete<Products, Products::name>(ProductInstance::instance()));
	search->ShowCancelButton(true);
	wxMenu* menu = new wxMenu;
	menu->AppendRadioItem(ID_SEARCH_BY_NAME, "Search by name");
	menu->AppendRadioItem(ID_SEARCH_BY_CATEGORY, "Search by category");
	menu->AppendRadioItem(ID_SEARCH_BY_PRICE, "Search by price");
	menu->Check(ID_SEARCH_BY_NAME, true);
	mSearchFlags.set(0, true);

	search->SetMenu(menu);
	bar->AddControl(search);
	
	
	bar->AddStretchSpacer();
	bar->AddTool(ID_QUICK_SORT_TEST, wxEmptyString, wxArtProvider::GetBitmap("search"));
	bar->AddTool(ID_GROUP_BY, wxEmptyString, wxArtProvider::GetBitmap("maximize"));
	bar->AddTool(ID_REMOVE_GROUP_BY, wxEmptyString, wxArtProvider::GetBitmap("minimize"));
	bar->AddTool(ID_ADD_PRODUCT, wxEmptyString, wxArtProvider::GetBitmap("action_add"));
	bar->AddTool(ID_REMOVE_PRODUCT, wxEmptyString, wxArtProvider::GetBitmap("action_remove"));

	//realise and add to manager
	bar->Realize();
	mPanelManager->AddPane(bar, wxAuiPaneInfo().Name(wxT("Tool")).Caption(wxT("Tool bar")).ToolbarPane().Top()
		.Resizable().MinSize(wxSize(-1, 30)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void ProductView::CreateInentoryToolBar()
{
	wxAuiToolBar* bar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW);
	bar->SetToolBitmapSize(wxSize(16, 16));
	bar->AddTool(ID_BACK, wxEmptyString, wxArtProvider::GetBitmap("arrow_back"));
	mInventoryProductName = bar->AddTool(ID_INVENTORY_PRODUCT_NAME, wxEmptyString, wxNullBitmap);
	bar->AddStretchSpacer();
	bar->AddTool(ID_INVENTORY_VIEW_TOOL_ADD, wxEmptyString, wxArtProvider::GetBitmap("action_add"));
	bar->AddTool(ID_INVENTORY_VIEW_TOOL_REMOVE, wxEmptyString, wxArtProvider::GetBitmap("action_remove"));

	bar->Realize();
	mPanelManager->AddPane(bar, wxAuiPaneInfo().Name(wxT("InventoryTool")).Caption(wxT("Inventory tool bar")).ToolbarPane().Top()
		.Resizable().MinSize(wxSize(-1, 30)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false).Hide());
}

void ProductView::CreateDataView()
{
	mDataView  = std::make_unique<wxDataViewCtrl>(this,ID_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES | wxNO_BORDER);
	mModel = new DataModel<Products>(ProductInstance::instance());
	//add columns

	mDataView->AppendTextColumn(ProductInstance::instance().get_name(0), 0, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mDataView->AppendTextColumn(ProductInstance::instance().get_name(1), 1, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mDataView->AppendTextColumn(ProductInstance::instance().get_name(2), 2, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mDataView->AppendTextColumn(ProductInstance::instance().get_name(3), 3, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mDataView->AppendTextColumn(ProductInstance::instance().get_name(4), 4, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mDataView->AllowMultiColumnSort(true);
	mDataView->ToggleSortByColumn(Products::name);
	wxDataViewModel* ProductModel = mModel;
	mDataView->AssociateModel(ProductModel);
	ProductModel->DecRef();
	//load test
	Products::set_default_row(100, "test", 0, 0, "0.00", 9);

	//test data
	ProductInstance::instance().add_in_order<Products::name>(GenRandomId<std::uint64_t>(), "Paracetamol", gen_random(), gen_random(), "now", 2345);
	ProductInstance::instance().add_in_order<Products::name>(GenRandomId<std::uint64_t>(), "Panadol", gen_random(), gen_random(), "now", 2345);
	ProductInstance::instance().add_in_order<Products::name>(GenRandomId<std::uint64_t>(), "Vitamin C", gen_random(), 0, "now", 2345);
	ProductInstance::instance().add_in_order<Products::name>(GenRandomId<std::uint64_t>(), "Vitamin D", gen_random(), gen_random(), "now", 2345);
	ProductInstance::instance().add_in_order<Products::name>(GenRandomId<std::uint64_t>(), "Vitamin E", gen_random(), gen_random(), "now", 2345);
	ProductInstance::instance().add_in_order<Products::name>(GenRandomId<std::uint64_t>(), "Malaria syrup", gen_random(), gen_random(), "now", 2345);
	ProductInstance::instance().add_in_order<Products::name>(1, "Malaria tablet", gen_random(), gen_random(), "now", 2345);
	ProductInstance::instance().add_in_order<Products::name>(GenRandomId<std::uint64_t>(), "Malaria injection", gen_random(), 0, "now", 2345);
	ProductInstance::instance().add_in_order<Products::name>(GenRandomId<std::uint64_t>(),"AntiBacteria", gen_random(), gen_random(), "now", 2345);
	ProductInstance::instance().add_in_order<Products::name>(0, "AntiBacteria drug", gen_random(), gen_random(), "now", 2345);

	Products::notification_data data{};
	data.count = ProductInstance::instance().size();
	ProductInstance::instance().notify<nl::notifications::add_multiple>(data);
	mPanelManager->AddPane(mDataView.get(), wxAuiPaneInfo().Name("DataView").Caption("ProductView").CenterPane());
}

void ProductView::CreateItemAttr()
{
	mInStock = std::make_shared<wxDataViewItemAttr>();
	mExpired = std::make_shared<wxDataViewItemAttr>();
	mInStock->SetBackgroundColour(wxTheColourDatabase->Find("Navajo_white"));
	mExpired->SetBackgroundColour(wxTheColourDatabase->Find("Tomato"));
}

void ProductView::CreateInventoryList()
{
	//this is for testing 
	InventoryInstance::instance().add(gen_random(), 0, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 0, nl::clock::now(), nl::clock::now(),gen_random(), gen_random(), gen_random(), 0, gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 0, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 0, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), 0, gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());
	InventoryInstance::instance().add(gen_random(), 1, nl::clock::now(), nl::clock::now(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random(), gen_random());

	//CreateInventory(0);
}

void ProductView::CreateInventory(std::uint64_t product_id)
{
	mInventoryView = std::make_unique<InventoryView>(product_id, this, ID_INVENTORY_VIEW);
	wxImageList* imageList = new wxImageList(16, 16);
	imageList->Add(wxArtProvider::GetBitmap("action_delete"));
	imageList->Add(wxArtProvider::GetBitmap("action_check"));
	CreateInentoryToolBar();
	mInventoryView->AssignImageList(imageList, wxIMAGE_LIST_SMALL);
	mPanelManager->AddPane(mInventoryView.get(), wxAuiPaneInfo().Name("Inventory").Caption("InventoryView").CenterPane().Hide());
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

void ProductView::ShowInventoryToolBar(const Products::row_t& row_)
{
	auto& pane = mPanelManager->GetPane("InventoryTool");
	wxAuiToolBar* bar = wxDynamicCast(pane.window, wxAuiToolBar);
	if (bar){
		const auto& name = nl::row_value<Products::name>(row_);
		mInventoryProductName->SetLabel(fmt::format("{} - Inventory card", name));
		bar->Refresh();
	}
	mPanelManager->GetPane("Tool").Hide();
	pane.Show();
}

void ProductView::HideInventoryToolBar()
{
	mPanelManager->GetPane("InventoryTool").Hide();
	mPanelManager->GetPane("Tool").Show();
}

void ProductView::OnAddProduct(wxCommandEvent& evt)
{
	ProductEntryDialog dialog(this);
	wxWindowID RetCode = dialog.ShowModal();
	if (RetCode == wxID_OK){

		Products::notification_data data{};
		ProductDetails::notification_data data_details{};
		data.row_iterator = ProductInstance::instance().add_in_order<Products::name>(std::move(dialog.product));
		data_details.row_iterator = ProductDetailsInstance::instance().add(std::move(dialog.productDetail));

		//notify the system on the added item
		ProductInstance::instance().notify(nl::notifications::add, data);
		ProductDetailsInstance::instance().notify<nl::notifications::add>(data_details);

		//highlight the just added item
		size_t index = ProductInstance::instance().get_index(data.row_iterator);
		index++;
		mDataView->Select(wxDataViewItem(wxUIntToPtr(index)));
		mDataView->EnsureVisible(wxDataViewItem(wxUIntToPtr(index)));
		mDataView->SetFocus();
	}
}

void ProductView::OnRemoveProduct(wxCommandEvent& evt)
{
	auto selected = mDataView->GetSelection();
	if (!selected.IsOk()){
		wxMessageBox("No product selected to remove", "Remove product", wxOK | wxICON_INFORMATION);
		return;
	}
	int index = mModel->GetDataViewItemIndex(selected);
	if (index != wxNOT_FOUND){
		Products::notification_data data;
		data.row_iterator = ProductInstance::instance().get_iterator(index);
		if (wxMessageBox(fmt::format("Are you sure you want to remove \"{}\"",
			nl::row_value<Products::name>(*data.row_iterator)), "Remove product",  wxICON_INFORMATION | wxYES_NO) == wxYES) {
			ProductInstance::instance().notify(nl::notifications::remove, data);
			ProductInstance::instance().del_row(data.row_iterator);
		}
	}
}

void ProductView::OnCheckInStock(wxCommandEvent& evt)
{
	wxBusyCursor cu;
	mDataView->Freeze();
	auto group = ProductInstance::instance().map_group_by<Products::stock_count>();
	auto instockIter = group.find(0);
	if (instockIter != group.end())
	{
		auto outstockvector = instockIter->second.isolate_column<Products::id>();
		mModel->AddAttribute(mInStock, std::move(outstockvector));
	}
	else{
		wxMessageBox("No product with a 0 stock count", "Stock check", wxICON_INFORMATION | wxOK);
	}
	mDataView->Thaw();
	mDataView->Refresh();
}

void ProductView::OnResetAttributes(wxCommandEvent& evt)
{
	wxBusyCursor cu;
	mDataView->Freeze();
	mModel->ResetAttributes();
	mDataView->Thaw();
	mDataView->Refresh();
}

void ProductView::OnSearchProduct(wxCommandEvent& evt)
{
	std::string search = evt.GetString().ToStdString();
	if (search.empty()) return;

	if (mSearchFlags.test(0)){
		OnSearchByName(search);
	}else if (mSearchFlags.test(1)){
		OnSearchByCategory(search);
	}else if (mSearchFlags.test(2)){
		OnSearchByPrice(search);
	}
}


//need to think about this more
void ProductView::OnSearchCleared(wxCommandEvent& evt)
{
	ProductInstance::instance().notify<nl::notifications::clear>({});
	Products::notification_data data;
	data.count = ProductInstance::instance().size();
	ProductInstance::instance().notify<nl::notifications::load>(data);
}

void ProductView::OnEraseBackground(wxEraseEvent& evt)
{
	evt.Skip();
}

//remove this, 
void ProductView::OnQuickSortTest(wxCommandEvent& evt)
{
	//ProductInstance::instance().quick_sort<Products::id>();
}

void ProductView::OnBack(wxCommandEvent& evt)
{
	if (!mPanelManager->GetPane("DataView").IsShown()){
		HideInventoryToolBar();
		mPanelManager->GetPane("Inventory").Hide();
		mPanelManager->GetPane("DataView").Show();
		mPanelManager->Update();
	}
}

void ProductView::OnInventoryViewColClick(wxListEvent& evt)
{
	if (mInventoryView)
	{
		mInventoryView->Freeze();
		mInventoryView->OnColumnHeaderClick(evt);
		mInventoryView->Thaw();
		mInventoryView->Refresh();
	}
}

void ProductView::OnInventoryAddTool(wxCommandEvent& evt)
{
	mInventoryView->OnAddInventory(evt);
}

void ProductView::OnInventoryRemoveTool(wxCommandEvent& evt)
{
}

void ProductView::OnSearchFlag(wxCommandEvent& evt)
{
	mSearchFlags.reset();
	switch (evt.GetId())
	{
	case ID_SEARCH_BY_NAME:
		mSearchFlags.set(0, true);
		search->SetDescriptiveText("Search product by name");
		break;
	case ID_SEARCH_BY_CATEGORY:
		mSearchFlags.set(1, true);
		search->SetDescriptiveText("Search product by category");
		break;
	case ID_SEARCH_BY_PRICE:
		mSearchFlags.set(2, true);
		search->SetDescriptiveText("Search product by price");
		break;
	default:
		break;
	}

}

void ProductView::OnSearchByName(const std::string& SearchString)
{
	if (SearchString.empty()) return;
	DoSearch(SearchString);
}

void ProductView::OnSearchByCategory(const std::string& SearchString)
{
	if (SearchString.empty()) return;
	
}

void ProductView::OnSearchByPrice(const std::string& SearchString)
{
	if (SearchString.empty()) return;

}

void ProductView::OnProductItemSelected(wxDataViewEvent& evt)
{
}

void ProductView::OnProductItemActivated(wxDataViewEvent& evt)
{
	//deferend creation, the pane is invalid, have to get another pane to show
	auto item = evt.GetItem();
	if (!item.IsOk()) return;
	int index = mModel->GetDataViewItemIndex(evt.GetItem());
	if (index == -1) return;
	auto& InventoryPane = mPanelManager->GetPane("Inventory");
	if (InventoryPane.IsOk()){
		mPanelManager->GetPane("DataView").Hide();
		auto& row = ProductInstance::instance()[index];
		if (mInventoryView->GetProductId() != nl::row_value<Products::id>(row)){
			mInventoryView->ResetProductInventoryList(nl::row_value<Products::id>(row));
		}
		InventoryPane.Show();
		ShowInventoryToolBar(row);
		mPanelManager->Update();
	}else{
		//inventory not created, create
		mPanelManager->GetPane("DataView").Hide();
		auto& row = ProductInstance::instance()[index];
		CreateInventory(nl::row_value<Products::id>(row));
		//get pane and update
		mPanelManager->GetPane("Inventory").Show();
		ShowInventoryToolBar(row);
		mPanelManager->Update();
	}
}

void ProductView::OnColumnHeaderClick(wxDataViewEvent& evt)
{
	wxBusyCursor cu;
	mDataView->Freeze();
	auto columnIndex = evt.GetColumn();
	int oldIndex = -1;
	wxDataViewColumn* col = mDataView->GetSortingColumn();
	if (col){
		oldIndex = col->GetModelColumn();
	}
	col = mDataView->GetColumn(columnIndex);
	if (columnIndex != wxNOT_FOUND){
		if(oldIndex != -1 && oldIndex != columnIndex) mDataView->ToggleSortByColumn(oldIndex);
		switch (columnIndex)
		{
		case Products::package_size:
			mDataView->ToggleSortByColumn(Products::package_size);
			col->SetSortOrder(!col->IsSortOrderAscending());
			mModel->Resort();
			break;
		case Products::id:
			mDataView->ToggleSortByColumn(Products::id);
			col->SetSortOrder(!col->IsSortOrderAscending());
			mModel->Resort();
			break;
		case Products::unit_price:
			mDataView->ToggleSortByColumn(Products::unit_price);
			col->SetSortOrder(!col->IsSortOrderAscending());
			mModel->Resort();
			break;
		case Products::name:
			mDataView->ToggleSortByColumn(Products::name);
			col->SetSortOrder(!col->IsSortOrderAscending());
			mModel->Resort();
			break;
		case Products::stock_count:
			mDataView->ToggleSortByColumn(Products::stock_count);
			col->SetSortOrder(!col->IsSortOrderAscending());
			mModel->Resort();
			break;
		default:
			break;
		}
	}
	mDataView->Thaw();
	mDataView->Refresh();
}

void ProductView::DoSearch(const std::string& searchString)
{
	Searcher<Products::name, Products> search(ProductInstance::instance());
	ProductInstance::instance().notify<nl::notifications::clear>({});
	mModel->ReloadIndices(search.Search(searchString));
}

void ProductView::RegisterNotification()
{
	InventoryInstance::instance().sink<nl::notifications::add>().add_listener<ProductView, & ProductView::OnInventoryAddNotification>(this);
	UsersInstance::instance().sink<nl::notifications::evt>().add_listener<ProductView, & ProductView::OnUsersNotification>(this);
}

void ProductView::UnregisterNotification()
{
	InventoryInstance::instance().sink<nl::notifications::add>().remove_listener<ProductView, &ProductView::OnInventoryAddNotification>(this);
	UsersInstance::instance().sink<nl::notifications::evt>().remove_listener<ProductView, & ProductView::OnUsersNotification>(this);

}

void ProductView::OnInventoryAddNotification(const Inventories::table_t& table, const Inventories::notification_data& data)
{
	uint32_t balance = nl::row_value<Inventories::balance>(*data.row_iterator);
	auto it = ProductInstance::instance().find_on<Products::id>(nl::row_value<Inventories::product_id>(*data.row_iterator));
	if (it != ProductInstance::instance().end())
	{
		nl::row_value<Products::stock_count>(*it) = balance;

		Products::notification_data data_p{};
		data_p.row_iterator = it;
		data_p.column = Products::stock_count;
		ProductInstance::instance().notify(nl::notifications::update, data_p);
		mModel->RemoveAttribute(nl::row_value<Products::id>(*it));
	}
}

void ProductView::OnUsersNotification(const Users::table_t& table, const Users::notification_data& data)
{
	switch (data.event_type)
	{
	//sign in 
	case 1: 

		break;
	//sign out
	case 0:

		break;
	}

}


//remove
int ProductView::gen_random()
{
	static std::mt19937 engine(std::random_device{}());
	static std::uniform_int_distribution<int> dist(0, 100);
	static auto random = std::bind(dist, engine);

	return random();
}
