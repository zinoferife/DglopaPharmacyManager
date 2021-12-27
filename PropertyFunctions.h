#pragma once
#include <wx/variant.h>

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

template<typename Instance, typename EnumArray>
void EnumProperty(const wxVariant& value, size_t column, Instance& instance, typename Instance::iterator iter, const EnumArray& arr) {
	static_assert(nl::detail::is_relation_v<Instance>, "Instnace is not a relation");
	typename Instance::notification_data data;
	data.row_iterator = iter;
	data.column = column;
	data.column_value = arr[value.GetInteger()];
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
	for (auto& str : values) {
		arrayString.emplace_back(std::move(str.ToStdString()));
	}
	data.column_value = fmt::format("{}", fmt::join(arrayString, ","));
	instance.notify<nl::notifications::update>(data);
}