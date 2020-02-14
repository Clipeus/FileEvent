#include "pch.h"
#include "FileEvent.h"
#include "FileEventApp.h"
#include "FileEventWnd.h"
#include "Utils.h"
#include <new.h>
#include <eh.h>

namespace Utils
{

int ReportBox(LPCWSTR lpszMessage, int nMode /*= MB_OK*/)
{
  return MessageBox(FileEventApp::GetApp()->GetMainWnd()->GetHandle(), lpszMessage, FileEventApp::GetApp()->GetAppName().c_str(), nMode);
}

int ReportBox(UINT uMesID, int nMode /*= MB_OK*/)
{
  return ReportBox(LoadString(uMesID).c_str(), nMode);
}

int ShowOSError(UINT uMesID /*= IDS_SYSTEM_ERROR*/, DWORD dwError /*= 0*/, int nMode /*= MB_OK|MB_ICONERROR*/)
{
  LPTSTR szErrorText = nullptr;

  if (!dwError)
    dwError = ::GetLastError();

  if (dwError)
    ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&szErrorText), 0, nullptr);

  std::wstring strText = LoadString(uMesID);

  if (szErrorText)
  {
    strText += szErrorText;
    LocalFree(szErrorText);
  }

  return ReportBox(strText.c_str(), nMode);
}

std::wstring LoadString(UINT uID)
{
  std::wstring result;
  LPWSTR lpszText = nullptr;

  int nLen = ::LoadString(FileEventApp::GetApp()->GetInstance(), uID, reinterpret_cast<LPWSTR>(&lpszText), 0);

  if (lpszText && nLen > 0)
    result.assign(lpszText, nLen);

  return result;
}

std::wstring StrFormat(LPCWSTR lpszFormat, ...)
{
  va_list args;
  va_start(args, lpszFormat);

  size_t size = _vscwprintf(lpszFormat, args);

  std::wstring str;
  str.resize(size);

  vswprintf_s(&str.front(), size + 1, lpszFormat, args);

  va_end(args);

  return str;
}

bool GetFixedInfo(VS_FIXEDFILEINFO& rFixedInfo)
{
  // get the filename of the executable containing the version resource
  TCHAR szFilename[MAX_PATH] = { 0 };
  if (GetModuleFileName(nullptr, szFilename, MAX_PATH) == 0)
  {
    _RPTFN(_CRT_ERROR, "GetModuleFileName failed with error %d\n", GetLastError());
    return false;
  }

  // allocate a block of memory for the version info
  DWORD dummy;
  DWORD dwSize = GetFileVersionInfoSize(szFilename, &dummy);
  if (dwSize == 0)
  {
    _RPTFN(_CRT_ERROR, "GetFileVersionInfoSize failed with error %d\n", GetLastError());
    return false;
  }
  std::vector<BYTE> data;
  data.resize(dwSize);

  // load the version info
  if (!GetFileVersionInfo(szFilename, 0, dwSize, data.data()))
  {
    _RPTFN(_CRT_ERROR, "GetFileVersionInfo failed with error %d\n", GetLastError());
    return false;
  }

  VS_FIXEDFILEINFO* pFixedInfo;
  UINT uiVerLen = 0;
  if (VerQueryValue(data.data(), TEXT("\\"), (void**)&pFixedInfo, (UINT*)&uiVerLen) == 0)
  {
    _RPTFN(_CRT_ERROR, "VerQueryValue failed with error %d\n", GetLastError());
    return false;
  }

  rFixedInfo = *pFixedInfo;
  return true;
}

bool WaitWithMessageLoop(HANDLE hEvent)
{
  DWORD dwRet;
  MSG msg;

  while (true)
  {
    dwRet = ::MsgWaitForMultipleObjects(1, &hEvent, FALSE, INFINITE, QS_ALLINPUT);

    if (dwRet == WAIT_OBJECT_0)
      return TRUE; // The event was signaled

    if (dwRet != WAIT_OBJECT_0 + 1)
      break; // Something else happened

    // There is one or more window message available. Dispatch them
    while (::PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
    {
      if (::GetMessage(&msg, NULL, 0, 0) > 0)
      {
        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
      }

      if (::WaitForSingleObject(hEvent, 0) == WAIT_OBJECT_0)
        return true; // Event is now signaled.
    }
  }
  return false;
}

bool IsWow64Process()
{
  typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
  static LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(::GetModuleHandleA("kernel32"), "IsWow64Process");

  BOOL bIsWow64 = FALSE;

  if (nullptr != fnIsWow64Process)
  {
    if (!fnIsWow64Process(::GetCurrentProcess(), &bIsWow64))
    {
      return false;
    }
  }
  return bIsWow64 != FALSE;
}

WaitCursor::WaitCursor()
{
  m_hPrevCursor = SetCursor(FileEventApp::GetApp()->GetWaitCursor());
  FileEventApp::GetApp()->SetWaitCursor(true);
}

WaitCursor::~WaitCursor()
{
  SetCursor(m_hPrevCursor ? m_hPrevCursor : FileEventApp::GetApp()->GetStdCursor());
  FileEventApp::GetApp()->SetWaitCursor(false);
}

LONG WINAPI UnExFilter(EXCEPTION_POINTERS* pExceptionInfo)
{
  FatalAppExit(0, LoadString(IDS_UNEXCEPTION).c_str());
  return EXCEPTION_CONTINUE_SEARCH;
}

void _thread_se_translator(unsigned int nCode, EXCEPTION_POINTERS* pException)
{
  _RPTF1(_CRT_ERROR, "SEH -> Exception code : %X\n", pException->ExceptionRecord->ExceptionCode);
  throw(pException);
}

int _handle_out_of_memory(size_t nSize)
{
  _RPTF1(_CRT_ERROR, "NEW -> Out of memory for %d bytes\n", nSize);
  if (IDCANCEL == ReportBox(IDS_OUT_OF_MEM, MB_RETRYCANCEL | MB_TASKMODAL | MB_ICONSTOP))
    return 0;
  return 1;
}

void SetGlobalHandlers()
{
  SetUnhandledExceptionFilter(UnExFilter);	// For Win32 SEH
  _set_se_translator(_thread_se_translator);	// For C++ try/catch
  _set_new_handler(_handle_out_of_memory);
  _set_new_mode(1);
}

}