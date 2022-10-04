#include "common.h"
#include "PrescriptionView.h"
BEGIN_EVENT_TABLE(PrescriptionView, wxPanel)
EVT_TOOL(PrescriptionView::ID_ADD_PRESCRIPTION, PrescriptionView::OnAddPrescription)
EVT_TOOL(PrescriptionView::ID_DISPENSE, PrescriptionView::OnDispense)
EVT_TOOL(PrescriptionView::ID_PREVIEW, PrescriptionView::OnLabelPreview)
EVT_DATAVIEW_ITEM_ACTIVATED(PrescriptionView::ID_DATA_VIEW, PrescriptionView::OnPrescriptionActivated)
EVT_TOOL(PrescriptionView::ID_BACK, PrescriptionView::OnBack)
EVT_TOOL(PrescriptionView::ID_REFRESH, PrescriptionView::OnRefresh)
EVT_TOOL(PrescriptionView::ID_SEARCH_FOR_PRESCRIPTION, PrescriptionView::OnSearchforPrescription)
EVT_TOOL(PrescriptionView::ID_GET_PRESCRIPTION_SOURCES, PrescriptionView::OnGetPrescriptionSources)
EVT_CHOICE(PrescriptionView::ID_PRESCRIPTION_SOURCE, PrescriptionView::OnPrescriptionChoiceChange)
END_EVENT_TABLE()


PrescriptionView::PrescriptionView(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size)
: wxPanel(parent, id, position, size){
	mPanelManager = std::make_unique<wxAuiManager>(this);
	CreateToolBar();
	CreateTable();
	CreateDispensaryView();
	CreateDispensaryToolBar();
	auto now = nl::clock::now();
	LoadPrescriptions(now - nl::days(1), now);
	SetDefaultAuiArt();
	mPanelManager->Update();
}

