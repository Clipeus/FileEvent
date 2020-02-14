#include "pch.h"
#include "FileEvent.h"
#include "FileEventApp.h"
#include "FileEventWnd.h"
#include "FileEventList.h"
#include "Utils.h"
#include "FileEventMonitor.h"
#include "AboutDlg.h"
#include "DetailsDlg.h"
#include "OptionsDlg.h"
#include "FindDlg.h"
#include "MonOptionsDlg.h"
#include "Registry.h"

constexpr int IDC_TOOLBAR_IMAGE_LIST = 0;
constexpr int IDC_MAIN_STATUSBAR = IDC_TOOLBAR_IMAGE_LIST + 1;
constexpr int IDC_LIST_VIEW = IDC_MAIN_STATUSBAR + 1;

constexpr int TOOLBAR_BUTTON_SIZE = 16;

constexpr int MENU_PIC_SIZE = 16;
constexpr int MENU_PIC_PLACE_SIZE = MENU_PIC_SIZE + (MENU_PIC_SIZE * 25 / 100); // + 25%
constexpr int MENU_PIC_INDENT = MENU_PIC_SIZE - 2;

constexpr int WM_MONITOREVENT = WM_USER + 1;
constexpr int WM_MONITORERROR = WM_MONITOREVENT + 1;

