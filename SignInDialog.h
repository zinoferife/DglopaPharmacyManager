#pragma once
#include <wx/wx.h>
#include <string>
#include "ActiveUser.h"


class SignInDialog :public wxDialog
{
public:
	SignInDialog(wxWindow* parent, ActiveUser* user);
	std::string mUserName;
	std::string mPassword;
private:
	wxTextCtrl* mUserNameCtrl;
	wxTextCtrl* mPasswordCtrl;
	wxButton* mOk;
	wxButton* mCancel;
	ActiveUser* mUser;
	virtual bool TransferDataFromWindow() override; 

	void OnOk(wxCommandEvent& evt);
	void OnCancel(wxCommandEvent& evt);

	DECLARE_EVENT_TABLE()

};

