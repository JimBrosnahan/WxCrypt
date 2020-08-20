/*
** Copyright (c) 2020 - Jim Brosnahan
**
** This demonstration source is distributed in the hope it will be useful,
** BUT WITHOUT ANY WARRANTY.
**
** wxWidgets retains their respective CopyRights
** https://www.wxwidgets.org/
** 
*/

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

// our AES wrapper class
#include "Crypto.h"

//
// class declarations
//
class Wxcrypt : public wxApp
{
public:
    virtual bool OnInit();
};

class AppFrame : public wxFrame
{
public:
    AppFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

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

//AppFrame events
wxBEGIN_EVENT_TABLE(AppFrame, wxFrame)
EVT_MENU(ID_FILEINPUT, AppFrame::FileInput)
EVT_MENU(ID_FILEOUTPUT, AppFrame::FileOutput)
EVT_MENU(ID_CRYPTBUTTON, AppFrame::CryptButton)
EVT_BUTTON(ID_CRYPTBUTTON, AppFrame::CryptButton)
EVT_MENU(wxID_EXIT, AppFrame::OnExit)
EVT_MENU(wxID_ABOUT, AppFrame::OnAbout)
wxEND_EVENT_TABLE()


wxIMPLEMENT_APP(Wxcrypt);
bool Wxcrypt::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    // create main frame
    AppFrame* frame = new AppFrame("Wxcrypt", wxPoint(50, 50), wxSize(450, 240));

    frame->Show(true);
    return true;
}

static const wxString choices[] =
{
    wxT("Encrypt"),
    wxT("Decrypt")
};


AppFrame::AppFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
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

void AppFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void AppFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox(wxT("Wxcrypt = WxWidgets + CryptoPP combined\nCopyright (c) 2020 - Jim Brosnahan"),
        "About", wxOK | wxICON_INFORMATION);
}

void AppFrame::FileInput(wxCommandEvent& event)
{
    browseFileName(wxT("Input File"), inputFile);

    std::string str = "file: ";
    str += inputFile;

    wxString text = inputFile;

    m_infileText->Clear();
    m_infileText->WriteText(text);
    //wxLogMessage(str.c_str());
}

void AppFrame::FileOutput(wxCommandEvent& event)
{
    browseFileName(wxT("Output File"), outputFile);

    std::string str = "file: ";
    str += outputFile;

    wxString text = outputFile;

    m_outfileText->Clear();
    m_outfileText->WriteText(text);
    //wxLogMessage(str.c_str());
}


void AppFrame::CryptButton(wxCommandEvent& event)
{
    //reread textbox for now, add event for filename textbox change
    inputFile = m_infileText->GetLineText(0);
    outputFile = m_outfileText->GetLineText(0);

    // check both filenames are filled in
    if ((inputFile.empty()) || (outputFile.empty()))
        return;

    int mode = m_combo->GetSelection();
    wxString pwd;
    bool res = PasswordEntry(wxT("Enter Passphrase"), pwd);

    if (mode == 0) {

        //encryption, verify both passphrases match
        wxString pwd2;
        bool res2 = PasswordEntry(wxT("Enter Again"), pwd2);

        if (!((res) && (res2) && (pwd.Cmp(pwd2) == 0))) {
            wxLogMessage(wxT("Passphrases do no match"));
            res = false;
        }
    }

    if (res) {

        int result;
        Crypto* C = new Crypto();
        std::string pp = std::string(pwd.c_str());

        result = C->Process( (mode == 0) ? Modes::ENCRYPT:Modes::DECRYPT,
            inputFile, outputFile, pp);

        delete C;
    }
}

bool AppFrame::browseFileName(const wxString prompt, std::string& file)
{
    wxFileDialog dialog (this, prompt, wxEmptyString, wxEmptyString );

    dialog.CenterOnScreen();
    dialog.SetDirectory(wxGetHomeDir());

    if (dialog.ShowModal() == wxID_OK)
    {
        file = dialog.GetCurrentlySelectedFilename();
    }

    return true;
}

bool AppFrame::PasswordEntry(const wxString prompt, wxString& pw)
{
    wxString pwd = wxGetPasswordFromUser(prompt);

    if (!pwd.empty())
    {
        pw = pwd;
        return true;
    }

    return false;
}
