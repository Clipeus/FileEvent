#pragma once
#include "DialogBase.h"

class DetailsDlg : public DialogBase
{
public:
  DetailsDlg(FileEventItem* pItem);

private:
  bool DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  bool OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
  bool OnWindowPosChanging(HWND hWnd, LPWINDOWPOS lpWindowPos);
  bool OnWindowPosChanged(HWND hWnd, LPWINDOWPOS lpWindowPos);

private:
  FileEventItem* m_pItem;
  RECT m_OldRect = { 0 };
};
