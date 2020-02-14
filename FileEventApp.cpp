#include "pch.h"
#include "FileEvent.h"
#include "FileEventApp.h"
#include "FileEventWnd.h"
#include "Utils.h"

FileEventApp* FileEventApp::s_pFileEventApp = nullptr;
std::once_flag FileEventApp::s_flag;

FileEventApp::FileEventApp()
{

}

FileEventApp::~FileEventApp()
{

}
FileEventApp* FileEventApp::GetApp()
{
  if (s_pFileEventApp == nullptr)
  {
    std::call_once(s_flag, []()
    {
      s_pFileEventApp = new FileEventApp;
    });
  }

  return s_pFileEventApp;
}

bool FileEventApp::Init(HINSTANCE hInstance, const std::wstring& strCmdLine)
{
  Utils::SetGlobalHandlers();

  HWND hWnd = FindWindow(FileEventWnd::GetClassName(), nullptr);
  if (hWnd)
  {
    if (::IsIconic(hWnd))
      ::ShowWindow(hWnd, SW_RESTORE);
    ::SetForegroundWindow(hWnd);
    return false;
  }

  m_hInstance = hInstance;

  INITCOMMONCONTROLSEX iccs = { 0 };
  iccs.dwSize = sizeof(iccs);
  iccs.dwICC = ICC_WIN95_CLASSES;
  ::InitCommonControlsEx(&iccs);

  m_strAppName = Utils::LoadString(IDS_APP_TITLE);

  m_hWaitCursor = ::LoadCursor(nullptr, IDC_WAIT);
  m_hStdCursor = ::LoadCursor(nullptr, IDC_ARROW);

  m_pMainWnd = std::make_unique<FileEventWnd>();
  return m_pMainWnd->Init();
}

int FileEventApp::Run(int nCmdShow)
{
  if (!m_pMainWnd->Create(nCmdShow))
    return 1;

  HACCEL hAccel = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_MAIN_ACCEL));
  if (!hAccel)
  {
    Utils::ShowOSError();
    return 1;
  }

  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0))
  {
    if (!TranslateAccelerator(m_pMainWnd->GetHandle(), hAccel, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return msg.wParam;
}
