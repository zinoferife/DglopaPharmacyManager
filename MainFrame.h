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
#include <nlohmann/json.hpp>

//nl stuff


#include "LogSink.h"
#include "Tables.h"
#include "TableMonoState.h"
#include "Instances.h"
#include "MainView.h"
#include "ArtProvider.h"
#include "ActiveUser.h"
#include "SignInDialog.h"
#include "DatabaseManger.h"
#include "CreateAccountDialog.h"


using namespace std::literals::string_literals;
namespace fs = std::filesystem;
namespace js = nlohmann;

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
		ID_LOG,
		ID_MODULE,
		ID_IMPORT_JSON,
		ID_USER_LOG_IN,
		ID_USER_LOG_OUT,
		ID_USER_PROFILE,
		ID_USER_CREATE_ACCOUNT,
		ID_BITMAP_PLACEHOLDER
	};

	enum
	{
		ID_TOOL_DOWNLOAD_DATA = wxID_HIGHEST + 400,
		ID_TOOL_USER,
		ID_TOOL
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
	void CreateTestUser();
	void CreateDatabaseMgr();
	void CreateNetworkManger();
	void Settings();
	

	wxSize ResizeTool(const std::string& string);
private:
	std::shared_ptr<wxTextCtrl> mLogBook;
	std::unique_ptr<wxAuiManager> mFrameManager;
	std::unique_ptr<MainView> mViewCtrl;
	std::shared_ptr<spdlog::logger> mLog;
	std::unique_ptr<ActiveUser> mActiveUser;
	std::unique_ptr<wxAuiToolBar> mToolBar;
	std::unique_ptr<DatabaseManager<Users>> mUsersDatabaseMgr;
	std::unique_ptr<wxStaticBitmap> mBitMapPlaceHolder{};
	//holders
	wxAuiToolBarItem* tool;
	wxAuiToolBarItem* stretchspacer;

//main frame event handlers
private:
	void OnAbout(wxCommandEvent& evt);
	void OnLog(wxCommandEvent& evt);
	void OnModule(wxCommandEvent& evt);
	void OnClose(wxCloseEvent& evt);
	void OnImportJson(wxCommandEvent& evt);
	void onEraseBackground(wxEraseEvent& evt);

	//tool bar events
	void OnUserBtnDropDown(wxAuiToolBarEvent& evt);
	void OnSignOut(wxCommandEvent& evt);
	void OnCreateAccount(wxCommandEvent& evt);

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

