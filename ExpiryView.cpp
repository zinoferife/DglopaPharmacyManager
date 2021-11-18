#include "common.h"
#include "ExpiryView.h"
BEGIN_EVENT_TABLE(ExpiryView, wxDialog)
EVT_BUTTON(wxID_OK, ExpiryView::OnOK)
EVT_BUTTON(wxID_CANCEL, ExpiryView::OnCancel)
EVT_BUTTON(ExpiryView::ID_APPLY, ExpiryView::OnApply)
EVT_DATAVIEW_ITEM_CONTEXT_MENU(ExpiryView::ID_DATA_VIEW, ExpiryView::OnLeftClick)
EVT_SEARCH(ExpiryView::ID_SEARCH, ExpiryView::OnSearch)
EVT_TEXT(ExpiryView::ID_SEARCH, ExpiryView::OnSearch)
EVT_CLOSE(ExpiryView::OnClose)
EVT_CHOICE(ExpiryView::ID_CHOICE, ExpiryView::OnChoice)
END_EVENT_TABLE()


ExpiryView::ExpiryView(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size)
: wxDialog(parent, id, "EXPIRY MODULE", position, size, wxCAPTION | wxRESIZE_BORDER | wxSYSTEM_MENU ){
	CreateViewManger();
	SetViewArt();
	CreateToolBar();
	CreateBottomToolBar();
	CreateDatabaseManger();
	CreateDataView();
	CreateTableView();
	mDialogManager->Update();
}

ExpiryView::~ExpiryView()
{
	mDialogManager->UnInit();
	mDialogManager.reset(nullptr);
	mDataView.release();
	mSearchBar.release();
}

