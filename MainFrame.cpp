#include "common.h"

#include "MainFrame.h"

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_CLOSE(MainFrame::OnClose)
EVT_ERASE_BACKGROUND(MainFrame::onEraseBackground)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
EVT_MENU(MainFrame::ID_LOG, MainFrame::OnLog)
EVT_MENU(MainFrame::ID_MODULE, MainFrame::OnModule)
EVT_MENU(MainFrame::ID_IMPORT_JSON, MainFrame::OnImportJson)
EVT_AUITOOLBAR_TOOL_DROPDOWN(MainFrame::ID_TOOL_USER, MainFrame::OnUserBtnDropDown)
EVT_MENU(MainFrame::ID_USER_CREATE_ACCOUNT, MainFrame::OnCreateAccount)
EVT_MENU(MainFrame::ID_USER_LOG_OUT, MainFrame::OnSignOut)
EVT_MENU(MainFrame::ID_NETWORK_SETTING, MainFrame::OnSetUpNetworkAddress)
END_EVENT_TABLE()


MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size)
: wxFrame(parent,id, "Afrobug PharmaOffice", position, size){
	mFrameManager = std::make_unique<wxAuiManager>(this);
	CreateLogBook();
	SetMainFrameArt();
	Settings();
	wxArtProvider::Push(new ArtProvider);
	CreateMenuBar();
	CreateToolBar();
	
	mLog->info("Main frame constructor");
	mLog->info("Creating database");
	CreateDatabase();


	mLog->info("Creating All tables");
	TableMonoState::CreateAllTables();

	mLog->info("Creating Main view");
	CreateDataView();

	CreateDatabaseMgr();
	mLog->info("Starting network context");
	NetInstance::instance().RunContext();

	mFrameManager->Update();
	mLog->info("Main frame constructed sucessfully");
}

MainFrame::~MainFrame()
{
	if (mFrameManager)
	{
		mFrameManager->UnInit();
	}
	mFrameManager.reset(nullptr);
}



void MainFrame::CreateLogBook()
{
	mLogBook = std::make_shared<wxTextCtrl>(this, LOG_BOOK, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH | wxTE_MULTILINE | wxTE_READONLY);
	mFrameManager->AddPane(mLogBook.get(), wxAuiPaneInfo().Name("mLogBook").Caption("Log").BestSize(300,400).Bottom().PaneBorder(false));
	mLog = spdlog::app_logger_mt("log", mLogBook);
	mFrameManager->Update();
}

void MainFrame::CreateToolBar()
{
	mToolBar.reset(new wxAuiToolBar(this, ID_TOOL, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT));
	mToolBar->SetToolBitmapSize(wxSize(16, 16));
	stretchspacer = mToolBar->AddStretchSpacer();

	tool = mToolBar->AddTool(ID_TOOL_USER, wxT("Sign in"), wxArtProvider::GetBitmap("login"));
	tool->SetHasDropDown(true);
	mToolBar->Realize();
	mFrameManager->AddPane(mToolBar.get(), wxAuiPaneInfo().Name(wxT("Tool")).Caption(wxT("Tool bar"))
		.ToolbarPane().Top().DockFixed().Resizable().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void MainFrame::CreateMenuBar()
{
	wxMenuBar* menubar = new wxMenuBar;
	wxMenu* file = new wxMenu;
	file->Append(ID_BACK_UP_DATA, "Back up data");
	file->Append(ID_IMPORT_JSON, "Import json data");

	wxMenu* Inventory = new wxMenu;
	Inventory->Append(ID_NEW_PRODUCT, "New product\tCtrl-D");
	Inventory->Append(ID_PRODUCT_SEARCH, "Product search \tCtrl-Q");

	wxMenu* views = new wxMenu;
	views->Append(ID_LOG, "Log");
	views->Append(ID_MODULE, "Modules");

	wxMenu* settings = new wxMenu;
	settings->Append(ID_NETWORK_SETTING, "Network settings");

	wxMenu* Help = new wxMenu;
	Help->Append(wxID_ABOUT);


	menubar->Append(file, wxT("&File"));
	menubar->Append(Inventory, wxT("&Products"));
	menubar->Append(views, wxT("&Views"));
	menubar->Append(settings, wxT("&Settings"));
	menubar->Append(Help, wxT("Help"));
	SetMenuBar(menubar);
}

void MainFrame::CreateStatusBar()
{
}


void MainFrame::CreateTables()
{
}

void MainFrame::CreateDatabase()
{
	//create a database connection
	auto DatabasePath = std::filesystem::current_path() / "asserts\\.data";
	if (!std::filesystem::exists(DatabasePath)){
		mLog->error("Database directory not found");
		return;
	}
	DatabasePath /= "pharmaOfficeDatabase.db";
	try {
		DatabaseInstance::instance(DatabasePath.string());
	}
	catch (std::exception& exp)
	{
		mLog->error("{}", exp.what());
		return;
	}
	DatabaseInstance::instance().set_update_handler([](void* arg, int evt, char const* database_name, char const* table_name, sqlite_int64 rowid){
		switch (evt)
		{
		case nl::database_evt::D_INSERT:
			spdlog::get("log")->info(" inserting into {}", table_name);
			break;
		case nl::database_evt::D_READ:
			spdlog::get("log")->info("Reading from {}, rowid {:d}", table_name, rowid);
			break;
		case nl::database_evt::D_UPDATE:
			spdlog::get("log")->info("Updating {}, rowid {:d}", table_name, rowid);
			break;
		}
		}, nullptr);
}

//This should be customisable
void MainFrame::SetMainFrameArt()
{
	SetBackgroundColour(*wxWHITE);
	wxAuiDockArt* art = mFrameManager->GetArtProvider();
	art->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 24);
	art->SetMetric(wxAUI_DOCKART_GRIPPER_SIZE, 5);
	art->SetMetric(wxAUI_DOCKART_SASH_SIZE, 5);
	art->SetColour(wxAUI_DOCKART_SASH_COLOUR, *wxWHITE);
	art->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, *wxWHITE);
	art->SetColour(wxAUI_DOCKART_BORDER_COLOUR, *wxWHITE);
	art->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	art->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_HORIZONTAL);
	mFrameManager->SetFlags(mFrameManager->GetFlags() | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_VENETIAN_BLINDS_HINT);
	wxInitAllImageHandlers();
}

