#include "common.h"
#include "SalesView.h"

BEGIN_EVENT_TABLE(SalesView, wxPanel)
EVT_TOOL(SalesView::ID_CHECKOUT, SalesView::OnCheckOut)
EVT_TOOL(SalesView::ID_ADD_PRODUCT, SalesView::OnAddProduct)
EVT_DATAVIEW_ITEM_START_EDITING(SalesView::ID_DATA_VIEW, SalesView::OnEditingStarting)
EVT_DATAVIEW_ITEM_EDITING_STARTED(SalesView::ID_DATA_VIEW, SalesView::OnEditingStarted)
EVT_DATAVIEW_ITEM_EDITING_DONE(SalesView::ID_DATA_VIEW, SalesView::OnEditingDone)
EVT_CHOICE(SalesView::ID_SAVE_LIST, SalesView::OnSaveListSelected)
EVT_TOOL(wxID_SAVE, SalesView::OnSalesSave)
END_EVENT_TABLE()

std::string SalesView::mEmptyString{};

SalesView::~SalesView()
{
	//the grid takes ownership
	if (mModel) {
		mModel.release();
	}
}

SalesView::SalesView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
: wxPanel(parent, id, pos, size){
	mViewManager = std::make_unique<wxAuiManager>(this);
	CreateTopToolBar();
	CreateBottomToolBar();
	CreateDataView();
	CreateSalesOutput();
	CreateInventoryDatabaseManger();
	SetDefaultAuiArt();
	
	mDataView->SetDropTarget(CreateDropTarget<SalesView>(this, Products::row_t{}));
	mViewManager->Update();
}

void SalesView::CreateTopToolBar()
{
	wxAuiToolBar* bar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT);
	bar->SetToolBitmapSize(wxSize(16, 16));
	bar->AddStretchSpacer();
	bar->AddTool(ID_SETTINGS, wxT("Settings"), wxArtProvider::GetBitmap("application"));
	bar->AddTool(ID_ADD_PRODUCT, wxT("Add product"), wxArtProvider::GetBitmap("action_add"));
	
	mSaveList = new wxChoice(bar, ID_SAVE_LIST, wxDefaultPosition, wxSize(200, -1), wxArrayString{});
	mSaveList->SetSelection(0);
	bar->AddControl(mSaveList, "Save list");
	mSaveList->Bind(wxEVT_PAINT, [=](wxPaintEvent& evt) {
		wxPaintDC dc(mSaveList);
		wxRect rect(0, 0, dc.GetSize().GetWidth(), dc.GetSize().GetHeight());

		dc.SetBrush(*wxWHITE);
		dc.SetPen(*wxGREY_PEN);
		dc.DrawRectangle(rect);
		dc.DrawBitmap(wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_OTHER, wxSize(10, 10)), wxPoint(rect.GetWidth() - 15, (rect.GetHeight() / 2) - 5));
		auto sel = mSaveList->GetStringSelection();
		if (!sel.IsEmpty()) {
			dc.DrawLabel(sel, rect, wxALIGN_CENTER);
		}
		});
	bar->AddSeparator();


	bar->Realize();
	mViewManager->AddPane(bar, wxAuiPaneInfo().Name(wxT("UpToolBar")).Caption(wxT("up tool bar")).ToolbarPane().Top()
		.Resizable().MinSize(wxSize(-1, 30)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false).Show());
}

void SalesView::CreateBottomToolBar()
{
	wxAuiToolBar* bar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT);
	bar->SetToolBitmapSize(wxSize(16, 16));
	bar->AddStretchSpacer();

	bar->AddTool(wxID_SAVE, wxT("Save"), wxArtProvider::GetBitmap("save"));
	bar->AddTool(ID_CHECKOUT, wxT("Checkout"), wxArtProvider::GetBitmap("action_add"));
	bar->Realize();
	mViewManager->AddPane(bar, wxAuiPaneInfo().Name(wxT("BottomToolBar")).Caption(wxT("Bottom tool bar")).ToolbarPane().Bottom()
		.Resizable().MinSize(wxSize(-1, 30)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).TopDockable(false).Show());
}

