#pragma once
#include "common.h"
#include <wx/listctrl.h>
#include <wx/aui/framemanager.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include "LogSink.h"
#include <Database.h>
#include <string>
#include <Singleton.h>

#include <relation.h>
#include <table.h>
#include <memory>
#include "Tables.h"
#include "ListControlFunctions.h"

constexpr const char* database_file_path = "C:\\Users\\ferif\\source\\repos\\SQLearn\\test2.db";
using namespace std::literals::string_literals;


class MainFrame : public wxFrame
{
public:

	enum
	{
		LOG_BOOK = wxID_HIGHEST + 1

	};

	MainFrame(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size);
	virtual ~MainFrame();
	
public:
	void CreateList();
	void CreateLogBook();
	void CreateToolBar();
	void CreateMenuBar();
	void CreateStatusBar();
	void Test();
	void CreateTables();
	
	inline std::shared_ptr<spdlog::logger>& GetLogger()
	{return mLog;}

private:
	void OnClose(wxCloseEvent& event);

private:
	std::unique_ptr<wxListCtrl> mList;
	std::shared_ptr<wxTextCtrl> mLogBook;
	std::unique_ptr<wxAuiManager> mFrameManager;
	std::shared_ptr<spdlog::logger> mLog;
	nl::database_connection connection{database_file_path};
	DECLARE_EVENT_TABLE()
private:
	Products products;
	Categories categories;

	wxListCtrl* new_list;

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
};

