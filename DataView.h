#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "Instances.h"
#include <Database.h>
#include <spdlog/spdlog.h>

#include "Views.h"
#include "ProductsView.h"
#include "SalesView.h"
#include <hierarchy_creation.h>

//all the types of views thats a list
typedef std::tuple<Products, Categories, Sales> views;
typedef nl::detail::gen_linear_hierarchy<views, ViewHandler> TheViews;

//data view for the whole system
class DataView : public wxListCtrl, public TheViews
{
public:
	DataView(wxWindow* parent, wxWindowID id, const wxPoint& position = wxDefaultPosition,
		const wxSize& size = wxDefaultSize, long style = wxLC_REPORT | wxLC_VIRTUAL);
	virtual ~DataView();

	//contants for imagelist
	enum Img
	{

	};

	enum class Views
	{
		Product_view,
		Sale_view
	};


	void ChangeView(Views view);
public:
//list events
	void OnCacheHint(wxListEvent& event);
	void OnItemSelected(wxListEvent& event);
	void OnColumnClicked(wxListEvent& event);
	void OnColumnRightClicked(wxListEvent& evnet);
	void OnRightClicked(wxListEvent& evt);
	void OnItemActivated(wxListEvent& evt);
protected:
	virtual wxString OnGetItemText(long item, long column) const override;
	virtual int OnGetItemImage(long item) const override;
	virtual int OnGetItemColumnImage(long item, long column) const override;
	virtual wxListItemAttr* OnGetItemAttr(long item) const override;
	virtual bool OnGetItemIsChecked(long item) const override;
public:
	void OnProductTableNotification(nl::notifications notif, typename Products::table_t& data,
		const typename Products::row_t& row);
	
	void OnSaleTableNotification(nl::notifications notif, typename Sales::table_t& data,
		const typename Sales::row_t& row);

	void SetColumnImage(size_t colunm, int image);
	void CreateImageList();
private:
	void ResetList();
	void DoSelectView();
	                                                                                                                                                           
private:
	Views mCurrentView;
	int i;
	wxItemAttr mDefaultAttr;
	DECLARE_EVENT_TABLE()
};

