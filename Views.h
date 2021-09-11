#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>

//nl
#include <hierarchy_creation.h>
#include <relation.h>

//dosent really need th virtual
//i would rather do specialisation than implement all these functions as overriden functions in DataView class

template<typename T, typename base>
class ViewHandler : public base
{
public:
	virtual ~ViewHandler() {}
	virtual void CreateList(T& data, wxListCtrl* control){}
	virtual void ShowList(T& data, wxListCtrl* control){}

//events handling, forwared from the list view
public:
	virtual void OnClicked(T& data, wxListCtrl* control, wxListEvent& evt){}
	virtual void OnRightClicked(T& data, wxListCtrl* control, wxListEvent& evt){}
	virtual void OnItemSelected(T& data, wxListCtrl* control, wxListEvent& evt){}
	virtual void OnItemActivated(T& data, wxListCtrl* control, wxListEvent& evt){}
	virtual wxString OnGetItemText(T& data, long item, long column) const {
		return wxString();
	}
	virtual int OnGetItemImage(T& data, long item) const {
		return -1;
	}
	virtual int OnGetItemColumnImage(T& data, long item, long column) const {
		return -1;
	}
	virtual wxItemAttr* OnGetItemColumnAttr(T& data, long item, long column) const {
		return nullptr;
	}
	virtual wxItemAttr* OnGetItemAttr(T& data, long item) const {
		return nullptr;
	}
	virtual bool OnGetItemIsChecked(T& data, long item) const {
		return nullptr;
	}
	
	/*
	virtual void OnItemDeleted(T& data, wxListCtrl* control, wxListEvent& evt){}

	virtual void OnItemInserted(T& data, wxListCtrl* control, wxListEvent& evt){}

	virtual void OnKeyDown(T& data, wxListCtrl* control, wxListEvent& evt){}

	virtual void OnColumnClicked(T& data, wxListCtrl* control, wxListEvent& evt){}

	virtual void onColumnRightClicked(T& data, wxListCtrl* control, wxListEvent& evt){}

	virtual void OnCacheHint(T& data, wxListCtrl* control, wxListEvent& evt){}

	virtual void OnChecked(T& data, wxListCtrl* control, wxListEvent& evt){}

	virtual void OnUnchecked(T& data, wxListCtrl* control, wxListEvent& evt){}
	*/
};





