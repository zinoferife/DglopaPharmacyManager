#pragma once
#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/dataview.h>
#include <wx/artprov.h>
#include <wx/srchctrl.h>
#include <wx/combobox.h>


#include "Searcher.h"
#include "DatabaseManger.h"
#include "Tables.h"
#include "DataModelBase.h"
#include "Instances.h"
#include "SearchAutoComplete.h"
#include "nlohmann/json.hpp"
#include "DispensaryView.h"

namespace js = nlohmann;
class PrescriptionView : public wxPanel
{
public:
	enum {
		ID_DATA_VIEW = wxID_HIGHEST + 2000,
		ID_ADD_PRESCRIPTION,
		ID_DISPENSARY,
		ID_DISPENSARY_TOOL_BAR,
		ID_DISPENSE,
		ID_SEARCH,
		ID_BACK
	};

	PrescriptionView(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size);
	
	void CreateToolBar();
	void LoadPrescriptions(const nl::date_time_t& start, const nl::date_time_t& stop);
	void CreateTable();
	void CreateDispensaryView();
	void CreateDispensaryToolBar();
	void InitDataView();
	void SetSpecialColumns();
	void SetDefaultAuiArt();
	~PrescriptionView();


private:

	//test
	void GenerateFakePrescription();
	void OnAddPrescription(wxCommandEvent& evt);
	void OnDispense(wxCommandEvent& evt);


	//evts 
	void OnPrescriptionActivated(wxDataViewEvent& evt);
	void OnBack(wxCommandEvent& evt);

	std::unique_ptr<wxAuiManager> mPanelManager;
	std::unique_ptr<DatabaseManager<Prescriptions>> mDatabaseManger;
	std::unique_ptr<wxDataViewCtrl> mDataView;
	std::unique_ptr<DispensaryView> mDispensaryView;
	DataModel<Prescriptions>* mModel; // dataview handles destruction

	//item attributes

	
	//
	DECLARE_EVENT_TABLE()


};

