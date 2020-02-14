#include "pch.h"
#include "FileEvent.h"
#include "FindDlg.h"
#include "Utils.h"

FindDlg::FindDlg() : DialogBase(MAKEINTRESOURCE(IDD_FIND_DLG))
{

}

void FindDlg::OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
  switch (id)
  {
    case IDOK:
    {
      m_eFlags = Flags::FD_NONE;

      if (::SendDlgItemMessage(hWnd, IDC_RADIO3, BM_GETCHECK, 0, 0))
        m_eFlags |= Flags::FD_PATH;
      else if (::SendDlgItemMessage(hWnd, IDC_RADIO2, BM_GETCHECK, 0, 0))
        m_eFlags |= Flags::FD_ACTION;
      else
        m_eFlags |= Flags::FD_ALLCOLUMN;

      if (::SendDlgItemMessage(hWnd, IDC_WORD_ONLY, BM_GETCHECK, 0, 0))
        m_eFlags |= Flags::FD_WORDONLY;

      m_strFindText.resize(MAX_PATH);
      int size = ::GetDlgItemText(hWnd, IDC_FINDTEXT, &m_strFindText.front(), MAX_PATH);
      m_strFindText.resize(size);
      if (m_strFindText.empty())
      {
        ::EndDialog(hWnd, IDCANCEL);
        break;
      }
    }
    case IDCANCEL:
    {
      ::EndDialog(hWnd, id);
      break;
    }
    case IDC_FINDTEXT:
    {
      if (codeNotify == EN_CHANGE)
      {
        std::wstring strFindText(MAX_PATH, 0);
        int size = ::GetDlgItemText(hWnd, IDC_FINDTEXT, strFindText.data(), strFindText.size());
        strFindText.resize(size);
        ::EnableWindow(::GetDlgItem(hWnd, IDOK), !strFindText.empty());
      }
      break;
    }
  }
}

bool FindDlg::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
  if ((m_eFlags & Flags::FD_ACTION) && (m_eFlags & Flags::FD_PATH))
    ::SendDlgItemMessage(hWnd, IDC_RADIO1, BM_SETCHECK, BST_CHECKED, 0);
  else if (m_eFlags & Flags::FD_ACTION)
    ::SendDlgItemMessage(hWnd, IDC_RADIO2, BM_SETCHECK, BST_CHECKED, 0);
  else if (m_eFlags & Flags::FD_PATH)
    ::SendDlgItemMessage(hWnd, IDC_RADIO3, BM_SETCHECK, BST_CHECKED, 0);

  if (m_eFlags & Flags::FD_WORDONLY)
    ::SendDlgItemMessage(hWnd, IDC_WORD_ONLY, BM_SETCHECK, BST_CHECKED, 0);
  if (!m_strFindText.empty())
    ::SetDlgItemText(hWnd, IDC_FINDTEXT, m_strFindText.c_str());
  else
    ::EnableWindow(GetDlgItem(hWnd, IDOK), false);

  return DialogBase::OnInitDialog(hWnd, hWndFocus, lParam);
}

bool FindDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    HANDLE_DLGMSG(hWnd, WM_INITDIALOG, OnInitDialog);
    HANDLE_DLGMSG(hWnd, WM_COMMAND, OnCommand);
  }
  return DialogBase::DlgProc(hWnd, uMsg, wParam, lParam);
}
