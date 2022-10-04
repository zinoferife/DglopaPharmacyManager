#include "common.h"
#include "SalesView.h"

BEGIN_EVENT_TABLE(SalesView, wxPanel)
EVT_TOOL(SalesView::ID_CHECKOUT, SalesView::OnCheckOut)
EVT_TOOL(SalesView::ID_ADD_PRODUCT, SalesView::OnAddProduct)
EVT_DATAVIEW_ITEM_START_EDITING(SalesView::ID_DATA_VIEW, SalesView::OnEditingStarting)
EVT_DATAVIEW_ITEM_EDITING_STARTED(SalesView::ID_DATA_VIEW, SalesView::OnEditingStarted)
EVT_DATAVIEW_ITEM_EDITING_DONE(SalesView::ID_DATA_VIEW, SalesView::OnEditingDone)
END_EVENT_TABLE()


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

void SalesView::OnCheckOut(wxCommandEvent& evnt)
{
	if (mSalesTable.empty()) {
		wxMessageBox("No Products To Checkout", "Sales", wxICON_INFORMATION | wxOK);
		return;
	}
	else {
		wxMessageBox(fmt::format("You are to pay {}", mSalesOutput->GetTotalTextValue()),
			"Sales", wxICON_INFORMATION | wxOK);
		return; 
	}
}

void SalesView::OnReturn(wxCommandEvent& evnt)
{

}

void SalesView::OnAddProduct(wxCommandEvent& evnt)
{
	wxMessageBox("Add product", "Sales", wxICON_INFORMATION | wxOK);
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
	if (dataItem.IsOk()) {
		auto index = mModel->GetDataViewItemIndex(dataItem);
		if (index == wxNOT_FOUND) {
			spdlog::get("log")->error("Invalid Item in sales Table");
			return;
		}
		spdlog::get("log")->info("Editing done on {:d}", nl::row_value<Sales::product_id>(mSalesTable[index]));
	}
}

const std::string& SalesView::GetProductNameByID(Sales::elem_t<Sales::product_id> id) const
{
	spdlog::get("log")->info("Id is {:d}", id);
	auto& p = ProductInstance::instance();
	auto iter = p.find_on<Products::id>(id);
	if (iter == p.end()) {
		return std::string{};
	}
	else {
		return nl::row_value<Products::name>(*iter);
	}
}



void SalesView::DropData(const Products::row_t& product)
{
	Sales::row_t row;
	if (!CheckInStock(product)) return;

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
