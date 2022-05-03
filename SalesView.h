#pragma once
//the main pos system 
//products are bought and the inventory is updated
//the view of the system

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/dataview.h>
#include <wx/artprov.h>
#include <wx/srchctrl.h>
#include <wx/combobox.h>
#include <wx/progdlg.h>
#include <wx/tglbtn.h>



#include "Tables.h"
#include "DataModelBase.h"
#include "Instances.h"
#include "SearchAutoComplete.h"
#include "InventoryView.h"
#include "ProductEntryDialog.h"
#include "Searcher.h"
#include "DatabaseManger.h"
#include "DetailView.h"
#include "ExpiryView.h"
#include "ListDisplayDialog.h"


#include <nl_uuid.h>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <fstream>
#include <unordered_map>
#include <set>
#include <bitset>
#include <memory>
#include <random>
#include <functional>


class SalesView
{
public:
	~SalesView();
	SalesView(wxWindow* parent,
		wxWindowID id,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize);
	//ids 
	enum {


	};


private:
	std::unique_ptr<wxAuiManager> mViewManager;
	std::unique_ptr<wxDataViewCtrl> mDataView;
	DataModel<Sales>* mModel;

	wxAuiToolBar* bar;
};

