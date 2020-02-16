#pragma once

#include "WindowBase.h"

class FileEventList;
class FileEventMonitor;
class FindDlg;

class FileEventWnd : public WindowBase, public IFileEventNotify
{
  enum class FindMode : uint8_t
  {
    Find,
    FindNext,
    FindPrev
  };

public:
  enum class SelectFileMode : uint8_t
  {
    OpenFile,
    SaveFile,
    SelectFolder
  };

public:
  FileEventWnd();
  ~FileEventWnd();

public:
  bool Init();
  bool Create(int nCmdShow);

public:
  static LPCWSTR GetClassName();
  std::wstring SelectFile(SelectFileMode enSelectFileMode) const;

public:
  //IFileEventNotify
  void OnEvent(FileEventItem* pItem);
  void OnError(int nIDs, DWORD dwError);

private:
  void ChangeUIState(HMENU hContextMenu = nullptr);
  void UpdateStatusBarText();
  void SaveFile(const std::wstring& path);
  void OpenFile(const std::wstring& path);
  void CopyToClipboard();
  bool OnFind(FindMode enFindMode);

private:
  LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  void OnDestroy(HWND hWnd);
  bool OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
  void OnSize(HWND hWnd, UINT state, int cx, int cy);
  void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify);
  void OnMenuSelect(HWND hWnd, HMENU hMenu, int item, HMENU hMenuPopup, UINT flags);
  void OnContextMenu(HWND hWnd, HWND hwndContext, UINT x, UINT y);
  LRESULT OnNotify(HWND hWnd, int idFrom, NMHDR* pNmndr);
  bool OnSetCursor(HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg);
  void OnActivate(HWND hWnd, UINT nState, HWND hWndActDeact, bool fMinimized);
  void OnMeasureItem(HWND hWnd, MEASUREITEMSTRUCT* lpMeasureItem);
  void OnDrawItem(HWND hWnd, const DRAWITEMSTRUCT* lpDrawItem);
  void OnInitMenuPopup(HWND hWnd, HMENU hMenu, UINT item, bool fSystemMenu);
  void OnMonitorEvent(HWND hWnd, FileEventItem* pItem);
  void OnMonitorError(HWND hWnd, int nIDs, DWORD dwError);

private:
  bool m_bComInited = false;
  bool m_bDirty = false;
  std::wstring m_strMonitorPath;
  std::wstring m_strSavedPath;
  HIMAGELIST m_hImageList = nullptr;
  HWND m_hToolBar = nullptr;
  HWND m_hStatusBar = nullptr;
  std::unique_ptr<FileEventList> m_spFileEventList;
  std::unique_ptr<FileEventMonitor> m_spFileEventMonitor;
  std::unique_ptr<FindDlg> m_pFindDlg;
};

