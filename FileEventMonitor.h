#pragma once

class FileEventMonitor
{
public:
  struct Options
  {
    std::wstring strPath;
    bool bWatchSubtree = true;
    DWORD dwNotifyFilter = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
      FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY;
  };

public:
  FileEventMonitor();
  ~FileEventMonitor();

public:
  bool Start(IFileEventNotify* pFileEventNotify, const Options* pOptions);
  bool Stop();

  void Suspend();
  void Resume();

  bool IsSuspended() const
  {
    return m_bStarted && m_bSuspended;
  }

  bool IsStarted() const
  {
    return m_bStarted;
  }

private:
  void Process();

private:
  std::atomic_bool m_bStarted = false;
  bool m_bSuspended = false;
  IFileEventNotify* m_pFileEventNotify = nullptr;
  std::unique_ptr<std::thread> m_pMonitorThread;
  Utils::unique_handle m_hExitEvent = Utils::make_unique_handle();
  Utils::unique_handle m_hSuspendResumeEvent = Utils::make_unique_handle();
  Options m_MonOptions;
};

