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


template<typename Instance>
void IntProperty(const wxVariant& value, size_t column, Instance& instance, typename Instance::iterator iter)
{
	static_assert(nl::detail::is_relation_v<Instance>, "Instnace is not a relation");
	typename Instance::notification_data data;
	data.row_iterator = iter;
	data.column = column;
	data.column_value = static_cast<unsigned int>(value.GetInteger());
	instance.notify<nl::notifications::update>(data);
}


template<typename Instance>
void LongLongProperty(const wxVariant& value, size_t column, Instance& instance, typename Instance::iterator iter)
{
	static_assert(nl::detail::is_relation_v<Instance>, "Instnace is not a relation");
	typename Instance::notification_data data;
	data.row_iterator = iter;
	data.column = column;
	data.column_value = static_cast<unsigned long long>(value.GetLongLong().GetValue());
	instance.notify<nl::notifications::update>(data);
}

template<typename Instance>
void StringProperty(const wxVariant& value, size_t column, Instance& instance, typename Instance::iterator iter)
{
	static_assert(nl::detail::is_relation_v<Instance>, "Instnace is not a relation");
	typename Instance::notification_data data;
	data.row_iterator = iter;
	data.column = column;
	data.column_value = value.GetString().ToStdString();
	instance.notify<nl::notifications::update>(data);
}

template<typename Instance>
void ArrayStringProperty(const wxVariant& value, size_t column, Instance& instance, typename Instance::iterator iter)
{
	static_assert(nl::detail::is_relation_v<Instance>, "Instnace is not a relation");
	typename Instance::notification_data data;
	data.row_iterator = iter;
	data.column = column;
	auto values = value.GetArrayString();
	std::vector<std::string> arrayString;
	arrayString.reserve(values.size());
	for (auto& str : values){
		arrayString.emplace_back(std::move(str.ToStdString()));
	}
	data.column_value = fmt::format("{}", fmt::join(arrayString, ","));
	instance.notify<nl::notifications::update>(data);
}

class DetailView : public wxPropertyGridManager
{

public:
	enum
	{
		ID_UPDATE
	};

	DetailView(typename Products::elem_t<Products::id> product_id, wxWindow* parent, wxWindowID id, const wxPoint& position = wxDefaultPosition,
		const wxSize& size = wxDefaultSize, long style = wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);
	~DetailView();

	void CreatePropertyGrid(typename Products::elem_t<Products::id> product_id);
	void CreateToolBar();
	void CreateGridPageArt();
	void CreatePropertyProductCallback(Products::iterator iter);
	void CreatePropertyProductDetailCallback(ProductDetails::iterator iter);

	void LoadDataIntoGrid(typename Products::elem_t<Products::id> product_id);
 
	void OnUpdate(wxCommandEvent& evt);
	void ResetModifiedFlag();
private:
	std::unordered_map<std::string, std::function<void(const wxVariant& value)>> mPropertyToValueCallback;
	DECLARE_EVENT_TABLE()
};

