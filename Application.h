#pragma once
#include "common.h"
#include "MainFrame.h"

class Application : public wxApp
{
public:
	Application() {}
	bool virtual OnInit() override;


};

DECLARE_APP(Application)
