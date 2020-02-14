#pragma once

class WindowBase
{
public:
  WindowBase();
  ~WindowBase();

public:
  bool Create(DWORD dwExStyle, const std::wstring& strClassName, const std::wstring& strWindowName = L"", DWORD dwStyle = WS_OVERLAPPEDWINDOW,
    int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT, HWND hWndParent = nullptr, HMENU hMenu = nullptr);

  HWND GetHandle() const
  {
    return m_hWnd;
  }
  operator HWND() const
  {
    return GetHandle();
  }

protected:
  virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  static LRESULT WINAPI s_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
  HWND m_hWnd = nullptr;
};

