#pragma once
#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/dataview.h>
#include <wx/artprov.h>
#include <wx/srchctrl.h>
#include <wx/combobox.h>
#include <wx/choice.h>

#include "Searcher.h"
#include "DatabaseManger.h"
#include "Tables.h"
#include "DataModelBase.h"
#include "Instances.h"
#include "SearchAutoComplete.h"
#include "nlohmann/json.hpp"
#include "DispensaryView.h"


#include <nl_http.h>

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
		ID_SHOW_PATIENT_FILE,
		ID_MAKE_INTERVENTION,
		ID_REPORT_INTERACTION,
		ID_ADD_DRUG_TO_PRESCRIPTION,
		ID_PREVIEW,
		ID_SEARCH,
		ID_SEARCH_FOR_PRESCRIPTION,
		ID_SUBSCRIBE,
		ID_REFRESH,
		ID_PSCHOICE,
		ID_PRESCRIPTION_SOURCE,
		ID_GET_PRESCRIPTION_SOURCES,
		ID_BACK
	};

	PrescriptionView(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size);
	
	void CreateToolBar();
	void LoadPrescriptions(const nl::date_time_t& start, const nl::date_time_t& stop);
	void CreateTable();
	void CreateDispensaryView();
	void CreateDispensaryToolBar();
	void CreatePrescriptionSourceChoice();
	void InitDataView();
	void SetSpecialColumns();
	void SetDefaultAuiArt();
	virtual ~PrescriptionView();


private:

	//test
	void GenerateFakePrescription();
	void OnAddPrescription(wxCommandEvent& evt);
	void OnDispense(wxCommandEvent& evt);
	void OnShowPaitientFile(wxCommandEvent& evt);
	void OnMakeIntervention(wxCommandEvent& evt);
	void OnReportInteraction(wxCommandEvent& evt);
	void OnAddDrugToPrescription(wxCommandEvent& evt);
	void OnLabelPreview(wxCommandEvent& evt);
	void OnRefresh(wxCommandEvent& evt);
	void OnPrescriptionChoiceChange(wxCommandEvent& evt);
	void OnSearchforPrescription(wxCommandEvent& evt);
	void OnGetPrescriptionSources(wxCommandEvent& evt);
	//evts 
	void OnPrescriptionActivated(wxDataViewEvent& evt);
	void OnBack(wxCommandEvent& evt);
private:
	void OnPrscriptionSource(const nl::js::json& res);
	void OnError(const std::string& what);

private:
	std::unique_ptr<wxAuiManager> mPanelManager;
	std::unique_ptr<DatabaseManager<Prescriptions>> mDatabaseManger;
	std::unique_ptr<wxDataViewCtrl> mDataView;
	std::unique_ptr<DispensaryView> mDispensaryView;
	DataModel<Prescriptions>* mModel; // dataview handles destruction
	//item attributes
	
	
	wxChoice* mPrescriptionSourceChoiceBox;


	//holds the source target name for the prescriptions 
	std::string mPrescriptionSource;  
	
	//
	DECLARE_EVENT_TABLE()


};