void SalesView::CreateDataView()
{
	mDataView = std::make_unique<wxDataViewCtrl>(this, SalesView::ID_DATA_VIEW,
		wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES | wxNO_BORDER);
	mModel = std::make_unique<DataModel<Sales>>(mSalesTable);

	//add columns
	mDataView->AppendTextColumn("Product name", 0, wxDATAVIEW_CELL_ACTIVATABLE, 200, wxALIGN_CENTER , wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Quantity", 4, wxDATAVIEW_CELL_EDITABLE, 200, wxALIGN_CENTER , wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Price", 6, wxDATAVIEW_CELL_ACTIVATABLE, 100, wxALIGN_CENTER , wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn(wxEmptyString , 60 , wxDATAVIEW_CELL_ACTIVATABLE, 100, wxALIGN_CENTER , wxDATAVIEW_COL_RESIZABLE);
	
	CreateSpecialColHandlers();
	wxDataViewModel* m = mModel.get();
	mDataView->AssociateModel(m);
	m->DecRef();

	mViewManager->AddPane(mDataView.get(), wxAuiPaneInfo().Name("DataView").Caption("Sales").CenterPane().Show());
	mViewManager->Update();
}

void SalesView::CreateSpecialColHandlers()
{
	mModel->SetSpecialColumnHandler(0, [this](size_t col, size_t row) -> wxVariant{
		// // O: insert return statement here
		spdlog::get("log")->info("ROW no is {:d}", row);
		auto& row_t = mSalesTable.at(row);
		auto id = nl::row_value<Sales::product_id>(row_t);
		const std::string& product_name = GetProductNameByID(id);
		return wxVariant{product_name};
	});
}

void SalesView::CreateSalesOutput()
{
	mSalesOutput = std::make_unique<SalesOutput>(this, ID_SALES_OUTPUT);
	mViewManager->AddPane(mSalesOutput.get(),
		wxAuiPaneInfo().Name("SalesOutput").Bottom().Resizable().MinSize(wxSize(-1, 120)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).TopDockable(false).CloseButton(false)
		.Show());
	mSalesOutput->SetDiscountText("0");
	mSalesOutput->SetTotalText("0.00");
	mSalesOutput->SetChangeText("0.00");
	mViewManager->Update();
}

void SalesView::CreateInventoryDatabaseManger()
{
	//the reason is to have a table that we can write into 
	mInvetoryDatabasemagr = std::make_unique<DatabaseManager<Inventories>>(mTempInventoryRel,
		DatabaseInstance::instance());

	

}

void SalesView::SetDefaultAuiArt()
{
	wxColour colour = wxTheColourDatabase->Find("Aqua");
	wxAuiDockArt* art = mViewManager->GetArtProvider();
	art->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 24);
	art->SetMetric(wxAUI_DOCKART_GRIPPER_SIZE, 5);
	art->SetMetric(wxAUI_DOCKART_SASH_SIZE, 5);
	art->SetColour(wxAUI_DOCKART_SASH_COLOUR, *wxWHITE);
	art->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, colour);
	art->SetColour(wxAUI_DOCKART_BORDER_COLOUR, *wxWHITE);
	art->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	art->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_HORIZONTAL);
	mViewManager->SetFlags(mViewManager->GetFlags() | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_VENETIAN_BLINDS_HINT);
}

void SalesView::SetSpecicalColumns()
{
}

void SalesView::OnCheckOut(wxCommandEvent& evnt)
{
	if (mSalesTable.empty()) {
		wxMessageBox("No Products To Checkout", "Sales", wxICON_INFORMATION | wxOK);
		return;
	}
	else {
		wxMessageBox(fmt::format("You are to pay {}", mSalesOutput->GetTotalTextValue()),
			"Sales", wxICON_INFORMATION | wxOK);
		DoCheckOut();
		mSalesOutput->ClearOutput();
		return; 
	}
}

void SalesView::OnReturn(wxCommandEvent& evnt)
{

}

