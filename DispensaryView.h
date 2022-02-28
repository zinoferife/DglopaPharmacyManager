#pragma once
#include <wx/wx.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>
#include <wx/notebook.h>
#include <wx/progdlg.h>


#include <wx/aui/aui.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibar.h>

#include "ArtProvider.h"
#include "Tables.h"
#include "Instances.h"
#include "DataModelBase.h"

#include <spdlog/spdlog.h>


#include <memory>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <chrono>


#include "PropertyFunctions.h"
#include "nlohmann/json.hpp"
#include "LabelPrintJob.h"
#include "ListDisplayDialog.h"


namespace js = nlohmann;

class DispensaryView : public wxPanel
{
public:

	enum {
		DATA_VIEW = wxID_HIGHEST + 500,
		PROPERTY_MANAGER
	};

	DispensaryView(wxWindow* parent, wxWindowID id);
	~DispensaryView();
	void SetDefaultArt();
	void Load(Prescriptions::iterator prescription);
	void LoadPropertyGrid(Prescriptions::iterator prescription);
	void CreateToolBar();
	void CreateDataView();
	void SetSpecialCol();
	void CreatePropertyGrid(Prescriptions::iterator prescription);
	void CreatePropertyGridToolBar();
	void CreatePropertyGridArt();
	void CreateEditProperty();
	void ResetViewData();
	void ResetModifiedStatus();
	void Dispense();
	void PreviewLabel();
	bool CheckDispensed();
	void SetUpPropertyCallBacks();
	void UpdatePrescription();
	//set update callback for the update
	void OnProperyUpdate(const medications::table_t& table, const medications::notification_data& data);
	std::set<size_t> mSelections;
private:
	void OnDataViewItemSelected(wxDataViewEvent& evt);
	void OnPropertyChanged(wxPropertyGridEvent& evt);
	void OnPropertyChanging(wxPropertyGridEvent& evt);
private:
	std::unique_ptr<wxAuiManager> mPanelManager;
	std::unique_ptr<wxDataViewCtrl> mDataView;
	std::unique_ptr<wxPropertyGridManager> mPropertyManager;
	std::unordered_map<std::string, std::function<void(const wxVariant& value)>> mPropertyToValueCallback;
	DataModel<medications>* mDataModel;
	medications::iterator mSelectedMedication;
	medications mMedicationTable;
	Prescriptions::iterator mPrescriptionIter;
	//edit text
	std::string EditText(const std::string& text);

	DECLARE_EVENT_TABLE()

};

