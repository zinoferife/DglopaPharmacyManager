#include "common.h"
#include "ProductView.h"
BEGIN_EVENT_TABLE(ProductView, wxPanel)
	EVT_TOOL(ProductView::ID_ADD_PRODUCT, ProductView::OnAddProduct)
	EVT_TOOL(ProductView::ID_REMOVE_PRODUCT, ProductView::OnRemoveProduct)
	EVT_TOOL(ProductView::ID_GROUP_BY, ProductView::OnCheckInStock)
	EVT_TOOL(ProductView::ID_REMOVE_GROUP_BY, ProductView::OnResetAttributes)
	EVT_TOOL(ProductView::ID_BACK, ProductView::OnBack)
	EVT_TOOL(ProductView::ID_QUICK_SORT_TEST, ProductView::OnQuickSortTest)
	EVT_TOOL(ProductView::ID_EXPIRY_VIEW, ProductView::OnExpiryView)
	EVT_TOOL(ProductView::ID_TO_JSON, ProductView::OnToJson)
	EVT_TOOL(ProductView::ID_ADD_CATEGORY, ProductView::OnCategory)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(ProductView::ID_FILE, ProductView::OnFile)
	EVT_MENU(ProductView::ID_SELECT_MULTIPLE, ProductView::OnSelectMultiple)
	EVT_MENU(ProductView::ID_UNSELECT_MULTIPLE, ProductView::OnUnSelectMultiple)
	EVT_SEARCH(ProductView::ID_SEARCH, ProductView::OnSearchProduct)
	EVT_SEARCH_CANCEL(ProductView::ID_SEARCH, ProductView::OnSearchCleared)
	EVT_TEXT(ProductView::ID_SEARCH, ProductView::OnSearchProduct)
	EVT_MENU(ProductView::ID_SEARCH_BY_NAME, ProductView::OnSearchFlag)
	EVT_MENU(ProductView::ID_SEARCH_BY_CATEGORY, ProductView::OnSearchFlag)
	EVT_MENU(ProductView::ID_SEARCH_BY_PRICE, ProductView::OnSearchFlag)
	EVT_DATAVIEW_ITEM_ACTIVATED(ProductView::ID_DATA_VIEW, ProductView::OnProductItemActivated)
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(ProductView::ID_DATA_VIEW, ProductView::OnProductContextMenu)
	EVT_DATAVIEW_ITEM_BEGIN_DRAG(ProductView::ID_DATA_VIEW, ProductView::OnDragStart)
	EVT_MENU(ProductView::ID_PRODUCT_CONTEXT_EDIT, ProductView::OnProductDetailView)
	EVT_MENU(ProductView::ID_PRODUCT_CONTEXT_REMOVE, ProductView::OnRemoveProduct)
	EVT_MENU(ProductView::ID_PRODUCT_CONTEXT_EXPORT_JSON, ProductView::OnProductExportJson)
	EVT_ERASE_BACKGROUND(ProductView::OnEraseBackground)
	EVT_LIST_COL_CLICK(ProductView::ID_INVENTORY_VIEW, ProductView::OnInventoryViewColClick)
	EVT_TOOL(ProductView::ID_INVENTORY_VIEW_TOOL_ADD, ProductView::OnInventoryAddTool)
	EVT_CHOICE(ProductView::ID_CATEGORY_LIST_CONTROL, ProductView::OnCategorySelected)
	EVT_AUI_PANE_CLOSE(ProductView::OnPaneClose)
END_EVENT_TABLE()

ProductView::ProductView()
{
}

ProductView::~ProductView()
{
	mPanelManager->UnInit();
	UnregisterNotification();
	mDataView.release();
	mInventoryView.release();
	mDetailView.release();
}

