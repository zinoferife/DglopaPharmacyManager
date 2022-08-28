#include "common.h"
#include "SalesView.h"

BEGIN_EVENT_TABLE(SalesView, wxPanel)
EVT_TOOL(SalesView::ID_CHECKOUT, SalesView::OnCheckOut)
EVT_TOOL(SalesView::ID_ADD_PRODUCT, SalesView::OnAddProduct)
END_EVENT_TABLE()


SalesView::~SalesView()
{
	//the grid takes ownership
}

SalesView::SalesView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
: wxPanel(parent, id, pos, size){
	mViewManager = std::make_unique<wxAuiManager>(this);
	CreateTopToolBar();
	CreateBottomToolBar();
	CreateDataView();
	SetDefaultAuiArt();
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
	mModel = new DataModel<Sales>(mSalesTable);

	//add columns
	mDataView->AppendTextColumn("Product name", 0, wxDATAVIEW_CELL_ACTIVATABLE, 200, wxALIGN_CENTER , wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Quantity", 4, wxDATAVIEW_CELL_EDITABLE, 200, wxALIGN_CENTER , wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Price", 6, wxDATAVIEW_CELL_ACTIVATABLE, 100, wxALIGN_CENTER , wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn(wxEmptyString , 60 , wxDATAVIEW_CELL_ACTIVATABLE, 100, wxALIGN_CENTER , wxDATAVIEW_COL_RESIZABLE);
	
	CreateSpecialColHandlers();
	wxDataViewModel* m = mModel;
	mDataView->AssociateModel(m);
	m->DecRef();

	mViewManager->AddPane(mDataView.get(), wxAuiPaneInfo().Name("DataView").Caption("Sales").CenterPane().Show());
	mViewManager->Update();
}

void SalesView::CreateSpecialColHandlers()
{
	mModel->SetSpecialColumnHandler(0, [this](size_t row, size_t col) -> wxVariant{
		// // O: insert return statement here
		auto& row_t = mSalesTable.at(row);
		auto id = nl::row_value<Sales::product_id>(row_t);
		const std::string& product_name = GetProductNameByID(id);
		return wxVariant{product_name};
	});
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
	wxMessageBox("Checkout", "Sales", wxICON_INFORMATION | wxOK);
}

void SalesView::OnReturn(wxCommandEvent& evnt)
{

}

void SalesView::OnAddProduct(wxCommandEvent& evnt)
{
	wxMessageBox("Add product", "Sales", wxICON_INFORMATION | wxOK);
}

const std::string& SalesView::GetProductNameByID(Sales::elem_t<Sales::product_id> id) const
{
	auto& p = ProductInstance::instance();
	auto iter = p.find_on<Products::id>(id);
	if (iter == p.end()) {
		return std::string{};
	}
	else {
		return nl::row_value<Products::name>(*iter);
	}
}
