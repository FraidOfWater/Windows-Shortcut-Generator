#include <windows.h>
#include <shobjidl.h>    // For IShellLinkW, CLSID_ShellLink, IID_IShellLinkW
#include <shlobj.h>      // For SHParseDisplayName, SHGetFolderPathW (or SHGetKnownFolderPath)
#include <objbase.h>     // For COM functions and CoTaskMemFree
#include <strsafe.h>     // For StringCchPrintfW
#include <iostream>
#include <shlwapi.h>     // PathIsUNCW, etc.

#pragma comment(lib, "shlwapi.lib")

// Helper to convert a path to PIDL
HRESULT SetShortcutTargetByPIDL(IShellLinkW* pShellLink, const wchar_t* longTargetPath)
{
    if (!pShellLink || !longTargetPath)
        return E_INVALIDARG;

    LPITEMIDLIST pidl = nullptr;
    SFGAOF sfgao = 0;

    // SHParseDisplayName can parse "\\?\" long paths
    HRESULT hr = SHParseDisplayName(longTargetPath, NULL, &pidl, 0, &sfgao);
    if (SUCCEEDED(hr) && pidl != nullptr)
    {
        hr = pShellLink->SetIDList(pidl);  // Set the target using PIDL
        CoTaskMemFree(pidl);               // Always free the PIDL
    }
    else
    {
        std::wcerr << L"SHParseDisplayName failed for: " << longTargetPath
                   << L" (HRESULT: 0x" << std::hex << hr << L")" << std::endl;
    }

    return hr;
}

int wmain(int argc, wchar_t* argv[])
{
    if (argc < 5)
    {
        std::wcerr << L"Usage: " << argv[0] << L" <target_path> <working_directory> <shortcut_directory> <shortcut_name>" << std::endl;
        return 1;
    }

    const wchar_t* longTargetPath = argv[1];
    const wchar_t* workingDirectory = argv[2];
    const wchar_t* shortcutDirectory = argv[3];
    const wchar_t* shortcutName = argv[4];
    
    // Initialize COM
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
    {
        std::wcerr << L"CoInitializeEx failed: 0x" << std::hex << hr << std::endl;
        return 1;
    }

    IShellLinkW* pShellLink = nullptr;
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                          IID_IShellLinkW, reinterpret_cast<void**>(&pShellLink));
    if (FAILED(hr))
    {
        std::wcerr << L"CoCreateInstance failed: 0x" << std::hex << hr << std::endl;
        CoUninitialize();
        return 1;
    }

    // Use PIDL-based target setting
    hr = SetShortcutTargetByPIDL(pShellLink, longTargetPath);
    if (FAILED(hr))
    {
        std::wcerr << L"Failed to set target by PIDL. HRESULT: 0x" << std::hex << hr << std::endl;
        pShellLink->Release();
        CoUninitialize();
        return 1;
    }

    // Set working dir and optional metadata
    pShellLink->SetWorkingDirectory(workingDirectory);
    pShellLink->SetDescription(L"My Custom Long Path Shortcut");

    IPersistFile* pPersistFile = nullptr;
    hr = pShellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&pPersistFile));
    if (SUCCEEDED(hr))
    {
        wchar_t lnkPath[32768] = {0};  // Maximum Windows path length with \\?\ prefix
        hr = StringCchPrintfW(lnkPath, ARRAYSIZE(lnkPath), L"%s\\%s.lnk", shortcutDirectory, shortcutName);

        if (SUCCEEDED(hr))
        {
            hr = pPersistFile->Save(lnkPath, TRUE);
            if (SUCCEEDED(hr))
            {
                std::wcout << L"Shortcut successfully created at: " << lnkPath << std::endl;
            }
            else
            {
                std::wcerr << L"Failed to save shortcut. HRESULT: 0x" << std::hex << hr << std::endl;
            }
        }
        else
        {
            std::wcerr << L"StringCchPrintfW failed. HRESULT: 0x" << std::hex << hr << std::endl;
        }
        pPersistFile->Release();
    }
    else
    {
        std::wcerr << L"QueryInterface for IPersistFile failed: 0x" << std::hex << hr << std::endl;
    }

    pShellLink->Release();
    CoUninitialize();
    return 0;
}
