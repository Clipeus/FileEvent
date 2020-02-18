#pragma once

#include "resource.h"
#include "Utils.h"

#ifndef UNICODE
#error UNICODE define required
#endif // !UNICODE

struct FileEventItem
{
  DWORD dwAction = 0;
  std::wstring strDirName;
  std::wstring strFileName;
  std::variant<UINT, std::wstring> varDescription;

  std::wstring GetDescription()
  {
    std::wstring strDescription = std::get_if<UINT>(&varDescription) != nullptr ? Utils::LoadString(std::get<UINT>(varDescription)) : std::get<std::wstring>(varDescription);
    if (std::get_if<UINT>(&varDescription) != nullptr && std::get<UINT>(varDescription) == IDS_UNKNOWN_ACTION)
      strDescription += std::to_wstring(dwAction);
    return strDescription;
  }
  std::wstring GetFullPath()
  {
    std::filesystem::path path = strDirName;
    path.append(strFileName);
    return path.wstring();
  }
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
