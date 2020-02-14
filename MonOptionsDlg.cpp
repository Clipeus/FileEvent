#include "pch.h"
#include "FileEvent.h"
#include "FileEventApp.h"
#include "FileEventWnd.h"
#include "Utils.h"
#include "FileEventMonitor.h"
#include "MonOptionsDlg.h"
#include "Registry.h"

MonOptionsDlg::MonOptionsDlg() : DialogBase(MAKEINTRESOURCE(IDD_MONITOR_OPTIONS))
{

}

void MonOptionsDlg::OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
  switch (id)
  {
    case IDOK:
    {
      m_Options.strPath.resize(MAX_PATH);
      int size = ::GetDlgItemText(hWnd, IDC_PATH, m_Options.strPath.data(), MAX_PATH);
      m_Options.strPath.resize(size);
      m_Options.bWatchSubtree = ::IsDlgButtonChecked(m_hWnd, IDC_SUBTREE);
      m_Options.dwNotifyFilter = 0;
      m_Options.dwNotifyFilter |= ::IsDlgButtonChecked(m_hWnd, IDC_FILE_NOTIFY_CHANGE_FILE_NAME) == BST_CHECKED ? FILE_NOTIFY_CHANGE_FILE_NAME : 0;
      m_Options.dwNotifyFilter |= ::IsDlgButtonChecked(m_hWnd, IDC_FILE_NOTIFY_CHANGE_DIR_NAME) == BST_CHECKED ? FILE_NOTIFY_CHANGE_DIR_NAME : 0;
      m_Options.dwNotifyFilter |= ::IsDlgButtonChecked(m_hWnd, IDC_FILE_NOTIFY_CHANGE_ATTRIBUTES) == BST_CHECKED ? FILE_NOTIFY_CHANGE_ATTRIBUTES : 0;
      m_Options.dwNotifyFilter |= ::IsDlgButtonChecked(m_hWnd, IDC_FILE_NOTIFY_CHANGE_SIZE) == BST_CHECKED ? FILE_NOTIFY_CHANGE_SIZE : 0;
      m_Options.dwNotifyFilter |= ::IsDlgButtonChecked(m_hWnd, IDC_FILE_NOTIFY_CHANGE_LAST_WRITE) == BST_CHECKED ? FILE_NOTIFY_CHANGE_LAST_WRITE : 0;
      m_Options.dwNotifyFilter |= ::IsDlgButtonChecked(m_hWnd, IDC_FILE_NOTIFY_CHANGE_LAST_ACCESS) == BST_CHECKED ? FILE_NOTIFY_CHANGE_LAST_ACCESS : 0;
      m_Options.dwNotifyFilter |= ::IsDlgButtonChecked(m_hWnd, IDC_FILE_NOTIFY_CHANGE_CREATION) == BST_CHECKED ? FILE_NOTIFY_CHANGE_CREATION : 0;
      m_Options.dwNotifyFilter |= ::IsDlgButtonChecked(m_hWnd, IDC_FILE_NOTIFY_CHANGE_SECURITY) == BST_CHECKED ? FILE_NOTIFY_CHANGE_SECURITY : 0;

      std::wstring strRoot = FileEventApp::GetApp()->GetRegistryRoot();
      Utils::RegistryW::SetValue(strRoot, L"MonitorPath", m_Options.strPath);
      Utils::RegistryW::SetValue(strRoot, L"WatchSubtree", m_Options.bWatchSubtree);
      Utils::RegistryW::SetValue(strRoot, L"NotifyFilter", m_Options.dwNotifyFilter);

      ::EndDialog(hWnd, IDOK);
      break;
    }
    case IDCANCEL:
    {
      ::EndDialog(hWnd, id);
      break;
    }
    case IDC_BROWSE:
    {
      if (codeNotify == BN_CLICKED)
      {
        std::wstring path = FileEventApp::GetApp()->GetMainWnd()->SelectFile(FileEventWnd::SelectFileMode::SelectFolder);
        if (!path.empty())
          ::SetDlgItemText(m_hWnd, IDC_PATH, path.c_str());
      }
      break;
    }
    case IDC_PATH:
    {
      if (codeNotify == EN_CHANGE)
      {
        std::wstring strFindText(MAX_PATH, 0);
        int size = ::GetDlgItemText(hWnd, IDC_PATH, strFindText.data(), strFindText.size());
        ::EnableWindow(::GetDlgItem(hWnd, IDOK), size > 0);
      }
      break;
    }
  }
}

bool MonOptionsDlg::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
  std::wstring strRoot = FileEventApp::GetApp()->GetRegistryRoot();
  Utils::RegistryW::Value(strRoot, L"MonitorPath", m_Options.strPath);
  Utils::RegistryW::Value(strRoot, L"WatchSubtree", (unsigned long&)m_Options.bWatchSubtree);
  Utils::RegistryW::Value(strRoot, L"NotifyFilter", m_Options.dwNotifyFilter);

  ::SetDlgItemText(m_hWnd, IDC_PATH, m_Options.strPath.c_str());
  ::CheckDlgButton(m_hWnd, IDC_SUBTREE, m_Options.bWatchSubtree);
  ::CheckDlgButton(m_hWnd, IDC_FILE_NOTIFY_CHANGE_FILE_NAME, m_Options.dwNotifyFilter & FILE_NOTIFY_CHANGE_FILE_NAME);
  ::CheckDlgButton(m_hWnd, IDC_FILE_NOTIFY_CHANGE_DIR_NAME, m_Options.dwNotifyFilter & FILE_NOTIFY_CHANGE_DIR_NAME);
  ::CheckDlgButton(m_hWnd, IDC_FILE_NOTIFY_CHANGE_ATTRIBUTES, m_Options.dwNotifyFilter & FILE_NOTIFY_CHANGE_ATTRIBUTES);
  ::CheckDlgButton(m_hWnd, IDC_FILE_NOTIFY_CHANGE_SIZE, m_Options.dwNotifyFilter & FILE_NOTIFY_CHANGE_SIZE);
  ::CheckDlgButton(m_hWnd, IDC_FILE_NOTIFY_CHANGE_LAST_WRITE, m_Options.dwNotifyFilter & FILE_NOTIFY_CHANGE_LAST_WRITE);
  ::CheckDlgButton(m_hWnd, IDC_FILE_NOTIFY_CHANGE_LAST_ACCESS, m_Options.dwNotifyFilter & FILE_NOTIFY_CHANGE_LAST_ACCESS);
  ::CheckDlgButton(m_hWnd, IDC_FILE_NOTIFY_CHANGE_CREATION, m_Options.dwNotifyFilter & FILE_NOTIFY_CHANGE_CREATION);
  ::CheckDlgButton(m_hWnd, IDC_FILE_NOTIFY_CHANGE_SECURITY, m_Options.dwNotifyFilter & FILE_NOTIFY_CHANGE_SECURITY);
  ::EnableWindow(::GetDlgItem(hWnd, IDOK), !m_Options.strPath.empty());

  return DialogBase::OnInitDialog(hWnd, hWndFocus, lParam);
}

bool MonOptionsDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    HANDLE_DLGMSG(hWnd, WM_INITDIALOG, OnInitDialog);
    HANDLE_DLGMSG(hWnd, WM_COMMAND, OnCommand);
  }
  return DialogBase::DlgProc(hWnd, uMsg, wParam, lParam);
}

FileEventMonitor::Options MonOptionsDlg::GetOptions() const
{
  return m_Options;
}
