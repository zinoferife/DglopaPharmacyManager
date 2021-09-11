#include "common.h"

#include "MainFrame.h"

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_CLOSE(MainFrame::OnClose)
EVT_LIST_ITEM_SELECTED(MainFrame::DATA_VIEW, MainFrame::OnItemSelected)
EVT_LIST_ITEM_ACTIVATED(MainFrame::DATA_VIEW, MainFrame::OnItemActivated)
EVT_LIST_ITEM_RIGHT_CLICK(MainFrame::DATA_VIEW, MainFrame::OnRightClicked)
END_EVENT_TABLE()


MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size)
: wxFrame(parent,id, "Afrobug pharmacy manager", position, size){
	mFrameManager = std::make_unique<wxAuiManager>(this);
	CreateLogBook();
	mLog->info("Main frame constructor");

	CreateDefaultView();

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

void MainFrame::OnCacheHint(wxListEvent& event)
{
	mDataView->OnCacheHint(event);
}

void MainFrame::OnItemSelected(wxListEvent& event)
{
	mDataView->OnItemSelected(event);
}

void MainFrame::OnColumnClicked(wxListEvent& event)
{
	mDataView->OnColumnClicked(event);
}

void MainFrame::OnColumnRightClicked(wxListEvent& event)
{
	mDataView->OnColumnRightClicked(event);
}

void MainFrame::OnRightClicked(wxListEvent& evt)
{
	mDataView->OnRightClicked(evt);
}

void MainFrame::OnItemActivated(wxListEvent& evt)
{
	mDataView->OnItemActivated(evt);
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

void MainFrame::CreateDefaultView()
{
	ProductInstance::instance().as<Products::id>("Serial number");
	ProductInstance::instance().as<Products::name>("Name");
	ProductInstance::instance().as<Products::package_size>("Package size");
	ProductInstance::instance().as<Products::stock_count>("Stock count");
	ProductInstance::instance().as<Products::unit_price>("Unit price");
	ProductInstance::instance().as<Products::category_id>("Category id");

	Products::set_default_row(100, "test", 0, 0, "0.00", 9);
	for (int i = 0; i < 100; i++)
	{
		ProductInstance::instance().add_default();
	}
	mDataView = std::make_unique<DataView>(this, DATA_VIEW);
	mDataView->SetItemCount(1000);
	mFrameManager->AddPane(mDataView.get(), wxAuiPaneInfo().Name("List").Caption("Product").CenterPane());
	mFrameManager->Update();
}

void MainFrame::CreateTables()
{
}


void MainFrame::OnClose(wxCloseEvent& event)
{
	spdlog::drop("log");
	mLog = nullptr;
	mLogBook = nullptr;

	event.Skip();
}

