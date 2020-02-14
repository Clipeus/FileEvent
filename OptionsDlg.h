#pragma once
#include "DialogBase.h"

class OptionsDlg : public DialogBase
{
public:
  OptionsDlg();

public:
  INT_PTR DoModal(HWND hWnd, LPCTSTR lpTemplateName = nullptr);

private:
  static INT_PTR CALLBACK s_DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  bool DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  bool OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
  void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify);
  LRESULT OnNotify(HWND hWnd, int idFrom, NMHDR* pNmndr);
};

