#include "App.h"
#include "MainFrame.h"
#include <wx/wx.h>
#include <iostream>
#include "Automata.h"
using namespace std;


wxIMPLEMENT_APP(App);

bool App::OnInit() {
	wxInitAllImageHandlers();

	MainFrame* mainFrame = new MainFrame("GUI");
	mainFrame->SetClientSize(800, 600);
	mainFrame->Center();
	mainFrame->Show();

	return true;
}