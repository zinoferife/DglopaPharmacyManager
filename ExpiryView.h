#pragma once
#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/dataview.h>
#include <wx/artprov.h>
#include <wx/srchctrl.h>
#include <wx/combobox.h>
#include <wx/progdlg.h>



#include "DataModelBase.h"
#include "DatabaseManger.h"
#include "Instances.h"
#include "Tables.h"
#include "Searcher.h"
#include "CalenderDialog.h"
#include "ArtProvider.h"

class ex_relation_t : public nl::vector_table<std::uint64_t, std::string, std::string, std::string, std::uint32_t, nl::date_time_t>
{
public:
	enum
	{
		id = 0,
		name,
		p_class,
		category_name,
		stock_count,
		expiry
	};
	BEGIN_COL_NAME("expiry")
		COL_NAME("id")
		COL_NAME("name")
		COL_NAME("class")
		COL_NAME("category")
		COL_NAME("stock_count")
		COL_NAME("expirty")
		END_COL_NAME()
	IMPLEMENT_GET_COL_NAME()
};


class ExpiryView : public wxDialog
{
public:
	enum {
		ID_SEARCH = wxID_HIGHEST + 23000,
		ID_DATA_VIEW,
		ID_CHOICE,
		ID_APPLY,
		ID_CALENDER,
		ID_EXPIRE_DATE,
		ID_FILTER,
		ID_ZERO_STOCK,
		ID_ADD_INVENTORY,
		ID_ADD_TO_ORDER
	};

	//product name, product class, product category, stock count, expiary
	ExpiryView(wxWindow* parent, wxWindowID id, const wxPoint& position = wxDefaultPosition,
		const wxSize& size = wxDefaultSize);
	~ExpiryView();

	void CreateToolBar();
	void CreateBottomToolBar();
	void CreateDataView();
	void CreateTableView();

	void CreateDatabaseManger();
	void CreateViewManger();
	void SetTableAttributes();
	void SetViewArt();
private:
	void OnOK(wxCommandEvent& evt);
	void OnCancel(wxCommandEvent& evt);
	void OnLeftClick(wxDataViewEvent& evt);
	void OnApply(wxCommandEvent& evt);
	void OnSearch(wxCommandEvent& evt);
	void OnSearchCleared(wxCommandEvent& evt);
	void OnClose(wxCloseEvent& evt);
	void OnChoice(wxCommandEvent& evt);
	//context menu callbacks
	void OnAddInventory(wxCommandEvent& evt);
	void OnZeroStock(wxCommandEvent& evt);
private:
	void resetChoice();
	void doSearch(const std::string& search_string);
	wxSize ResizeTool(const std::string& tool);
	std::unique_ptr<wxAuiManager> mDialogManager;
	std::unique_ptr<wxDataViewCtrl> mDataView;
	std::unique_ptr<DatabaseManager<ex_relation_t>> mDatabaseManger;
	std::unique_ptr<wxSearchCtrl> mSearchBar;
	DataModel<ex_relation_t>* mModel;

	//buttons
	nl::date_time_t mTimePoint;
	wxButton* mOk;
	wxButton* mCancel;
	wxButton* mApply;
	wxChoice* mDropDown;
	wxAuiToolBarItem* mDateStatus;
	wxAuiToolBar* mTopBar;
	ex_relation_t ExpireTable;
	std::shared_ptr<wxDataViewItemAttr> mExpired;
	DECLARE_EVENT_TABLE()
};

