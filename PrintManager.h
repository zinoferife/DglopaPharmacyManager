#pragma once
#include <queue>
#include <memory>


#include <wx/print.h>

class PrintManager
{
public:
	PrintManager();
	void PushPrintJob(std::shared_ptr<wxPrintout> printjob);
	void PrintJob(wxWindow* parent);
	void Preview(wxWindow* parent, wxPrintout* previeout, wxPrintout* printout);
private:
	std::queue<std::shared_ptr<wxPrintout>> mJobs;
};