TBBUTTON tbButtons[] =
{
  {MAKELONG(0,  IDC_TOOLBAR_IMAGE_LIST), ID_FILE_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {MAKELONG(1,  IDC_TOOLBAR_IMAGE_LIST), ID_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0},
  {MAKELONG(2,  IDC_TOOLBAR_IMAGE_LIST), ID_EDIT_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {MAKELONG(3,  IDC_TOOLBAR_IMAGE_LIST), ID_EDIT_CLEAR, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {MAKELONG(4,  IDC_TOOLBAR_IMAGE_LIST), ID_EDIT_FIND, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0},
  {MAKELONG(5,  IDC_TOOLBAR_IMAGE_LIST), ID_MONITOR_START, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {MAKELONG(6,  IDC_TOOLBAR_IMAGE_LIST), ID_MONITOR_PAUSE, TBSTATE_ENABLED, TBSTYLE_CHECK, 0, 0},
  {MAKELONG(7,  IDC_TOOLBAR_IMAGE_LIST), ID_MONITOR_STOP, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {MAKELONG(8,  IDC_TOOLBAR_IMAGE_LIST), ID_VIEW_DETAILS, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
  {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0},
  {MAKELONG(9,  IDC_TOOLBAR_IMAGE_LIST), ID_VIEW_OPTIONS, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
};

constexpr int ABOUT_IMAGE_INDEX = 10;


FileEventWnd::FileEventWnd()
{
}

FileEventWnd::~FileEventWnd()
{

}

bool FileEventWnd::Init()
{
  WNDCLASSEX wc = { 0 };

  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = 0;
  wc.lpfnWndProc = WindowBase::s_WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = FileEventApp::GetApp()->GetInstance();
  wc.hIcon = (HICON)LoadImage(FileEventApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, 32, 32, 0);
  wc.hIconSm = (HICON)LoadImage(FileEventApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, 16, 16, 0);
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.hbrBackground = nullptr;
  wc.lpszMenuName = MAKEINTRESOURCE(IDM_MAIN);
  wc.lpszClassName = GetClassName();

  if (!::RegisterClassEx(&wc))
  {
    Utils::ShowOSError();
    return false;
  }

  return true;
}

bool FileEventWnd::Create(int nCmdShow)
{
  if (!WindowBase::Create(WS_EX_ACCEPTFILES, GetClassName(), FileEventApp::GetApp()->GetAppName()))
  {
    Utils::ShowOSError();
    return false;
  }

  _ASSERTE(m_hWnd);

  ::ShowWindow(m_hWnd, nCmdShow);
  ::UpdateWindow(m_hWnd);

  return true;
}

LPCWSTR FileEventWnd::GetClassName()
{
  return L"FileEventMainWindow";
}

void FileEventWnd::ChangeUIState()
{
  UpdateStatusBarText();

  long nSelCount = ListView_GetSelectedCount(*m_spFileEventList);
  long nCount = ListView_GetItemCount(*m_spFileEventList);

  HMENU hMainMenu = ::GetMenu(m_hWnd);
  HMENU hMenu = ::GetSubMenu(hMainMenu, 0);
  
  ::EnableMenuItem(hMenu, ID_FILE_OPEN, MF_BYCOMMAND | (!m_spFileEventMonitor->IsStarted() ? MF_ENABLED : MF_GRAYED));
  ::SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_FILE_OPEN, MAKELONG(!m_spFileEventMonitor->IsStarted(), 0));

  ::EnableMenuItem(hMenu, ID_FILE_SAVE, MF_BYCOMMAND | (nCount > 0 && m_bDirty ? MF_ENABLED : MF_GRAYED));
  ::SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_FILE_SAVE, MAKELONG(nCount > 0 && m_bDirty, 0));
  ::EnableMenuItem(hMenu, ID_FILE_SAVE_AS, MF_BYCOMMAND | (nCount > 0 ? MF_ENABLED : MF_GRAYED));

  hMenu = ::GetSubMenu(hMainMenu, 1);

  ::EnableMenuItem(hMenu, ID_EDIT_COPY, MF_BYCOMMAND | (nSelCount > 0 ? MF_ENABLED : MF_GRAYED));
  ::SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_COPY, MAKELONG(nSelCount > 0, 0));

  ::EnableMenuItem(hMenu, ID_EDIT_CLEAR, MF_BYCOMMAND | (m_spFileEventMonitor->IsStarted() ? MF_ENABLED : MF_GRAYED));
  ::SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_CLEAR, MAKELONG(m_spFileEventMonitor->IsStarted(), 0));

  ::EnableMenuItem(hMenu, ID_EDIT_FIND, MF_BYCOMMAND | (nCount > 0 ? MF_ENABLED : MF_GRAYED));
  ::SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_FIND, MAKELONG(nCount > 0, 0));

  ::EnableMenuItem(hMenu, ID_EDIT_FINDNEXT, MF_BYCOMMAND | (nCount > 0 ? MF_ENABLED : MF_GRAYED));
  ::SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_FINDNEXT, MAKELONG(nCount > 0, 0));

  ::EnableMenuItem(hMenu, ID_EDIT_FINDPREVIOUS, MF_BYCOMMAND | (nCount > 0 ? MF_ENABLED : MF_GRAYED));
  ::SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_FINDPREVIOUS, MAKELONG(nCount > 0, 0));

  ::EnableMenuItem(hMenu, ID_EDIT_SELECTALL, MF_BYCOMMAND | (nCount > 0 ? MF_ENABLED : MF_GRAYED));
  ::SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_EDIT_FINDPREVIOUS, MAKELONG(nCount > 0, 0));

  hMenu = ::GetSubMenu(hMainMenu, 2);

  ::EnableMenuItem(hMenu, ID_VIEW_DETAILS, MF_BYCOMMAND | (nSelCount > 0 ? MF_ENABLED : MF_GRAYED));
  ::SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_VIEW_DETAILS, MAKELONG(nSelCount > 0, 0));

  hMenu = ::GetSubMenu(hMainMenu, 3);

  ::EnableMenuItem(hMenu, ID_MONITOR_START, MF_BYCOMMAND | (!m_spFileEventMonitor->IsStarted() ? MF_ENABLED : MF_GRAYED));
  ::SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_MONITOR_START, MAKELONG(!m_spFileEventMonitor->IsStarted(), 0));

  ::EnableMenuItem(hMenu, ID_MONITOR_PAUSE, MF_BYCOMMAND | (m_spFileEventMonitor->IsStarted() ? MF_ENABLED : MF_GRAYED));
  ::SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_MONITOR_PAUSE, MAKELONG(m_spFileEventMonitor->IsStarted(), 0));

  ::EnableMenuItem(hMenu, ID_MONITOR_STOP, MF_BYCOMMAND | (m_spFileEventMonitor->IsStarted() ? MF_ENABLED : MF_GRAYED));
  ::SendMessage(m_hToolBar, TB_ENABLEBUTTON, ID_MONITOR_STOP, MAKELONG(m_spFileEventMonitor->IsStarted(), 0));

  MENUITEMINFO MenuInfo = { 0 };
  MenuInfo.cbSize = sizeof(MenuInfo);
  MenuInfo.fMask = MIIM_STRING;
  std::wstring str = Utils::LoadString(m_spFileEventMonitor->IsSuspended() ? IDS_RESUME_MENU : IDS_PAUSE_MENU);
  MenuInfo.dwTypeData = &str.front();
  MenuInfo.cch = str.size();
  ::SetMenuItemInfo(hMenu, ID_MONITOR_PAUSE, false, &MenuInfo);

  if (m_spFileEventMonitor->IsStarted())
  {
    TBBUTTONINFO BtnInfo = { 0 };
    BtnInfo.cbSize = sizeof(BtnInfo);
    BtnInfo.dwMask = TBIF_STATE;
    BtnInfo.fsState = m_spFileEventMonitor->IsSuspended() ? TBSTATE_CHECKED | TBSTATE_ENABLED : TBSTATE_ENABLED;
    ::SendMessage(m_hToolBar, TB_SETBUTTONINFO, ID_MONITOR_PAUSE, (LPARAM)&BtnInfo);

    m_strSavedPath.clear();
  }
  else
  {
    m_strMonitorPath.clear();
  }

  std::wstring title; 
  if (!m_strSavedPath.empty())
  {
    std::filesystem::path path = m_strSavedPath;
    title = path.filename().wstring() + L" - ";
  }
  title += Utils::LoadString(IDS_APP_TITLE);

  if (m_spFileEventMonitor->IsStarted())
    title += L" (" + m_strMonitorPath + L")";

  if (m_bDirty)
    title += L"*";

  ::SetWindowText(m_hWnd, title.c_str());
}

void FileEventWnd::UpdateStatusBarText()
{
  long nSelCount = ListView_GetSelectedCount(*m_spFileEventList);
  long nCount = ListView_GetItemCount(*m_spFileEventList);

  SendMessage(m_hStatusBar, SB_SETTEXT, 0, (LPARAM)Utils::StrFormat(Utils::LoadString(IDS_IDLE_STATUS).c_str(), nCount, nSelCount).c_str());
}

void FileEventWnd::OnEvent(FileEventItem* pItem)
{
  ::PostMessage(m_hWnd, WM_MONITOREVENT, 0, reinterpret_cast<LPARAM>(pItem));
}

void FileEventWnd::OnError(int nIDs, DWORD dwError)
{
  ::PostMessage(m_hWnd, WM_MONITORERROR, nIDs, dwError);
}

