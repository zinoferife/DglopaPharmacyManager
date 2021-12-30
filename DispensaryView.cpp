#include "common.h"
#include "DispensaryView.h"

BEGIN_EVENT_TABLE(DispensaryView, wxPanel)
EVT_DATAVIEW_SELECTION_CHANGED(DispensaryView::DATA_VIEW, DispensaryView::OnDataViewItemSelected)
END_EVENT_TABLE()

DispensaryView::DispensaryView(wxWindow* parent, wxWindowID id)
	: wxPanel(parent, id, wxDefaultPosition, wxDefaultSize) {
	mPanelManager = std::make_unique<wxAuiManager>(this);
	SetDefaultArt();
	CreateDataView();
	mPanelManager->Update();
}

DispensaryView::~DispensaryView()
{
	mPanelManager->UnInit();
	mDataView.release();
	mPropertyManager.release();
}

void DispensaryView::SetDefaultArt()
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

void DispensaryView::Load(Prescriptions::iterator prescription)
{
	if (!mPropertyManager) {
		CreatePropertyGrid(prescription);
	}
	else {
		LoadPropertyGrid(prescription);
	}
	std::stringstream json_stream(nl::row_value<Prescriptions::medication>(*prescription), std::ios::in);
	js::json med;
	json_stream >> med;
	if (med.empty()) {
		wxMessageBox("Medication list corrupted", "Prescriptions", wxICON_ERROR | wxOK);
		return;
	}
	try {
		for (auto drug = med.begin(); drug != med.end(); drug++) {
			auto& value = drug.value();
			mMedicationTable.add(GenRandomId(), value["medication_name"], value["dosage_form"], value["strength"], value["dir_for_use"], value["quantity"]);
		}
	}
	catch (js::json::type_error& error) {
		spdlog::get("log")->error("JSON ERROR: {}", error.what());
		wxMessageBox("Incorrect medication list", "Dispensary view", wxICON_ERROR | wxOK);
		return;
	}
	medications::notification_data data;
	data.count = mMedicationTable.size();
	mMedicationTable.notify(nl::notifications::load, data);
}


