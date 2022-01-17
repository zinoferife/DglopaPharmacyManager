#pragma once
#include <wx/print.h>
#include "Tables.h"



class LabelPrintJob : public wxPrintout
{
public:
	LabelPrintJob(medications& mMedTable);
	LabelPrintJob();
	LabelPrintJob(const LabelPrintJob& lhs);
	LabelPrintJob& operator=(const LabelPrintJob& lhs);

	virtual ~LabelPrintJob();
	inline void SetMedicationTable(medications& meds) { mMedTable = meds; }
	inline void SetPatientName(const std::string& name) { mPatientName = name; }
	inline void SetPharmacyName(const std::string& name) { mPharmacyName = name;}

	void Draw(wxDC* dc, const medications::row_t& med);
	void PrepareDc(wxDC* dc, const medications::row_t& med);

	virtual bool HasPage(int pageNum) override;
	virtual void GetPageInfo(int* minPage, int* maxPage, int* pageFrom, int* pageTo) override;
	virtual bool OnPrintPage(int pageNum) override;
	virtual bool OnBeginDocument(int startPage, int endPage) override;

private:
	wxSize CalculateMaximums(const medications::row_t& med);


	medications mMedTable;
	std::string mPatientName;
	std::string mPharmacyName;
};