void FileEventWnd::OpenFile(const std::wstring& path)
{
  Utils::unique_handle m_hFile = Utils::make_unique_handle(::CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
  if (m_hFile.get() == INVALID_HANDLE_VALUE)
  {
    Utils::ShowOSError(IDS_OPEN_FILE_FAILED);
    return;
  }

  LARGE_INTEGER li;
  if (!::GetFileSizeEx(m_hFile.get(), &li))
  {
    Utils::ShowOSError();
    return;
  }

  if (li.HighPart > 0)
  {
    Utils::ReportBox(IDS_FILE_IS_TO_LARGE, MB_OK | MB_ICONERROR);
    return;
  }

  std::string strBuffer(li.LowPart, 0);
  DWORD dwRead = 0;
  if (!::ReadFile(m_hFile.get(), strBuffer.data(), li.LowPart, &dwRead, nullptr))
  {
    Utils::ShowOSError(IDS_READ_FILE_FAILED);
    return;
  }

  m_spFileEventList->Clear();

  bool bFirstLine = true;
  size_t current_pos = 0;
  std::wstring strDirPath;

  while (true)
  {
    size_t pos = strBuffer.find("\r\n", current_pos);
    if (pos == std::string::npos)
      break;

    std::string strLineA = strBuffer.substr(current_pos, pos - current_pos);
    current_pos = pos + 2;

    if (bFirstLine)
    {
      bFirstLine = false;

      int size = ::MultiByteToWideChar(CP_ACP, 0, strLineA.c_str(), -1, nullptr, 0);
      strDirPath.resize(size);
      ::MultiByteToWideChar(CP_ACP, 0, strLineA.c_str(), -1, strDirPath.data(), strDirPath.size());
      strDirPath.resize(strDirPath.size() - 1);
      continue;
    }

    int size = ::MultiByteToWideChar(CP_ACP, 0, strLineA.c_str(), -1, nullptr, 0);
    std::wstring strLine(size, 0);
    ::MultiByteToWideChar(CP_ACP, 0, strLineA.c_str(), -1, strLine.data(), strLine.size());
    strLine.resize(strLine.size() - 1);

    std::wstring strPart1 = strLine, strPart2, strPart3;
    pos = strLine.find(L";");
    if (pos != std::string::npos)
    {
      strPart1 = strLine.substr(0, pos);

      size_t pos2 = strLine.find(L";", ++pos);
      if (pos2 != std::string::npos)
      {
        strPart2 = strLine.substr(pos, pos2 - pos);
        pos = strLine.find(L";", ++pos2);
        if (pos != std::string::npos)
          strPart3 = strLine.substr(pos2, pos - pos2);
        else
          strPart3 = strLine.substr(pos2);
      }
    }

    std::unique_ptr<FileEventItem> pItem(new FileEventItem);
    pItem->dwAction = std::stol(strPart1);
    pItem->strDescription = strPart2;
    pItem->strDirName = strDirPath;
    pItem->strFileName = strPart3;

    m_spFileEventList->AddItem(pItem.release());
  }

  m_bDirty = false;
  m_strSavedPath = path;
}

void FileEventWnd::SaveFile(const std::wstring& path)
{
  Utils::unique_handle m_hFile = Utils::make_unique_handle(::CreateFile(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
  if (m_hFile.get() == INVALID_HANDLE_VALUE)
  {
    Utils::ShowOSError(IDS_SAVE_FILE_FAILED);
    return;
  }

  bool bFirstLine = true;
  long nCount = ListView_GetItemCount(*m_spFileEventList);
  for (int i = 0; i < nCount; i++)
  {
    FileEventItem* pItem = m_spFileEventList->GetItem(i);
    std::filesystem::path path = pItem->strDirName;
    path.append(pItem->strFileName);

    if (bFirstLine)
    {
      bFirstLine = false;

      int size = ::WideCharToMultiByte(CP_ACP, 0, pItem->strDirName.c_str(), -1, nullptr, 0, nullptr, nullptr);
      std::string strPath(size, 0);
      ::WideCharToMultiByte(CP_ACP, 0, pItem->strDirName.c_str(), -1, strPath.data(), strPath.size(), nullptr, nullptr);
      strPath.resize(strPath.size() - 1);
      strPath += +"\r\n";

      DWORD dwWritten = 0;
      if (!::WriteFile(m_hFile.get(), strPath.c_str(), strPath.size(), &dwWritten, nullptr))
      {
        Utils::ShowOSError(IDS_WRITE_FILE_FAILED);
        return;
      }
    }

    int size = ::WideCharToMultiByte(CP_ACP, 0, pItem->strDescription.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string strDescr(size, 0);
    ::WideCharToMultiByte(CP_ACP, 0, pItem->strDescription.c_str(), -1, strDescr.data(), strDescr.size(), nullptr,nullptr);
    strDescr.resize(strDescr.size() - 1);

    size = ::WideCharToMultiByte(CP_ACP, 0, path.wstring().c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string strPath(size, 0);
    ::WideCharToMultiByte(CP_ACP, 0, path.wstring().c_str(), -1, strPath.data(), strPath.size(), nullptr, nullptr);
    strPath.resize(strPath.size() - 1);

    std::string strBuffer = std::to_string(pItem->dwAction) + ";" + strDescr + ";" + strPath + "\r\n";
    DWORD dwWritten = 0;
    if (!::WriteFile(m_hFile.get(), strBuffer.c_str(), strBuffer.size(), &dwWritten, nullptr))
    {
      Utils::ShowOSError(IDS_WRITE_FILE_FAILED);
      return;
    }
  }

  m_bDirty = false;
  m_strSavedPath = path;
}

void FileEventWnd::CopyToClipboard()
{
  if (::OpenClipboard(m_hWnd) && ::EmptyClipboard())
  {
    std::wstring strBuffer;
    int nIndex = -1;

    while (FileEventItem* pItem = m_spFileEventList->GetNextItem(nIndex, true))
    {
      std::filesystem::path path = pItem->strDirName;
      path.append(pItem->strFileName);

      strBuffer += pItem->strDescription + L"\r\n"; 
      strBuffer += path.wstring().c_str();
      strBuffer += + L"\r\n\r\n";
    }

    auto set_clipboard_data = [](const void* lpBuffer, size_t nSize, UINT uFormat)
    {
      HGLOBAL hClipData = ::GlobalAlloc(GHND, nSize);
      _ASSERTE(hClipData);
      if (hClipData)
      {
        LPWSTR lpszClipData = reinterpret_cast<LPWSTR>(::GlobalLock(hClipData));
        if (lpszClipData)
          std::memcpy(lpszClipData, lpBuffer, nSize);

        ::GlobalUnlock(hClipData);

        HANDLE hClip = ::SetClipboardData(uFormat, hClipData);
        if (!hClip)
          ::GlobalFree(hClipData);
      }
    };

    set_clipboard_data(strBuffer.c_str(), strBuffer.size() * sizeof(wchar_t), CF_UNICODETEXT);

    int size = ::WideCharToMultiByte(CP_ACP, 0, strBuffer.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string strBufferA(size, 0);
    ::WideCharToMultiByte(CP_ACP, 0, strBuffer.c_str(), -1, strBufferA.data(), strBufferA.size(), nullptr, nullptr);
    strBufferA.resize(strBufferA.size() - 1);
    set_clipboard_data(strBufferA.c_str(), strBufferA.size() * sizeof(char), CF_TEXT);

    ::CloseClipboard();
  }
}

std::wstring FileEventWnd::SelectFile(SelectFileMode enSelectFileMode) const
{
  if (!m_bComInited)
    ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

  std::wstring path;

  IFileDialog* pfd = nullptr;
  if (SUCCEEDED(::CoCreateInstance(enSelectFileMode == SelectFileMode::SaveFile ? CLSID_FileSaveDialog : CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd))))
  {
    DWORD dwOptions;
    if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
    {
      if (enSelectFileMode == SelectFileMode::OpenFile)
        pfd->SetOptions(dwOptions | FOS_FILEMUSTEXIST | FOS_PATHMUSTEXIST);
      else if (enSelectFileMode == SelectFileMode::SaveFile)
        pfd->SetOptions(dwOptions | FOS_PATHMUSTEXIST | FOS_OVERWRITEPROMPT | FOS_NOREADONLYRETURN);
      else if (enSelectFileMode == SelectFileMode::SelectFolder)
        pfd->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_PATHMUSTEXIST);
    }

    if (enSelectFileMode != SelectFileMode::SelectFolder)
    {
      COMDLG_FILTERSPEC filter[2];
      std::wstring str1 = Utils::LoadString(IDS_FILE_FILTER_CSV_1);
      filter[0].pszName = str1.c_str();
      std::wstring str2 = Utils::LoadString(IDS_FILE_FILTER_CSV_2);
      filter[0].pszSpec = str2.c_str();
      std::wstring str3 = Utils::LoadString(IDS_FILE_FILTER_ALL_1);
      filter[1].pszName = str3.c_str();
      std::wstring str4 = Utils::LoadString(IDS_FILE_FILTER_ALL_2);
      filter[1].pszSpec = str4.c_str();
      pfd->SetFileTypes(2, filter);
    }

    if (SUCCEEDED(pfd->Show(m_hWnd)))
    {
      IShellItem* psi;
      if (SUCCEEDED(pfd->GetResult(&psi)))
      {
        LPWSTR lpFolder;
        if (SUCCEEDED(psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &lpFolder)))
        {
          path = lpFolder;
          ::CoTaskMemFree(lpFolder);
        }
        psi->Release();
      }
    }
    pfd->Release();
  }

  return path;
}

bool FileEventWnd::OnFind(FindMode enFindMode)
{
  auto compare = [this](const std::wstring& strText, const std::wstring& strFindText, int nIndex) -> bool
  {
    auto end_find = [this](int nIndex) -> bool
    {
      int nLastIndex = -1;

      while ((nLastIndex = ListView_GetNextItem(*m_spFileEventList, nLastIndex, LVNI_SELECTED)) != -1)
        ListView_SetItemState(*m_spFileEventList, nLastIndex, 0, LVIS_FOCUSED | LVIS_SELECTED);

      ListView_SetItemState(*m_spFileEventList, nIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
      ListView_EnsureVisible(*m_spFileEventList, nIndex, false);
      return true;
    };

    if (m_pFindDlg->GetFindFlags() & FindDlg::Flags::FD_ACTION)
    {
      if (strText == strFindText)
      {
        return end_find(nIndex);
      }
    }
    else
    {
      if (strText.find(strFindText.c_str()) != std::wstring::npos)
      {
        return end_find(nIndex);
      }
    }

    return false;
  };

  if (!m_pFindDlg)
    m_pFindDlg = std::make_unique<FindDlg>();

  if (enFindMode == FindMode::Find || m_pFindDlg->GetFindText().empty())
  {
    if (IDOK != m_pFindDlg->DoModal(m_hWnd))
      return false;
  }

  bool bSelection = ListView_GetSelectedCount(*m_spFileEventList) > 1;
  std::wstring strFindText = m_pFindDlg->GetFindText();
  std::transform(strFindText.cbegin(), strFindText.cend(), strFindText.begin(), toupper);

  int nIndex = ListView_GetNextItem(*m_spFileEventList, -1, LVNI_SELECTED);//m_spFileEventList->GetFirstSelItemIndex();
  if (nIndex == -1)
    nIndex = 0;
  else if (enFindMode != FindMode::Find)
    nIndex = enFindMode == FindMode::FindPrev ? nIndex - 1 : nIndex + 1;

  FileEventItem* pItem = m_spFileEventList->GetItem(nIndex);
  if (pItem)
  {
    do
    {
      if (m_pFindDlg->GetFindFlags() & FindDlg::Flags::FD_ACTION)
      {
        std::wstring strText = pItem->strDescription;
        std::transform(strText.cbegin(), strText.cend(), strText.begin(), toupper);

        if (compare(strText, strFindText, nIndex))
          return true;
      }

      if (m_pFindDlg->GetFindFlags() & FindDlg::Flags::FD_PATH)
      {
        std::filesystem::path path = pItem->strDirName;
        path.append(pItem->strFileName);
        std::wstring strText = path.wstring();
        std::transform(strText.cbegin(), strText.cend(), strText.begin(), toupper);

        if (compare(strText, strFindText, nIndex))
          return true;
      }
    }
    while (pItem = m_spFileEventList->GetNextItem(nIndex, bSelection, enFindMode == FindMode::FindPrev));
  }

  if (!m_pFindDlg->GetFindText().empty())
    Utils::ReportBox(Utils::StrFormat(Utils::LoadString(bSelection ? IDS_CANNOT_FIND_SEL : IDS_CANNOT_FIND).c_str(), m_pFindDlg->GetFindText().c_str()).c_str(), MB_ICONINFORMATION | MB_OK);

  return false;
}

LRESULT FileEventWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    HANDLE_MSG(hWnd, WM_CONTEXTMENU, OnContextMenu);
    HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
    HANDLE_MSG(hWnd, WM_SIZE, OnSize);
    HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
    HANDLE_MSG(hWnd, WM_MENUSELECT, OnMenuSelect);
    HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
    HANDLE_MSG(hWnd, WM_NOTIFY, OnNotify);
    HANDLE_MSG(hWnd, WM_SETCURSOR, OnSetCursor);
    HANDLE_MSG(hWnd, WM_ACTIVATE, OnActivate);
    HANDLE_MSG(hWnd, WM_INITMENUPOPUP, OnInitMenuPopup);
    HANDLE_MSG(hWnd, WM_MEASUREITEM, OnMeasureItem), true; // should return true
    HANDLE_MSG(hWnd, WM_DRAWITEM, OnDrawItem), true; // should return true
    case WM_MONITOREVENT:
      return OnMonitorEvent(hWnd, reinterpret_cast<FileEventItem*>(lParam)), 0L;
    case WM_MONITORERROR:
      return OnMonitorError(hWnd, wParam, lParam), 0L;

    default:
      return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
}

void FileEventWnd::OnDestroy(HWND hWnd)
{
  std::vector<unsigned char> buf(sizeof(WINDOWPLACEMENT));
  LPWINDOWPLACEMENT pWinPos = reinterpret_cast<LPWINDOWPLACEMENT>(buf.data());
  pWinPos->length = sizeof(WINDOWPLACEMENT);
  ::GetWindowPlacement(m_hWnd, pWinPos);
  Utils::RegistryW::SetValue(FileEventApp::GetApp()->GetRegistryRoot(), L"WindowPos", buf);

  if (m_spFileEventMonitor->IsStarted())
    m_spFileEventMonitor->Stop();

  if (m_hImageList)
    ::ImageList_Destroy(m_hImageList);

  if (m_bComInited)
    ::CoUninitialize();

  ::PostQuitMessage(0);
}

bool FileEventWnd::OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
  m_hWnd = hWnd;

  m_hToolBar = ::CreateWindowEx(0, TOOLBARCLASSNAME, nullptr, WS_CHILD | WS_BORDER | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | TBSTYLE_WRAPABLE, 0, 0, 0, 0, m_hWnd, nullptr, FileEventApp::GetApp()->GetInstance(), nullptr);
  if (!m_hToolBar)
  {
    Utils::ShowOSError();
    return false;
  }

  m_hImageList = ::ImageList_Create(TOOLBAR_BUTTON_SIZE, TOOLBAR_BUTTON_SIZE, ILC_COLOR32 | ILC_MASK, 10, 0);
  if (!m_hImageList)
  {
    Utils::ShowOSError();
    return false;
  }

  ::ImageList_AddIcon(m_hImageList, ::LoadIcon(FileEventApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_OPEN_BTN)));
  ::ImageList_AddIcon(m_hImageList, ::LoadIcon(FileEventApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_SAVE_BTN)));
  ::ImageList_AddIcon(m_hImageList, ::LoadIcon(FileEventApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_COPY_BTN)));
  ::ImageList_AddIcon(m_hImageList, ::LoadIcon(FileEventApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_CLEAR_BTN)));
  ::ImageList_AddIcon(m_hImageList, ::LoadIcon(FileEventApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_FIND_BTN)));
  ::ImageList_AddIcon(m_hImageList, ::LoadIcon(FileEventApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_START_BTN)));
  ::ImageList_AddIcon(m_hImageList, ::LoadIcon(FileEventApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_PAUSE_BTN)));
  ::ImageList_AddIcon(m_hImageList, ::LoadIcon(FileEventApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_STOP_BTN)));
  ::ImageList_AddIcon(m_hImageList, ::LoadIcon(FileEventApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_DETAILS_BTN)));
  ::ImageList_AddIcon(m_hImageList, ::LoadIcon(FileEventApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_OPTION_BTN)));
  ::ImageList_AddIcon(m_hImageList, ::LoadIcon(FileEventApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_MAIN)));

  ::SendMessage(m_hToolBar, TB_SETIMAGELIST, IDC_TOOLBAR_IMAGE_LIST, (LPARAM)m_hImageList);
  ::SendMessage(m_hToolBar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
  ::SendMessage(m_hToolBar, TB_ADDBUTTONS, sizeof(tbButtons) / sizeof(tbButtons[0]), (LPARAM)&tbButtons);
  ::SendMessage(m_hToolBar, TB_AUTOSIZE, 0, 0);

  m_hStatusBar = ::CreateWindowEx(0, STATUSCLASSNAME, nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER | SBARS_SIZEGRIP, 0, 0, 0, 0, m_hWnd, (HMENU)IDC_MAIN_STATUSBAR, FileEventApp::GetApp()->GetInstance(), nullptr);
  if (!m_hStatusBar)
  {
    Utils::ShowOSError();
    return false;
  }

  RECT rect;
  ::GetClientRect(m_hWnd, &rect);
  m_spFileEventList = std::make_unique<FileEventList>();
  if (!m_spFileEventList->Create(WS_EX_CLIENTEDGE, WC_LISTVIEW, L"", WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS, 0, 0, rect.right, rect.bottom, hWnd, (HMENU)IDC_LIST_VIEW))
  {
    Utils::ShowOSError();
    return false;
  }

  if (!m_spFileEventList->Init())
  {
    Utils::ShowOSError();
    return false;
  }

  m_spFileEventMonitor = std::make_unique<FileEventMonitor>();

  ChangeUIState();

  std::vector<unsigned char> buf(sizeof(WINDOWPLACEMENT));
  if (Utils::RegistryW::Value(FileEventApp::GetApp()->GetRegistryRoot(), L"WindowPos", buf))
    ::SetWindowPlacement(m_hWnd, reinterpret_cast<LPWINDOWPLACEMENT>(buf.data()));

  return true;
}