void SalesView::OnAddProduct(wxCommandEvent& evnt)
{
	SearchProduct product(this, wxID_ANY);
	if (product.ShowModal() == wxID_OK) {
		GetDataFromProductSearch(product.GetSelectedProduct());
	}
	else {
		spdlog::get("log")->info("This is not a product");
	}

}

void SalesView::OnSalesSave(wxCommandEvent& evt)
{
	//save button clicked 
	DoSave();
}

void SalesView::OnSaveListSelected(wxCommandEvent& evt)
{
	DoRestore();
}

void SalesView::OnEditingStarted(wxDataViewEvent& evt)
{
	auto dataItem = evt.GetItem();
	if (dataItem.IsOk()) {
		auto index = mModel->GetDataViewItemIndex(dataItem);
		if (index == wxNOT_FOUND) {
			spdlog::get("log")->error("Invalid Item in sales Table");
			return;
		}
		spdlog::get("log")->info("Editing started on {:d}", nl::row_value<Sales::product_id>(mSalesTable[index]));
	}
}

void SalesView::OnEditingStarting(wxDataViewEvent& evt)
{
	auto dataItem = evt.GetItem();
	if (dataItem.IsOk()) {
		auto index = mModel->GetDataViewItemIndex(dataItem);
		if (index == wxNOT_FOUND) {
			spdlog::get("log")->error("Invalid Item in sales Table");
			return;
		}
		spdlog::get("log")->info("Editing starting on {:d}", nl::row_value<Sales::product_id>(mSalesTable[index]));
	}
}

void SalesView::OnEditingDone(wxDataViewEvent& evt)
{
	auto dataItem = evt.GetItem();
	auto var = evt.GetValue();
	if (dataItem.IsOk()) {
		auto index = mModel->GetDataViewItemIndex(dataItem);
		if (index == wxNOT_FOUND) {
			spdlog::get("log")->error("Invalid Item in sales Table");
			return;
		}

		spdlog::get("log")->info("Editing done on {:d}, new value {:d}", nl::row_value<Sales::product_id>(mSalesTable[index]),
			var.GetLong());
		StoreEditedValue(evt.GetValue(), dataItem, 1);
	}
}

const std::string& SalesView::GetProductNameByID(Sales::elem_t<Sales::product_id> id) const
{
	spdlog::get("log")->info("Id is {:d}", id);
	auto& p = ProductInstance::instance();
	auto iter = p.find_on<Products::id>(id);
	if (iter == p.end()) {
		return mEmptyString;
	}
	else {
		return nl::row_value<Products::name>(*iter);
	}
}



void SalesView::DropData(const Products::row_t& product)
{
	Sales::row_t row;
	if (!CheckInStock(product)) return;
	if (CheckIfAdded(nl::row_value<Products::id>(product))) return;

	if (!CheckProductClass(product)) {
		if (wxMessageBox(fmt::format("{} is a Prescription only medicine, Do you wish to continue sale?",
			nl::row_value<Products::name>(product)), "Sales",
			wxICON_QUESTION | wxYES_NO) == wxNO) {
			return;
		}
	}
	nl::row_value<Sales::product_id>(row) = nl::row_value<Products::id>(product);
	nl::row_value<Sales::user_id>(row) = 64;
	nl::row_value<Sales::customer_id>(row) = 64;
	nl::row_value<Sales::price>(row) = nl::row_value<Products::unit_price>(product);
	nl::row_value<Sales::amount>(row) = 1;
	nl::row_value<Sales::date>(row) = nl::clock::now();

	Sales::notification_data data;
	data.row_iterator = mSalesTable.add(row);
	mSalesTable.notify<nl::notifications::add>(data);

	size_t index = mSalesTable.get_index(data.row_iterator);
	wxDataViewItem item(wxUIntToPtr(++index));
	mDataView->Select(item);
	mDataView->EnsureVisible(item);
	mDataView->SetFocus();

	UpdateTotal();
	Update();
}

