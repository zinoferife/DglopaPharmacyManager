#include "common.h"

#include "MainFrame.h"

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_CLOSE(MainFrame::OnClose)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
END_EVENT_TABLE()


MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size)
: wxFrame(parent,id, "Afrobug pharmacy manager", position, size){
	mFrameManager = std::make_unique<wxAuiManager>(this);
	CreateLogBook();
	CreateMenuBar();

	
	mLog->info("Main frame constructor");
	mLog->info("Creating database");
	CreateDatabase();
	mLog->info("Creating main view");
	CreateMainView();

	mFrameManager->Update();
	mLog->info("Main frame constructed sucessfully");
}

MainFrame::~MainFrame()
{
	if (mFrameManager)
	{
		mFrameManager->UnInit();
	}
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


void MainFrame::CreateDefaultView()
{
	DatabaseInstance::instance(database_file_path);
	ProductInstance::instance().as<Products::id>("Serial number");
	ProductInstance::instance().as<Products::name>("Name");
	ProductInstance::instance().as<Products::package_size>("Package size");
	ProductInstance::instance().as<Products::stock_count>("Stock count");
	ProductInstance::instance().as<Products::unit_price>("Unit price");
	ProductInstance::instance().as<Products::category_id>("Category id");

	Products::set_default_row(100, "test", 0, 0, "0.00", 9);
	for (int i = 0; i < 300; i++)
	{
		ProductInstance::instance().add(i, nl::to_string_date(nl::clock::now()), 100, 123, "now", 2345);
	}
}

void MainFrame::CreateTables()
{
}

void MainFrame::CreateMainView()
{
	CreateDefaultView();
	mMainView = std::make_unique<MainView>(this, MAIN_VIEW);
	auto& added = ProductInstance::instance().add(100, nl::to_string_date(nl::clock::now()), 0, 0, "0.00", 9);
	ProductInstance::instance().notify(nl::notifications::add, added);
	mFrameManager->AddPane(mMainView.get(), wxAuiPaneInfo().Name("MainView").Caption("View").CenterPane());
	mFrameManager->Update();
}

void MainFrame::CreateDatabase()
{
	//create a database connection to the 
	try {
		DatabaseInstance::instance(database_file_path);
	}
	catch (std::exception& exp)
	{
		mLog->error("{}", exp.what());
	}
}

void MainFrame::SetMainFrameArt()
{
	SetBackgroundColour(*wxWHITE);
}

void MainFrame::OnClose(wxCloseEvent& event)
{
	spdlog::drop("log");
	mLog = nullptr;
	mLogBook = nullptr;

	event.Skip();
}

void MainFrame::OnAbout(wxCommandEvent& evt)
{
	wxAboutDialogInfo info;
	info.SetName(wxT("Afrobug PharmaOffice"));
	info.SetVersion(wxT("0.0.0 pre beta"));
	info.SetDescription(wxT("Pharmacy mamagement system aid in the managment of pharmaceutical products, sale, transactions, prescription, expiry and so much more"));
	info.SetCopyright(wxT("(C) 2021 Afrobug Software"));

	wxAboutBox(info);
}

