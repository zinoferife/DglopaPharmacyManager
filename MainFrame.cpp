#include "common.h"

#include "MainFrame.h"

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE()


MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size)
: wxFrame(parent,id, "Afrobug pharmacy manager", position, size){
	mFrameManager = std::make_unique<wxAuiManager>(this);
	CreateLogBook();
	mLog->info("Main frame constructor");
	CreateTables();
	CreateList();
	Test();
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

void MainFrame::CreateList()
{
	mList.reset(new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT));
	mFrameManager->AddPane(mList.get(), wxAuiPaneInfo().Name("mList").Caption("List").CenterPane());
	mFrameManager->Update();
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
}

void MainFrame::CreateStatusBar()
{
}

void MainFrame::Test()
{
	
}

void MainFrame::CreateTables()
{
	products.as<Products::id>("Serial number");
	products.as<Products::name>("Name");
	products.as<Products::package_size>("Package size");
	products.as<Products::stock_count>("Stock count");
	products.as<Products::unit_price>("Unit price");
	products.as<Products::category_id>("Category id");


	new_list = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
	Products::set_default_row(100, "test", 0, 0, "0.00", 9);
	for (int i = 0; i < 100; i++)
	{
		products.add_default();
	}

	prepare_list(new_list, products);
	ShowList(products, new_list);
	mFrameManager->AddPane(new_list, wxAuiPaneInfo().Name("List").Caption("Product").Left());
	mFrameManager->Update();
}


void MainFrame::OnClose(wxCloseEvent& event)
{
	spdlog::drop("log");
	mLog = nullptr;
	mLogBook = nullptr;

	event.Skip();
}

