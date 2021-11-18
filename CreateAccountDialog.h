#pragma once
#include <wx/wx.h>
#include "Instances.h"
#include <wx/statline.h>
#include <wx/collpane.h>
#include <wx/spinctrl.h>
#include <wx/valnum.h>


#include <wx/aui/aui.h>
#include "SearchAutoComplete.h"
#include <algorithm>
#include <sstream>
#include <memory>
#include <bcrypt/bcrypt.h>

class CreateAccountDialog : public wxDialog
{
public:
	CreateAccountDialog(wxWindow* parent, wxWindowID id, const wxPoint& position = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0L);
	void CreateDialog();
	void SizeDialog();
	virtual bool TransferDataFromWindow() override;
	virtual bool Validate() override;
	void OnOK(wxCommandEvent& evt);
	void OnCancel(wxCommandEvent& evt);

	constexpr inline const Users::row_t& GetNewUser() const {return mUser;}
private:
	nl::blob_t HashUserPassword(const std::string& password);
	Users::row_t mUser;
	wxStaticText* mTitleText;
	wxTextCtrl* mSurnameCtrl;
	wxTextCtrl* mNameCtrl;
	wxTextCtrl* mUsernameCtrl;
	wxTextCtrl* mPhoneNumberCtrl;
	wxTextCtrl* mPasswordCtrl;
	wxTextCtrl* mConfirmPasswordCtrl;
	wxButton* mOk;
	wxButton* mCancel;

	DECLARE_EVENT_TABLE()
};

