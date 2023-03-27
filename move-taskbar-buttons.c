#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#include "TTLib.h"

static LRESULT CALLBACK WindowProc(HWND hwnd,
                            UINT uMsg,
                            WPARAM wParam,
                            LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_HOTKEY: {
            WORD vkey = HIWORD(lParam);
            if (!TTLib_ManipulationStart()) {
                wprintf(L"TTLib_ManipulationStart() failed\n");
                return 0;
            }

            HANDLE hTaskbar = TTLib_GetMainTaskbar();
            if (!hTaskbar) {
                wprintf(L"TTLib_GetMainTaskbar() failed\n");
                goto ret;
            }

            HANDLE hActiveButtonGroup = TTLib_GetActiveButtonGroup(hTaskbar);
            if (!hActiveButtonGroup) {
                wprintf(L"TTLib_GetActiveButtonGroup() failed\n");
                goto ret;
            }

            if (vkey == VK_LEFT || vkey == VK_RIGHT) {
                HANDLE hActiveButton = TTLib_GetActiveButton(hTaskbar);
                if (!hActiveButton) {
                    wprintf(L"TTLib_GetActiveButton() failed\n");
                    goto ret;
                }

                int bcount;
                if (TTLib_GetButtonCount(hActiveButtonGroup, &bcount)) {
                    for (int i = 0; i < bcount; i++) {
                        HANDLE hButton = TTLib_GetButton(hActiveButtonGroup, i);
                        if (!hButton) {
                            wprintf(L"TTLib_GetButton() failed\n");
                            goto ret;
                        }
                        if (hButton == hActiveButton) {
                            if (vkey == VK_LEFT) {
                                TTLib_ButtonMoveInButtonGroup(
                                    hActiveButtonGroup, i,
                                    i ? (i - 1) : (bcount - 1));
                            } else if (vkey == VK_RIGHT) {
                                TTLib_ButtonMoveInButtonGroup(
                                    hActiveButtonGroup, i, (i + 1) % bcount);
                            }
                            break;
                        }
                    }
                } else {
                    wprintf(L"TTLib_GetButtonCount() failed\n");
                    goto ret;
                }
            } else if (vkey == VK_DOWN || vkey == VK_UP) {
                int gcount;
                if (TTLib_GetButtonGroupCount(hTaskbar, &gcount)) {
                    for (int i = 0; i < gcount; i++) {
                        HANDLE hButtonGroup = TTLib_GetButtonGroup(hTaskbar, i);
                        if (!hButtonGroup) {
                            wprintf(L"TTLib_GetButtonGroup() failed\n");
                            goto ret;
                        }
                        if (hButtonGroup == hActiveButtonGroup) {
                            if (vkey == VK_DOWN) {
                                TTLib_ButtonGroupMove(
                                    hTaskbar, i, i ? (i - 1) : (gcount - 1));
                            } else if (vkey == VK_UP) {
                                TTLib_ButtonGroupMove(hTaskbar, i,
                                                      (i + 1) % gcount);
                            }
                            break;
                        }
                    }
                } else {
                    wprintf(L"TTLib_GetButtonGroupCount() failed\n");
                    goto ret;
                }
            }
        ret:
            TTLib_ManipulationEnd();
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nShowCmd) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    WCHAR *windowClassName = L"move-tasbkar-buttons";
    wc.lpszClassName = windowClassName;

    if (!RegisterClass(&wc)) {
        wprintf(L"RegisterClass failed\n");
        return 1;
    }

    HWND hwnd =
        CreateWindow(windowClassName, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
    if (!hwnd) {
        wprintf(L"CreateWindow failed\n");
        return 1;
    }

    // ShowWindow(hwnd, nCmdShow);

    struct {
        wchar_t *key;
        BOOL ret;
    } regkeys[4];

    regkeys[0].ret = RegisterHotKey(
        hwnd, 1, MOD_WIN | MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_LEFT);
    regkeys[0].key = L"Win+Ctrl+Alt+Left";

    regkeys[1].ret = RegisterHotKey(
        hwnd, 1, MOD_WIN | MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_RIGHT);
    regkeys[1].key = L"Win+Ctrl+Alt+Right";

    regkeys[2].ret = RegisterHotKey(
        hwnd, 1, MOD_WIN | MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_UP);
    regkeys[2].key = L"Win+Ctrl+Alt+Up";

    regkeys[3].ret = RegisterHotKey(
        hwnd, 1, MOD_WIN | MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_DOWN);
    regkeys[3].key = L"Win+Ctrl+Alt+Down";

    BOOL reg_ret = TRUE;
    wchar_t failed_keys[160];
    size_t offset = 0;
    for (unsigned i = 0; i < 4; i++) {
        if (!regkeys[i].ret) {
            size_t len = wcslen(regkeys[i].key);
            wcsncpy(failed_keys + offset, regkeys[i].key, len);
            offset += len;
            failed_keys[offset] = L'\n';
            offset++;
            reg_ret = FALSE;
        }
    }
    failed_keys[offset] = L'\0';
    if (!reg_ret) {
        wchar_t err_msg[260];
        swprintf(err_msg, 260, L"failed to register hotkeys:\n\n%s",
                 failed_keys);
        MessageBoxW(hwnd, err_msg, NULL, MB_OK);
        return 1;
    }

    BOOL bSuccess = FALSE;
    DWORD dwError;
    dwError = TTLib_Init();
    if (dwError == TTLIB_OK) {
        wprintf(L"Loading 7+ Taskbar Tweaking library into explorer...\n");
        dwError = TTLib_LoadIntoExplorer();
        if (dwError == TTLIB_OK) {
            MSG msg = {0};
            while ((bSuccess = GetMessage(&msg, NULL, 0, 0))) {
                if (bSuccess == -1) {
                    bSuccess = FALSE;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            TTLib_UnloadFromExplorer();
        } else
            wprintf(L"TTLib_LoadIntoExplorer() failed with error %u\n",
                    dwError);
        TTLib_Uninit();
    } else
        wprintf(L"TTLib_Init() failed with error %u\n", dwError);

    return bSuccess ? 0 : 1;
}
