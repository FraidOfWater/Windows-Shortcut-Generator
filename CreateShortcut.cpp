#include <windows.h>
#include <shobjidl.h>    // For IShellLinkW, CLSID_ShellLink, IID_IShellLinkW
#include <shlobj.h>      // For SHParseDisplayName, SHGetFolderPathW (or SHGetKnownFolderPath)
#include <objbase.h>     // For COM functions and CoTaskMemFree
#include <strsafe.h>     // For StringCchPrintfW
#include <iostream>

// Helper function to set the shortcut's target using a PIDL.
HRESULT SetShortcutTargetByPIDL(IShellLinkW* pShellLink, const wchar_t* longTargetPath)
{
    if (!pShellLink || !longTargetPath)
        return E_INVALIDARG;

    LPITEMIDLIST pidl = nullptr;
    SFGAOF sfgao = 0;

    // Convert the long (possibly "\\?\"-prefixed) path to a PIDL.
    HRESULT hr = SHParseDisplayName(longTargetPath, NULL, &pidl, 0, &sfgao);
    if (SUCCEEDED(hr) && pidl != nullptr)
    {
        // Set the PIDL on the shell link.
        hr = pShellLink->SetIDList(pidl);
        // Always free the PIDL allocated by SHParseDisplayName.
        CoTaskMemFree(pidl);
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

    // Initialize COM library.
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        std::wcerr << L"CoInitialize failed: 0x" << std::hex << hr << std::endl;
        return 1;
    }

    // Get command-line arguments
    const wchar_t* longTargetPath = argv[1];
    const wchar_t* workingDirectory = argv[2];
    const wchar_t* shortcutDirectory = argv[3];
    const wchar_t* shortcutName = argv[4];
    

    // Create an instance of IShellLinkW.
    IShellLinkW* pShellLink = nullptr;
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                          IID_IShellLinkW, reinterpret_cast<void**>(&pShellLink));
    if (FAILED(hr))
    {
        std::wcerr << L"CoCreateInstance failed: 0x" << std::hex << hr << std::endl;
        CoUninitialize();
        return 1;
    }

    // Set the target using the PIDL approach.
    hr = SetShortcutTargetByPIDL(pShellLink, longTargetPath);
    if (FAILED(hr))
    {
        std::wcerr << L"SetShortcutTargetByPIDL failed: 0x" << std::hex << hr << std::endl;
        pShellLink->Release();
        CoUninitialize();
        return 1;
    }

    // Set the working directory
    pShellLink->SetWorkingDirectory(workingDirectory);
    // Set a description for the shortcut
    pShellLink->SetDescription(L"My Custom Long Path Shortcut");

    // Obtain the IPersistFile interface to save the shortcut.
    IPersistFile* pPersistFile = nullptr;
    hr = pShellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&pPersistFile));
    if (SUCCEEDED(hr))
    {
        wchar_t lnkPath[MAX_PATH] = {0};
        // Combine the shortcut directory and shortcut name
        hr = StringCchPrintfW(lnkPath, MAX_PATH, L"%s\\%s.lnk", shortcutDirectory, shortcutName);
        if (SUCCEEDED(hr))
        {
            // Save the shortcut
            hr = pPersistFile->Save(lnkPath, TRUE);
            if (SUCCEEDED(hr))
            {
                std::wcout << L"Shortcut created successfully at: " << lnkPath << std::endl;
            }
            else
            {
                std::wcerr << L"IPersistFile::Save failed: 0x" << std::hex << hr << std::endl;
            }
        }
        else
        {
            std::wcerr << L"StringCchPrintfW failed: 0x" << std::hex << hr << std::endl;
        }
        pPersistFile->Release();
    }
    else
    {
        std::wcerr << L"QueryInterface for IPersistFile failed: 0x" << std::hex << hr << std::endl;
    }

    // Release our IShellLinkW pointer and uninitialize COM.
    pShellLink->Release();
    CoUninitialize();
    return 0;
}
