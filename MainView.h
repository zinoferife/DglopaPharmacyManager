#pragma once
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>
#include "DataView.h"
#include "Instances.h"
#include <spdlog/spdlog.h>
#include <memory>

class MainView : public wxSplitterWindow
{
public:
	enum
	{
		DATA_VIEW,
		TREE_VIEW
	};

	MainView(wxWindow* parent, wxWindowID id, const wxPoint& position = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~MainView() {}

public:
	//creation functions
	void CreateTree();
	void CreateDataView();


// tree event table
public:
	void OnTreeItemActivated(wxTreeEvent& evt);
	void OnTreeGetInfo(wxTreeEvent& evt);
	void OnTreeSetInfo(wxTreeEvent& evt);
	void OnTreeCollapsed(wxTreeEvent& evt);
	void OnTreeCollapsing(wxTreeEvent& evt);
	void OnTreeExpanded(wxTreeEvent& evt);
	void OnTreeExpanding(wxTreeEvent& evt);
	void OnTreeSelectionChanged(wxTreeEvent& evt);
	void OnTreeSelectionChanging(wxTreeEvent& evt);
	void OnTreeStateImageClick(wxTreeEvent& evt);
	void OnTreeItemMenu(wxTreeEvent& evt);

//data view event table
public:
	void OnCacheHint(wxListEvent& evt);
	void OnItemSelected(wxListEvent& evt);
	void OnColumnClicked(wxListEvent& evt);
	void OnColumnRightClicked(wxListEvent& evt);
	void OnRightClicked(wxListEvent& evt);
	void OnItemActivated(wxListEvent& evt);

private:
	std::unique_ptr<wxTreeCtrl> mTree;
	std::unique_ptr<DataView> mDataView;
	DECLARE_EVENT_TABLE()
};

