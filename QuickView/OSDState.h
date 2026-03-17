#pragma once
#include <string>
#include <d2d1.h>
#include <d2d1helper.h>

enum class OSDPosition { Bottom, Top, TopRight };

struct OSDState {
    std::wstring Message;
    std::wstring MessageLeft;  // For compare mode
    std::wstring MessageRight; // For compare mode
    bool IsCompareOSD = false;
    DWORD StartTime = 0;
    DWORD Duration = 2000;
    bool IsError = false;
    bool IsWarning = false;
    D2D1_COLOR_F CustomColor = D2D1::ColorF(D2D1::ColorF::Black, 0.0f);
    OSDPosition Position = OSDPosition::Bottom;

    void Show(HWND hwnd, const std::wstring& msg, bool error = false, bool warning = false, D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::White), OSDPosition pos = OSDPosition::Bottom) {
        Message = msg;
        IsCompareOSD = false;
        StartTime = GetTickCount();
        IsError = error;
        IsWarning = warning;
        CustomColor = color;
        Position = pos;
        if (hwnd) {
            SetTimer(hwnd, 999, 250, nullptr);
            InvalidateRect(hwnd, nullptr, FALSE);
        }
    }

    void ShowCompare(HWND hwnd, const std::wstring& left, const std::wstring& right, D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::White)) {
        MessageLeft = left;
        MessageRight = right;
        Message = L"COMPARE"; // Dummy to trigger visibility
        IsCompareOSD = true;
        StartTime = GetTickCount();
        IsError = false;
        IsWarning = false;
        CustomColor = color;
        Position = OSDPosition::Bottom;
        if (hwnd) {
            SetTimer(hwnd, 999, 250, nullptr);
            InvalidateRect(hwnd, nullptr, FALSE);
        }
    }

    bool IsVisible() const {
        return (!Message.empty() || IsCompareOSD) && (GetTickCount() - StartTime) < Duration;
    }
};
