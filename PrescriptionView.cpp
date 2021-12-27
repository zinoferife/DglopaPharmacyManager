#include "common.h"
#include "PrescriptionView.h"
BEGIN_EVENT_TABLE(PrescriptionView, wxPanel)
EVT_TOOL(ID_ADD_PRESCRIPTION, OnAddPrescription)
EVT_DATAVIEW_ITEM_ACTIVATED(PrescriptionView::ID_DATA_VIEW, PrescriptionView::OnPrescriptionActivated)
EVT_TOOL(PrescriptionView::ID_BACK, PrescriptionView::OnBack)
END_EVENT_TABLE()


PrescriptionView::PrescriptionView(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size)
: wxPanel(parent, id, position, size){
	mPanelManager = std::make_unique<wxAuiManager>(this);
	CreateToolBar();
	CreateTable();
	CreateDispensaryView();
	auto now = nl::clock::now();
	LoadPrescriptions(now - nl::days(1), now);
	SetDefaultAuiArt();
	mPanelManager->Update();
}

void PrescriptionView::CreateToolBar()
{
	auto bar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW);
	bar->SetToolBitmapSize(wxSize(16, 16));
	bar->AddTool(ID_BACK, wxEmptyString, wxArtProvider::GetBitmap("arrow_back"));
	bar->AddStretchSpacer();
	bar->AddTool(ID_ADD_PRESCRIPTION, wxEmptyString, wxArtProvider::GetBitmap("action_add"));
	bar->Realize();
	mPanelManager->AddPane(bar, wxAuiPaneInfo().Name(wxT("Tool")).Caption(wxT("Tool bar")).ToolbarPane().Top()
		.Resizable().MinSize(wxSize(-1, 30)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void PrescriptionView::LoadPrescriptions(const nl::date_time_t& start, const nl::date_time_t& stop)
{
	nl::query loadQ;


}

void PrescriptionView::CreateTable()
{
	mDataView = std::make_unique<wxDataViewCtrl>(this, ID_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES | wxNO_BORDER);
	mModel = new DataModel<Prescriptions>(PrescriptionInstance::instance());
	mDatabaseManger = std::make_unique<DatabaseManager<Prescriptions>>(PrescriptionInstance::instance(), DatabaseInstance::instance());
	mDataView->AssociateModel(mModel);
	mModel->DecRef();
	InitDataView();
	SetSpecialColumns();
}

void PrescriptionView::CreateDispensaryView()
{
	mDispensaryView = std::make_unique<DispensaryView>(this, ID_DISPENSARY);
	mPanelManager->AddPane(mDispensaryView.get(), wxAuiPaneInfo().Name("DispensaryView").Caption("Dispensary").CenterPane().Hide());
	mPanelManager->Update();
}

void PrescriptionView::InitDataView()
{
	//create the columns For the data
	mDataView->AppendTextColumn("Date issued", 2, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Patient name", 4, wxDATAVIEW_CELL_INERT, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Patient Address", 5, wxDATAVIEW_CELL_INERT, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Prescriber name", 8, wxDATAVIEW_CELL_INERT, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Prescriber health care center address", 9, wxDATAVIEW_CELL_INERT, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Prescriber lincence number", 10, wxDATAVIEW_CELL_INERT, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Prescription status", 11, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);

	mPanelManager->AddPane(mDataView.get(), wxAuiPaneInfo().Name("DataView").Caption("Prescriptions").CenterPane());
}

void PrescriptionView::SetSpecialColumns()
{
	//set upspecial columns
	mModel->SetSpecialColumnHandler(11, [](size_t col, size_t index)-> wxVariant{
		return wxVariant("PENDING");
	});
}

void PrescriptionView::SetDefaultAuiArt()
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

PrescriptionView::~PrescriptionView()
{
	mPanelManager->UnInit();
	mDataView.release();
}

void PrescriptionView::GenerateFakePrescription()
{
	auto& instance = PrescriptionInstance::instance();
	nl::uuid id;
	id.generate();

	js::json med1 = {
		{"medication_name", "Paracetamol"},
		{"dosage_form", "Tablet"},
		{"strength", "500mg"},
		{"dir_for_use", "Takee 2 tablets when required"},
		{"quantity", 30}
	};
	
	js::json med2 = {
		{"medication_name", "Vitamin C"},
		{"dosage_form", "Tablet"},
		{"strength", "500mg"},
		{"dir_for_use", "Takee 2 tablets when required"},
		{"quantity", 30}
	};

	js::json med3 = {
		{"medication_name", "Aspirin"},
		{"dosage_form", "Tablet"},
		{"strength", "75mg"},
		{"dir_for_use", "Takee 2 tablets when required"},
		{"quantity", 30}
	};

	js::json object = { med1, med2, med3 };
	std::string medications = js::to_string(object);
	Prescriptions::notification_data data;
	//continue here tomorrow
	data.row_iterator = instance.add(GenRandomId(), id, nl::clock::now(), medications, "Zino ferife", "433 DBS road, Asaba, Delta state",
		25, "36.5", "Dr Vivienne Ferife", "Federal Medical Center, Asaba, Delta state", "00098834561");
	instance.notify<nl::notifications::add>(data);
}

void PrescriptionView::OnAddPrescription(wxCommandEvent& evt)
{
	//test
	GenerateFakePrescription();
}

void PrescriptionView::OnPrescriptionActivated(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (item.IsOk()){
		int index = mModel->GetDataViewItemIndex(item);
		auto& disPane = mPanelManager->GetPane("DispensaryView");
		if (disPane.IsOk()) {
			mPanelManager->GetPane("DataView").Hide();
			mDispensaryView->Load(PrescriptionInstance::instance().get_iterator(index));
			disPane.Show();
			mPanelManager->Update();
		}
	}
}

void PrescriptionView::OnBack(wxCommandEvent& evt)
{
	if (!mPanelManager->GetPane("DataView").IsShown()) {
		mPanelManager->GetPane("DispensaryView").Hide();
		mPanelManager->GetPane("DataView").Show();
		mPanelManager->Update();
	}

}
