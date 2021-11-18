#include "common.h"
#include "CalenderDialog.h"

BEGIN_EVENT_TABLE(CalendarDialog, wxDialog)
EVT_CALENDAR(CalendarDialog::ID_CALENDAR, OnCalender)
END_EVENT_TABLE()

CalendarDialog::CalendarDialog(wxWindow* parent)
	: wxDialog(parent, wxID_ANY, wxT("Pick an expiry date")) {
	mCalender = new wxCalendarCtrl(this, ID_CALENDAR, wxDateTime::Now(), wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	boxSizer->Add(mCalender, wxSizerFlags().Align(wxCENTER).Border(wxALL, 2));
	SetSizer(boxSizer);
	boxSizer->SetSizeHints(this);
}

void CalendarDialog::OnCalender(wxCalendarEvent& evnt)
{
	if (IsModal()) EndModal(wxID_OK);
	else {
		SetReturnCode(wxID_OK);
		this->Show(false);
	}
}