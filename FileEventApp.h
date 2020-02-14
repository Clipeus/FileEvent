#pragma once

class FileEventWnd;

class FileEventApp
{
  FileEventApp();

public:
  ~FileEventApp();
  static FileEventApp* GetApp();

public:
  bool Init(HINSTANCE hInstance, const std::wstring& strCmdLine);
  int Run(int nCmdShow);

public:
  HINSTANCE GetInstance() const
  {
    return m_hInstance;
  }
  FileEventWnd* GetMainWnd() const
  {
    return m_pMainWnd.get();
  }
  std::wstring GetAppName() const
  {
    return m_strAppName;
  }
  std::wstring GetRegistryRoot() const
  {
    return L"SOFTWARE\\LMSoft\\File Event Monitor";
  }
  bool IsWaitCursor() const
  {
    return m_bWaitCursor;
  }
  void SetWaitCursor(bool bWait)
  {
    m_bWaitCursor = bWait;
  }
  HCURSOR GetWaitCursor() const
  {
    return m_hWaitCursor;
  }
  HCURSOR GetStdCursor() const
  {
    return m_hStdCursor;
  }

private:
  static FileEventApp* s_pFileEventApp;
  static std::once_flag s_flag;

  std::wstring m_strAppName;
  HINSTANCE m_hInstance = nullptr;
  std::unique_ptr<FileEventWnd> m_pMainWnd;
  bool m_bWaitCursor = false;
  HCURSOR m_hWaitCursor = nullptr;
  HCURSOR m_hStdCursor = nullptr;
};