void ExpiryView::CreateToolBar()
{
	auto bar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT);
	wxArrayString choices;
	choices.push_back("Today");
	choices.push_back("A month");
	choices.push_back("2 months");
	choices.push_back("Select duration");
	mDropDown = new wxChoice(bar, ID_CHOICE, wxDefaultPosition, wxSize(200, -1), choices);
	mDropDown->Bind(wxEVT_PAINT, [=](wxPaintEvent& evt) {
		wxPaintDC dc(mDropDown);
		wxRect rect(0, 0, dc.GetSize().GetWidth(), dc.GetSize().GetHeight());

		dc.SetBrush(*wxWHITE);
		dc.SetPen(*wxGREY_PEN);
		dc.DrawRectangle(rect);
		dc.DrawBitmap(wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_OTHER, wxSize(10, 10)), wxPoint(rect.GetWidth() - 15, (rect.GetHeight() / 2) - 5));
		auto sel = mDropDown->GetStringSelection();
		if (!sel.IsEmpty()) {
			dc.DrawLabel(sel, rect, wxALIGN_CENTER);
		}
	});
	mApply = new wxButton(bar, ID_APPLY, "Apply");
	bar->SetBackgroundColour(wxTheColourDatabase->Find("Aqua"));
	mSearchBar = std::make_unique<wxSearchCtrl>(bar, ID_SEARCH, wxEmptyString, wxDefaultPosition, wxSize(300, -1), wxWANTS_CHARS);
	mSearchBar->SetDescriptiveText("Search product");
	mSearchBar->SetToolTip("Search for an expired product");
	mSearchBar->ShowCancelButton(true);
	bar->AddControl(mSearchBar.get(), "SearchBar");
	bar->AddSpacer(10);
	bar->AddTool(ID_FILTER, "Filter:", wxNullBitmap);
	bar->AddSpacer(10);
	bar->AddControl(mDropDown);
	bar->AddSpacer(10);
	bar->AddControl(mApply);
	bar->AddStretchSpacer();
	mDateStatus = bar->AddTool(ID_EXPIRE_DATE, wxEmptyString, wxNullBitmap);
	bar->Realize();
	mDialogManager->AddPane(bar, wxAuiPaneInfo().Name(wxT("Tool")).Caption(wxT("Tool bar")).ToolbarPane().Top()
		.Resizable().MinSize(wxSize(-1, 30)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
	mTopBar = bar; //cache bar for other functions
}

void ExpiryView::CreateBottomToolBar()
{
	auto bar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT);
	mOk = new wxButton(bar, wxID_OK, "OK");
	mCancel = new wxButton(bar, wxID_CANCEL, "Cancel");
	bar->AddTool(2334, "This is a test", wxArtProvider::GetBitmap("action_delete"));
	bar->AddStretchSpacer();
	bar->AddControl(mOk, "OK");
	bar->AddControl(mCancel, "Cancel");

	bar->Realize();
	mDialogManager->AddPane(bar, wxAuiPaneInfo().Name(wxT("BottomTool")).Caption(wxT("Tool bar")).ToolbarPane().Bottom()
		.Resizable().MinSize(wxSize(-1, 30)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void ExpiryView::CreateDataView()
{
	mDataView = std::make_unique<wxDataViewCtrl>(this, ID_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES | wxNO_BORDER);
	mModel = new DataModel<ex_relation_t>(ExpireTable);
	mDataView->AppendTextColumn("Product name", 1, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mDataView->AppendTextColumn("Product class", 2, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mDataView->AppendTextColumn("Category", 3, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mDataView->AppendTextColumn("Stock count", 4, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mDataView->AppendTextColumn("Date expired", 5, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	wxDataViewModel* ProductModel = mModel;
	mDataView->AssociateModel(ProductModel);
	ProductModel->DecRef();

	mDialogManager->AddPane(mDataView.get(), wxAuiPaneInfo().Name("ExpiryView").Caption("Expiry view").CenterPane().Show());
	mDialogManager->Update();
}

void ExpiryView::CreateTableView()
{
	nl::query q;
	auto product_id = fmt::format("{}.{}", Products::table_name, Products::col_names[Products::id]);
	auto product_name = fmt::format("{}.{}", Products::table_name, Products::col_names[Products::name]);
	auto product_sc = fmt::format("{}.{}", Products::table_name, Products::col_names[Products::stock_count]);
	auto product_cat_id = fmt::format("{}.{}", Products::table_name, Products::col_names[Products::category_id]);
	auto category_id = fmt::format("{}.{}", Categories::table_name, Categories::col_names[Categories::id]);
	auto category_name = fmt::format("{}.{}", Categories::table_name, Categories::col_names[Categories::name]);
	auto product_detail_id = fmt::format("{}.{}", ProductDetails::table_name, ProductDetails::col_names[ProductDetails::id]);
	auto product_detail_class = fmt::format("{}.{}", ProductDetails::table_name, ProductDetails::col_names[ProductDetails::p_class]);
	auto inventories_id = fmt::format("{}.{}", Inventories::table_name, Inventories::col_names[Inventories::product_id]);
	auto inventories_ex_date = fmt::format("{}.{}", Inventories::table_name, Inventories::col_names[Inventories::date_expiry]);

	try {
		q.select(product_id, product_name, product_detail_class, category_name, product_sc, inventories_ex_date)
			.from(std::string_view(Products::table_name), std::string_view(ProductDetails::table_name),
				std::string_view(Categories::table_name),
				std::string_view(Inventories::table_name))
			.where(fmt::format("{} = {} AND {} = {} AND {} = {} AND {}", product_id, product_detail_id, product_cat_id, category_id, product_id,
				inventories_id,
				fmt::format("{} < :time", inventories_ex_date)));
		spdlog::get("log")->info("{}", q.get_query());
	}
	catch (fmt::format_error& error){
		spdlog::get("log")->error("{}", error.what());
		return;
	}
	if (!mDatabaseManger->AddQuery("load", q)){
		spdlog::get("log")->error("invalid query {}", q.get_query());
		return;
	}
	//loads the query into table
	mTimePoint = nl::clock::now();
	mDatabaseManger->BindOnStatement("load", std::tuple<nl::date_time_t>{mTimePoint}, "time");
	mDatabaseManger->LoadTable();
	mTopBar->Freeze();
	auto DateStatus = fmt::format("Expires before: {}", nl::to_string_date(mTimePoint));
	mDateStatus->SetMinSize(ResizeTool(DateStatus));
	mDateStatus->SetLabel(DateStatus);
	mTopBar->Realize();
	mTopBar->Thaw();
	mTopBar->Refresh();
	mDialogManager->Update();
	SetTableAttributes();
}

void ExpiryView::CreateDatabaseManger()
{
	mDatabaseManger = std::make_unique<DatabaseManager<ex_relation_t>>(ExpireTable, DatabaseInstance::instance());
	
}

void ExpiryView::CreateViewManger()
{
	mDialogManager = std::make_unique<wxAuiManager>(this);
}

void ExpiryView::SetTableAttributes()
{
	mExpired = std::make_shared<wxDataViewItemAttr>();
	mExpired->SetBackgroundColour(wxTheColourDatabase->Find("Tomato"));
	auto ids = ExpireTable.isolate_column<ex_relation_t::id>();
	mModel->AddAttribute(mExpired, std::move(ids));
}

void ExpiryView::SetViewArt()
{
	wxColour colour = wxTheColourDatabase->Find("Aqua");
	wxAuiDockArt* art = mDialogManager->GetArtProvider();
	art->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 24);
	art->SetMetric(wxAUI_DOCKART_GRIPPER_SIZE, 5);
	art->SetMetric(wxAUI_DOCKART_SASH_SIZE, 5);
	art->SetColour(wxAUI_DOCKART_SASH_COLOUR, *wxWHITE);
	art->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, colour);
	art->SetColour(wxAUI_DOCKART_BORDER_COLOUR, *wxWHITE);
	art->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	art->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_HORIZONTAL);
	mDialogManager->SetFlags(mDialogManager->GetFlags() | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_VENETIAN_BLINDS_HINT);
}

void ExpiryView::OnOK(wxCommandEvent& evt)
{
	if (IsModal()) EndModal(wxID_OK);
	else {
		SetReturnCode(wxID_OK);
		this->Show(false);
	}
}

void ExpiryView::OnCancel(wxCommandEvent& evt)
{
	if (IsModal()) EndModal(wxID_CANCEL);
	else {
		SetReturnCode(wxID_CANCEL);
		this->Show(false);
	}
}

void ExpiryView::OnSearch(wxCommandEvent& evt)
{
	auto search_string = evt.GetString().ToStdString();
	if (search_string.empty()) return;
	doSearch(search_string);


}

void ExpiryView::OnSearchCleared(wxCommandEvent& evt)
{

}

void ExpiryView::OnClose(wxCloseEvent& evt)
{
	evt.Skip();
}

void ExpiryView::OnChoice(wxCommandEvent& evt)
{
}

void ExpiryView::OnAddInventory(wxCommandEvent& evt)
{
}

void ExpiryView::OnZeroStock(wxCommandEvent& evt)
{
}

void ExpiryView::OnLeftClick(wxDataViewEvent& evt)
{
	wxMenu* menu = new wxMenu;
	auto add_inventory = menu->Append(ID_ADD_INVENTORY, "Add inventory");
	auto add_to_order = menu->Append(ID_ADD_TO_ORDER, "Add to order list");
	auto zero_stock = menu->Append(ID_ZERO_STOCK, "Set stock to zero");
	zero_stock->SetBitmap(wxArtProvider::GetBitmap("action_delete"));
	add_to_order->SetBitmap(wxArtProvider::GetBitmap("action_check"));
	add_inventory->SetBitmap(wxArtProvider::GetBitmap("action_add"));


	PopupMenu(menu);
}

void ExpiryView::OnApply(wxCommandEvent& evt)
{
	static std::array<nl::date_time_t, 3> time_points{mTimePoint, (mTimePoint + nl::days(31)), (mTimePoint + nl::days(62))};
	auto selection = mDropDown->GetSelection();
	if (selection == wxNOT_FOUND) return;
	else if (selection == 3){
		CalendarDialog dialog(this);
		nl::date_time_t date;
		if (dialog.ShowModal() == wxID_OK)
		{
			auto dateTime = dialog.mCalender->GetDate();
			auto tm = dateTime.GetTm();
			std::tm time_tm{ 0 };
			time_tm.tm_year = tm.year - 1900; // from 1900
			time_tm.tm_mon = tm.mon - 1; // from 0 t0 11
			time_tm.tm_mday = tm.mday;
			date = nl::clock::from_time_t(std::mktime(&time_tm));
			ExpireTable.clear();
			ExpireTable.notify<nl::notifications::clear>({});
			mDatabaseManger->BindOnStatement("load", std::tuple<nl::date_time_t>{date}, "time");
			try {
				spdlog::get("log")->info("{}", nl::to_string_date(date));
			}
			catch (fmt::format_error& error)
			{
				spdlog::get("log")->error("{}", error.what());
			}
			mDatabaseManger->LoadTable();
			mTopBar->Freeze();
			mDateStatus->SetLabel(fmt::format("Expires before: {}", nl::to_string_date(date)));
			mTopBar->Realize();
			mTopBar->Thaw();
			mTopBar->Refresh();
			mDialogManager->Update();
			return;
		}
		else { return; }
	}
	ExpireTable.clear();
	ExpireTable.notify<nl::notifications::clear>({});
	mDatabaseManger->BindOnStatement("load", std::tuple<nl::date_time_t>{time_points[selection]}, "time");
	spdlog::get("log")->info("{}", nl::to_string_date(time_points[selection]));
	mDatabaseManger->LoadTable();
	mTopBar->Freeze();
	auto toolName = fmt::format("Expires before: {}", nl::to_string_date(time_points[selection]));
	mDateStatus->SetMinSize(ResizeTool(toolName));
	mDateStatus->SetLabel(toolName);
	mTopBar->Realize();
	mTopBar->Thaw();
	mTopBar->Refresh();
	mDialogManager->Update();
}

void ExpiryView::resetChoice()
{
}

void ExpiryView::doSearch(const std::string& search_string)
{
	Searcher<ex_relation_t::name, ex_relation_t> search(ExpireTable);
	ExpireTable.notify<nl::notifications::clear>({});
	mModel->ReloadIndices(search.Search(search_string));
}

wxSize ExpiryView::ResizeTool(const std::string& tool)
{
	std::string x = "X";
	int w, h, temp;
	mTopBar->GetTextExtent(x, &temp, &h);
	mTopBar->GetTextExtent(tool, &w, &temp);
	return wxSize(w, h);
}
