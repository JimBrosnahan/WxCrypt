// MyWxApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>


#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

// our AES wrapper class
#include "Crypto.h"

//
// class declarations
//
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    wxComboBox* m_combo;
    wxTextCtrl* m_infileText;
    wxTextCtrl* m_outfileText;
    wxButton*   m_runbutton;

    std::string inputFile;
    std::string outputFile;

private:
    bool browseFileName(const wxString prompt, std::string& file);
    bool PasswordEntry(const wxString prompt, wxString& pw);

    void FileInput(wxCommandEvent& event);
    void FileOutput(wxCommandEvent& event);
    void CryptButton(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    wxDECLARE_EVENT_TABLE();
};

enum
{
    ID_FILEINPUT = 1,
    ID_FILEOUTPUT,
    ID_COMBO,
    ID_CRYPTBUTTON
};

//MyFrame events
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_FILEINPUT, MyFrame::FileInput)
EVT_MENU(ID_FILEOUTPUT, MyFrame::FileOutput)
EVT_MENU(ID_CRYPTBUTTON, MyFrame::CryptButton)
EVT_BUTTON(ID_CRYPTBUTTON, MyFrame::CryptButton)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()


wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    // create main frame
    MyFrame* frame = new MyFrame("MyWxApp", wxPoint(50, 50), wxSize(450, 240));

    frame->Show(true);
    return true;
}

static const wxString choices[] =
{
    wxT("Encrypt"),
    wxT("Decrypt")
};


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    //dropdown menubar
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_FILEINPUT, "Input File");
    menuFile->Append(ID_FILEOUTPUT, "Output File");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu* menuHelp = new wxMenu;
    //menuHelp->Append(ID_CRYPTBUTTON, "test");
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Ready");

    //the panel to populate with controls + text
    wxPanel* panel = new wxPanel(this);

    //input/output filenames
    m_infileText = new wxTextCtrl(panel, ID_FILEINPUT, wxEmptyString, wxPoint(15, 20));
    m_outfileText = new wxTextCtrl(panel, ID_FILEOUTPUT, wxEmptyString, wxPoint(15, 50));

    //dropdown listbox
    m_combo = new wxComboBox(panel, ID_COMBO, wxT("Encrypt"),
        wxPoint(15, 100), wxDefaultSize,
        sizeof(choices) / sizeof(choices[0]), choices,
        wxCB_DROPDOWN);
    m_combo->SetSelection(0);

    //run button
    m_runbutton = new wxButton(panel, ID_CRYPTBUTTON, wxT("Run"), wxPoint(200, 100), wxDefaultSize);

}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    int sel = m_combo->GetSelection();
    std::string str = "select: ";

    str += std::to_string(sel);

    wxMessageBox(str,
        "About", wxOK | wxICON_INFORMATION);
}

void MyFrame::FileInput(wxCommandEvent& event)
{
    browseFileName(wxT("Input File"), inputFile);

    std::string str = "file: ";
    str += inputFile;

    wxString text = inputFile;

    m_infileText->Clear();
    m_infileText->WriteText(text);
    //wxLogMessage(str.c_str());
}

void MyFrame::FileOutput(wxCommandEvent& event)
{
    browseFileName(wxT("Output File"), outputFile);

    std::string str = "file: ";
    str += outputFile;

    wxString text = outputFile;

    m_outfileText->Clear();
    m_outfileText->WriteText(text);
    //wxLogMessage(str.c_str());
}


void MyFrame::CryptButton(wxCommandEvent& event)
{
    //reread textbox for now, add event for filename textbox change
    inputFile = m_infileText->GetLineText(0);
    outputFile = m_outfileText->GetLineText(0);

    // check both filenames are filled in
    if ((inputFile.empty()) || (outputFile.empty()))
        return;

    wxString pwd;
    bool res = PasswordEntry(wxT("Enter Passphrase"), pwd);

    //TODO when sel == encrypt, request passphrase 2nd time for verification
    //wxLogMessage("PE result: %d, PP: %s", res, pwd);

    if (res) {

        int result;
        Crypto* C = new Crypto();
        std::string pp = std::string(pwd.c_str());

        if (m_combo->GetSelection() == 0) //encrypt
            result = C->Encrypt(inputFile, outputFile, pp);
        else
            result = C->Decrypt(inputFile, outputFile, pp);

        delete C;
    }
}

bool MyFrame::browseFileName(const wxString prompt, std::string& file)
{
    wxFileDialog dialog
    (
        this,
        prompt,
        wxEmptyString,
        wxEmptyString);

    //dialog.SetExtraControlCreator()
    dialog.CenterOnScreen();
    dialog.SetDirectory(wxGetHomeDir());

    if (dialog.ShowModal() == wxID_OK)
    {
        file = dialog.GetCurrentlySelectedFilename();
    }

    return true;
}

bool MyFrame::PasswordEntry(const wxString prompt, wxString& pw)
{
    wxString pwd = wxGetPasswordFromUser(prompt);

    if (!pwd.empty())
    {
        pw = pwd;
        return true;
    }

    return false;
}
