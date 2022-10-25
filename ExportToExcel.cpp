#include "common.h"
#include "ExportToExcel.h"

size_t ExportToExcel::MaxCols = 10000;
size_t ExportToExcel::MaxRows = 20000000;

ExportToExcel::ExportToExcel(const fs::path& filepath)
{
		mDoc.create(filepath.string());
}

void ExportToExcel::PushWorkSheet(const std::string& Sheetname)
{
	mSheets.push(Sheetname);
	if (mDoc.workbook().worksheetExists(Sheetname)) return;
	try {
		mDoc.workbook().addWorksheet(Sheetname);
	}
	catch (const excel::XLException& exp) {
		throw std::logic_error(exp.what());
	}
}

void ExportToExcel::PopWorkSheet()
{
	if (mSheets.empty()) return;
	mSheets.pop();

}

bool ExportToExcel::Save()
{
	mDoc.save();
	return false;
}
