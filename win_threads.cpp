#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <list>
#include <string>

DWORD WINAPI thread_copy_file(PVOID filenames)
{
    std::wstring* names =(std::wstring*)filenames;
    wprintf(L"Copy from %s to %s\n", names[0].c_str(), names[1].c_str());
    BOOL result = CopyFile(names[0].c_str(), names[1].c_str(), FALSE);
    if (result)
        return 0;
    else
        return -1;
}

int main()
{
    std::wstring work_dir = L"work_dir";
    std::wstring dest = L"dest";

    std::list<std::wstring> files;
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile((work_dir+L"/*").c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                files.push_back(fd.cFileName);
            }
        } while (FindNextFile(hFind, &fd));
        FindClose(hFind);
    }

    size_t count = files.size();
    DWORD* dwThreadID = new DWORD[count];
    HANDLE* hThread = new HANDLE[count];

    auto it = files.begin();
    std::wstring** params = new std::wstring*[count];
    for (int i=0; it != files.end(); it++, i++)
    {
        std::wstring file = *it;
        wprintf(L"%s - Create thread\n", file.c_str());
        params[i] = new std::wstring[2];
        params[i][0] = (work_dir + L"/" + file);
        params[i][1] = (dest + L"/" + file);
        hThread[i] = CreateThread(NULL, 0, thread_copy_file, (PVOID)params[i], 0, &dwThreadID[i]);

        if (!hThread)
            wprintf(L"Couldn't create thread\n");
    }

    WaitForMultipleObjects(count, hThread, TRUE, INFINITE);
    printf("Success!\n");
}