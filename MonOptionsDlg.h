#pragma once
#include "DialogBase.h"

class MonOptionsDlg : public DialogBase
{
public:
  MonOptionsDlg();

public:
  FileEventMonitor::Options GetOptions() const;

private:
  bool DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  bool OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
  void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify);

private:
  FileEventMonitor::Options m_Options;
};

