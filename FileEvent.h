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
  std::wstring strDescription;
};

__interface IFileEventNotify
{
  void OnEvent(FileEventItem* pItem);
  void OnError(int nIDs, DWORD dwError);
};
