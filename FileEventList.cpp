#include "pch.h"
#include "FileEvent.h"
#include "FileEventApp.h"
#include "FileEventList.h"
#include "Utils.h"

const int COLUMN_WIDTH = 425;

FileEventList::FileEventList()
{

}

FileEventList::~FileEventList()
{

}

bool FileEventList::Init()
{
  ::SendMessage(m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

  std::wstring str;
  LV_COLUMN EventList;

  EventList.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
  EventList.fmt = LVCFMT_LEFT;
  EventList.cx = COLUMN_WIDTH;
  EventList.cchTextMax = 255;
  str = Utils::LoadString(IDS_ACTION_COLUMN);
  EventList.pszText = &str.front();
  EventList.iSubItem = -1;
  if (-1 == ListView_InsertColumn(m_hWnd, 0, &EventList))
    return false;

  EventList.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
  EventList.fmt = LVCFMT_LEFT;
  EventList.cchTextMax = 255;
  str = Utils::LoadString(IDS_FILENAME_COLUMN);
  EventList.cx = COLUMN_WIDTH;
  EventList.pszText = &str.front();
  if (-1 == ListView_InsertColumn(m_hWnd, 1, &EventList))
    return false;

  return true;
}

bool FileEventList::AddItem(FileEventItem* pItem)
{
  LVITEM lvItem;

  lvItem.mask = LVIF_TEXT | LVIF_PARAM;
  lvItem.pszText = &pItem->strDescription.front();
  lvItem.cchTextMax = 255;//pItem->strDescription.size();
  lvItem.iSubItem = 0;
  lvItem.iItem = ListView_GetItemCount(m_hWnd);
  lvItem.lParam = (LONG_PTR)pItem;
  if (-1 == ListView_InsertItem(m_hWnd, &lvItem))
    return false;

  std::filesystem::path path = pItem->strDirName;
  path.append(pItem->strFileName);
  std::wstring str = path.wstring();
  lvItem.mask = LVIF_TEXT;
  lvItem.pszText = &str.front();
  lvItem.cchTextMax = str.size();
  lvItem.iSubItem = 1;
  if (!ListView_SetItem(m_hWnd, &lvItem))
    return false;

  return true;
}

FileEventItem* FileEventList::GetItem(int nIndex)
{
  LV_ITEM item = { 0 };
  item.mask = LVIF_PARAM;
  item.iItem = nIndex;
  ListView_GetItem(m_hWnd, &item);
  return reinterpret_cast<FileEventItem*>(item.lParam);
}

FileEventItem* FileEventList::GetCurrentItem()
{
  int nIndex = ListView_GetNextItem(m_hWnd, -1, LVNI_SELECTED | LVNI_FOCUSED);
  if (nIndex == -1)
    return nullptr;

  return GetItem(nIndex);
}

FileEventItem* FileEventList::GetNextItem(int& nIndex, bool bSelectedOnly, bool bAbove)
{
  nIndex = ListView_GetNextItem(m_hWnd, nIndex, (bSelectedOnly ? LVNI_SELECTED : 0) | (nIndex != -1 ? (bAbove ? LVNI_ABOVE : LVNI_BELOW) : 0));
  if (nIndex == -1)
    return nullptr;

  return GetItem(nIndex);
}

void FileEventList::SelectAll()
{
  int nCount = ListView_GetItemCount(m_hWnd);
  for (int i = 0; i < nCount; i++)
  {
    ListView_SetItemState(m_hWnd, i, LVIS_SELECTED, LVIS_SELECTED);
  }
}

void FileEventList::Clear()
{
  ListView_DeleteAllItems(m_hWnd);
}

void FileEventList::AdjustRect(int x, int y, int cx, int cy, bool bMove /*= true*/)
{
  if (bMove)
    ::MoveWindow(m_hWnd, x, y, cx, cy, true);

  LV_COLUMN EventList;

  EventList.mask = LVCF_WIDTH;
  ListView_GetColumn(m_hWnd, 0, &EventList);

  if (cx > (EventList.cx + COLUMN_WIDTH))
    EventList.cx = cx - EventList.cx - 4;
  else
    EventList.cx = COLUMN_WIDTH - 4;

  ListView_SetColumn(m_hWnd, 1, &EventList);
}