void MainFrame::CreateDataView()
{
	mViewCtrl = std::make_unique<MainView>(this, MAIN_VIEW);
	mFrameManager->AddPane(mViewCtrl.get(), wxAuiPaneInfo().Name("ViewCtrl").Caption("View").CenterPane());
	mFrameManager->Update();
}

void MainFrame::CreateTestUser()
{
	nl::uuid id;
	id.generate();
	Users::notification_data data;
	data.row_iterator = UsersInstance::instance().add(GenRandomId(), "Ferife", "Zino", "09131861793", 10, "zino", id, nl::blob_t{});
	UsersInstance::instance().notify<nl::notifications::add>(data);
}

void MainFrame::CreateDatabaseMgr()
{
	mUsersDatabaseMgr = std::make_unique<DatabaseManager<Users>>(UsersInstance::instance(), DatabaseInstance::instance());
	mUsersDatabaseMgr->CreateTable();
	mUsersDatabaseMgr->LoadTable();
}

void MainFrame::Settings()
{
	wxTheColourDatabase->AddColour("Aqua", wxColour(240, 255, 255));
	wxTheColourDatabase->AddColour("Navajo_white", wxColour(255, 222, 173));
	wxTheColourDatabase->AddColour("Tomato", wxColour(255, 99, 71));
	wxTheColourDatabase->AddColour("Papaya whip", wxColour(255, 239, 213));
}


wxSize MainFrame::ResizeTool(const std::string& string)
{
	std::string x = "X";
	int w, h, temp;
	mToolBar->GetTextExtent(x, &temp, &h);
	mToolBar->GetTextExtent(string, &w, &temp);
	return wxSize(w, h);
}

void MainFrame::OnClose(wxCloseEvent& event)
{
	spdlog::drop("log");
	mLog = nullptr;
	mLogBook = nullptr;

	event.Skip();
}

void MainFrame::OnImportJson(wxCommandEvent& evt)
{
	wxFileDialog dialog(this);
	if (dialog.ShowModal() == wxID_OK)
	{
		std::string filename = dialog.GetPath().ToStdString();
		if (filename.empty()) return;
		mViewCtrl->ImportJson(filename);
	}
}

void MainFrame::onEraseBackground(wxEraseEvent& evt)
{
	evt.Skip();
}

void MainFrame::OnUserBtnDropDown(wxAuiToolBarEvent& evt)
{
	if (!mActiveUser) {
		mActiveUser.reset(new ActiveUser(UsersInstance::instance()));
	}
	if (!mActiveUser->IsSignedIn())
	{
		if (evt.IsDropDownClicked()){
			wxMenu* menu = new wxMenu;
			auto create_acc = menu->Append(ID_USER_CREATE_ACCOUNT, wxT("Create account"));
			create_acc->SetBitmaps(wxArtProvider::GetBitmap("save"));
			mToolBar->PopupMenu(menu);
			return;
		}

		SignInDialog dialog(this, mActiveUser.get());
		if (dialog.ShowModal() == wxID_OK) {
			//signed in 
			mToolBar->Freeze();
			std::string name = fmt::format("{} {}",
				nl::row_value<Users::surname>(mActiveUser->GetActiveUser()),
				nl::row_value<Users::name>(mActiveUser->GetActiveUser()));
			tool->SetMinSize(ResizeTool(name));
			tool->SetLabel(name);
			tool->SetBitmap(wxArtProvider::GetBitmap("user"));
			mToolBar->SetToolDropDown(tool->GetId(), true);
			mToolBar->Realize();
			mToolBar->Thaw();
			mToolBar->Refresh();
			mFrameManager->Update();

			//send a sign in event
			Users::notification_data data;
			data.row_iterator = UsersInstance::instance()
				.find_on<Users::username>(nl::row_value<Users::username>(mActiveUser->GetActiveUser()));
			data.event_type = 1;
			UsersInstance::instance().notify<nl::notifications::evt>(data);
		}
	}
	else{
		if (evt.IsDropDownClicked())
		{
			wxMenu* menu = new wxMenu;
			auto profile = menu->Append(ID_USER_PROFILE, wxT("Profile"));
			auto log_out = menu->Append(ID_USER_LOG_OUT, wxT("Log out"));
			auto create_acc = menu->Append(ID_USER_CREATE_ACCOUNT, wxT("Create account"));

			profile->SetBitmaps(wxArtProvider::GetBitmap("user"));
			log_out->SetBitmaps(wxArtProvider::GetBitmap("rss"));
			create_acc->SetBitmaps(wxArtProvider::GetBitmap("save"));


			mToolBar->PopupMenu(menu);
			return;
		}
	}
}

