#include "pch.h"
#include "FileEvent.h"
#include "FileEventApp.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
  std::unique_ptr<FileEventApp> app(::GetApp());

  if (!app->Init(hInstance, lpCmdLine))
    return 1;

  return app->Run(nCmdShow);
}