void FileEventWnd::OnSize(HWND hWnd, UINT state, int cx, int cy)
{
  ::SendMessage(m_hToolBar, WM_SIZE, state, 0);
  ::SendMessage(m_hStatusBar, WM_SIZE, state, 0);

  RECT Rect;
  int nTop, nBottom;

  ::GetWindowRect(m_hToolBar, &Rect);
  nTop = Rect.bottom - Rect.top - 1;

  ::GetWindowRect(m_hStatusBar, &Rect);
  nBottom = Rect.bottom - Rect.top;

  m_spFileEventList->AdjustRect(0, nTop, cx, cy - nBottom - nTop);
}

void FileEventWnd::OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
  switch (id)
  {
    case ID_FILE_EXIT:
    {
      ::PostMessage(hWnd, WM_CLOSE, 0, 0);
      break;
    }
    case ID_FILE_OPEN:
    {
      std::wstring path = SelectFile(SelectFileMode::OpenFile);
      if (!path.empty())
        OpenFile(path);

      break;
    }
    case ID_FILE_SAVE:
    case ID_FILE_SAVE_AS:
    {
      std::wstring path = m_strSavedPath;
      if (m_strSavedPath.empty() || id == ID_FILE_SAVE_AS)
        path = SelectFile(SelectFileMode::SaveFile);

      if (!path.empty())
        SaveFile(path);

      break;
    }
    case ID_EDIT_COPY:
    {
      CopyToClipboard();
      break;
    }
    case ID_EDIT_CLEAR:
    {
      m_spFileEventList->Clear();
      break;
    }
    case ID_EDIT_FIND:
    {
      OnFind(FindMode::Find);
      break;
    }
    case ID_EDIT_FINDNEXT:
    {
      OnFind(FindMode::FindNext);
      break;
    }
    case ID_EDIT_FINDPREVIOUS:
    {
      OnFind(FindMode::FindPrev);
      break;
    }
    case ID_EDIT_SELECTALL:
    {
      m_spFileEventList->SelectAll();
      break;
    }
    case ID_VIEW_DETAILS:
    {
      FileEventItem* pItem = m_spFileEventList->GetCurrentItem();
      if (pItem != nullptr)
      {
        DetailsDlg dlg(pItem);
        dlg.DoModal(m_hWnd);
      }
      break;
    }
    case ID_VIEW_OPTIONS:
    {
      OptionsDlg dlg;
      dlg.DoModal(hWnd);
      break;
    }
    case ID_MONITOR_START:
    {
      MonOptionsDlg dlg;
      if (IDOK == dlg.DoModal(m_hWnd))
      {
        m_spFileEventList->Clear();
        FileEventMonitor::Options options = dlg.GetOptions();
        m_strMonitorPath = options.strPath;
        m_spFileEventMonitor->Start(this, &options);
      }

      break;
    }
    case ID_MONITOR_PAUSE:
    {
      if (m_spFileEventMonitor->IsSuspended())
        m_spFileEventMonitor->Resume();
      else if (!m_spFileEventMonitor->IsSuspended())
        m_spFileEventMonitor->Suspend();

      break;
    }
    case ID_MONITOR_STOP:
    {
      m_spFileEventMonitor->Stop();
      m_strMonitorPath.clear();
      break;
    }
    case ID_HELP_ABOUT:
    {
      AboutDlg dlg;
      dlg.DoModal(m_hWnd);
      break;
    }
  }
  ChangeUIState();
}

