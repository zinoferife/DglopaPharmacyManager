#include "common.h"
#include "SignInDialog.h"
BEGIN_EVENT_TABLE(SignInDialog, wxDialog)
EVT_BUTTON(wxID_OK, SignInDialog::OnOk)
EVT_BUTTON(wxID_CANCEL, SignInDialog::OnCancel)
END_EVENT_TABLE()

SignInDialog::SignInDialog(wxWindow* parent, ActiveUser* user)
: wxDialog(parent, wxID_ANY, "SIGN IN"), mUser(user){
	SetBackgroundColour(*wxWHITE);
	wxStaticText* texts[4];
	texts[0] = new wxStaticText;
	texts[1] = new wxStaticText;
	texts[2] = new wxStaticText;


	texts[0]->Create(this, wxID_ANY, "Please enter username and password");
	texts[1]->Create(this, wxID_ANY, "Username: ");
	texts[2]->Create(this, wxID_ANY, "Password: ");
	mUserNameCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1));
	mPasswordCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_PASSWORD);
	mOk = new wxButton(this, wxID_OK, "OK");
	mCancel = new wxButton(this, wxID_CANCEL, "CANCEL");

	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* okCancleSizer = new wxBoxSizer(wxHORIZONTAL);
	wxFlexGridSizer* flexSizer = new wxFlexGridSizer(2, 2, 5, 5);


	flexSizer->Add(texts[1], wxSizerFlags().Align(wxLEFT));
	flexSizer->Add(mUserNameCtrl, wxSizerFlags().Align(wxLEFT).Border(wxALL, 5).Expand());

	flexSizer->Add(texts[2], wxSizerFlags().Align(wxLEFT));
	flexSizer->Add(mPasswordCtrl, wxSizerFlags().Align(wxLEFT).Border(wxALL, 5).Expand());

	okCancleSizer->AddStretchSpacer();
	okCancleSizer->Add(mOk, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
	okCancleSizer->Add(mCancel, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
	
	boxSizer->Add(texts[0], wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));
	boxSizer->AddStretchSpacer();
	boxSizer->Add(flexSizer, wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));
	boxSizer->Add(okCancleSizer, wxSizerFlags().Expand().Border(wxALL, 5));

	SetDefaultItem(mOk);
	topSizer->Add(boxSizer, wxSizerFlags().Expand().Border(wxALL, 5));
	SetSizer(topSizer);
	topSizer->SetSizeHints(this);
	Center();

}

bool SignInDialog::TransferDataFromWindow()
{
	mUserName = mUserNameCtrl->GetValue().ToStdString();
	mPassword = mPasswordCtrl->GetValue().ToStdString();
	if (mUserName.empty() && mPassword.empty()) return false;
	return mUser->Authenticate(mPassword, mUserName);
}


void SignInDialog::OnOk(wxCommandEvent& evt)
{
	if (Validate() && TransferDataFromWindow())
	{
		if (IsModal()) EndModal(wxID_OK);
		else{
			SetReturnCode(wxID_OK);
			Show(false);
		}
	}
	else{
		wxMessageBox("Invalid Username or password", "SIGN IN", wxICON_ERROR | wxOK);
	}
	
}

void SignInDialog::OnCancel(wxCommandEvent& evt)
{
	if (IsModal()) EndModal(wxID_CANCEL);
	else{
		SetReturnCode(wxID_CANCEL);
		Show(false);
	}
}
