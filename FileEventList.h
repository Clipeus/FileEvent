#pragma once
#include "WindowBase.h"

class FileEventList : public WindowBase
{
public:
  FileEventList();
  ~FileEventList();

public:
  bool Init();
  bool AddItem(FileEventItem* pItem);
  FileEventItem* GetItem(int nIndex = -1);
  FileEventItem* GetCurrentItem();
  FileEventItem* GetNextItem(int& nIndex, bool bSelectedOnly, bool bAbove = false);
  void SelectAll();
  void Clear();
  void AdjustRect(int x, int y, int cx, int cy, bool bMove = true);
};