void FileEventWnd::OnMenuSelect(HWND hWnd, HMENU hMenu, int item, HMENU hMenuPopup, UINT flags)
{
  std::wstring strText;

  if ((flags == 0xFFFFFFFF) && (hMenu == nullptr))
  {
    UpdateStatusBarText();
  }
  else
  {
    if ((flags & MF_POPUP) || (flags & MFT_SEPARATOR))
    {
      strText.clear();
    }
    else
    {
      strText = Utils::LoadString(item == ID_MONITOR_PAUSE && m_spFileEventMonitor->IsSuspended() ? ID_MONITOR_RESUME : item);
      size_t pos = strText.find(L'\n');
      if (std::wstring::npos != pos)
        strText = strText.substr(0, pos);
    }
    SendMessage(m_hStatusBar, SB_SETTEXT, 0, (LPARAM)strText.c_str());
  }
}

void FileEventWnd::OnContextMenu(HWND hWnd, HWND hwndContext, UINT x, UINT y)
{
}

LRESULT FileEventWnd::OnNotify(HWND hWnd, int idFrom, NMHDR* pNmndr)
{
  switch (pNmndr->code)
  {
    case TTN_GETDISPINFO:
    {
      LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)pNmndr;
      std::wstring strText = Utils::LoadString(idFrom == ID_MONITOR_PAUSE && m_spFileEventMonitor->IsSuspended() ? ID_MONITOR_RESUME : idFrom);
      size_t pos = strText.find(L'\n');
      if (std::wstring::npos != pos)
        strText = strText.substr(pos + 1);
      wcsncpy_s(lpTTT->szText, strText.c_str(), sizeof(lpTTT->szText));
      return true;
    }
    case LVN_ITEMCHANGED:
    case LVN_INSERTITEM:
    {
      ChangeUIState();
      break;
    }
    case LVN_DELETEITEM:
    {
      NM_LISTVIEW* plvItem = (NM_LISTVIEW*)pNmndr;
      if (plvItem->lParam)
      {
        delete (FileEventItem*)plvItem->lParam;
      }
    }
    case NM_DBLCLK:
    {
      OnCommand(hWnd, ID_VIEW_DETAILS, pNmndr->hwndFrom, 0);
      break;
    }
  }
  return false;
}

