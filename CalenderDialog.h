#pragma once
#include <wx/wx.h>
#include <wx/calctrl.h>

class CalendarDialog : public wxDialog
{
public:
	enum
	{
		ID_CALENDAR = wxID_HIGHEST + 3445
	};

	CalendarDialog(wxWindow* parent);
	wxCalendarCtrl* mCalender;
	void OnCalender(wxCalendarEvent& evnt);

	DECLARE_EVENT_TABLE()
};