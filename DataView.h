#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "Instances.h"
#include <Database.h>
#include <spdlog/spdlog.h>

//data view for the whole system
class DataView : public wxListCtrl
{
public:
	typedef size_t view_t;
	struct ViewEvent
	{
		std::string mDescription;
		view_t mEventFor;
	};

	DataView(const std::string& database, wxWindow* parent, wxWindowID id, const wxPoint& position = wxDefaultPosition,
		const wxSize& size = wxDefaultSize, long style = wxLC_ICON | wxLC_REPORT | wxLC_VIRTUAL | wxLC_HRULES);

	virtual ~DataView();

	//contants for imagelist
	enum Img
	{

	};

	enum Views
	{
		Product_v

	};
	
//views events
public:
	void OnProductView(ViewEvent& event);
	void OnSalesView(ViewEvent& event);
	void OnInventoryView(ViewEvent& event);
	void OnUsersView(ViewEvent& event);
	void OnCustomersView(ViewEvent& event);
	void OnCategoryChange(ViewEvent& event);

public:
//list events
	void OnCacheHint(wxListEvent& event);
	void OnItemSelected(wxListEvent& event);
	void OnColumnClick(wxListEvent& event);
	void OnColumnRightClick(wxListEvent& evnet);

public:
	void SetColumnImage(size_t colunm, int image);
	void CreateImageList();
	void SetLogger(std::shared_ptr<spdlog::logger> Logger);
private:
	void ResetList();
	void DoSelectView();


	template<typename relation_t>
	static void PrepareList(wxListCtrl* list, const relation_t& rel, std::array<size_t, relation_t::column_count>& ImageList)
	{
		for (size_t i = 0; i < relation_t::column_count; i++)
		{
			wxListItem itemCol;
			itemCol.SetText(rel.get_name(i));
			itemCol.SetAlign(wxLIST_FORMAT_CENTER);
			list->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
			SetColumnImage(i, ImageList[i]);
			list->InsertColumn(i, itemCol);
		}
	}
private:
	nl::database_connection& connection;
	view_t mCurrentView;
	std::shared_ptr<spdlog::logger> mLogger;

	DECLARE_EVENT_TABLE()
};

