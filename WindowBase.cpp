#include "pch.h"
#include "FileEvent.h"
#include "FileEventApp.h"
#include "WindowBase.h"

WindowBase::WindowBase()
{

}

WindowBase::~WindowBase()
{
}

bool WindowBase::Create(DWORD dwExStyle, const std::wstring& strClassName, const std::wstring& strWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu)
{
  m_hWnd = ::CreateWindowEx(dwExStyle, strClassName.c_str(), strWindowName.c_str(), dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, ::GetApp()->GetInstance(), reinterpret_cast<LPVOID>(this));
  return m_hWnd != nullptr;
}

LRESULT WindowBase::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT WINAPI WindowBase::s_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  WindowBase* pWnd = nullptr;
  if (WM_NCCREATE == uMsg)
  {
    LPCREATESTRUCTA lpCS = reinterpret_cast<LPCREATESTRUCTA>(lParam);
    pWnd = reinterpret_cast<WindowBase*>(lpCS->lpCreateParams);
    _ASSERTE(pWnd);
    if (pWnd)
      pWnd->m_hWnd = hWnd;
    ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
  }
  else
  {
    pWnd = reinterpret_cast<WindowBase*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
  }

  if (pWnd)
    return pWnd->WndProc(hWnd, uMsg, wParam, lParam);

  return -1;
}