bool FileEventWnd::OnSetCursor(HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg)
{
  return FORWARD_WM_SETCURSOR(hWnd, hWndCursor, codeHitTest, msg, ::DefWindowProc);
}

void FileEventWnd::OnActivate(HWND hWnd, UINT nState, HWND hWndActDeact, bool fMinimized)
{
  if (nState == WA_ACTIVE)
    ::SetFocus(*m_spFileEventList);
}

void FileEventWnd::OnMeasureItem(HWND hWnd, MEASUREITEMSTRUCT* lpMeasureItem)
{
  if (lpMeasureItem->CtlType == ODT_MENU)
  {
    std::wstring srtCaption(256, 0);
    int nSize = ::GetMenuString((HMENU)lpMeasureItem->itemData, lpMeasureItem->itemID, srtCaption.data(), srtCaption.size(), MF_BYCOMMAND);
    if (nSize)
      srtCaption.resize(nSize);
    else
      srtCaption = L"-";

    HDC hDC = ::GetWindowDC(m_hWnd);

    SIZE Size;
    ::GetTextExtentPoint32(hDC, srtCaption.c_str(), srtCaption.size(), &Size);

    ::ReleaseDC(m_hWnd, hDC);

    if (srtCaption != L"-")
      lpMeasureItem->itemHeight = Size.cy < MENU_PIC_PLACE_SIZE ? MENU_PIC_PLACE_SIZE : Size.cy;
    else
      lpMeasureItem->itemHeight = Size.cy / 2;

    lpMeasureItem->itemWidth = Size.cx + MENU_PIC_PLACE_SIZE + MENU_PIC_INDENT;
  }
}

