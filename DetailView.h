#pragma once
#include <wx/wx.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>
#include <wx/notebook.h>

#include "ArtProvider.h"
#include "Tables.h"
#include "Instances.h"
#include <spdlog/spdlog.h>


#include <memory>
#include <sstream>
#include <unordered_map>
#include <functional>


#include "PropertyFunctions.h"

class DetailView : public wxPropertyGridManager
{

public:
	enum
	{
		ID_UPDATE
	};

	DetailView(typename Products::elem_t<Products::id> product_id, wxWindow* parent, wxWindowID id, const wxPoint& position = wxDefaultPosition,
		const wxSize& size = wxDefaultSize, long style = wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);
	virtual ~DetailView();

	void CreatePropertyGrid(typename Products::elem_t<Products::id> product_id);
	void CreateToolBar();
	void CreateGridPageArt();
	void CreatePropertyProductCallback(Products::iterator iter);
	void CreatePropertyProductDetailCallback(ProductDetails::iterator iter);

	void LoadDataIntoGrid(typename Products::elem_t<Products::id> product_id);
 
	void OnUpdate(wxCommandEvent& evt);
	void ResetModifiedFlag();
	constexpr bool IsCreated() const { return mCreated; }
private:
	bool mCreated;
	std::unordered_map<std::string_view, std::function<void(const wxVariant& value)>> mPropertyToValueCallback;
	DECLARE_EVENT_TABLE()
};

