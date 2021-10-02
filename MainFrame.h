#pragma once
#include "common.h"
#include <wx/listctrl.h>
#include <wx/aui/framemanager.h>
#include <wx/aboutdlg.h>
#include <wx/aui/dockart.h>
#include <wx/aui/auibar.h>
#include <wx/srchctrl.h>

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <string>
#include <memory>
#include <filesystem>

//nl stuff
#include <Singleton.h>
#include <Database.h>
#include <relation.h>
#include <table.h>
#include <nl_time.h>


#include "LogSink.h"
#include "Tables.h"
#include "Instances.h"
#include "DataModel.h"
#include "MainView.h"
#include "ArtProvider.h"


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
		ID_BACK_UP_DATA = wxID_HIGHEST + 200,
		ID_NEW_PRODUCT,
		ID_PRODUCT_SEARCH,
		ID_LOG
	};

	enum
	{
		ID_TOOL_DOWNLOAD_DATA = wxID_HIGHEST + 400,
		ID_TOOL_USER
	};


	MainFrame(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size);
	virtual ~MainFrame();


private:
	void CreateLogBook();
	void CreateToolBar();
	void CreateMenuBar();
	void CreateStatusBar();
	void CreateTables();
	void CreateDatabase();
	void SetMainFrameArt();
	void CreateDataView();
	void Settings();
private:
	std::shared_ptr<wxTextCtrl> mLogBook;
	std::unique_ptr<wxAuiManager> mFrameManager;
	std::unique_ptr<MainView> mViewCtrl;
	std::shared_ptr<spdlog::logger> mLog;


//main frame event handlers
private:
	void OnAbout(wxCommandEvent& evt);
	void OnLog(wxCommandEvent& evt);
	void OnClose(wxCloseEvent& evt);
	void onEraseBackground(wxEraseEvent& evt);

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