void DispensaryView::CreateToolBar()
{
	auto bar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW);

	bar->Realize();
	mPanelManager->AddPane(bar, wxAuiPaneInfo().Name(wxT("Tool")).Caption(wxT("Tool bar")).ToolbarPane().Top()
		.Resizable().MinSize(wxSize(-1, 30)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void DispensaryView::CreateDataView()
{
	mDataModel = new DataModel<medications>(mMedicationTable);
	mDataView = std::make_unique<wxDataViewCtrl>(this, DATA_VIEW, wxDefaultPosition, wxDefaultSize,
		wxDV_ROW_LINES | wxNO_BORDER);
	mDataView->AssociateModel(mDataModel);
	mDataModel->DecRef();
	//columns
	mDataView->AppendToggleColumn("Approve", 10, wxDATAVIEW_CELL_ACTIVATABLE, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Id", 0, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Medication", 1, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Dosage form", 2, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Strength", 3, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Direction for use", 4, wxDATAVIEW_CELL_INERT, 400, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Quantity", 5, wxDATAVIEW_CELL_INERT, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	SetSpecialCol();

	mPanelManager->AddPane(mDataView.get(), wxAuiPaneInfo().Name("DataView").Caption("Medications").CenterPane().PinButton());
}

void DispensaryView::SetSpecialCol()
{
	mDataModel->SetSpecialColumnHandler(10, [self = this](size_t col, size_t row) -> wxVariant {
		return (self->mSelections.find(row) != self->mSelections.end());
		});

	mDataModel->SetSpecialSetColumnHandler(10, [self = this](size_t col, size_t row, const wxVariant& var) ->bool {
		if (var.GetBool()) {
			self->mSelections.insert(row);
		}
		else {
			self->mSelections.erase(row);
		}
		return false;
		});


}

void DispensaryView::CreatePropertyGrid(Prescriptions::iterator prescription)
{
	Freeze();
	mPropertyManager = std::make_unique<wxPropertyGridManager>(this, PROPERTY_MANGER,
		wxDefaultPosition, wxSize(500,-1), wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPGMAN_DEFAULT_STYLE);
	CreatePropertyGridToolBar();
	auto page = mPropertyManager->AddPage("Prescription detail", wxArtProvider::GetBitmap("action_check"));
	page->Append(new wxPropertyCategory("Patient details"));
	page->Append(new wxStringProperty("Patients name", wxPG_LABEL, nl::row_value<Prescriptions::patient_name>(*prescription)));
	page->Append(new wxIntProperty("Patients age", wxPG_LABEL, nl::row_value<Prescriptions::patient_age>(*prescription)));
	page->Append(new wxStringProperty("Patients weight", wxPG_LABEL, nl::row_value<Prescriptions::patient_weight>(*prescription)));
	page->Append(new wxStringProperty("Patients address", wxPG_LABEL, nl::row_value<Prescriptions::patient_address>(*prescription)));
	page->Append(new wxPropertyCategory("Prescribers details"));
	page->Append(new wxStringProperty("Prescribers name", wxPG_LABEL, nl::row_value<Prescriptions::prescriber_name>(*prescription)));
	page->Append(new wxStringProperty("Prescribers licience number", wxPG_LABEL, nl::row_value<Prescriptions::prescriber_licence_number>(*prescription)));
	page->Append(new wxStringProperty("Prescribers health center", wxPG_LABEL, nl::row_value<Prescriptions::prescriber_health_center>(*prescription)));
	CreatePropertyGridArt();
	CreateEditProperty();
	mPanelManager->AddPane(mPropertyManager.get(), wxAuiPaneInfo().Name("PrescriptionDetail").Caption("Prescription detail").Bottom().MinSize(500,-1).Layer(1).Position(1).PinButton().MinimizeButton());
	mPanelManager->Update();
	Thaw();
}

void DispensaryView::LoadPropertyGrid(Prescriptions::iterator prescription)
{
	auto page = mPropertyManager->GetPage("Prescription detail");
	if (page) {
		mPropertyManager->Freeze();
		page->GetProperty("Patients name")->SetValue(nl::row_value<Prescriptions::patient_name>(*prescription));
		page->GetProperty("Patients age")->SetValue(static_cast<std::int32_t>(nl::row_value<Prescriptions::patient_age>(*prescription)));
		page->GetProperty("Patients weight")->SetValue(nl::row_value<Prescriptions::patient_weight>(*prescription));
		page->GetProperty("Patients address")->SetValue(nl::row_value<Prescriptions::patient_address>(*prescription));
		page->GetProperty("Prescribers name")->SetValue(nl::row_value<Prescriptions::prescriber_name>(*prescription));
		page->GetProperty("Prescribers licience number")->SetValue(nl::row_value<Prescriptions::prescriber_licence_number>(*prescription));
		page->GetProperty("Prescribers health center")->SetValue(nl::row_value<Prescriptions::prescriber_health_center>(*prescription));
		mPropertyManager->Thaw();
		mPanelManager->GetPane("PrescriptionDetail").Show();
		mPanelManager->Update();
	}
}
void DispensaryView::CreatePropertyGridToolBar()
{
	auto tool = mPropertyManager->GetToolBar();
	if (tool) {
		tool->SetWindowStyleFlag(wxTB_HORZ_TEXT);
		tool->Realize();
		Update();
	}

}

void DispensaryView::CreatePropertyGridArt()
{
	auto grid = mPropertyManager->GetPage("Prescription detail")->GetGrid();
	grid->SetBackgroundColour(*wxWHITE);
	grid->SetCaptionBackgroundColour(wxTheColourDatabase->Find("Aqua"));
	grid->SetCaptionTextColour(*wxBLACK);

}

void DispensaryView::CreateEditProperty()
{
	auto EditPrescription = mPropertyManager->AddPage("Prescription edit", wxArtProvider::GetBitmap("action_add"));
	EditPrescription->Append(new wxPropertyCategory("Prescribed Medication details", wxPG_LABEL));
	EditPrescription->Append(new wxStringProperty("Medication", wxPG_LABEL));
	EditPrescription->Append(new wxStringProperty("Dosage form", wxPG_LABEL));
	EditPrescription->Append(new wxStringProperty("Strength", wxPG_LABEL));
	EditPrescription->Append(new wxStringProperty("Direction for use", wxPG_LABEL));
	EditPrescription->Append(new wxIntProperty("Quantity", wxPG_LABEL));
}

void DispensaryView::ResetViewData()
{
	for (int i = 0; i < 2; i++) {
		auto iter = mPropertyManager->GetPage(i)->GetIterator();
		while (!iter.AtEnd()) {
			auto prop = iter.GetProperty();
			if (prop && !prop->IsCategory()) {
				prop->SetValue(wxVariant());
			}
			(void)iter++;
		}
	}
	mSelections.clear();
	mMedicationTable.clear();
	medications::notification_data data;
	mMedicationTable.notify<nl::notifications::clear>(data);
}

void DispensaryView::Dispense()
{
	if (mSelections.empty()) { 
		wxMessageBox("No item selected to be dispensed", "DISPENSARY", wxOK | wxICON_INFORMATION);
		return; 
	}
	wxProgressDialog dialog("DISPENSARY", fmt::format("Dispensing {:d} drugs", mSelections.size()), 100, this, wxPD_SMOOTH);
	int update_value = (int)(100 / mSelections.size());
	int update = 0;
	for (auto index : mSelections) {
		update += update_value;
		dialog.Update(update, fmt::format("Dispensing {}", nl::row_value<medications::mediction_name>(mMedicationTable[index])));
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	dialog.Update(100, "Drugs dispensed");
}

void DispensaryView::OnDataViewItemSelected(wxDataViewEvent& evt)
{
	spdlog::get("log")->info("{:d} selected", mDataModel->GetDataViewItemIndex(evt.GetItem()));
	auto item = evt.GetItem();
	if(item.IsOk())
	{
		auto sel = mMedicationTable.get_iterator(mDataModel->GetDataViewItemIndex(item));
		if (sel == mMedicationTable.end()) {
			return;
		}
		auto edp = mPropertyManager->GetPage("Prescription edit");
		if (edp) {
			mPropertyManager->Freeze();
			edp->GetProperty("Medication")->SetValue(nl::row_value<medications::mediction_name>(*sel));
			edp->GetProperty("Dosage form")->SetValue(nl::row_value<medications::dosage_form>(*sel));
			edp->GetProperty("Strength")->SetValue(nl::row_value<medications::strength>(*sel));
			edp->GetProperty("Direction for use")->SetValue(nl::row_value < medications::dir_for_use>(*sel));
			edp->GetProperty("Quantity")->SetValue(static_cast<std::int32_t>(nl::row_value<medications::quanity>(*sel)));
			mPropertyManager->Thaw();
		}
	}
}
