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


namespace js = nlohmann;
class medications : public nl::vector_table<std::uint64_t, std::string, std::string, std::string, std::string, std::uint32_t>
{
public:
	BEGIN_COL_NAME("medications")
		COL_NAME("id")
		COL_NAME("medication_name")
		COL_NAME("dosage_form")
		COL_NAME("strength")
		COL_NAME("dir_for_use")
		COL_NAME("quantity")
	END_COL_NAME()

	enum {
		id = 0,
		mediction_name,
		dosage_form,
		strength,
		dir_for_use,
		quanity
	};
};


class DispensaryView : public wxPanel
{
public:

	enum {
		DATA_VIEW = wxID_HIGHEST + 500,
		PROPERTY_MANGER
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
	void Dispense();

	std::set<size_t> mSelections;
private:
	void OnDataViewItemSelected(wxDataViewEvent& evt);

private:
	std::unique_ptr<wxAuiManager> mPanelManager;
	std::unique_ptr<wxDataViewCtrl> mDataView;
	std::unique_ptr<wxPropertyGridManager> mPropertyManager;

	DataModel<medications>* mDataModel;
	medications mMedicationTable;

	DECLARE_EVENT_TABLE()

};

