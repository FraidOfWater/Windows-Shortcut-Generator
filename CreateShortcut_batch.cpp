#include <windows.h>
#include <shobjidl.h>    // For IShellLinkW, CLSID_ShellLink, IID_IShellLinkW
#include <shlobj.h>      // For SHParseDisplayName, SHGetFolderPathW (or SHGetKnownFolderPath)
#include <objbase.h>     // For COM functions and CoTaskMemFree
#include <strsafe.h>     // For StringCchPrintfW
#include <iostream>
#include <shlwapi.h>     // PathIsUNCW, etc.

#pragma comment(lib, "shlwapi.lib")
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <locale>
#include <codecvt>

// Helper function to set the shortcut's target using a PIDL.
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

// Helper to parse a quoted line into 4 strings
bool ParseLine(const std::wstring& line, std::vector<std::wstring>& outArgs)
{
    std::wistringstream stream(line);
    std::wstring arg;
    bool inQuotes = false;
    wchar_t ch;
    std::wstring current;

    while (stream.get(ch))
    {
        if (ch == L'"')
        {
            inQuotes = !inQuotes;
            if (!inQuotes && !current.empty())
            {
                outArgs.push_back(current);
                current.clear();
            }
        }
        else if (inQuotes)
        {
            current += ch;
        }
    }

    return outArgs.size() == 4;
}

int wmain()
{
    SetConsoleOutputCP(CP_UTF8);
    std::ios_base::sync_with_stdio(false);
    std::wcout.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));

    std::wifstream inputFile("arguments.txt");
    std::ios_base::sync_with_stdio(false);
    inputFile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));

    if (!inputFile)
    {
        std::wcerr << L"Failed to open arguments.txt" << std::endl;
        return 1;
    }

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
    {
        std::wcerr << L"CoInitializeEx failed: 0x" << std::hex << hr << std::endl;
        return 1;
    }

    std::wstring line;
    while (std::getline(inputFile, line))
    {
        if (line.empty()) continue;

        std::vector<std::wstring> args;
        if (!ParseLine(line, args))
        {
            std::wcerr << L"Skipping malformed line: " << line << std::endl;
            continue;
        }

        const wchar_t* longTargetPath = args[0].c_str();
        const wchar_t* workingDirectory = args[1].c_str();
        const wchar_t* shortcutDirectory = args[2].c_str();
        const wchar_t* shortcutName = args[3].c_str();

        std::wcout << L"Creating shortcut for: " << shortcutName << std::endl;

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
    }
    CoUninitialize();
    return 0;
}
