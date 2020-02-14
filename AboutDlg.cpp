#include "pch.h"
#include "FileEvent.h"
#include "AboutDlg.h"
#include "Utils.h"

AboutDlg::AboutDlg() : DialogBase(MAKEINTRESOURCE(IDD_ABOUTBOX))
{

}

bool AboutDlg::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
  return DialogBase::OnInitDialog(hWnd, hWndFocus, lParam);
}

bool AboutDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    HANDLE_DLGMSG(hWnd, WM_INITDIALOG, OnInitDialog);
  }
  return DialogBase::DlgProc(hWnd, uMsg, wParam, lParam);
}