void FileEventWnd::OnDrawItem(HWND hWnd, const DRAWITEMSTRUCT* lpDrawItem)
{
  if (lpDrawItem->CtlType == ODT_MENU)
  {
    std::wstring srtCaption(256, 0);
    int nSize = ::GetMenuString((HMENU)lpDrawItem->itemData, lpDrawItem->itemID, srtCaption.data(), srtCaption.size(), MF_BYCOMMAND);
    if (nSize)
      srtCaption.resize(nSize);
    else
      srtCaption.clear();

    int nImageIndex = -1;
    HIMAGELIST hImageList = (HIMAGELIST)::SendMessage(m_hToolBar, TB_GETIMAGELIST, 0, 0);
    if (hImageList && !srtCaption.empty())
    {
      if (ID_HELP_ABOUT == lpDrawItem->itemID)
      {
        nImageIndex = ABOUT_IMAGE_INDEX;
      }
      else
      {
        TBBUTTONINFO tbi = { 0 };
        tbi.cbSize = sizeof(tbi);
        tbi.dwMask = TBIF_IMAGE;
        if (::SendMessage(m_hToolBar, TB_GETBUTTONINFO, lpDrawItem->itemID, (LPARAM)&tbi) != -1)
          nImageIndex = tbi.iImage;
      }
    }

    RECT Rect;
    ::CopyRect(&Rect, &lpDrawItem->rcItem);

    if (!srtCaption.empty())
    {
      std::wstring strShortKey;
      size_t pos = srtCaption.find(L'\t');
      if (std::wstring::npos != pos)
      {
        strShortKey = srtCaption.substr(pos + 1);
        srtCaption = srtCaption.substr(0, pos);
      }

      if ((lpDrawItem->itemState & ODS_GRAYED) && (lpDrawItem->itemState & ODS_SELECTED))
      {
        ::FillRect(lpDrawItem->hDC, &Rect, ::GetSysColorBrush(COLOR_3DLIGHT));
      }
      else if (lpDrawItem->itemState & ODS_SELECTED)
      {
        ::FillRect(lpDrawItem->hDC, &Rect, ::GetSysColorBrush(COLOR_MENUHILIGHT));
      }
      else
      {
        ::FillRect(lpDrawItem->hDC, &Rect, ::GetSysColorBrush(COLOR_MENUBAR));
      }

      Rect.left += MENU_PIC_PLACE_SIZE;
      Rect.right -= MENU_PIC_PLACE_SIZE;
      ::OffsetRect(&Rect, MENU_PIC_INDENT, 0);

      ::SetBkMode(lpDrawItem->hDC, TRANSPARENT);

      if (lpDrawItem->itemState & ODS_GRAYED)
      {
        if (!(lpDrawItem->itemState & ODS_SELECTED))
        {
          ::SetTextColor(lpDrawItem->hDC, ::GetSysColor(COLOR_HIGHLIGHTTEXT));
          ::DrawText(lpDrawItem->hDC, srtCaption.c_str(), srtCaption.size(), &Rect, DT_LEFT | DT_VCENTER | DT_EXPANDTABS | DT_SINGLELINE);
          if (!strShortKey.empty())
            ::DrawText(lpDrawItem->hDC, strShortKey.c_str(), strShortKey.size(), &Rect, DT_RIGHT | DT_VCENTER | DT_EXPANDTABS | DT_SINGLELINE);
        }

        ::SetTextColor(lpDrawItem->hDC, ::GetSysColor(COLOR_GRAYTEXT));
        ::OffsetRect(&Rect, -1, -1);
        ::DrawText(lpDrawItem->hDC, srtCaption.c_str(), srtCaption.size(), &Rect, DT_LEFT | DT_VCENTER | DT_EXPANDTABS | DT_SINGLELINE);
        if (!strShortKey.empty())
          ::DrawText(lpDrawItem->hDC, strShortKey.c_str(), strShortKey.size(), &Rect, DT_RIGHT | DT_VCENTER | DT_EXPANDTABS | DT_SINGLELINE);
      }
      else
      {
        if (lpDrawItem->itemState & ODS_SELECTED)
          ::SetTextColor(lpDrawItem->hDC, ::GetSysColor(COLOR_HIGHLIGHTTEXT));

        ::DrawText(lpDrawItem->hDC, srtCaption.c_str(), srtCaption.size(), &Rect, DT_LEFT | DT_VCENTER | DT_EXPANDTABS | DT_SINGLELINE);
        if (!strShortKey.empty())
          ::DrawText(lpDrawItem->hDC, strShortKey.c_str(), strShortKey.size(), &Rect, DT_RIGHT | DT_VCENTER | DT_EXPANDTABS | DT_SINGLELINE);
      }

      if (hImageList && nImageIndex != -1)
      {
        ::ImageList_Draw(hImageList, nImageIndex, lpDrawItem->hDC,
          lpDrawItem->rcItem.left + (MENU_PIC_PLACE_SIZE - MENU_PIC_SIZE) / 2,
          lpDrawItem->rcItem.top + (MENU_PIC_PLACE_SIZE - MENU_PIC_SIZE) / 2, ILD_NORMAL /*| (lpDrawItem->itemState & ODS_GRAYED ? ILD_OVERLAYMASK : 0)*/);

        if (lpDrawItem->itemState & ODS_SELECTED)
        {
          RECT Rect;
          ::CopyRect(&Rect, &lpDrawItem->rcItem);
          Rect.right = Rect.left + MENU_PIC_PLACE_SIZE;
          Rect.bottom = Rect.top + MENU_PIC_PLACE_SIZE;
        }
      }
    }
    else
    {
      ::FillRect(lpDrawItem->hDC, &Rect, ::GetSysColorBrush(COLOR_MENU));
      Rect.bottom -= 3;
      ::DrawEdge(lpDrawItem->hDC, &Rect, EDGE_ETCHED, BF_BOTTOM);
    }
  }
}