void MainFrame::OnSignOut(wxCommandEvent& evt)
{
	if (wxMessageBox("Are you sure you want to sign out?", "SIGN OUT", wxYES_NO | wxICON_INFORMATION) == wxYES)
	{
		mToolBar->Freeze();
		//tool->SetMinSize(ResizeTool("Sign in"));
		tool->SetLabel(wxT("Sign in"));
		tool->SetBitmap(wxArtProvider::GetBitmap("login"));
		mToolBar->SetToolDropDown(tool->GetId(), true);
		mToolBar->Realize();
		mToolBar->Thaw();
		mToolBar->Refresh();
		mFrameManager->Update();

		Users::notification_data data;
		data.row_iterator = UsersInstance::instance()
			.find_on<Users::username>(nl::row_value<Users::username>(mActiveUser->GetActiveUser()));
		data.event_type = 0;
		UsersInstance::instance().notify<nl::notifications::evt>(data);

	}

}

void MainFrame::OnCreateAccount(wxCommandEvent& evt)
{
	CreateAccountDialog dialog(this, wxID_ANY);
	if (dialog.ShowModal() == wxID_OK){
		Users::notification_data data;
		data.row_iterator= UsersInstance::instance().add(dialog.GetNewUser());
		UsersInstance::instance().notify<nl::notifications::add>(data);
	}
}

void MainFrame::OnSetUpNetworkAddress(wxCommandEvent& evt)
{
	wxTextEntryDialog addressDialog(this, "Enter a valid Address", "Network setting");
	if (addressDialog.ShowModal() == wxID_OK) {
		std::string address = addressDialog.GetValue().ToStdString();
		if (VerifyAddress(address)) {
			wxTextEntryDialog portDialog(this, "Enter a valid port", "Network Setting");
			if (portDialog.ShowModal() == wxID_OK) {
				std::string port = portDialog.GetValue().ToStdString();
				try {
					int portNum = std::stoi(port);
					NetInstance::instance().SetServerSubscriberEndpoint({ asio::ip::address::from_string(address), 
						static_cast<std::uint16_t>(portNum) });
					wxMessageBox(fmt::format("Connecting to {} at port {:d}", NetInstance::instance().GetServerSubscriberEndpointAddressAsString()
						, portNum), "Network Settings", wxICON_INFORMATION | wxOK);
				}
				catch (std::invalid_argument const& ex) {
					wxMessageBox(fmt::format("Invalid port, {}", ex.what()), "Network Setting",
						wxICON_ERROR | wxOK);
				}
			}
		}
		else {
			wxMessageBox("Invalid network address entered", "Network Setting", wxICON_ERROR | wxOK);
		}
	}
		 
}

bool MainFrame::VerifyAddress(const std::string& address)
{
	std::stringstream in(address, std::ios::in);
	std::string temp;
	int count = 0;
	while (std::getline(in, temp, '.')) {
		if (temp.empty()) {
			return false;
		}
		for (auto& c : temp) {
			if (!std::isdigit(c)) return false;
		}
		count++;
	}
	if (count != 4) {
		return false;
	}
	return true;
}

void MainFrame::OnAbout(wxCommandEvent& evt)
{
	wxAboutDialogInfo info;
	info.SetName(wxT("Afrobug PharmaOffice"));
	info.SetVersion(wxT("0.0.1 pre beta"));
	info.SetDescription(wxT("Pharmacy mamagement system aid in the managment of pharmaceutical products, sale, transactions, prescription, expiry and so much more"));
	info.SetCopyright(wxT("(C) 2021 Afrobug Software"));
	info.AddDeveloper("Ferife Zino :)");
	wxAboutBox(info);
}

void MainFrame::OnLog(wxCommandEvent& evt)
{
	auto& logPane = mFrameManager->GetPane("mLogBook");
	if (logPane.IsOk() && !logPane.IsShown()){
		logPane.Show();
		mFrameManager->Update();
	}
}

void MainFrame::OnModule(wxCommandEvent& evt)
{
	if (mViewCtrl) {
		mViewCtrl->ShowModuleSelect();
	}
}

