#pragma once
#include <wx/wx.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/statline.h>



class ListDisplayDialog : public wxDialog
{
public:
	ListDisplayDialog(wxWindow* parent, const std::string& message, const std::string& caption, const wxArrayString& listString);
	void CreateDialog(const std::string& message, const wxArrayString& strings);
	void SizeDialog();

	void OnOk(wxCommandEvent& evt);
	void OnCancel(wxCommandEvent& evt);
private:
	wxButton* mOk;
	wxButton* mCancel;
	wxListBox* mListControl;
	wxStaticText* mMessage;
	DECLARE_EVENT_TABLE()

};

