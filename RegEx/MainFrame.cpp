#include "MainFrame.h"
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/dcbuffer.h>
#include "Automata.h"
using namespace std;

/*
TODO

NFA creation is rewritten, works in all current cases
converting to DFA is DONE
Minimize DFA is DONE


Brackets within brackets
a(a+(cb)*) --WORKS

a((a+b)c)* --WORKS

a(a+c(dc)*) --WORKS

a(a+(cb)*)* --WORKS

a(a+c(dc)*b) --WORKS

((a+b)(a+b))* --WORKS

a(a+(cb)*c)d --WORKS

(0(0(0(0(0+1)(0+1))*+1(0(1(0+1)(0+1)*)*0)*)*)*) --WORKS

(1(0(0(1(0+1)(0+1)*)*0+1(0+1)(0+1)*))*) --WORKS


((0(0(0(0(0+1)(0+1))*+1(0(1(0+1)(0+1)*)*0)*)*)*)+(1(0(0(1(0+1)(0+1)*)*0+1(0+1)(0+1)*))*)) --WORKS

((a+b)*(aa+bb)(a+b)*(ab+ba)(a+b)*+(ab+ba)(a+b)*(aa+bb)(a+b)*)* --WORKS

((ab+ba)(a+b)*(aa+bb)(a+b)*)* --WORKS

(a+b)*(aa+bb)(a+b)*(ab+ba)(a+b)* --WORKS

*/



MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title){



	windowPanel = new wxPanel(this);
	NFAbutton = new wxButton(windowPanel, wxID_ANY, "Generate NFA", wxPoint(150,400), wxSize(100, 35));
	DFAbutton = new wxButton(windowPanel, wxID_ANY, "Convert to DFA", wxPoint(150, 450), wxSize(100, 35));
	minimizeButton = new wxButton(windowPanel, wxID_ANY, "Minimize DFA", wxPoint(150, 500), wxSize(100, 35));
	testInputButton = new wxButton(windowPanel, wxID_ANY, "Test Input", wxPoint(350, 500), wxSize(100, 35));
	textCtrl = new wxTextCtrl(windowPanel, wxID_ANY, "(a+b)*(aa+bb)(a+b)*(ab+ba)(a+b)*", wxPoint(300, 470), wxSize(200, -1));
	resetButton = new wxButton(windowPanel, wxID_ANY, "Reset", wxPoint(350, 550), wxSize(100, 35));
	inputTestCtrl = new wxStaticText(windowPanel, wxID_ANY, "", wxPoint(375, 450), wxSize(200, -1));

	if (!machine)
		resetButton->Enable(false);

	
	testInputButton->Enable(false);
	DFAbutton->Enable(false);
	minimizeButton->Enable(false);

	testInputButton->Bind(wxEVT_BUTTON, &MainFrame::runInput, this);
	NFAbutton->Bind(wxEVT_BUTTON, &MainFrame::generateNFA, this);
	DFAbutton->Bind(wxEVT_BUTTON, &MainFrame::convertToDFA, this);
	minimizeButton->Bind(wxEVT_BUTTON, &MainFrame::minimizeDFA, this);
	resetButton->Bind(wxEVT_BUTTON, &MainFrame::resetMachine, this);


}

void MainFrame::generateNFA(wxCommandEvent& evt) {

	NFAbutton->Enable(false);
	

	if (staticBitmap) {
		staticBitmap->Destroy();
		staticBitmap = nullptr;
	}
	string regEx = textCtrl->GetValue().ToStdString();
	textCtrl->ChangeValue("");
	if (machine) {
		machine->reset();
		machine->createNFA(regEx);
	}
	else
		machine = new Automata(regEx);
	
	

	machine->print(this);

	testInputButton->Enable(true);
	DFAbutton->Enable(true);
	resetButton->Enable(true);
	//textCtrl->Enable(false);
	

}

void MainFrame::convertToDFA(wxCommandEvent& evt)
{
	DFAbutton->Enable(false);

	if (staticBitmap) {
		staticBitmap->Destroy();
		staticBitmap = nullptr;
	}

	machine->convertToDFA();
	machine->print(this);

	minimizeButton->Enable(true);
}

void MainFrame::minimizeDFA(wxCommandEvent& evt)
{
	minimizeButton->Enable(false);

	if (staticBitmap) {
		staticBitmap->Destroy();
		staticBitmap = nullptr;
	}

	machine->minimizeDFA();
	machine->print(this);

	
}
void MainFrame::resetMachine(wxCommandEvent& evt)
{

	if (machine)
		machine->reset();
	else
		return;

	if (staticBitmap) {
		staticBitmap->Destroy();
		staticBitmap = nullptr;
	}
	textCtrl->ChangeValue("");
	inputTestCtrl->SetLabel("");
	testInputButton->Enable(false);
	NFAbutton->Enable(true);
	DFAbutton->Enable(false);
	minimizeButton->Enable(false);
	resetButton->Enable(false);
	//textCtrl->Enable(true);
}

void MainFrame::runInput(wxCommandEvent& evt)
{
	string input = textCtrl->GetValue().ToStdString();

	if (machine->input(input)) {
		inputTestCtrl->SetForegroundColour(*wxBLUE);
		inputTestCtrl->SetLabel("Accepted");
		
	}
	else {
		
		inputTestCtrl->SetForegroundColour(*wxRED);
		inputTestCtrl->SetLabel("Rejected");
		
		
	}
}