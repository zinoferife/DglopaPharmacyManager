#include "common.h"
#include "ListDisplayDialog.h"
BEGIN_EVENT_TABLE(ListDisplayDialog, wxDialog)
EVT_BUTTON(wxID_OK, ListDisplayDialog::OnOk)
EVT_BUTTON(wxID_CANCEL, ListDisplayDialog::OnCancel)
END_EVENT_TABLE()


ListDisplayDialog::ListDisplayDialog(wxWindow* parent, const std::string& message, const std::string& caption, const wxArrayString& listString)
: wxDialog(parent, wxID_ANY, caption){
	CreateDialog(message, listString);
	SizeDialog();
	SetBackgroundColour(*wxWHITE);
}

void ListDisplayDialog::CreateDialog(const std::string& message, const wxArrayString& strings)
{
	mMessage = new wxStaticText(this, wxID_ANY, message);
	mListControl = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, strings);
	mOk = new wxButton(this, wxID_OK, "OK");
	mCancel = new wxButton(this, wxID_CANCEL, "CANCEL");
}

void ListDisplayDialog::SizeDialog()
{
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* okCancelSizer = new wxBoxSizer(wxHORIZONTAL);

	boxSizer->Add(mMessage, wxSizerFlags().Align(wxLEFT).Proportion(0).Border(wxALL, 5));
	boxSizer->Add(mListControl, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 5));
	boxSizer->Add(new wxStaticLine(this, wxID_ANY), wxSizerFlags().Expand().Proportion(0).Border(wxALL, 5));

	okCancelSizer->AddStretchSpacer();
	okCancelSizer->Add(mOk, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 2));
	okCancelSizer->Add(mCancel, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 2));
	boxSizer->Add(okCancelSizer, wxSizerFlags().Expand().Proportion(0).Border(wxALL, 5));

	topSizer->Add(boxSizer, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 5));
	SetSizer(topSizer);
	topSizer->SetSizeHints(this);
	Center();
}

void ListDisplayDialog::OnOk(wxCommandEvent& evt)
{
	if (IsModal()) EndModal(wxID_OK);
	else {
		SetReturnCode(wxID_OK);
		Show(false);
	}

}

void ListDisplayDialog::OnCancel(wxCommandEvent& evt)
{
	if (IsModal()) EndModal(wxID_CANCEL);
	else {
		SetReturnCode(wxID_CANCEL);
		Show(false);
	}

}