bool SalesView::CheckInStock(const Products::row_t& row) const
{
	auto stock = nl::row_value<Products::stock_count>(row);
	if (stock == 0) {
		wxMessageBox(fmt::format("{} is out of stock",
			nl::row_value<Products::name>(row)), "Sales", wxICON_INFORMATION | wxOK);
		return false;
	}
	return true;
}


//setting option to turn this off
//it can get really annoying
bool SalesView::CheckProductClass(const Products::row_t& row) const
{
	auto iter = ProductDetailsInstance::instance().find_on<ProductDetails::id>(nl::row_value<Products::id>(row));
	if (iter != ProductDetailsInstance::instance().end())
	{
		//product detail is found
		const std::string& ref_class = nl::row_value<ProductDetails::p_class>(*iter);
		if (ref_class == "POM") {
			return false;
		}
	}
	return true;
}

void SalesView::GetDataFromProductSearch(const SearchProduct::view_t::row_t& SelectedRow)
{
	Sales::row_t row;
	if (CheckIfAdded(nl::row_value<Products::id>(SelectedRow))) return;

	nl::row_value<Sales::product_id>(row) = nl::row_value<0>(SelectedRow);
	nl::row_value<Sales::user_id>(row) = 64;
	nl::row_value<Sales::customer_id>(row) = 64;
	nl::row_value<Sales::price>(row) = nl::row_value<2>(SelectedRow);
	nl::row_value<Sales::amount>(row) = 1;
	nl::row_value<Sales::date>(row) = nl::clock::now();

	Sales::notification_data data;
	data.row_iterator = mSalesTable.add(row);
	mSalesTable.notify<nl::notifications::add>(data);

	size_t index = mSalesTable.get_index(data.row_iterator);
	wxDataViewItem item(wxUIntToPtr(++index));
	mDataView->Select(item);
	mDataView->EnsureVisible(item);
	mDataView->SetFocus();

	UpdateTotal();
	Update();

}

void SalesView::StoreEditedValue(const wxVariant& data, const wxDataViewItem& item, size_t col)
{
	if (!item.IsOk()) return;
	mModel->SetValue(data, item, col);
	mModel->ValueChanged(item, col);
	UpdateTotal();
	Update();
}

void SalesView::UpdateTotal()
{
	if (mSalesTable.empty()) {
		return;
	}
	float total = 0.0f;
	float temp = 0.0f;
	for (auto& record : mSalesTable) {
		try {
			temp = atof(nl::row_value<Sales::price>(record).c_str());
		}catch(...){
			spdlog::get("log")->critical("Invalid price {} in {} product, crital error",
				nl::row_value<Sales::price>(record), GetProductNameByID(nl::row_value<Sales::product_id>(record)));
			return;
		}
		total += temp * nl::row_value<Sales::amount>(record);
	}
	mSalesOutput->SetTotalText(fmt::format("{:.2f}", total));
}

void SalesView::DoCheckOut()
{
	//nothing to checkout
	if (mSalesTable.empty())return;
	//load the check out dialog
	

	StoreDataInInventory();
	mSalesTable.clear();
	mSalesTable.notify(nl::notifications::clear, {});

}

bool SalesView::CheckIfAdded(Products::elem_t<Products::id> id)
{
	//loop through the table flag if id already exsist
	auto iter = mSalesTable.find_on<Sales::product_id>(id);
	if (iter != mSalesTable.end()) {
		//increase the quantity.. 
		(nl::row_value<Sales::amount>(*iter))++;
		UpdateTotal();
		return true;
	}
	return false;
}

