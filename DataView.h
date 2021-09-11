#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "Instances.h"
#include <Database.h>
#include <spdlog/spdlog.h>

#include "Views.h"
#include "ProductsView.h"
#include <hierarchy_creation.h>

//all the types of views thats a list
typedef std::tuple<Products, Categories, Sales> views;
typedef nl::detail::gen_linear_hierarchy<views, ViewHandler> TheViews;

//data view for the whole system
class DataView : public wxListCtrl, public TheViews
{
public:
	typedef size_t view_t;
	struct ViewEvent
	{
		std::string mDescription;
		view_t mEventFor;
	};

	DataView(wxWindow* parent, wxWindowID id, const wxPoint& position = wxDefaultPosition,
		const wxSize& size = wxDefaultSize, long style = wxLC_REPORT | wxLC_VIRTUAL | wxLC_HRULES);

	virtual ~DataView();

	//contants for imagelist
	enum Img
	{

	};

	enum Views
	{
		Product_v
	};
public:
//list events
	void OnCacheHint(wxListEvent& event);
	void OnItemSelected(wxListEvent& event);
	void OnColumnClicked(wxListEvent& event);
	void OnColumnRightClicked(wxListEvent& evnet);
	void OnRightClicked(wxListEvent& evt);
	void OnItemActivated(wxListEvent& evt);
protected:
	virtual wxString OnGetItemText(long item, long column) const;
	virtual int OnGetItemImage(long item) const;
	virtual int OnGetItemColumnImage(long item, long column) const;
	virtual wxItemAttr* OnGetItemColumnAttr(long item, long column) const;
	virtual wxItemAttr* OnGetItemAttr(long item) const;
	virtual bool OnGetItemIsChecked(long item) const;
public:
	void SetColumnImage(size_t colunm, int image);
	void CreateImageList();
private:
	void ResetList();
	void DoSelectView();

private:
	view_t mCurrentView;
	int i;
	wxItemAttr mDefaultAttr;
	DECLARE_EVENT_TABLE()
};

