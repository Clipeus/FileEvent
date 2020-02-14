#pragma once
#include "DialogBase.h"

class AboutDlg : public DialogBase
{
public:
  AboutDlg();

private:
  bool DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  bool OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
};
