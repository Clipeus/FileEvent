#pragma once

#include "resource.h"

#ifndef UNICODE
#error UNICODE define required
#endif // !UNICODE

struct FileEventItem
{
  DWORD dwAction = 0;
  std::wstring strDirName;
  std::wstring strFileName;
  std::variant<UINT, std::wstring> varDescription;
};

enum class FileEventState : uint8_t
{
  Stoped,
  Started,
  Suspened
};

__interface IFileEventNotify
{
  void OnState(FileEventState eState, LPCWSTR lpszPath);
  void OnEvent(FileEventItem* pItem);
  void OnError(int nIDs, DWORD dwError);
};
