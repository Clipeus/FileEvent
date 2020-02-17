#include "pch.h"
#include "FileEvent.h"
#include "FileEventApp.h"
#include "OptionsDlg.h"
#include "Utils.h"

OptionsDlg::OptionsDlg()
{

}

INT_PTR OptionsDlg::DoModal(HWND hWnd, LPCTSTR lpTemplateName /*= nullptr*/)
{
  PROPSHEETPAGE PSheetPage[1];
  PROPSHEETHEADER PSheetHeader;

  PSheetPage[0].dwSize = sizeof(PROPSHEETPAGE);
  PSheetPage[0].dwFlags = PSP_DEFAULT;
  PSheetPage[0].hInstance = ::GetApp()->GetInstance();
  PSheetPage[0].pszIcon = 0;
  PSheetPage[0].pszTemplate = MAKEINTRESOURCE(IDD_GENERAL);
  PSheetPage[0].pfnDlgProc = s_DlgProc;
  PSheetPage[0].pszTitle = nullptr;
  PSheetPage[0].lParam = reinterpret_cast<LPARAM>(this);

  PSheetHeader.dwSize = sizeof(PROPSHEETHEADER);
  PSheetHeader.dwFlags = PSH_USEICONID | PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
  PSheetHeader.hInstance = ::GetApp()->GetInstance();
  PSheetHeader.pszIcon = MAKEINTRESOURCE(IDI_MAIN);
  PSheetHeader.hwndParent = hWnd;
  std::wstring strCaption = Utils::LoadString(IDS_CAPTION_SET);
  PSheetHeader.pszCaption = strCaption.c_str();
  PSheetHeader.nPages = 1;
  PSheetHeader.nStartPage = 0;
  PSheetHeader.ppsp = (LPPROPSHEETPAGE)&PSheetPage;

  return ::PropertySheet(&PSheetHeader);
}

void OptionsDlg::OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
  //EnableWindow(GetDlgItem(hWnd, IDC_RADIO1), (SendDlgItemMessage(hWnd, IDC_CHECK2, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED));
  //EnableWindow(GetDlgItem(hWnd, IDC_RADIO2), (SendDlgItemMessage(hWnd, IDC_CHECK2, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED));
  //EnableWindow(GetDlgItem(hWnd, IDC_RADIO3), (SendDlgItemMessage(hWnd, IDC_CHECK3, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED));
  //EnableWindow(GetDlgItem(hWnd, IDC_RADIO4), (SendDlgItemMessage(hWnd, IDC_CHECK3, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED));
}

LRESULT OptionsDlg::OnNotify(HWND hWnd, int idFrom, NMHDR* pNmndr)
{
  if (pNmndr->code == PSN_APPLY)
  {
    //::GetApp()->GetOptions()->bURAS = (SendDlgItemMessage(hWnd, IDC_CHECK1, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED);
    //::GetApp()->GetOptions()->bDNSDWIF = (SendDlgItemMessage(hWnd, IDC_CHECK2, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED);
    //::GetApp()->GetOptions()->bDNSDWEF = (SendDlgItemMessage(hWnd, IDC_CHECK3, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED);
    //::GetApp()->GetOptions()->bMI = (SendDlgItemMessage(hWnd, IDC_RADIO1, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED);
    //::GetApp()->GetOptions()->bAI = (SendDlgItemMessage(hWnd, IDC_RADIO3, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED);
  }
  return false;
}

bool OptionsDlg::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
  //if (MoveYourApp::GetApp()->GetOptions()->bURAS)
  //  SendDlgItemMessage(hWnd, IDC_CHECK1, BM_SETCHECK, BST_CHECKED, 0);

  //if (MoveYourApp::GetApp()->GetOptions()->bDNSDWIF)
  //{
  //  SendDlgItemMessage(hWnd, IDC_CHECK2, BM_SETCHECK, BST_CHECKED, 0);
  //  if (MoveYourApp::GetApp()->GetOptions()->bMI)
  //    SendDlgItemMessage(hWnd, IDC_RADIO1, BM_SETCHECK, BST_CHECKED, 0);
  //  else
  //    SendDlgItemMessage(hWnd, IDC_RADIO2, BM_SETCHECK, BST_CHECKED, 0);
  //}
  //else
  //{
  //  SendDlgItemMessage(hWnd, IDC_RADIO1, BM_SETCHECK, BST_CHECKED, 0);
  //  EnableWindow(GetDlgItem(hWnd, IDC_RADIO1), false);
  //  EnableWindow(GetDlgItem(hWnd, IDC_RADIO2), false);
  //}
  //if (MoveYourApp::GetApp()->GetOptions()->bDNSDWEF)
  //{
  //  SendDlgItemMessage(hWnd, IDC_CHECK3, BM_SETCHECK, BST_CHECKED, 0);
  //  if (MoveYourApp::GetApp()->GetOptions()->bAI)
  //    SendDlgItemMessage(hWnd, IDC_RADIO3, BM_SETCHECK, BST_CHECKED, 0);
  //  else
  //    SendDlgItemMessage(hWnd, IDC_RADIO4, BM_SETCHECK, BST_CHECKED, 0);
  //}
  //else
  //{
  //  SendDlgItemMessage(hWnd, IDC_RADIO3, BM_SETCHECK, BST_CHECKED, 0);
  //  EnableWindow(GetDlgItem(hWnd, IDC_RADIO3), false);
  //  EnableWindow(GetDlgItem(hWnd, IDC_RADIO4), false);
  //}

  return DialogBase::OnInitDialog(hWnd, hWndFocus, lParam);
}

bool OptionsDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    HANDLE_DLGMSG(hWnd, WM_DESTROY, OnDestroy);
    HANDLE_DLGMSG(hWnd, WM_INITDIALOG, OnInitDialog);
    HANDLE_DLGMSG(hWnd, WM_COMMAND, OnCommand);
    HANDLE_MSG(hWnd, WM_NOTIFY, OnNotify);
  }
  return DialogBase::DlgProc(hWnd, uMsg, wParam, lParam);
}

INT_PTR OptionsDlg::s_DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  OptionsDlg* dlg = reinterpret_cast<OptionsDlg*>(GetWindowLongPtr(hWnd, DWLP_USER));

  if (uMsg == WM_INITDIALOG)
  {
    dlg = reinterpret_cast<OptionsDlg*>(reinterpret_cast<LPPROPSHEETPAGE>(lParam)->lParam);
    _ASSERTE(dlg);
    if (dlg)
      dlg->m_hWnd = hWnd;
    ::SetWindowLongPtr(hWnd, DWLP_USER, reinterpret_cast<LONG_PTR>(dlg));
  }

  if (dlg)
    return dlg->DlgProc(hWnd, uMsg, wParam, lParam);

  return SetDlgMsgResult(hWnd, uMsg, false);
}