void SalesView::StoreDataInInventory()
{
	//called on check out 
	if (mSalesTable.empty()) {
		return;
	}
	std::uint64_t receptNumber = GenRandomId();
	nl::date_time_t now = nl::clock::now();
	for (auto& row : mSalesTable) {
		std::uint64_t id = GenRandomId();
		auto inven_row = MostRecentInventoryEntry(nl::row_value<Sales::product_id>(row));
		//DEBUG
		spdlog::get("log")->info("id {:d}, balance: {:d}",
				nl::row_value<Inventories::product_id>(inven_row), 
				nl::row_value<Inventories::balance>(inven_row));
		//DEBUG END

		mTempInventoryRel.emplace_back(
			Inventories::row_t{
			 id, nl::row_value<Sales::product_id>(row),
			 now, nl::row_value<Inventories::date_expiry>(inven_row), receptNumber,
			 0, nl::row_value<Sales::amount>(row), (nl::row_value<Inventories::balance>(inven_row) - nl::row_value<Sales::amount>(row)),
			 0, 0 });
	}
	mInvetoryDatabasemagr->FlushTable();
	for (auto& irow : mTempInventoryRel){
		InventoriesDatabaseSignal::Signal( nl::row_value<Inventories::id>(irow), InventoriesDatabaseSignal::DSM_FUNC::DSM_INSERT,0);
	}
	mTempInventoryRel.clear();
}

Inventories::row_t SalesView::MostRecentInventoryEntry(Products::elem_t<Products::id> id)
{
	nl::query q;
	auto product_col_name = Inventories::get_col_name<Inventories::product_id>();
	auto date_issed_name = Inventories::get_col_name<Inventories::date_issued>();
	q.select("*")
		.from(Inventories::table_name)
		.where(fmt::format("({},{}) IN ", product_col_name, date_issed_name))
		.beg_sub_query()
			.select(fmt::format("{}, MAX({})", product_col_name, date_issed_name))
			.from(Inventories::table_name)
			.where(fmt::format("{} = \'{:d}\'", product_col_name, id))
			.groupby(Inventories::get_col_name<Inventories::product_id>())
		.end_sub_query();
	try {
		auto stat = DatabaseInstance::instance().prepare_query(q);
		spdlog::get("log")->info(q.get_query());
		//use the exeptions from the database to throw on bad query
		if (stat == nl::database::BADSTMT) {
			spdlog::get("log")->error("{}", DatabaseInstance::instance().get_error_msg());
			return Inventories::row_t{};
		}
		//most resently added row should have the upto date balance for this product
		return DatabaseInstance::instance().retrive_row<Inventories::row_t>(stat);
	}
	catch (nl::database_exception& exp) {
		spdlog::get("log")->critical(exp.what());
		return Inventories::row_t{};
	}
}



void SalesView::DoSave()
{
	const int ListCount = mSaveList->GetCount();
	if (ListCount == mSalesTables.size()) {
		//Can nolonger do save
		wxMessageBox("Cannot Save new sales list, check out the previously saved lists to save new one",
			"Sales", wxICON_INFORMATION | wxOK);
		return;
	}
	std::string NewName = fmt::format("Sale list {}", ListCount);
	mSaveList->AppendString(NewName);
	mSaveList->Select(mSaveList->GetCount() - 1);

	//no need to save if its empty 
	if (mSalesTable.empty()) return;
	

	//swap with the mSalesTable
	mSalesTable.swap(mSalesTables[mSaveList->GetCount()  - 1]);
	//mSalesTable should be empty 
	//send a clear 
	mSalesTable.notify(nl::notifications::clear, {});

}

void SalesView::DoRestore()
{
	int selected = mSaveList->GetSelection();
	if (selected == wxNOT_FOUND) {
		// no selection
		wxMessageBox("Invalid selection", "Sales", wxICON_ERROR | wxOK);
		return;
	}
	auto& sale_table = mSalesTables[selected];
	if (!sale_table.empty()) {
		// not empty slot, there is data to restore
		if (!mSalesTable.empty()) {
			//warn that restoring a saved sales list would override the new list
			if (wxMessageBox("Restoring a save sales list would override the current sales list, do you wish to continue?",
				"Sales", wxICON_INFORMATION | wxYES_NO) == wxNO) {
				return;
			}
			else {
				mSalesTable.clear();
				mSalesTable.notify(nl::notifications::clear, {});
			}
		}
		//the checks is successful, then go ahead and remove the now restored thing
		mSaveList->Delete(selected);

		mSalesTable.swap(sale_table);

		//notify a load
		Sales::notification_data data;
		data.count = mSalesTable.size();

		//notify the model to notify the reload the view
		mSalesTable.notify(nl::notifications::load, data);
	}

}



