#include "common.h"

#include "MainFrame.h"

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_CLOSE(MainFrame::OnClose)
EVT_ERASE_BACKGROUND(MainFrame::onEraseBackground)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
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

	mLog->info("Creating Main view");
	CreateDataView();

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
	wxAuiToolBar* toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW);
	toolbar->SetToolBitmapSize(wxSize(16, 16));
	toolbar->AddStretchSpacer();
	toolbar->AddTool(ID_TOOL_DOWNLOAD_DATA, wxT("Download data"), wxArtProvider::GetBitmap("download"));
	toolbar->AddTool(ID_TOOL_USER, wxT("User"), wxArtProvider::GetBitmap("user"));
	toolbar->Realize();
	mFrameManager->AddPane(toolbar, wxAuiPaneInfo().Name(wxT("Tool")).Caption(wxT("Tool bar"))
		.ToolbarPane().Top().DockFixed().Resizable().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void MainFrame::CreateMenuBar()
{
	wxMenuBar* menubar = new wxMenuBar;
	wxMenu* file = new wxMenu;
	file->Append(ID_BACK_UP_DATA, "Back up data");

	wxMenu* Inventory = new wxMenu;
	Inventory->Append(ID_NEW_PRODUCT, "New product\tCtrl-D");
	Inventory->Append(ID_PRODUCT_SEARCH, "Product search \tCtrl-Q");

	wxMenu* Help = new wxMenu;
	Help->Append(wxID_ABOUT);


	menubar->Append(file, wxT("&File"));
	menubar->Append(Inventory, wxT("&Products"));
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
	}
}

//This should be customisable
void MainFrame::SetMainFrameArt()
{
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

void MainFrame::Settings()
{
	wxTheColourDatabase->AddColour("Aqua", wxColour(240, 255, 255));
}

void MainFrame::OnClose(wxCloseEvent& event)
{
	spdlog::drop("log");
	mLog = nullptr;
	mLogBook = nullptr;

	event.Skip();
}

void MainFrame::onEraseBackground(wxEraseEvent& evt)
{
	evt.Skip();
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

