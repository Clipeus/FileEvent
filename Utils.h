#pragma once
#include <crtdbg.h>

namespace Utils
{
// The normal HANDLE_MSG macro in WINDOWSX.H does not work properly for dialog
// boxes because DlgProc's return a bool instead of an LRESULT (like
// WndProcs). This HANDLE_DLGMSG macro corrects the problem:
#define HANDLE_DLGMSG(hWnd, message, fn)                          \
		 case (message): return (SetDlgMsgResult(hWnd, uMsg,               \
				HANDLE_##message((hWnd), (wParam), (lParam), (fn))))

#define UNIQUE_VALUE(NAME, TYPE, DELETER) \
	using unique_##NAME = std::unique_ptr<std::remove_pointer<TYPE>::type, decltype(&DELETER)>; \
	inline unique_##NAME make_unique_##NAME(TYPE value = nullptr) \
	{ \
			return unique_##NAME(value, DELETER); \
	}

#define UNIQUE_PTR(NAME, PTR, DELETER) \
	using unique_##NAME = std::unique_ptr<PTR, decltype(&DELETER)>; \
	inline unique_##NAME make_unique_##NAME(PTR* ptr = nullptr) \
	{ \
			return unique_##NAME(ptr, DELETER); \
	}

inline void CloseHandleAndSaveLastError(HANDLE hObject)
{
  DWORD le = ::GetLastError();
  ::CloseHandle(hObject);
  ::SetLastError(le);
}

UNIQUE_VALUE(handle, HANDLE, CloseHandleAndSaveLastError)

inline void RegCloseKeyAndSaveLastError(HKEY hKey)
{
  DWORD le = ::GetLastError();
  ::RegCloseKey(hKey);
  ::SetLastError(le);
}

UNIQUE_VALUE(regkey, HKEY, RegCloseKeyAndSaveLastError)

void SetGlobalHandlers();

bool GetFixedInfo(VS_FIXEDFILEINFO& rFixedInfo);
std::wstring LoadString(UINT uID);
std::wstring StrFormat(LPCWSTR lpszFormat, ...);

int ShowOSError(UINT uMesID = IDS_SYSTEM_ERROR, DWORD dwError = 0, int nMode = MB_OK | MB_ICONERROR);
int ReportBox(LPCWSTR lpszMessage, int nMode = MB_OK);
int ReportBox(UINT uMesID, int nMode = MB_OK);

inline void ScreenToClient(HWND hParentWnd, LPRECT lpRect)
{
  ScreenToClient(hParentWnd, (LPPOINT)lpRect);
  ScreenToClient(hParentWnd, ((LPPOINT)lpRect) + 1);
}

bool WaitWithMessageLoop(HANDLE hEvent);
bool IsWow64Process();

class WaitCursor
{
public:
  WaitCursor();
  ~WaitCursor();

protected:
  HCURSOR m_hPrevCursor = nullptr;
};

}