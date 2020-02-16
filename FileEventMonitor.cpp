#include "pch.h"
#include "FileEvent.h"
#include "FileEventApp.h"
#include "Utils.h"
#include "FileEventMonitor.h"

FileEventMonitor::FileEventMonitor()
{

}

FileEventMonitor::~FileEventMonitor()
{

}

bool FileEventMonitor::Start(IFileEventNotify* pFileEventNotify, const Options* pOptions)
{
  Stop();

  m_hExitEvent = Utils::make_unique_handle(::CreateEvent(nullptr, true, false, nullptr));
  if (!m_hExitEvent)
    return false;

  m_hSuspendResumeEvent = Utils::make_unique_handle(::CreateEvent(nullptr, false, false, nullptr));
  if (!m_hSuspendResumeEvent)
    return false;

  m_pFileEventNotify = pFileEventNotify;
  m_MonOptions = *pOptions;
  m_pMonitorThread = std::make_unique<std::thread>(&FileEventMonitor::Process, this);

  return true;
}

bool FileEventMonitor::Stop()
{
  if (m_bStarted)
  {
    ::SetEvent(m_hExitEvent.get());
    HANDLE hThread = reinterpret_cast<HANDLE>(m_pMonitorThread->native_handle());
    Utils::WaitWithMessageLoop(hThread);
  }

  if (m_pMonitorThread)
    m_pMonitorThread->join();

  m_pMonitorThread.reset();
  m_hExitEvent.reset();
  m_hSuspendResumeEvent.reset();

  return true;
}

void FileEventMonitor::Suspend()
{
  _ASSERTE(!m_bSuspended);
  _ASSERTE(m_hSuspendResumeEvent.get());

  if (m_hSuspendResumeEvent)
    ::SetEvent(m_hSuspendResumeEvent.get());
}

void FileEventMonitor::Resume()
{
  _ASSERTE(m_bSuspended);
  _ASSERTE(m_hSuspendResumeEvent.get());

  if (m_hSuspendResumeEvent)
    ::SetEvent(m_hSuspendResumeEvent.get());
}

void FileEventMonitor::Process()
{
  m_bStarted = true;
  m_pFileEventNotify->OnState(FileEventState::Started, m_MonOptions.strPath.c_str());

  Utils::unique_handle hDir = Utils::make_unique_handle(::CreateFile(m_MonOptions.strPath.c_str(),
    GENERIC_READ | FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr));

  if (hDir.get() == INVALID_HANDLE_VALUE)
  {
    m_pFileEventNotify->OnError(IDS_OPEN_DIR_FAILED, ::GetLastError());
    return;
  }

  Utils::unique_handle hEvent = Utils::make_unique_handle(::CreateEvent(nullptr, false, false, nullptr));
  OVERLAPPED Overlapped;
  Overlapped.OffsetHigh = 0;
  Overlapped.hEvent = hEvent.get();

  std::array<HANDLE, 3> arrEvents;
  arrEvents[0] = m_hExitEvent.get();
  arrEvents[1] = m_hSuspendResumeEvent.get();
  arrEvents[2] = Overlapped.hEvent;

  bool result = true;
  while (result)
  {
    std::vector<uint8_t> vecBuf;
    vecBuf.resize(2048);
    DWORD dwRet = 0;

    result = ::ReadDirectoryChangesW(hDir.get(), vecBuf.data(), vecBuf.size(), m_MonOptions.bWatchSubtree, m_MonOptions.dwNotifyFilter, &dwRet, &Overlapped, nullptr);

    if (!result)
    {
      m_pFileEventNotify->OnError(IDS_MONITOR_FAILED, ::GetLastError());
      break;
    }

    DWORD dwRetCode = ::WaitForMultipleObjects(arrEvents.size(), arrEvents.data(), false, INFINITE);
    if (dwRetCode == WAIT_OBJECT_0)
    {
      break;
    }
    else if (dwRetCode == WAIT_OBJECT_0 + 1)
    {
      m_bSuspended = true;
      m_pFileEventNotify->OnState(FileEventState::Suspened, m_MonOptions.strPath.c_str());

      dwRetCode = ::WaitForMultipleObjects(arrEvents.size() - 1, arrEvents.data(), false, INFINITE);
      if (dwRetCode == WAIT_OBJECT_0)
        break;

      m_bSuspended = false;
      m_pFileEventNotify->OnState(FileEventState::Started, m_MonOptions.strPath.c_str());
    }
    else if (dwRetCode == WAIT_OBJECT_0 + 2)
    {
      FILE_NOTIFY_INFORMATION* pNotify;
      int offset = 0;

      do
      {
        pNotify = (FILE_NOTIFY_INFORMATION*)&vecBuf[offset];

        FileEventItem* pItem = new FileEventItem;
        pItem->strDirName = m_MonOptions.strPath;
        pItem->strFileName.assign(pNotify->FileName, pNotify->FileName + pNotify->FileNameLength);
        pItem->dwAction = pNotify->Action;

        switch (pNotify->Action)
        {
          case FILE_ACTION_ADDED:
            pItem->strDescription = Utils::LoadString(IDS_FILE_ACTION_ADDED);
            break;
          case FILE_ACTION_REMOVED:
            pItem->strDescription = Utils::LoadString(IDS_FILE_ACTION_REMOVED);
            break;
          case FILE_ACTION_MODIFIED:
            pItem->strDescription = Utils::LoadString(IDS_FILE_ACTION_MODIFIED);
            break;
          case FILE_ACTION_RENAMED_OLD_NAME:
            pItem->strDescription = Utils::LoadString(IDS_FILE_ACTION_RENAMED_OLD_NAME);
            break;
          case FILE_ACTION_RENAMED_NEW_NAME:
            pItem->strDescription = Utils::LoadString(IDS_FILE_ACTION_RENAMED_NEW_NAME);
            break;
          default:
            pItem->strDescription = Utils::LoadString(IDS_UNKNOWN_ACTION);
            pItem->strDescription += std::to_wstring(pNotify->Action);
            break;
        }

        m_pFileEventNotify->OnEvent(pItem);

        offset += pNotify->NextEntryOffset;
      }
      while (pNotify->NextEntryOffset); //(offset != 0);
    }
    else
    {
      _RPTF0(_CRT_WARN, "Waiting failed\n");
    }
  }

  m_bSuspended = false;
  m_bStarted = false;
  m_pFileEventNotify->OnState(FileEventState::Stoped, m_MonOptions.strPath.c_str());

  _RPTF0(_CRT_WARN, "Exit Monitor Process\n");
}
