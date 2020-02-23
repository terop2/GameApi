#include <stdio.h>
#include <unknwn.h>

int WinMain(HINSTANCE h, HINSTANCE h2, LPSTR line, int n)
{
  int argc=0;
  char *argv[] = { "main" };

    GUID guid;
    wchar_t wzGuid[39] = { 0 };
    int count = (1 < argc) ? _wtoi(argv[1]) : 1;

    for (int i = 0; i < count; ++i) 
    {
        if (SUCCEEDED(CoCreateGuid(&guid)) && StringFromGUID2(&guid, wzGuid, sizeof(wzGuid) / sizeof(wzGuid[0])))
        {
            _putws(wzGuid);
        }
    }
}
