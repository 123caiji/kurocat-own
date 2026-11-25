#pragma once
// Imported `.kurocat` skins live in %APPDATA%\Tama\skins, one file per skin named <content-hash>.kurocat.
// Built-in coats are not stored here.
#include <windows.h>
#include <shlobj.h>
#include <algorithm>
#include <string>
#include <vector>
#include "../../shared/SkinSpec.hpp"

inline std::wstring utf8ToWide(const std::string& s) {
    if (s.empty()) return std::wstring();
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    std::wstring w((size_t)std::max(0, n), L'\0');
    if (n > 0) MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), &w[0], n);
    return w;
}

struct SkinLibrary {
    std::vector<SkinSpec> skins; // sorted by name

    static std::wstring baseDir() {
        wchar_t path[MAX_PATH] = {0};
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, path)) && path[0]) return std::wstring(path) + L"\\Tama";
        wchar_t env[MAX_PATH] = {0};
        DWORD n = GetEnvironmentVariableW(L"APPDATA", env, MAX_PATH);
        return (n > 0 && n < MAX_PATH ? std::wstring(env) : std::wstring(L".")) + L"\\Tama";
    }
    static std::wstring dir() { return baseDir() + L"\\skins"; }

    static bool ensureDir() {
        for (const std::wstring& d : {baseDir(), dir()}) {
            if (!CreateDirectoryW(d.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) return false;
        }
        return true;
    }

    static bool readFile(const std::wstring& path, std::string& bytes, std::string& error) {
        HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h == INVALID_HANDLE_VALUE) { error = "无法打开这个文件"; return false; }
        LARGE_INTEGER size = {};
        if (!GetFileSizeEx(h, &size) || size.QuadPart > (LONGLONG)SkinSpec::MaxFileBytes) {
            CloseHandle(h);
            error = size.QuadPart > (LONGLONG)SkinSpec::MaxFileBytes ? "皮肤文件超过 2 MB 上限" : "无法读取这个文件";
            return false;
        }
        bytes.assign((size_t)size.QuadPart, '\0');
        DWORD read = 0;
        BOOL ok = bytes.empty() || ReadFile(h, &bytes[0], (DWORD)bytes.size(), &read, nullptr);
        CloseHandle(h);
        if (!ok || read != bytes.size()) { error = "无法读取这个文件"; return false; }
        return true;
    }

    /// 便携皮肤目录：exe 同目录下的 skins\（随包分发的皮肤放这里）
    static std::wstring portableDir() {
        wchar_t exe[MAX_PATH] = {0};
        if (!GetModuleFileNameW(nullptr, exe, MAX_PATH)) return std::wstring();
        std::wstring p(exe);
        size_t slash = p.find_last_of(L"\\");
        if (slash == std::wstring::npos) return std::wstring();
        return p.substr(0, slash) + L"\\skins";
    }

    void scanDir(const std::wstring& folder, std::vector<SkinSpec>& out) {
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileW((folder + L"\\*.kurocat").c_str(), &fd);
        if (h == INVALID_HANDLE_VALUE) return;
        do {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            std::string bytes, err;
            SkinSpec s;
            if (readFile(folder + L"\\" + fd.cFileName, bytes, err) && SkinSpec::parse(bytes, s, err)) {
                bool dup = false;
                for (const auto& e : out) if (e.id == s.id) dup = true;
                if (!dup) out.push_back(std::move(s));
            }
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }

    void reload() {
        skins.clear();
        scanDir(portableDir(), skins);   // 1) 随包分发（exe 同目录 skins\）
        scanDir(dir(), skins);           // 2) 用户导入（%APPDATA%\Tama\skins）
        std::sort(skins.begin(), skins.end(), [](const SkinSpec& a, const SkinSpec& b) { return a.name < b.name; });
    }

    const SkinSpec* find(const std::string& id) const {
        for (const auto& s : skins) if (s.id == id) return &s;
        return nullptr;
    }

    // Validates `path`, copies it into the skins folder and returns the new skin's id.
    bool importFile(const std::wstring& path, std::string& idOut, std::string& error) {
        std::string bytes;
        if (!readFile(path, bytes, error)) return false;
        SkinSpec spec;
        if (!SkinSpec::parse(bytes, spec, error)) return false;
        if (!ensureDir()) { error = "无法创建皮肤文件夹 %APPDATA%\\Tama\\skins"; return false; }
        std::wstring target = dir() + L"\\" + utf8ToWide(spec.id) + L".kurocat";
        std::wstring tmp = target + L".tmp";
        HANDLE h = CreateFileW(tmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h == INVALID_HANDLE_VALUE) { error = "无法写入皮肤文件夹 %APPDATA%\\Tama\\skins"; return false; }
        DWORD written = 0;
        BOOL ok = WriteFile(h, bytes.data(), (DWORD)bytes.size(), &written, nullptr) && written == bytes.size();
        ok = CloseHandle(h) && ok;
        if (!ok || !MoveFileExW(tmp.c_str(), target.c_str(), MOVEFILE_REPLACE_EXISTING)) {
            DeleteFileW(tmp.c_str());
            error = "保存皮肤失败（磁盘已满或没有权限？）";
            return false;
        }
        idOut = spec.id;
        reload();
        return true;
    }

    bool remove(const std::string& id) {
        if (id.size() != 16 || id.find_first_not_of("0123456789abcdef") != std::string::npos) return false;
        bool ok = DeleteFileW((dir() + L"\\" + utf8ToWide(id) + L".kurocat").c_str()) != FALSE;
        reload();
        return ok;
    }
};
