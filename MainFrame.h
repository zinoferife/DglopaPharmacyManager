#pragma once
#include "common.h"
#include <wx/listctrl.h>
#include <wx/aui/framemanager.h>
#include <wx/aboutdlg.h>

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <string>
#include <memory>

//nl stuff
#include <Singleton.h>
#include <Database.h>
#include <relation.h>
#include <table.h>
#include <nl_time.h>


#include "LogSink.h"
#include "Tables.h"
#include "Instances.h"
#include "DataView.h"

#include "MainView.h"


constexpr const char* database_file_path = "C:\\Users\\ferif\\source\\repos\\SQLearn\\test2.db";
using namespace std::literals::string_literals;


class MainFrame : public wxFrame
{
public:

	enum
	{
		LOG_BOOK = wxID_HIGHEST + 1,
		DATA_VIEW,
		MAIN_VIEW
	};

	//menu ID
	enum
	{
		ID_BACK_UP_DATA,
		ID_NEW_PRODUCT,
		ID_PRODUCT_SEARCH
	};

	MainFrame(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size);
	virtual ~MainFrame();


private:
	void CreateLogBook();
	void CreateToolBar();
	void CreateMenuBar();
	void CreateStatusBar();
	void CreateDefaultView();
	void CreateTables();
	void CreateMainView();
	void CreateDatabase();
	void SetMainFrameArt();

private:
	std::unique_ptr<MainView> mMainView;
	std::shared_ptr<wxTextCtrl> mLogBook;
	std::unique_ptr<wxAuiManager> mFrameManager;
	std::shared_ptr<spdlog::logger> mLog;


//main frame event handlers
private:
	void OnAbout(wxCommandEvent& evt);
	void OnClose(wxCloseEvent& event);


private:
	

	//debug
	template<typename func>
	void time_it(const std::string& text, func c)
	{
		auto start = std::chrono::high_resolution_clock::now();
		c();
		auto stop = std::chrono::high_resolution_clock::now();
		float run = std::chrono::duration<float, std::chrono::milliseconds::period>(stop - start).count();
		mLog->info("{}: {}ms", text, run);
	}

	DECLARE_EVENT_TABLE()
};