ProductView::ProductView(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize size)
:wxPanel(parent, id, position, size){
	mPanelManager = std::make_unique<wxAuiManager>(this);
	RegisterNotification();
	ConnectDatabaseSignals();
	CreateItemAttr();
	SetDefaultArt();
	CreateToolBar();
	CreateInventoryList();
	//should be in mainFrame?? no not really
	CreateDatabaseMgr();
	CreateDataView();
	InitJsonConverter();
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
	search->ShowCancelButton(true);
	wxMenu* menu = new wxMenu;
	menu->AppendRadioItem(ID_SEARCH_BY_NAME, "Search by name");
	menu->AppendRadioItem(ID_SEARCH_BY_CATEGORY, "Search by category");
	menu->AppendRadioItem(ID_SEARCH_BY_PRICE, "Search by price");
	menu->Check(ID_SEARCH_BY_NAME, true);
	mSearchFlags.set(0, true);

	search->SetMenu(menu);
	bar->AddControl(search);
	bar->AddSeparator();
	wxArrayString choices;
	choices.push_back(all_categories);
	bar->AddTool(ID_FILE, wxEmptyString, wxArtProvider::GetBitmap("file"))->SetHasDropDown(true);
	bar->AddStretchSpacer();
	categories = new wxChoice(bar, ID_CATEGORY_LIST_CONTROL, wxDefaultPosition, wxSize(200, -1), choices);
	categories->SetSelection(0);
	bar->AddControl(categories, "Categories");
	categories->Bind(wxEVT_PAINT, [=](wxPaintEvent& evt) {
		wxPaintDC dc(categories);
		wxRect rect(0, 0, dc.GetSize().GetWidth(), dc.GetSize().GetHeight());

		dc.SetBrush(*wxWHITE);
		dc.SetPen(*wxGREY_PEN);
		dc.DrawRectangle(rect);
		dc.DrawBitmap(wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_OTHER, wxSize(10, 10)), wxPoint(rect.GetWidth() - 15, (rect.GetHeight() / 2) - 5));
		auto sel = categories->GetStringSelection();
		if (!sel.IsEmpty()){
			dc.DrawLabel(sel, rect, wxALIGN_CENTER);
		}
	});
	bar->AddSeparator();
	bar->AddTool(ID_EXPIRY_VIEW, wxT("Expired Products"), wxArtProvider::GetBitmap("bandage"));
	bar->AddTool(ID_GROUP_BY, wxT("Out Of Stock"), wxArtProvider::GetBitmap("bag"));
	bar->AddTool(ID_REMOVE_GROUP_BY, wxT("Reset"), wxArtProvider::GetBitmap("minimize"));
	bar->AddTool(ID_ADD_PRODUCT, wxT("Add Product"), wxArtProvider::GetBitmap("action_add"));
	bar->AddTool(ID_ADD_CATEGORY, wxT("Add Category"), wxArtProvider::GetBitmap("action_add"));
	bar->AddTool(ID_REMOVE_PRODUCT, wxT("Remove Product"), wxArtProvider::GetBitmap("action_remove"));
	bar->AddTool(ID_TO_JSON, wxT("Statistics"), wxArtProvider::GetBitmap("save"));

	//realise and add to manager
	bar->Realize();
	mPanelManager->AddPane(bar, wxAuiPaneInfo().Name(wxT("Tool")).Caption(wxT("Tool bar")).ToolbarPane().Top()
		.Resizable().MinSize(wxSize(-1, 30)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void ProductView::CreateInentoryToolBar()
{
	wxAuiToolBar* bar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT);
	bar->SetToolBitmapSize(wxSize(16, 16));
	bar->AddTool(ID_BACK, wxEmptyString, wxArtProvider::GetBitmap("arrow_back"));
	mInventoryProductName = bar->AddTool(ID_INVENTORY_PRODUCT_NAME, wxEmptyString, wxNullBitmap);
	bar->AddStretchSpacer();
	bar->AddTool(ID_INVENTORY_VIEW_TOOL_ADD, wxT("Add inventory"), wxArtProvider::GetBitmap("action_add"));
	bar->AddTool(ID_INVENTORY_VIEW_TOOL_REMOVE, wxT("Remove inventory"), wxArtProvider::GetBitmap("action_remove"));

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
	mDataView->AppendTextColumn(ProductInstance::instance().get_name(1), 1, wxDATAVIEW_CELL_INERT, 100, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mDataView->AppendTextColumn(ProductInstance::instance().get_name(2), 2, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mDataView->AppendTextColumn(ProductInstance::instance().get_name(3), 3, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mDataView->AppendTextColumn(ProductInstance::instance().get_name(4), 4, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mDataView->AppendBitmapColumn("In Stock", 5, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	
	mDataView->AllowMultiColumnSort(false);
	CreateSpecialColumnHandlers();
	wxDataViewModel* ProductModel = mModel;
	mDataView->AssociateModel(ProductModel);
	ProductModel->DecRef();
	//load test

	mDatabaseMgr->LoadTable();
	mDatabaseDetailMgr->LoadTable();
	mDatabaseCatgoryMgr->LoadTable();
	LoadCategoryToChoiceBox();
	mPanelManager->AddPane(mDataView.get(), wxAuiPaneInfo().Name("DataView").Caption("ProductView").CenterPane());
}

void ProductView::CreateSpecialColumnHandlers()
{
	mModel->SetSpecialColumnHandler(5, [](size_t col, size_t row) -> wxVariant {
		auto rowIter = ProductInstance::instance()[row];
		if (nl::row_value<Products::stock_count>(rowIter) == 0) {
			return wxVariant(wxArtProvider::GetBitmap("action_delete"));
		}
		else {
			return wxVariant(wxArtProvider::GetBitmap("action_check"));
		}
		});

	mModel->SetSpecialColumnHandler(6, [self = this](size_t col, size_t row)-> wxVariant {
		if (self->mSelectedProductIndex.find(row) != self->mSelectedProductIndex.end()) {
			return wxVariant(true);
		}
		else {
			return wxVariant(false);
		}
		});
	
	mModel->SetSpecialSetColumnHandler(6, [self = this](size_t col, size_t row, const wxVariant& varient) -> bool {
		if (varient.GetBool()) {
			self->mSelectedProductIndex.insert(row);
			return false;
		}
		else {
			self->mSelectedProductIndex.erase(row);
			return false;
		}
	});

}

void ProductView::LoadCategoryToChoiceBox()
{
	auto cat = CategoriesInstance::instance().isolate_column<Categories::name>();
	//size_t i = 0;
	wxArrayString strArry;
	strArry.reserve(cat.size());
	for (auto&& name : cat){
		strArry.push_back(std::move(name));
	}
	categories->Insert(strArry, 1);
}

void ProductView::CreateItemAttr()
{
	mInStock = std::make_shared<wxDataViewItemAttr>();
	mExpired = std::make_shared<wxDataViewItemAttr>();
	mModified = std::make_shared<wxDataViewItemAttr>();
	mHeaderAttributes = std::make_shared<wxDataViewItemAttr>();
	mInStock->SetBackgroundColour(wxTheColourDatabase->Find("Navajo_white"));
	mExpired->SetBackgroundColour(wxTheColourDatabase->Find("Tomato"));
	mModified->SetBold(true);
	mHeaderAttributes->SetBackgroundColour(wxTheColourDatabase->Find("Papaya whip"));
}

void ProductView::CreateInventoryList()
{
	//this is for testing 
	
}

void ProductView::CreateDatabaseMgr()
{
	mDatabaseMgr.reset(new DatabaseManager<Products>(ProductInstance::instance(), DatabaseInstance::instance()));
	mDatabaseMgr->CreateTable();

	mDatabaseCatgoryMgr.reset(new DatabaseManager<Categories>(CategoriesInstance::instance(), DatabaseInstance::instance()));
	mDatabaseCatgoryMgr->CreateTable();

	mDatabaseDetailMgr.reset(new DatabaseManager<ProductDetails>(ProductDetailsInstance::instance(), DatabaseInstance::instance()));
	mDatabaseDetailMgr->CreateTable();
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
	mInventoryView->GetProductInventory().sink<nl::notifications::add>()
		.add_listener<ProductView, & ProductView::OnInventoryAddNotification>(this);
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

void ProductView::ImportProductsFromJson(std::fstream& file)
{
	auto format_cat_name = [&](std::string& cat_name) -> std::string& {
		std::transform(cat_name.begin(), cat_name.end(), cat_name.begin(), [&](char& c) {
			return std::toupper(c);
		});
		return cat_name;
	};

	js::json json;
	wxProgressDialog progress("Importing json", "Reading file...");
	file >> json;
	if (json.empty()){
		wxMessageBox("Json file is corrupted, invalid json file", "JSON IMPORT", wxICON_ERROR | wxOK);
		return;
	}
	Products products;
	ProductDetails products_detail;
	Categories category;
	progress.Update(10, "Reading products");
	for (auto cat_obj = json.begin(); cat_obj != json.end(); (void)cat_obj++){
		std::string cat_name = cat_obj.key();
		auto cat_iter = category.find_on<Categories::name>(format_cat_name(cat_name));
		if (cat_iter == category.end()){
			cat_iter = category.add(GenRandomId(), cat_name);
		}
		for (auto product_obj = cat_obj->begin(); product_obj != cat_obj->end(); (void)product_obj++){
			auto& value = product_obj.value();
			Products::row_t product;
			ProductDetails::row_t product_detail;
			size_t progress_value = 10;
			try {
				nl::row_value<Products::id>(product) = GenRandomId();
				nl::row_value<Products::category_id>(product) = nl::row_value<Categories::id>(*cat_iter);
				nl::row_value<Products::name>(product) = product_obj.key();
				nl::row_value<Products::package_size>(product) = value["Package size"];
				nl::row_value<Products::stock_count>(product) = value["Stock count"];
				nl::row_value<Products::unit_price>(product) = js::to_string(value["Unit price"]);
				

				nl::row_value<ProductDetails::id>(product_detail) = nl::row_value<Products::id>(product);
				nl::row_value<ProductDetails::active_ing>(product_detail) = fmt::to_string(value["Product Active ingredent"]);
				nl::row_value<ProductDetails::description>(product_detail) = fmt::to_string(value["Product Description"]);
				nl::row_value<ProductDetails::p_class>(product_detail) = fmt::to_string(value["Product class"]);
				nl::row_value<ProductDetails::dir_for_use>(product_detail) = fmt::to_string(value["Direction for use"]);
				nl::row_value<ProductDetails::health_conditions>(product_detail) = fmt::format("{}", fmt::join(value["Health tags"], ","));
				nl::row_value<ProductDetails::stock_limit>(product_detail) = 0;
				products.add(product);
				products_detail.add(product_detail);
				if (progress_value < 80) {
					progress.Update(++progress_value, product_obj.key());
				}
			}
			catch (js::json::type_error& error){
				spdlog::get("log")->error("JSON ERROR: {}", error.what());
				wxMessageBox("JSON FATAL ERROR", "JSON IMPORT", wxICON_ERROR | wxOK);
				return;
			}
		}
	}
	Products::notification_data data;
	data.count = ProductInstance::instance().append_relation(products);
	ProductInstance::instance().notify<nl::notifications::add_multiple>(data);

	ProductDetails::notification_data data_details;
	data_details.count = ProductDetailsInstance::instance().append_relation(products_detail);
	ProductDetailsInstance::instance().notify<nl::notifications::add_multiple>(data_details);

	Categories::notification_data data_category;
	data_category.count = CategoriesInstance::instance().append_relation(category);
	CategoriesInstance::instance().notify<nl::notifications::add_multiple>(data_category);

	mDatabaseMgr->InsertTable(products);
	mDatabaseDetailMgr->InsertTable(products_detail);
	mDatabaseCatgoryMgr->InsertTable(category);

	progress.Update(100, "Sucessfully read json file");
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
	if (mSelectedProductIndex.empty())
	{
		int index = mModel->GetDataViewItemIndex(selected);
		if (index != wxNOT_FOUND) {
			Products::notification_data data;
			data.row_iterator = ProductInstance::instance().get_iterator(index);
			if (wxMessageBox(fmt::format("Are you sure you want to remove \"{}\"",
				nl::row_value<Products::name>(*data.row_iterator)), "Remove product", wxICON_INFORMATION | wxYES_NO) == wxYES) {
				ProductInstance::instance().notify(nl::notifications::remove, data);
				mDatabaseMgr->DeleteTable(*data.row_iterator);


				ProductInstance::instance().del_row(data.row_iterator);

				//remove all instances of this delete
			}
		}
	}
	else{
		auto asRef = ProductInstance::instance().isolate_column_as_ref<Products::name>();
		wxArrayString names;
		for (auto& i : mSelectedProductIndex) {
			names.push_back(static_cast<std::string&>(asRef[i]));
		}
		ListDisplayDialog dialog(this, "Are you sure you want to delete these products", "Delete products", names);
		dialog.SetSize({ 700, 300 });
		if (dialog.ShowModal() == wxID_OK) {
			//do the delete one after the other
			wxMessageBox("Deleted test as fake", "FAKE DELETE");
		}
		else {
			mSelectedProductIndex.clear();
		}
	}
}

void ProductView::OnCheckInStock(wxCommandEvent& evt)
{
	wxBusyCursor cu;
	mDataView->Freeze();
	auto stock_rel = ProductInstance::instance().select<Products::id, Products::stock_count>()
		.join_on<0, 0>(ProductDetailsInstance::instance().select<ProductDetails::id, ProductDetails::stock_limit>());
	auto ids = stock_rel.where([&](auto& row) -> bool {
		return (nl::row_value<1>(row) <= nl::row_value<3>(row));
	}).isolate_column<0>();

	mModel->AddAttribute(mInStock, std::move(ids));
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
	DoCategorySelect(categories->GetStringSelection().ToStdString());
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

void ProductView::OnFile(wxAuiToolBarEvent& evt)
{
	if (evt.IsDropDownClicked()) {
		wxMenu* menu = new wxMenu;
		menu->Append(ID_SELECT_MULTIPLE, "select");
		menu->Append(ID_UNSELECT_MULTIPLE, "unselect");
		PopupMenu(menu);
	}
}

void ProductView::OnCategory(wxCommandEvent& evt)
{
	wxTextEntryDialog dialog(this, "Please enter the name of the category: ",
		"Create New Category");
	if (dialog.ShowModal() == wxID_OK) {
		std::string CatName = dialog.GetValue().ToStdString();
		if (CatName.empty()) {
			wxMessageBox("Cannot add an empty category", "Create category", wxICON_ERROR | wxOK);
		}
		else {
			auto iter = CategoriesInstance::instance().add(GenRandomId(), CatName);
			Categories::notification_data data;
			data.row_iterator = iter;
			CategoriesInstance::instance().notify(nl::notifications::add, data);
		}	
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
	Searcher<Categories::name, Categories> categorySearcher(CategoriesInstance::instance());
	auto CatIndices = categorySearcher.Search(SearchString);
	if (CatIndices.empty()) return;

	ProductInstance::instance().notify<nl::notifications::clear>({});
	auto ProductIndices = ProductInstance::instance()
		.where_index([&](Products::row_t& row) {
		for (auto& i : CatIndices) {
			if (nl::row_value<Products::category_id>(row) == nl::row_value<Categories::id>(CategoriesInstance::instance()[i])) {
				return true;
			}
		}
		return false;
	});
	mModel->ReloadIndices(std::move(ProductIndices));
}

void ProductView::OnSearchByPrice(const std::string& SearchString)
{
	
}

void ProductView::OnCategorySelected(wxCommandEvent& evt)
{
	DoCategorySelect(categories->GetStringSelection().ToStdString());
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
			mInventoryView->LoadInventory(nl::row_value<Products::id>(row), nl::clock::now());
		}
		InventoryPane.Show();
		ShowInventoryToolBar(row);
		mPanelManager->Update();
	}else{
		//inventory not created, create
		mPanelManager->GetPane("DataView").Hide();
		auto& row = ProductInstance::instance()[index];
		CreateInventory(nl::row_value<Products::id>(row));
		mInventoryView->LoadInventory(nl::clock::now());

		//get pane and update
		mPanelManager->GetPane("Inventory").Show();
		ShowInventoryToolBar(row);
		mPanelManager->Update();
	}
}


void ProductView::OnProductContextMenu(wxDataViewEvent& evt)
{
	wxMenu* menu = new wxMenu;
	auto edit_m = menu->Append(ID_PRODUCT_CONTEXT_EDIT, "Edit");
	auto remove_m = menu->Append(ID_PRODUCT_CONTEXT_REMOVE, "Remove");

	wxMenu* sExportMenu = new wxMenu;
	sExportMenu->Append(ID_PRODUCT_CONTEXT_EXPORT_JSON, "Json");

	auto export_m = menu->Append(ID_PRODUCT_CONTEXT_EXPORT, "Export", sExportMenu);

	edit_m->SetBitmaps(wxArtProvider::GetBitmap("reply"));
	remove_m->SetBitmap(wxArtProvider::GetBitmap("action_remove"));
	export_m->SetBitmap(wxArtProvider::GetBitmap("download"));
	PopupMenu(menu);
}

void ProductView::OnProductDetailView(wxCommandEvent& evt)
{
	auto selectedItem = mDataView->GetSelection();
	if (!selectedItem.IsOk()){
		wxMessageBox("No item selected", "EDIT PRODUCT", wxICON_INFORMATION | wxOK);
		return;
	}
	auto index = mModel->GetDataViewItemIndex(selectedItem);
	if (index == wxNOT_FOUND){
		spdlog::get("log")->error("Invalid index, fatal");
		return;
	}
	if (!mDetailView) {
		Freeze();
		mDetailView = std::make_unique<DetailView>(nl::row_value<Products::id>(ProductInstance::instance()[index]),
			this, wxID_ANY, wxDefaultPosition, wxSize(500, -1));
		if(!mDetailView->IsCreated()){
			mDetailView.reset(nullptr);
			Thaw();
			return;
		}
		mPanelManager->AddPane(mDetailView.get(), wxAuiPaneInfo().Name("DetailView").Caption("Product edit").Right().Show());
		mPanelManager->Update();
		Thaw();
		return;
	}
 	if (mDetailView->IsAnyModified()){
		//changing from the previous, but not updated changes
		if (wxMessageBox("Changing edited product without saving changes, continue?", "EDIT PRODUCT", wxICON_WARNING | wxYES_NO) != wxYES){
			return;
		}
	}
	mDetailView->LoadDataIntoGrid(nl::row_value<Products::id>(ProductInstance::instance()[index]));
	if (!mPanelManager->GetPane("DetailView").IsShown()) {
		mDetailView->Show();
		mPanelManager->GetPane("DetailView").Show();
	}
	mPanelManager->Update();
}

void ProductView::OnExpiryView(wxCommandEvent& evt)
{
	ExpiryView viewDialog(this, wxID_ANY, wxDefaultPosition, wxSize(1057, 642)); //4:3
	if (viewDialog.ShowModal() == wxID_OK){
		wxMessageBox("OK", "Expiry View");
	}
	else wxMessageBox("CANCEL", "Expiry View");
}

void ProductView::OnSelectMultiple(wxCommandEvent& evt)
{
	static bool toggle = false;
	static wxDataViewColumn* column = nullptr;
	toggle = !toggle;
	if (toggle){
		column = mDataView->PrependToggleColumn(wxEmptyString, 6, wxDATAVIEW_CELL_ACTIVATABLE, -1, wxALIGN_CENTER, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	}
	else{
		if (column)
		{
			mSelectedProductIndex.clear();
			mDataView->DeleteColumn(column);
			column = nullptr;
		}
	}
}

void ProductView::OnUnSelectMultiple(wxCommandEvent& evt)
{
	mDataView->Freeze();
	for (auto& i : mSelectedProductIndex){

		spdlog::get("log")->info(" {} index {:d}",
			nl::row_value<Products::name>(ProductInstance::instance()[i]), i);
	}
	mSelectedProductIndex.clear();
	mDataView->Thaw();
	mDataView->Update();
}

void ProductView::OnPaneClose(wxAuiManagerEvent& evt)
{
	auto paneInfo = evt.GetPane();
	if (paneInfo) {
		if (paneInfo->name == "DetailView") {
			if (mDetailView->IsAnyModified()) {
				if (wxMessageBox("Changing edited product without saving changes, continue?", "EDIT PRODUCT", wxICON_WARNING | wxYES_NO) != wxYES) {
					evt.Veto();
					return;
				}
			}
		}
	}
}
std::vector<std::string> SplitHealthTags(const std::string& condition_string)
{
	if (condition_string.empty()) return {};
	std::stringstream stream(condition_string, std::ios::in);
	std::vector<std::string> tags;
	std::string temp;
	while (std::getline(stream, temp, ',')) {
		tags.push_back(temp);
	}
	return tags;
}

//TODO: use the tuple_json converter in nitrolite
void ProductView::OnProductExportJson(wxCommandEvent& evt)
{
	if (!mSelectedProductIndex.empty()) {
		Products products;
		//save file 
		wxFileDialog saveFileDialog(this, _("Save Json file"), "", "",
				"Json files (*.json)|*.json", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

		if (saveFileDialog.ShowModal() == wxID_CANCEL)
			return;     // the user changed idea...

		std::string path = saveFileDialog.GetPath().ToStdString();
		auto full_path = std::filesystem::path(path);
		if (full_path.extension() != ".json") {
			wxMessageBox("File must be a json file", "Export data", wxICON_INFORMATION | wxOK);
			return;
		}
		std::fstream file(full_path, std::ios::out);
		wxProgressDialog proDlg(wxT("Download data"), wxT("Save Data as Json"), 100, this, wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_REMAINING_TIME);

		products.reserve(mSelectedProductIndex.size());
		for (auto index : mSelectedProductIndex) {
			products.add(ProductInstance::instance()[index]);
		}
		auto ppd = products.join_on<Products::id, ProductDetails::id>(ProductDetailsInstance::instance());
		auto cat_product = ppd.join_on<Products::category_id, Categories::id>(CategoriesInstance::instance());
		auto cat_group = cat_product.map_group_by<Products::category_id>();
		js::json out;
		for (auto& rel_pair : cat_group) {
			js::json category_json; 
			auto& rel = rel_pair.second;
			for (auto& rel_row : rel) {
				js::json product;
				js::json health_tag = js::json::array();
				auto tags = SplitHealthTags(nl::row_value<nl::j_<ProductDetails::health_conditions, Products::row_t>>(rel_row));
				for (auto&& tag : tags) {
					health_tag.push_back(tag);
				}
				product["Product Active ingredent"] = nl::row_value<nl::j_<ProductDetails::active_ing, Products::row_t>>(rel_row);
				product["Product Description"] = nl::row_value<nl::j_<ProductDetails::description, Products::row_t>>(rel_row);
				product["Product name"] = nl::row_value<Products::name>(rel_row);
				product["Product class"] = nl::row_value<nl::j_<ProductDetails::p_class, Products::row_t>>(rel_row);
				product["Stock count"] = nl::row_value<Products::stock_count>(rel_row);
				product["Health tags"] = health_tag;
				product["Package size"] = nl::row_value<Products::package_size>(rel_row);
				product["Direction for use"] = nl::row_value<nl::j_<ProductDetails::dir_for_use, Products::row_t>>(rel_row);
				product["Unit price"] = nl::row_value<Products::unit_price>(rel_row);
				category_json[nl::row_value<Products::name>(rel_row)] = product;
			}
			out[nl::row_value<nl::j_<Categories::name, decltype(ppd)::row_t>>(rel[0])] = category_json;
		}
		file << out.dump(4);
	}
	else {
		wxMessageBox("No item selected to be exported; click select to export items", "Export data", wxICON_INFORMATION | wxOK);
	}

}

void ProductView::DoSearch(const std::string& searchString)
{
	Searcher<Products::name, Products> search(ProductInstance::instance());
	ProductInstance::instance().notify<nl::notifications::clear>({});
	mModel->ReloadIndices(search.Search(searchString));
}

void ProductView::DoCategorySelect(const std::string& SearchString)
{
	if (SearchString.empty()) return; 
	if (SearchString == all_categories) {
		ProductInstance::instance().notify<nl::notifications::clear>({});
		Products::notification_data data;
		data.count = ProductInstance::instance().size();
		ProductInstance::instance().notify<nl::notifications::load>(data);
	}
	else {
		Searcher<Categories::name, Categories> categorySearcher(CategoriesInstance::instance());
		auto CatIndices = categorySearcher.Search(SearchString);
		if (CatIndices.empty()) return;

		ProductInstance::instance().notify<nl::notifications::clear>({});
		auto ProductIndices = ProductInstance::instance()
			.where_index([&](Products::row_t& row) {
			for (auto& i : CatIndices) {
				if (nl::row_value<Products::category_id>(row) == nl::row_value<Categories::id>(CategoriesInstance::instance()[i])) {
					return true;
				}
			}
			return false;
				});
		mModel->ReloadIndices(std::move(ProductIndices));
	}
}

void ProductView::RegisterNotification()
{
	UsersInstance::instance().sink<nl::notifications::evt>().add_listener<ProductView, & ProductView::OnUsersNotification>(this);
	CategoriesInstance::instance().sink<nl::notifications::add>().add_listener<ProductView, & ProductView::OnCategoryAddNotification>(this);
	ProductInstance::instance().sink<nl::notifications::update>().add_listener<ProductView, & ProductView::OnProductUpdateNotification>(this);
	ProductDetailsInstance::instance().sink<nl::notifications::update>().add_listener<ProductView, & ProductView::OnProductDetailUpdateNotification>(this);
}

void ProductView::UnregisterNotification()
{
	UsersInstance::instance().sink<nl::notifications::evt>().remove_listener<ProductView, & ProductView::OnUsersNotification>(this);
	CategoriesInstance::instance().sink<nl::notifications::add>().remove_listener<ProductView, & ProductView::OnCategoryAddNotification>(this);
	ProductInstance::instance().sink<nl::notifications::update>().remove_listener<ProductView, & ProductView::OnProductUpdateNotification>(this);
	ProductDetailsInstance::instance().sink<nl::notifications::update>().remove_listener<ProductView, & ProductView::OnProductDetailUpdateNotification>(this);
}

void ProductView::ConnectDatabaseSignals()
{
	InventoriesDatabaseSignal::Connect(std::bind(&ProductView::OnDatabaseInventorySignal, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));


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
		mDatabaseMgr->UpdateTable(nl::row_value<Products::id>(*it), ProductInstance::instance().get<Products::stock_count>(it),
			Products::get_col_name<Products::stock_count>());
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

void ProductView::OnCategoryAddNotification(const Categories::table_t& table, const Categories::notification_data& data)
{
	//insert into choicebox
	wxString name = nl::row_value<Categories::name>(*(data.row_iterator));
	categories->Insert(1, &name, categories->GetCount());
}

void ProductView::OnProductUpdateNotification(const Products::table_t& table, const Products::notification_data& data)
{
	try {
		switch (data.column)
		{
		case Products::name:
			if (std::holds_alternative<Products::elem_t<Products::name>>(data.column_value)) {
				nl::row_value<Products::name>(*data.row_iterator) = std::get<Products::elem_t<Products::name>>(data.column_value);
				mDatabaseMgr->UpdateTable(nl::row_value<Products::id>(*data.row_iterator),
					std::tuple<Products::elem_t<Products::name>>(std::get<Products::elem_t<Products::name>>(data.column_value)), Products::col_names[data.column]);
			}
			break;
		case Products::package_size:
			if (std::holds_alternative<Products::elem_t<Products::package_size>>(data.column_value)) {
				nl::row_value<Products::package_size>(*data.row_iterator) = std::get<Products::elem_t<Products::package_size>>(data.column_value);
				mDatabaseMgr->UpdateTable(nl::row_value<Products::id>(*data.row_iterator),
					std::tuple<Products::elem_t<Products::package_size>>(std::get<Products::elem_t<Products::package_size>>(data.column_value)), Products::col_names[data.column]);
			}
			break;
		case Products::stock_count:
			if (std::holds_alternative<Products::elem_t<Products::stock_count>>(data.column_value)) {
				nl::row_value<Products::stock_count>(*data.row_iterator) = std::get<Products::elem_t<Products::stock_count>>(data.column_value);
				mDatabaseMgr->UpdateTable(nl::row_value<Products::id>(*data.row_iterator),
					std::tuple<Products::elem_t<Products::stock_count>>(std::get<Products::elem_t<Products::stock_count>>(data.column_value)), Products::col_names[data.column]);
			}
			break;
		case Products::unit_price:
			if (std::holds_alternative<Products::elem_t<Products::unit_price>>(data.column_value)) {
				nl::row_value<Products::unit_price>(*data.row_iterator) = std::get<Products::elem_t<Products::unit_price>>(data.column_value);
				mDatabaseMgr->UpdateTable(nl::row_value<Products::id>(*data.row_iterator),
					std::tuple<Products::elem_t<Products::unit_price>>(std::get<Products::elem_t<Products::unit_price>>(data.column_value)), Products::col_names[data.column]);
			}
			break;
		}

	}
	catch (std::exception& except){
		spdlog::get("log")->critical("Varient not the correct type, {}", except.what());
		return;
	}
	mModel->AddAttribute(mModified, nl::row_value<Products::id>(*data.row_iterator));
}

void ProductView::OnProductDetailUpdateNotification(const ProductDetails::table_t&, const ProductDetails::notification_data& data)
{
	try {
		switch (data.column)
		{
		case ProductDetails::p_class:
			if (std::holds_alternative<ProductDetails::elem_t<ProductDetails::p_class>>(data.column_value)) {
				nl::row_value<ProductDetails::p_class>(*data.row_iterator) = std::get<ProductDetails::elem_t<ProductDetails::p_class>>(data.column_value);
				mDatabaseDetailMgr->UpdateTable(nl::row_value<ProductDetails::id>(*data.row_iterator),
					std::tuple<ProductDetails::elem_t<ProductDetails::p_class>>(std::get<ProductDetails::elem_t<ProductDetails::p_class>>(data.column_value)), ProductDetails::col_names[data.column]);
			}
			break;
		case ProductDetails::active_ing:
			if (std::holds_alternative<ProductDetails::elem_t<ProductDetails::active_ing>>(data.column_value)) {
				nl::row_value<ProductDetails::active_ing>(*data.row_iterator) = std::get<ProductDetails::elem_t<ProductDetails::active_ing>>(data.column_value);
				mDatabaseDetailMgr->UpdateTable(nl::row_value<ProductDetails::id>(*data.row_iterator),
					std::tuple<ProductDetails::elem_t<ProductDetails::active_ing>>(std::get<ProductDetails::elem_t<ProductDetails::active_ing>>(data.column_value)), ProductDetails::col_names[data.column]);
			}
			break;
		case ProductDetails::description:
			if (std::holds_alternative<ProductDetails::elem_t<ProductDetails::description>>(data.column_value)) {
				nl::row_value<ProductDetails::description>(*data.row_iterator) = std::get<ProductDetails::elem_t<ProductDetails::description>>(data.column_value);
				mDatabaseDetailMgr->UpdateTable(nl::row_value<ProductDetails::id>(*data.row_iterator),
					std::tuple<ProductDetails::elem_t<ProductDetails::description>>(std::get<ProductDetails::elem_t<ProductDetails::description>>(data.column_value)), ProductDetails::col_names[data.column]);
			}
			break;
		case ProductDetails::dir_for_use:
			if (std::holds_alternative<ProductDetails::elem_t<ProductDetails::dir_for_use>>(data.column_value)) {
				nl::row_value<ProductDetails::dir_for_use>(*data.row_iterator) = std::get<ProductDetails::elem_t<ProductDetails::dir_for_use>>(data.column_value);
				mDatabaseDetailMgr->UpdateTable(nl::row_value<ProductDetails::id>(*data.row_iterator),
					std::tuple<ProductDetails::elem_t<ProductDetails::dir_for_use>>(std::get<ProductDetails::elem_t<ProductDetails::dir_for_use>>(data.column_value)), ProductDetails::col_names[data.column]);
			}
			break;
		case ProductDetails::health_conditions:
			if (std::holds_alternative<ProductDetails::elem_t<ProductDetails::health_conditions>>(data.column_value)) {
				nl::row_value<ProductDetails::health_conditions>(*data.row_iterator) = std::get<ProductDetails::elem_t<ProductDetails::health_conditions>>(data.column_value);
				mDatabaseDetailMgr->UpdateTable(nl::row_value<ProductDetails::id>(*data.row_iterator),
					std::tuple<ProductDetails::elem_t<ProductDetails::health_conditions>>(std::get<ProductDetails::elem_t<ProductDetails::health_conditions>>(data.column_value)), ProductDetails::col_names[data.column]);
			}
			break;
		case ProductDetails::stock_limit:
			if (std::holds_alternative<ProductDetails::elem_t<ProductDetails::stock_limit>>(data.column_value)) {
				nl::row_value<ProductDetails::stock_limit>(*data.row_iterator) = std::get<ProductDetails::elem_t<ProductDetails::stock_limit>>(data.column_value);
				mDatabaseDetailMgr->UpdateTable(nl::row_value<ProductDetails::id>(*data.row_iterator),
					std::tuple<ProductDetails::elem_t<ProductDetails::stock_limit>>(std::get<ProductDetails::elem_t<ProductDetails::stock_limit>>(data.column_value)), ProductDetails::col_names[data.column]);
			}
			break;

		}


	}
	catch (std::exception& except){
		spdlog::get("log")->critical("Variant not the correct type, {}", except.what());
	}
	mModel->AddAttribute(mModified, nl::row_value<ProductDetails::id>(*data.row_iterator));
}

void ProductView::OnDragStart(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	//we are dragging a selected item
	if (item.IsOk())
	{
		int index = mModel->GetDataViewItemIndex(item);
		if (index != -1) {
			auto& row = ProductInstance::instance()[index];
			evt.SetDataObject(CreateDataObject(row));
		}

	}

}

void ProductView::OnToJson(wxCommandEvent& evt)
{
	nl::js::json j;
	auto time_start = std::chrono::high_resolution_clock::now();
	for (auto iter = ProductInstance::instance().begin();
		iter != ProductInstance::instance().end(); iter++) {
		j[nl::row_value<Products::name>(*iter)] = product_json_converter::convert_to_json(*iter);
	}
	auto time_stop = std::chrono::high_resolution_clock::now();
	float run = std::chrono::duration<float, std::chrono::milliseconds::period>(time_stop - time_start).count();
	spdlog::get("log")->info("it took {:f}ms to seralise the whole product database", run);
	std::filesystem::path path = std::filesystem::current_path() / "datastuff.json";
	std::fstream file(path, std::ios::out);
	if (file.is_open()) {
		file << j.dump(4);
	}


}

void ProductView::InitJsonConverter()
{
	product_json_converter::obj_name_vec_t& names = product_json_converter::get_obj_names();
	for (int i = 0; i < Products::column_count; i++) {
		names[i] = ProductInstance::instance().get_name(i);
	}
	product_json_converter::set_all_state();
}

void ProductView::OnDatabaseInventorySignal(InventoriesDatabaseSignal::PrimaryKey_t key, 
	InventoriesDatabaseSignal::DSM_FUNC func, size_t col)
{
	switch (func) {
	case InventoriesDatabaseSignal::DSM_FUNC::DSM_INSERT:
		try {
			nl::query q;
			q.select("*").from(Inventories::table_name).where(fmt::format("{} = {:d}", Inventories::get_col_name<Inventories::id>(), key));
			auto stat = DatabaseInstance::instance().prepare_query(q);
			auto row = DatabaseInstance::instance().retrive_row<Inventories::row_t>(stat);
			DatabaseInstance::instance().remove_statement(stat);

			uint32_t balance = nl::row_value<Inventories::balance>(row);
			auto it = ProductInstance::instance().find_on<Products::id>(nl::row_value<Inventories::product_id>(row));
			if (it != ProductInstance::instance().end())
			{
				nl::row_value<Products::stock_count>(*it) = balance;

				Products::notification_data data_p{};
				data_p.row_iterator = it;
				data_p.column = Products::stock_count;
				ProductInstance::instance().notify(nl::notifications::update, data_p);
				mModel->RemoveAttribute(nl::row_value<Products::id>(*it));
				mDatabaseMgr->UpdateTable(nl::row_value<Products::id>(*it), ProductInstance::instance().get<Products::stock_count>(it),
					Products::get_col_name<Products::stock_count>());
				
				OpenInventoryView(it);
			}
		}
		catch (nl::database_exception& exp) {
			spdlog::get("log")->critical(exp.what());
		}
		break;
	default:
		break;
	}

}

void ProductView::OpenInventoryView(Products::const_iterator iterator)
{
	//the test for the iterator should happen before this function is calles 
	try {
		auto& row = *iterator;
		wxAuiPaneInfo& inventoryPane = mPanelManager->GetPane("Inventory");
		if (inventoryPane.IsOk() && inventoryPane.IsShown()) {
			//we are currently on an inventory pane window and its shown to the user
			if (nl::row_value<Products::id>(row) == mInventoryView->GetProductId()) {
				mInventoryView->Freeze();
				mInventoryView->LoadInventory(nl::clock::now());
				mInventoryView->Thaw();
				mInventoryView->Refresh();
			}
		}
	}
	catch (std::exception& e) {
		spdlog::get("log")->critical(e.what());
	}
}


