#pragma once
//exports to excel
#include <OpenXlsl/OpenXLSX.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <filesystem>
#include <relation.h>
#include <stack>
#include <fmt/format.h>
#include <nl_time.h>
#include <string_view>
#include <rel_view.h>


namespace fs = std::filesystem;
namespace excel = OpenXLSX;
//creates a document per instance 

template<typename row_t, size_t i>
struct loop {

	static void do_put(const row_t& row, excel::XLCellIterator& iter)
	{
		using arg_t = std::decay_t<std::tuple_element_t<i, row_t>>;
		static_assert(nl::detail::is_database_type<arg_t>::value, "invalid row type");
		if constexpr (!nl::detail::is_std_ref_type<arg_t>::value) {
			if constexpr (std::is_convertible_v<arg_t, std::string>) {
				iter->value().set(nl::row_value<i>(row));
			}
			else if constexpr (std::is_integral_v<arg_t> || std::is_floating_point_v<arg_t>) {
				iter->value().set(nl::row_value<i>(row));
			}
			else if constexpr (std::is_same_v<arg_t, nl::uuid>) {
				//uuid type
				iter->value().set(
					boost::uuids::to_string(static_cast<const boost::uuids::uuid&>(nl::row_value<i>(row))));
			}
			else if constexpr (std::is_same_v<arg_t, nl::blob_t>) {
				//blob type, cannot write blob to excel, 
				iter->value().set("N/A");
			}
			else if constexpr (std::is_same_v<arg_t, nl::date_time_t>) {
				//date time
				iter->value().set(nl::to_string_date(nl::row_value<i>(row)));
			}
		} else {
			using arg_ref_t = typename arg_t::type;
			if constexpr (std::is_convertible_v<arg_ref_t, std::string>) {
				iter->value().set(nl::row_value<i>(row).get());
			}
			else if constexpr (std::is_integral_v<arg_ref_t> || std::is_floating_point_v<arg_t>) {
				iter->value().set(nl::row_value<i>(row).get());
			}
			else if constexpr (std::is_same_v<arg_ref_t, nl::uuid>) {
				//uuid type
				iter->value().set(
					boost::uuids::to_string(static_cast<const boost::uuids::uuid&>(nl::row_value<i>(row).get())));
			}
			else if constexpr (std::is_same_v<arg_ref_t, nl::blob_t>) {
				//blob type, cannot write blob to excel, 
				iter->value().set("N/A");
			}
			else if constexpr (std::is_same_v<arg_ref_t, nl::date_time_t>) {
				//date time
				iter->value().set(nl::to_string_date(nl::row_value<i>(row).get()));
			}
		}
	}

	//puts row value in excel cell iter
	//loop to 0 first to write the first col value before the rest
	static void put(const row_t& row, excel::XLCellIterator& iter)
	{
		loop<row_t, i - 1>::put(row, iter);
		std::advance(iter, 1);
		do_put(row, iter);
	}
};

template<typename row_t>
struct loop<row_t, 0> {

	static void do_put(const row_t& row, excel::XLCellIterator& iter)
	{
		constexpr size_t col = 0;
		using arg_t = std::decay_t<std::tuple_element_t<col, row_t>>;
		static_assert(nl::detail::is_database_type<arg_t>::value, "invalid row type");
		if constexpr (!nl::detail::is_std_ref_type<arg_t>::value) {
			if constexpr (std::is_convertible_v<arg_t, std::string>) {
				iter->value().set(nl::row_value<col>(row));
			}
			else if constexpr (std::is_integral_v<arg_t> || std::is_floating_point_v<arg_t>) {
				iter->value().set(nl::row_value<col>(row));
			}
			else if constexpr (std::is_same_v<arg_t, nl::uuid>) {
				//uuid type
				iter->value().set(
					boost::uuids::to_string(static_cast<const boost::uuids::uuid&>(nl::row_value<col>(row))));
			}
			else if constexpr (std::is_same_v<arg_t, nl::blob_t>) {
				//blob type, cannot write blob to excel, 
				iter->value().set("N/A");
			}
			else if constexpr (std::is_same_v<arg_t, nl::date_time_t>) {
				//date time
				iter->value().set(nl::to_string_date(nl::row_value<col>(row)));
			}
		} else {
			using arg_ref_t = typename arg_t::type;
			if constexpr (std::is_convertible_v<arg_ref_t, std::string>) {
				iter->value().set(nl::row_value<col>(row).get());
			}
			else if constexpr (std::is_integral_v<arg_ref_t> || std::is_floating_point_v<arg_t>) {
				iter->value().set(nl::row_value<col>(row).get());
			}
			else if constexpr (std::is_same_v<arg_ref_t, nl::uuid>) {
				//uuid type
				iter->value().set(
					boost::uuids::to_string(static_cast<const boost::uuids::uuid&>(nl::row_value<col>(row).get())));
			}
			else if constexpr (std::is_same_v<arg_ref_t, nl::blob_t>) {
				//blob type, cannot write blob to excel, 
				iter->value().set("N/A");
			}
			else if constexpr (std::is_same_v<arg_ref_t, nl::date_time_t>) {
				//date time
				iter->value().set(nl::to_string_date(nl::row_value<col>(row).get()));
			}

		}

	}

	//puts row value in excel cell iter
	static void put(const row_t& row, excel::XLCellIterator& iter)
	{
		do_put(row, iter);
	}
};


class ExportToExcel
{
public:
	ExportToExcel(const fs::path& filepath);
	~ExportToExcel() {}
	//reads or writes to execel

	void PushWorkSheet(const std::string& Sheetname);
	void PopWorkSheet();
	bool Save();



	template<typename relation, std::enable_if_t<std::disjunction_v<nl::detail::is_relation<relation>, 
			nl::detail::is_rel_view<relation>>, int> = 0>
	void DumpRelationAsExecl(const relation& rel, const std::array<std::string_view,
		relation::column_count>& column_names = {}) {
		//use the top most sheet
		if (mSheets.empty()) {
			throw std::logic_error("Cannot write to an empty document, push a work sheet");
		}
		constexpr size_t col_size = relation::column_count;
		const size_t row_size = rel.size();

		//set some hardlimits 
		if (row_size >= MaxRows || col_size >= MaxCols) {
			throw std::logic_error("Maximum excel cells exceded");
		}
		//we have a space to work
		std::string& Sheet = mSheets.top();
		auto wks = mDoc.workbook().worksheet(Sheet);
		//write the first row if column_names is not empty
		size_t first_row = 1;
		size_t first_col = 1;
		if (!column_names.empty()) {
			first_row++;
			for (size_t i = 0; i < column_names.size(); i++) {
				//A1: {1,1}
				excel::XLCellReference ref(1, i + 1);
				const std::string_view& v = column_names[i];
				wks.cell(ref).value().set(v.data());
			}
		}

		//now for the rest of the rows
		auto range = wks.range(excel::XLCellReference(first_row, first_col), excel::XLCellReference(row_size, col_size));
		auto rel_iter = rel.begin();
		

		for (auto iter = range.begin();
			iter != range.end() && rel_iter != rel.end(); iter++, rel_iter++) {
			loop<typename relation::row_t, relation::column_count - 1>::put(*rel_iter, iter);
		}
	}





	static size_t MaxRows;
	static size_t MaxCols;

private:

	std::stack<std::string> mSheets;
	excel::XLDocument mDoc;
};

