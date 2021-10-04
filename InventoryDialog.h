#pragma once
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/calctrl.h>
#include <wx/bmpbuttn.h>
#include <wx/sizer.h>

#include "Tables.h"
#include "ArtProvider.h"


class InventoryDialog : public wxDialog
{
public:
	InventoryDialog(Inventories::row_t& row_, wxWindow* parent);

	virtual bool TransferDataFromWindow() override;
	virtual bool TransferDataToWindow() override;

	enum
	{
		ID_CALENDAR = wxID_HIGHEST+ 2333,
		ID_IMPROPER_DATE
	};

private:
	void CreateDialog();
	void SizeDialog();

	wxStaticText* texts;
	wxSpinCtrl* mQuantityInControl;
	wxTextCtrl* mExpiryDate;
	wxTextCtrl* mInvoiceWayBill;
	wxBitmapButton* mCalenderButton;
	wxButton* mOkCancel;

private:
	void OnOk(wxCommandEvent& evt);
	void OnCancel(wxCommandEvent& evt);
	void OnCalendar(wxCommandEvent& evt);


	DECLARE_EVENT_TABLE()

private:
	Inventories::row_t& row;
};

