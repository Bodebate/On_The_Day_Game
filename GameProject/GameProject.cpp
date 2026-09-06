// GameProject.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "GameProject.h"
#include "GameLogicClass.h"
#include <string>
#include <algorithm>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Global Variables for Backgrounds
GameLogicClass MyClass = GameLogicClass();
HBITMAP hActiveBmp = nullptr;      // Points to the image currently shown on screen
HBITMAP hBmpMainMenu = nullptr;    // Image 1: The Initial Startup Screen
HBITMAP hBmpLeftScene = nullptr;   // Image 2: Accessed by clicking Left
HBITMAP hBmpMiddleScene = nullptr; // Image 3: Accessed by clicking Middle
HBITMAP hBmpRightScene = nullptr;  // Image 4: Accessed by clicking Right
BITMAP bmpInfo;                    // Stores structural dimensions of the active image

// State Tracking
bool bIsOnMainMenu = true;         // True at startup; blocks accidental clicks later

bool bShowTextBox = false;          // Tracks if the hacker text box overlay is open
std::wstring boxDisplayText = L"";  // What the text box is displaying right now
std::wstring userInputText = L"";   // Stores what the user types (Only used on Left image)

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.


    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GAMEPROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAMEPROJECT));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

// Helper function: Converts std::wstring (Win32) to std::string (Your Class)
std::string WStringToString(const std::wstring& wstr) {
    return std::string(wstr.begin(), wstr.end());
}

