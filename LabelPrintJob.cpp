#include "common.h"
#include "LabelPrintJob.h"

LabelPrintJob::LabelPrintJob(medications& mMeds)
: mMedTable(mMeds){
}

LabelPrintJob::LabelPrintJob()
{
}

LabelPrintJob::LabelPrintJob(const LabelPrintJob& lhs)
{
	mMedTable = lhs.mMedTable;
	mPatientName = lhs.mPatientName;
	mPharmacyName = lhs.mPharmacyName;
}

LabelPrintJob& LabelPrintJob::operator=(const LabelPrintJob& lhs)
{
	mMedTable = lhs.mMedTable;
	mPatientName = lhs.mPatientName;
	mPharmacyName = lhs.mPharmacyName;
	return (*this);
}

LabelPrintJob::~LabelPrintJob()
{
}

void LabelPrintJob::Draw(wxDC* dc, const medications::row_t& med)
{
	PrepareDc(dc, med);
	int w, h, margin = 5;
	dc->GetTextExtent(mPatientName, &w, &h);
	int height = h;
	dc->DrawText(mPatientName, 0, 0);
	h += margin;
	dc->DrawLine(0, h, 100, h);
	h += margin;
	dc->DrawText(nl::row_value<medications::mediction_name>(med), 0, h);
	h += height + margin;
	dc->DrawText(nl::row_value<medications::dosage_form>(med), 0, h);
	h += height + margin;
	dc->DrawText(nl::row_value<medications::strength>(med), 0, h);
	h += height + margin;
	dc->DrawText(nl::row_value<medications::dir_for_use>(med), 0, h);
	h += height + margin;
	dc->DrawText(std::to_string(nl::row_value<medications::quanity>(med)), 0, h);
	h += height + margin + margin;
	dc->DrawLine(0, h, 100, h);
}

void LabelPrintJob::PrepareDc(wxDC* dc, const medications::row_t& med)
{
	//set margins and scale
	auto maxs = CalculateMaximums(med);
	//atleast 50 device units margins
	float marginX = 50;
	float marginY = 50;

	//add margins to the print area
	maxs += wxSize(marginX * 2, marginY * 2);

	//get the size of the device context in pixels
	int w, h;
	dc->GetSize(&w, &h);

	// Calculate a suitable scaling factor
	float scaleX = (float)(w / maxs.GetX());
	float scaleY = (float)(h / maxs.GetY());

	// Use x or y scaling factor, whichever fits on the DC
	float actualScale = wxMin(scaleX, scaleY);

	// Calculate the position on the DC for centring the graphic
	float posX = (float)((w - (100 * actualScale)) / 2.0);
	float posY = (float)((h - (100 * actualScale)) / 2.0);

	// Set the scale and origin
	dc->SetUserScale(actualScale, actualScale);
	dc->SetDeviceOrigin((long)posX, (long)posY);
	dc->SetPen(*wxBLACK_PEN);
}

bool LabelPrintJob::HasPage(int pageNum)
{
	return (pageNum <= mMedTable.size());
}

void LabelPrintJob::GetPageInfo(int* minPage, int* maxPage, int* pageFrom, int* pageTo)
{
	*minPage = 1;
	*maxPage = mMedTable.size();
	*pageFrom = 1;
	*pageTo = mMedTable.size();
}

bool LabelPrintJob::OnPrintPage(int pageNum)
{
	wxDC* dc = GetDC();
	if (dc) {
		//pagenum starts from 1
		int index = pageNum - 1;
		if (index >= mMedTable.size()) return false;
		Draw(dc, mMedTable[index]);
		dc->SetDeviceOrigin(0, 0);
		dc->SetUserScale(1.0, 1.0);

		return true;
	}
	return false;
}

bool LabelPrintJob::OnBeginDocument(int startPage, int endPage)
{
	if (!wxPrintout::OnBeginDocument(startPage, endPage))
		return false;

	return true;
}

wxSize LabelPrintJob::CalculateMaximums(const medications::row_t& med)
{
	//test
	return wxSize(100, 100);
}