void FileEventWnd::OnInitMenuPopup(HWND hWnd, HMENU hMenu, UINT item, bool fSystemMenu)
{
  if (!fSystemMenu)
  {
    HIMAGELIST hImageList = (HIMAGELIST)::SendMessage(m_hToolBar, TB_GETIMAGELIST, 0, 0);
    if (hImageList)
    {
      int nCount = ::GetMenuItemCount(hMenu);
      for (int i = 0; i < nCount; i++)
      {
        MENUITEMINFO mii = { 0 };
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_FTYPE | MIIM_DATA;
        ::GetMenuItemInfo(hMenu, i, true, &mii);

        if (mii.wID != ID_HELP_ABOUT && !mii.dwItemData)
        {
          mii.fMask = MIIM_FTYPE | MIIM_DATA;
          mii.fType |= MFT_OWNERDRAW;
          mii.dwItemData = (DWORD)hMenu;
        }
        ::SetMenuItemInfo(hMenu, i, true, &mii);
      }
    }
  }
}

void FileEventWnd::OnMonitorEvent(HWND hWnd, FileEventItem* pItem)
{
  m_spFileEventList->AddItem(pItem);
  m_bDirty = true;
}

void FileEventWnd::OnMonitorError(HWND hWnd, int nIDs, DWORD dwError)
{
  m_spFileEventMonitor->Stop();
  ChangeUIState();
  Utils::ShowOSError(nIDs, dwError);
}
