#include "pch.h"
#include "FileEvent.h"
#include "DetailsDlg.h"
#include "Utils.h"

DetailsDlg::DetailsDlg(FileEventItem* pItem) : DialogBase(MAKEINTRESOURCE(IDD_DETAILS)), m_pItem(pItem)
{
}

bool DetailsDlg::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
  std::wstring strAction = std::to_wstring(m_pItem->dwAction);
  std::filesystem::path path = m_pItem->strDirName;
  path.append(m_pItem->strFileName);
  std::wstring strDescription = std::get_if<UINT>(&m_pItem->varDescription) != nullptr ? Utils::LoadString(std::get<UINT>(m_pItem->varDescription)) : std::get<std::wstring>(m_pItem->varDescription);

  ::SetDlgItemText(m_hWnd, IDC_INFO, 
    Utils::StrFormat(Utils::LoadString(IDS_INFO_FORMAT).c_str(), 
      m_pItem->strDirName.c_str(), m_pItem->dwAction, strDescription.c_str(), path.c_str()).c_str());

  return DialogBase::OnInitDialog(hWnd, hWndFocus, lParam);
}

bool DetailsDlg::OnWindowPosChanging(HWND hWnd, LPWINDOWPOS lpWindowPos)
{
  ::GetWindowRect(m_hWnd, &m_OldRect);
  return false;
}

bool DetailsDlg::OnWindowPosChanged(HWND hWnd, LPWINDOWPOS lpWindowPos)
{
  int dx = lpWindowPos->cx - (m_OldRect.right - m_OldRect.left);
  int dy = lpWindowPos->cy - (m_OldRect.bottom - m_OldRect.top);

  RECT rect;
  HWND hButton = ::GetDlgItem(m_hWnd, IDOK);
  ::GetWindowRect(hButton, &rect);
  Utils::ScreenToClient(m_hWnd, &rect);
  ::MoveWindow(hButton, rect.left + dx, rect.top + dy, rect.right - rect.left, rect.bottom - rect.top, true);

  HWND hInfo = ::GetDlgItem(m_hWnd, IDC_INFO);
  ::GetWindowRect(hInfo, &rect);
  Utils::ScreenToClient(m_hWnd, &rect);
  ::MoveWindow(hInfo, rect.left, rect.top, rect.right - rect.left + dx, rect.bottom - rect.top + dy, true);

  return false;
}

bool DetailsDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    HANDLE_DLGMSG(hWnd, WM_INITDIALOG, OnInitDialog);
    case WM_WINDOWPOSCHANGING:
      return SetDlgMsgResult(hWnd, uMsg, OnWindowPosChanging(hWnd, reinterpret_cast<LPWINDOWPOS>(lParam)));
    case WM_WINDOWPOSCHANGED:
      return SetDlgMsgResult(hWnd, uMsg, OnWindowPosChanged(hWnd, reinterpret_cast<LPWINDOWPOS>(lParam)));
  }
  return DialogBase::DlgProc(hWnd, uMsg, wParam, lParam);
}