// Helper function: Converts std::string (Your Class) to std::wstring (Win32)
std::wstring StringToWString(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAMEPROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GAMEPROJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
            // 1. Load all 4 discrete image files from your project folder
            hBmpMainMenu = (HBITMAP)LoadImageW(nullptr, L"HomeImage.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            hBmpLeftScene = (HBITMAP)LoadImageW(nullptr, L"Computer.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            hBmpMiddleScene = (HBITMAP)LoadImageW(nullptr, L"MorseCodeChart.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            hBmpRightScene = (HBITMAP)LoadImageW(nullptr, L"MorseCode.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

            // Error validation safety check
            if (!hBmpMainMenu || !hBmpLeftScene || !hBmpMiddleScene || !hBmpRightScene) {
                MessageBoxW(hWnd, L"Error: One or more .bmp files failed to load!", L"Error", MB_OK);
            }

            // 2. Set the initial active background to the Main Menu
            hActiveBmp = hBmpMainMenu;
            bIsOnMainMenu = true;

            if (hActiveBmp) {
                GetObject(hActiveBmp, sizeof(bmpInfo), &bmpInfo);
            }
        }
        break;
    case WM_LBUTTONDOWN:
    {
        int mouseX = LOWORD(lParam);
        int mouseY = HIWORD(lParam);

        RECT rect;
        GetClientRect(hWnd, &rect);
        int windowWidth = rect.right - rect.left;
        int windowHeight = rect.bottom - rect.top;

        if (bIsOnMainMenu)
        {
            // ---- MAIN MENU NAVIGATION ----
            int oneThirdWidth = windowWidth / 3;
            bShowTextBox = false; // Reset text boxes when entering scenes
            userInputText = L"";

            if (mouseX < oneThirdWidth) {
                hActiveBmp = hBmpLeftScene;
                bIsOnMainMenu = false;
            }
            else if (mouseX < (oneThirdWidth * 2)) {
                hActiveBmp = hBmpMiddleScene;
                bIsOnMainMenu = false;
            }
            else {
                hActiveBmp = hBmpRightScene;
                bIsOnMainMenu = false;
            }
        }
        else
        {
            // ---- SUB-IMAGE LOGIC ----
            int twoThirdsHeight = (windowHeight * 2) / 3;

            if (mouseY >= twoThirdsHeight)
            {
                // Bottom 1/3: Go back to Main Menu
                hActiveBmp = hBmpMainMenu;
                bIsOnMainMenu = true;
                bShowTextBox = false;
            }
            else
            {
                // Top 2/3: Open Hacker Box (EXCEPT inside the Middle Scene)
                if (hActiveBmp != hBmpMiddleScene)
                {
                    bShowTextBox = true;

                    if (hActiveBmp == hBmpLeftScene)
                    {
                        // LEFT: Prompt user for real-time console input tracking
                        boxDisplayText = L"ACCESS GRANTED.\nENTER OVERRIDE CRYPTO-KEY:\n> " + userInputText;
                    }
                    else if (hActiveBmp == hBmpRightScene)
                    {
                        // RIGHT: Automatically pull Morse Code data from your class logic
                        std::string morseCodeStr = MyClass.GetMorseCode();
                        std::wstring wMorseCode = StringToWString(morseCodeStr);

                        boxDisplayText = L"INTERCEPTING TRANSMISSION...\nMORSE SIGNAL DECODED:\n\n" + wMorseCode;
                    }
                }
            }
        }


        if (hActiveBmp) { GetObject(hActiveBmp, sizeof(bmpInfo), &bmpInfo); }
        InvalidateRect(hWnd, nullptr, TRUE);
    }
    break;
    case WM_CHAR:
    {
        // Only capture typing if the text box overlay is open AND we are looking at the Left Panel
        if (bShowTextBox && hActiveBmp == hBmpLeftScene)
        {
            wchar_t ch = (wchar_t)wParam;

            if (ch == L'\b') // Backspace
            {
                if (!userInputText.empty()) {
                    userInputText.pop_back();
                }
                boxDisplayText = L"ACCESS GRANTED.\nENTER OVERRIDE CRYPTO-KEY:\n> " + userInputText;
            }
            else if (ch == L'\r' || ch == L'\n') // USER PRESSES ENTER KEY
            {
                // 1. Convert the current layout typed buffer into a standard string
                std::string standardInput = WStringToString(userInputText);

                // 2. Evaluate using your custom class logic function
                if (MyClass.CheckMorseCode(standardInput))
                {
                    boxDisplayText = L"SUCCESS!\nCRACK COMPLETED.\nMAIN SYSTEM UNLOCKED.";
                }
                else
                {
                    boxDisplayText = L"INVALID CRYPTO-KEY.\nINTRUDER ALERT TRIGGERED.\nTRY AGAIN:\n> ";
                    userInputText = L""; // Wipe user line to let them retype
                }
            }
            else if (ch >= 32) // Standard character append rule
            {
                userInputText += ch;
                boxDisplayText = L"ACCESS GRANTED.\nENTER OVERRIDE CRYPTO-KEY:\n> " + userInputText;
            }

            // Instantly notify windows loop to paint text adjustments
            InvalidateRect(hWnd, nullptr, TRUE);
        }
    }
    break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        if (hActiveBmp != nullptr)
        {
            RECT rect;
            GetClientRect(hWnd, &rect);
            int windowWidth = rect.right - rect.left;
            int windowHeight = rect.bottom - rect.top;

            HDC hMemDC = CreateCompatibleDC(hdc);
            HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hActiveBmp);

            // Draw the dynamic background stretched to match current window boundaries
            StretchBlt(
                hdc, 0, 0, windowWidth, windowHeight,
                hMemDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight,
                SRCCOPY
            );
            // --- DRAW HACKER TEXT BOX OVERLAY ---
            if (bShowTextBox)
            {
                // 1. Calculate an overlay window size center-anchored rectangle
                int boxW = 500;
                int boxH = 200;
                int boxX = (windowWidth - boxW) / 2;
                int boxY = (windowHeight - boxH) / 3; // Positioned slightly higher up

                RECT boxRect = { boxX, boxY, boxX + boxW, boxY + boxH };

                // 2. Draw 90s style solid terminal black background fill
                HBRUSH hBlackBrush = CreateSolidBrush(RGB(10, 16, 10));
                FillRect(hdc, &boxRect, hBlackBrush);

                // 3. Frame it with a bright Matrix green matrix outline border frame
                HBRUSH hGreenBrush = CreateSolidBrush(RGB(50, 255, 50));
                FrameRect(hdc, &boxRect, hGreenBrush);

                // 4. Setup retro terminal green monospace text styling configurations
                SetTextColor(hdc, RGB(50, 255, 50));
                SetBkMode(hdc, TRANSPARENT);

                // 5. Select a retro system font built into Windows
                HFONT hFont = CreateFontW(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                    ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, L"Courier New");
                HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

                // Add padding margins inside your black box so text doesn't hit the border outline
                RECT textRect = { boxRect.left + 15, boxRect.top + 15, boxRect.right - 15, boxRect.bottom - 15 };

                // 6. Draw string layout blocks
                DrawTextW(hdc, boxDisplayText.c_str(), -1, &textRect, DT_LEFT | DT_WORDBREAK | DT_EDITCONTROL);

                // Cleanup font GDI drawing objects 
                SelectObject(hdc, hOldFont);
                DeleteObject(hFont);
                DeleteObject(hBlackBrush);
                DeleteObject(hGreenBrush);
            }
    
            SelectObject(hMemDC, hOldBmp);
            DeleteDC(hMemDC);
        }

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_ERASEBKGND:
        return 1; // Handled internally to prevent white background flicker when changing scenes

    case WM_DESTROY:
        if (hBmpMainMenu)    DeleteObject(hBmpMainMenu);
        if (hBmpLeftScene)   DeleteObject(hBmpLeftScene);
        if (hBmpMiddleScene) DeleteObject(hBmpMiddleScene);
        if (hBmpRightScene)  DeleteObject(hBmpRightScene);

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
