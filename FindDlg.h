#pragma once
#include "DialogBase.h"

class FindDlg : public DialogBase
{
public:
  enum struct Flags : uint8_t
  {
    FD_NONE = 0x00,
    FD_ACTION = 0x01,
    FD_PATH = 0x02,
    FD_ALLCOLUMN = 0x03,
    FD_WORDONLY = 0x04,
  };

public:
  FindDlg();

public:
  std::wstring GetFindText() const
  {
    return m_strFindText;
  }
  Flags GetFindFlags() const
  {
    return m_eFlags;
  }

private:
  bool DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  bool OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
  void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify);

private:
  std::wstring m_strFindText;
  Flags m_eFlags = Flags::FD_ALLCOLUMN;
};

inline FindDlg::Flags& operator |= (FindDlg::Flags& lhs, const FindDlg::Flags& rhs)
{
  return lhs = static_cast<FindDlg::Flags>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

inline uint8_t operator & (const FindDlg::Flags& lhs, const FindDlg::Flags& rhs)
{
  return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
}
