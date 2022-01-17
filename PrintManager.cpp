#include "common.h"
#include "PrintManager.h"

// Global print data, to remember settings during the session
wxPrintData* g_PrintData = (wxPrintData*)NULL;

// Global page setup data
wxPageSetupData* g_pageSetupData = (wxPageSetupData*)NULL;
PrintManager::PrintManager()
{
	g_PrintData = new wxPrintData;
	g_pageSetupData = new wxPageSetupDialogData;
}

void PrintManager::PushPrintJob(std::shared_ptr<wxPrintout> printjob)
{
	mJobs.push(printjob);
}

void PrintManager::PrintJob(wxWindow* parent)
{
	auto printout = mJobs.front();
	wxPrintDialogData printDialogData(*g_PrintData);
	wxPrinter printer(&printDialogData);
	if (printout) {
		if (!printer.Print(parent, &(*printout))) {
			if (GetLastError() == wxPRINTER_ERROR) {
				wxMessageBox("Problem printing the document", "Printing", wxICON_ERROR | wxOK);
			}
			else {
				wxMessageBox("Printing cancelled", "Printing", wxOK);
			}
		}
	}
	(*g_PrintData) = printer.GetPrintDialogData().GetPrintData();
	mJobs.pop();
}

void PrintManager::Preview(wxWindow* parent,wxPrintout* previewout, wxPrintout* printout)
{
	//copy the print out so that the copy is deleted.
	wxPrintDialogData printDialogData(*g_PrintData);
	//cannnout use the same pointer this is weird
	wxPrintPreview* preview = new wxPrintPreview( previewout, printout, &printDialogData);
	if (!preview->Ok())
	{
		delete preview;
		wxMessageBox(wxT("There was a problem previewing.\nPerhaps your current printer is not set correctly?"),
			wxT("Previewing"), wxOK);
		return;
	}

	wxPreviewFrame* frame = new wxPreviewFrame(preview, parent,
		wxT("Label Print Preview"));
	frame->Centre(wxBOTH);
	frame->Initialize();
	frame->Show(true);

}