void PrescriptionView::CreateToolBar()
{
	auto bar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW);
	bar->SetToolBitmapSize(wxSize(16, 16));
	//search bar
	auto search = new wxSearchCtrl(bar, ID_SEARCH, wxEmptyString, wxDefaultPosition, wxSize(300, -1), wxWANTS_CHARS);
	search->SetDescriptiveText("Search prescription by patient name");
	search->ShowCancelButton(true);
	bar->AddControl(search);
	bar->AddSeparator();
	mPrescriptionSourceChoiceBox = new wxChoice(bar, ID_PRESCRIPTION_SOURCE, wxDefaultPosition, wxSize(200, -1), wxArrayString{});
	bar->AddControl(mPrescriptionSourceChoiceBox, "Prescription sources");
	mPrescriptionSourceChoiceBox->Bind(wxEVT_PAINT, [=](wxPaintEvent& evt) {
		wxPaintDC dc(mPrescriptionSourceChoiceBox);
		wxRect rect(0, 0, dc.GetSize().GetWidth(), dc.GetSize().GetHeight());

		dc.SetBrush(*wxWHITE);
		dc.SetPen(*wxGREY_PEN);
		dc.DrawRectangle(rect);
		dc.DrawBitmap(wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_OTHER, wxSize(10, 10)), wxPoint(rect.GetWidth() - 15, (rect.GetHeight() / 2) - 5));
		auto sel = mPrescriptionSourceChoiceBox->GetStringSelection();
		if (!sel.IsEmpty()) {
			dc.DrawLabel(sel, rect, wxALIGN_CENTER);
		}
		});
	mPrescriptionSourceChoiceBox->SetHelpText("Select Prescription Source");
	mPrescriptionSourceChoiceBox->SetLabelText("Select Prescription Source");
	bar->AddTool(ID_GET_PRESCRIPTION_SOURCES, wxT("Prescription source"),
		wxArtProvider::GetBitmap("download"));

	bar->AddStretchSpacer();
	bar->AddTool(ID_ADD_PRESCRIPTION, wxT("Add fake prescription"), wxArtProvider::GetBitmap("action_add"));
	bar->AddTool(ID_SUBSCRIBE, wxT("Connect"), wxArtProvider::GetBitmap("reply"));
	bar->AddTool(ID_REFRESH, wxT("Refresh"), wxArtProvider::GetBitmap("file"));
	bar->Realize();
	mPanelManager->AddPane(bar, wxAuiPaneInfo().Name(wxT("Tool")).Caption(wxT("Tool bar")).ToolbarPane().Top()
		.Resizable().MinSize(wxSize(-1, 30)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void PrescriptionView::LoadPrescriptions(const nl::date_time_t& start, const nl::date_time_t& stop)
{
	nl::query loadQ;
	auto start_rep = nl::to_representation(start);
	auto stop_rep = nl::to_representation(stop);
	loadQ.select("*").from(Prescriptions::table_name).where(fmt::format("{} > {:d} AND {} < {:d}",
		Prescriptions::get_col_name<Prescriptions::date_issued>(),
		start_rep, 
		Prescriptions::get_col_name<Prescriptions::date_issued>(),
		stop_rep));

	//send the query message to the server to get prescriptions from the source selected

	auto string = loadQ.get_query();
	spdlog::get("log")->info("{}", string);
	bool stats= mDatabaseManger->AddQuery("load", loadQ);
	if (!stats) {
		std::string error = DatabaseInstance::instance().get_error_msg();
		spdlog::get("log")->error("{}", error);
		mDatabaseManger->ChangeQuery("load", loadQ);
	}
	mDatabaseManger->LoadTable();
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
	mDatabaseManger->CreateTable();
}

void PrescriptionView::CreateDispensaryView()
{
	mDispensaryView = std::make_unique<DispensaryView>(this, ID_DISPENSARY);
	mPanelManager->AddPane(mDispensaryView.get(), wxAuiPaneInfo().Name("DispensaryView").Caption("Dispensary").CenterPane().Hide());
	mPanelManager->Update();
}

void PrescriptionView::CreateDispensaryToolBar()
{
	auto bar = new wxAuiToolBar(this, ID_DISPENSARY_TOOL_BAR, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW);
	bar->SetToolBitmapSize(wxSize(16, 16));
	bar->AddTool(ID_BACK, wxEmptyString, wxArtProvider::GetBitmap("arrow_back"));
	bar->AddStretchSpacer();
	bar->AddTool(ID_ADD_DRUG_TO_PRESCRIPTION, "Add Drug To Prescription", wxArtProvider::GetBitmap("action_add"));
	bar->AddTool(ID_SHOW_PATIENT_FILE,"Show Patient File", wxArtProvider::GetBitmap("folder"));
	bar->AddTool(ID_MAKE_INTERVENTION,"Add Drug Intervention", wxArtProvider::GetBitmap("reply"));
	bar->AddTool(ID_REPORT_INTERACTION,"Report Drug Interaction", wxArtProvider::GetBitmap("comments"));
	bar->AddTool(ID_PREVIEW, "Preiew Label", wxArtProvider::GetBitmap("file"));
	bar->AddTool(ID_DISPENSE, "Dispense", wxArtProvider::GetBitmap("download"));
	bar->Realize();
	mPanelManager->AddPane(bar, wxAuiPaneInfo().Name(wxT("DispensaryToolBar")).Caption(wxT("Tool bar")).ToolbarPane().Top()
		.Resizable().MinSize(wxSize(-1, 30)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false).Hide());

}

void PrescriptionView::CreatePrescriptionSourceChoice()
{
	//this is where communication occurs with the server
	if (!mPrescriptionSourceChoiceBox) {
		spdlog::get("log")->error("Cannot create prescription choice");
		return;
	}
	//start session
	auto sp = std::make_shared<nl::session<http::string_body>>(NetworkManagerInstance::instance().GetIoContex());
	if (sp) {
		//for now, later the source path would contain the pharmacy id which would used by the server to get sources in the
		//area or the locations that are closer to the pharmacy

		//still doesnt feel right,
		//the wait is still long and the cancel dont return a 
		//send a get request to the server to get all the prescrption sources 
		auto future = sp->req<http::verb::get>("localhost", "3000", "/prescriptions/sources/all");
		std::future_status status;
		wxProgressDialog dialog("Prescription Source", 
			fmt::format("Connecting to {}", "localhost"),
			100,
			this, 
			wxPD_APP_MODAL | wxPD_CAN_ABORT);
		std::chrono::milliseconds duration(3);
		bool cont = true;
		int i = 0;
		do {
			cont = dialog.Update( i + duration.count(), fmt::format("Waiting on {}", "localhost"));
			if (!cont) {
				if (wxMessageBox("Do really want to cancel",
					"Prescription Source", wxICON_WARNING | wxYES_NO) == wxYES) {
					sp->cancel();
					break;
				}
			}
			status = future.wait_for(duration);
		} while (status != std::future_status::ready);

		try {
			auto string_data = future.get();
			dialog.Update(100, fmt::format("Successful"));
			OnPrscriptionSource(js::json::parse(string_data));
		}
		catch (const nl::session_error& error) {
			const beast::error_code& ec = error.error_code();
			if(ec != beast::errc::operation_canceled) {
				wxMessageBox(fmt::format("{}, {:d}", ec.message(), ec.value()),
					"Prescription Source", wxICON_ERROR | wxOK);
			}
		}
	}
}

void PrescriptionView::InitDataView()
{
	//create the columns For the data
	mDataView->AppendTextColumn("Date issued", 2, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Patient name", 4, wxDATAVIEW_CELL_INERT, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Patient Address", 5, wxDATAVIEW_CELL_INERT, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Prescription status", 11, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);

	mPanelManager->AddPane(mDataView.get(), wxAuiPaneInfo().Name("DataView").Caption("Prescriptions").CenterPane());
}

void PrescriptionView::SetSpecialColumns()
{
	static std::array<std::string, static_cast<int>(precription_state::max)> StateToText
	{ "PENDING", "COMPLETED", "REJECTED", "INVALID", "PARTIAL COMPLETED" };
	//set upspecial columns
	mModel->SetSpecialColumnHandler(11, [&](size_t col, size_t index)-> wxVariant{
		return wxVariant(StateToText[static_cast<int>(nl::row_value<Prescriptions::prescription_state>(PrescriptionInstance::instance()[index]))]);
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
		{"dir_for_use", "Take 2 tablets when required"},
		{"quantity", 30},
		{"status", "pending"}
	};
	
	js::json med2 = {
		{"medication_name", "Vitamin C"},
		{"dosage_form", "Tablet"},
		{"strength", "500mg"},
		{"dir_for_use", "Take 2 tablets when required"},
		{"quantity", 30},
		{"status", "pending"}
	};

	js::json med3 = {
		{"medication_name", "Aspirin"},
		{"dosage_form", "Tablet"},
		{"strength", "75mg"},
		{"dir_for_use", "Take 2 tablets when required"},
		{"quantity", 30},
		{"status", "pending"}
	};
	//changee from object to array
	js::json object = js::json::array({ med1, med2, med3 });
	std::string medications = js::to_string(object);
	Prescriptions::notification_data data;
	//continue here tomorrow
	data.row_iterator = instance.add(GenRandomId(), id, nl::clock::now(), medications, "Zino ferife", "433 DBS road, Asaba, Delta state",
		25, "36.5", "Dr Vivienne Ferife", "Federal Medical Center, Asaba, Delta state", "00098834561", precription_state::pending);
	instance.notify<nl::notifications::add>(data);
}

void PrescriptionView::OnAddPrescription(wxCommandEvent& evt)
{
	//test
	GenerateFakePrescription();
}

void PrescriptionView::OnDispense(wxCommandEvent& evt)
{
	mDispensaryView->Dispense();
}

void PrescriptionView::OnShowPaitientFile(wxCommandEvent& evt)
{
}

void PrescriptionView::OnMakeIntervention(wxCommandEvent& evt)
{
}

void PrescriptionView::OnReportInteraction(wxCommandEvent& evt)
{
}

void PrescriptionView::OnAddDrugToPrescription(wxCommandEvent& evt)
{
}

void PrescriptionView::OnLabelPreview(wxCommandEvent& evt)
{
	mDispensaryView->PreviewLabel();
}

void PrescriptionView::OnRefresh(wxCommandEvent& evt)
{
	//reloads new precriptions from the server,

}

void PrescriptionView::OnPrescriptionChoiceChange(wxCommandEvent& evt)
{
}

void PrescriptionView::OnSearchforPrescription(wxCommandEvent& evt)
{
}

void PrescriptionView::OnGetPrescriptionSources(wxCommandEvent& evt)
{
	//this is where communication occurs with the server
	if (!mPrescriptionSourceChoiceBox) {
		spdlog::get("log")->error("Cannot create prescription choice");
	}
	//start session
	auto sp = std::make_shared<nl::session<http::string_body>>(NetworkManagerInstance::instance().GetIoContex());
	if (sp) {
		//for now, later the source path would contain the pharmacy id which would used by the server to get sources in the
		//area or the locations that are closer to the pharmacy

		//send a get request to the server to get all the prescrption sources 
		auto future = sp->req<http::verb::get>("localhost", "3000", "/prescriptions/sources/all");
		std::future_status status;
		wxProgressDialog dialog("Prescription Source",
			fmt::format("Connecting to {}", "localhost"),
			100,
			this,
			wxPD_APP_MODAL | wxPD_CAN_ABORT);
		std::chrono::seconds duration(5);
		bool cont = true;
		int i = 0;
		do {
			status = future.wait_for(duration);
			cont = dialog.Update(i + duration.count(), fmt::format("Waiting on {}", "localhost"));
			if (!cont) {
				if (wxMessageBox("Do really want to cancel",
					"Prescription Source", wxICON_WARNING | wxYES_NO) == wxYES) {
					sp->cancel();
					break;
				}
			}
		} while (status != std::future_status::ready);

		try {
			auto string_data = future.get();
			OnPrscriptionSource(js::json::parse(string_data));
		}
		catch (const nl::session_error& error) {
			const beast::error_code& ec = error.error_code();
			if (ec != beast::errc::operation_canceled) {
				wxMessageBox(fmt::format("{}, {:d}", ec.message(), ec.value()),
					"Prescription Source", wxICON_ERROR | wxOK);
			}
		}
	}

}



void PrescriptionView::OnPrescriptionActivated(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (item.IsOk()){
		int index = mModel->GetDataViewItemIndex(item);
		if (index == -1) return;
		auto& disPane = mPanelManager->GetPane("DispensaryView");
		if (disPane.IsOk()) {
			mPanelManager->GetPane("DataView").Hide();
			mPanelManager->GetPane("Tool").Hide();
			mDispensaryView->Load(PrescriptionInstance::instance().get_iterator(index));
			disPane.Show();
			mPanelManager->GetPane("DispensaryToolBar").Show();
			mPanelManager->Update();
		}
	}
}

void PrescriptionView::OnBack(wxCommandEvent& evt)
{
	if (!mPanelManager->GetPane("DataView").IsShown()) {
		mDispensaryView->ResetViewData();
		mPanelManager->GetPane("DispensaryView").Hide();
		mPanelManager->GetPane("DispensaryToolBar").Hide();
		mPanelManager->GetPane("DataView").Show();
		mPanelManager->GetPane("Tool").Show();
		mPanelManager->Update();
	}

}

void PrescriptionView::OnPrscriptionSource(const nl::js::json& res)
{
	//this is called from  the network thread, it needs to do its work fast and finish
	auto sources_iter = res.find("prescription_sources");
	if (sources_iter == res.end()) {
		spdlog::get("log")->error("Prescripton source; invalid Json in response from Server");
		return;
	}
	if (!sources_iter->is_array()) {
		spdlog::get("log")->error("Prescription source: invalid json response, sources not in the format that is acceptable from the server");
		return;
	}

	wxArrayString mArray;
	mArray.reserve(sources_iter->size());
	for (int i = 0; i < sources_iter->size(); i++) {
		mArray.push_back(std::string((*sources_iter)[i]));
	}

	//can i write into the mPrescriptionChoice widget from another thread??
	mPrescriptionSourceChoiceBox->Append(mArray);

}

void PrescriptionView::OnError(const std::string& what)
{

}
