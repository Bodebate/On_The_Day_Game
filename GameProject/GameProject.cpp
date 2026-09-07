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


//
int MorseCodeBox[4] = { 0,0,0,0 }; //bottom x; bottom y; top x; top y
int CesarBox[4] = { 0,0,0,0 }; //bottom x; bottom y; top x; top y
int AlgorithmBox[4] = { 0,0,0,0 };//bottom x; bottom y; top x; top y
int SolveBox[4] = { 0,0,0,0 };//bottom x; bottom y; top x; top y
int backBox[4] = { 0,0,0,0 };//bottom x; bottom y; top x; top y


// Global Variables for Backgrounds
GameLogicClass MyClass = GameLogicClass();
HBITMAP hActiveBmp = nullptr;      // Points to the image currently shown on screen
HBITMAP hBmpMainMenu = nullptr;    // Image 1: The Initial Startup Screen
HBITMAP hBmpSolver = nullptr;   // Image 2: Accessed by clicking solver
HBITMAP hBmpCypher = nullptr; // Image 3: Accessed by clicking cypher
HBITMAP hBmpMorse = nullptr;  // Image 4: Accessed by clicking Morse code
HBITMAP hBmpAlgorithm = nullptr;    // Image 5: Accessed by clicking Algorithm
BITMAP bmpInfo;                    // Stores structural dimensions of the active image

// State Tracking
bool bIsOnMainMenu = true;         // True at startup; blocks accidental clicks later

bool bShowInputBox = false;         // Tracks if the input box overlay is open
bool bShowInfoBox = false;          // Tracks if the info box overlay is open
std::wstring boxDisplayText = L"";  // What the info box is displaying right now
std::wstring userInputText = L"";   // Stores what the user types (Only used on Left image)

std::wstring UserInputDisplayText = L"";   // Stores what the user types (Only used on Left image)
std::wstring UserInputText = L"";   // Stores what the user types (Only used on Left image)
std::wstring infoBoxDisplayText = L"";  // What the info box is displaying right now



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

// Converts std::wstring (Win32) to std::string 
std::string WStringToString(const std::wstring& wstr) {
    return std::string(wstr.begin(), wstr.end());
}

// Helper function: Converts std::string to std::wstring (Win32)
std::wstring StringToWString(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

// dynamically assign the size of the click boxes based on the current window size
void assignButtons() {
    RECT rect;
    GetClientRect(GetActiveWindow(), &rect);
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;
    // Calculate the positions of the boxes based on the current window size


    //                       Morse Code Box ( stage 1 accurate)
    //==============================================================================
    MorseCodeBox[0] = int(windowWidth / 7); // Bottom x
    MorseCodeBox[1] = int(windowHeight / 100 * 0); // Bottom y
    MorseCodeBox[2] = int(windowWidth / 2); // Top x
    MorseCodeBox[3] = int(windowHeight / 3); // Top y


    //                       Cesar Box (stage 1 accurate)
    //==============================================================================
    CesarBox[0] = int(windowWidth / 3 * 2); // Bottom x
    CesarBox[1] = int(windowHeight / 5 * 1.8); // Bottom y
    CesarBox[2] = int(windowWidth); // Top x
    CesarBox[3] = int(windowHeight / 5 * 3.2); // Top y

    //                       Algorithm  (stage 1 accurate)
    //==============================================================================
    AlgorithmBox[0] = int(windowWidth / 110 * 2); // Bottom x
    AlgorithmBox[1] = int(windowHeight / 4); // Bottom y
    AlgorithmBox[2] = int(windowWidth / 3); // Top x
    AlgorithmBox[3] = int(windowHeight / 4 * 3); // Top y


    //                       Solve Box ( stage 1 accurate)
    //==============================================================================
    SolveBox[0] = int(windowWidth / 2.99); // Bottom x
    SolveBox[1] = int(windowHeight / 5 * 1.8); // Bottom y
    SolveBox[2] = int(windowWidth / 3 * 2); // Top x
    SolveBox[3] = int(windowHeight / 5 * 3.8); // Top y

    //                       Back Box (stage 1 accurate)
    //==============================================================================
    backBox[0] = int(windowWidth / 4 * 3); // Bottom x
    backBox[1] = int(windowHeight * 0); // Bottom y
    backBox[2] = int(windowWidth); // Top x
    backBox[3] = int(windowHeight / 10); // Top y
}

// dynamic creation of input box for user to type in
void DrawInputBox(HDC hdc, int windowWidth, int windowHeight)
{
    if (bShowInputBox)
    {
        // 1. Calculate an overlay window size center-anchored rectangle
        int boxW = windowWidth / 110 * 50;
        int boxH = windowHeight / 100 * 99;
        int boxX = (windowWidth - boxW) / 100 * 98;
        int boxY = (windowHeight - boxH) / 2; // Positioned slightly higher up

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
        DrawTextW(hdc, UserInputDisplayText.c_str(), -1, &textRect, DT_LEFT | DT_WORDBREAK | DT_EDITCONTROL);

        // Cleanup font GDI drawing objects 
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
        DeleteObject(hBlackBrush);
        DeleteObject(hGreenBrush);
    }
}

// dynamic creation of info box for user to read information from
void DrawInfoBox(HDC hdc, int windowWidth, int windowHeight)
{
    if (bShowInfoBox)
    {
        // 1. Calculate an overlay window size center-anchored rectangle
        int boxW = windowWidth / 110 * 50;
        int boxH = windowHeight / 100 * 99;
        int boxX = (windowWidth) / 100 * 2;
        int boxY = (windowHeight - boxH) / 2; // Positioned slightly higher up

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
        DrawTextW(hdc, infoBoxDisplayText.c_str(), -1, &textRect, DT_LEFT | DT_WORDBREAK | DT_EDITCONTROL);

        // Cleanup font GDI drawing objects 
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
        DeleteObject(hBlackBrush);
        DeleteObject(hGreenBrush);
    }
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
            hBmpMorse = (HBITMAP)LoadImageW(nullptr, L"BlackBackground.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            hBmpCypher = (HBITMAP)LoadImageW(nullptr, L"BlackBackground.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            hBmpSolver = (HBITMAP)LoadImageW(nullptr, L"BlackBackground.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            hBmpAlgorithm = (HBITMAP)LoadImageW(nullptr, L"BlackBackground.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

            // Error validation safety check
            if (!hBmpMainMenu || !hBmpMorse || !hBmpCypher || !hBmpSolver) {
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
		assignButtons(); // Dynamically assign the click box sizes based on current window size
        RECT rect;
        GetClientRect(hWnd, &rect);
        int windowWidth = rect.right - rect.left;
        int windowHeight = rect.bottom - rect.top;

        mouseY = windowHeight - mouseY; // Invert Y coordinate to match top-left origin

        if (bIsOnMainMenu)
        {
            // ---- MAIN MENU NAVIGATION ---- 
            if(mouseX >= MorseCodeBox[0] && mouseY >= MorseCodeBox[1] && mouseX <= MorseCodeBox[2] && mouseY <= MorseCodeBox[3])
            {
                // Handle Morse Code Box click
				hActiveBmp = hBmpMorse;
                UserInputDisplayText = L"#Add your code here:\n#Use the examples on the left to help you\n\n";

                bIsOnMainMenu = false;
                bShowInfoBox = true;
                bShowInputBox = true;
            }
            else if (mouseX >= CesarBox[0] && mouseY >= CesarBox[1] && mouseX <= CesarBox[2] && mouseY <= CesarBox[3])
            {
                // Handle Cesar Code Box click

                hActiveBmp = hBmpCypher;
                UserInputDisplayText = L"#Add your code here:\n#Use the examples on the left to help you\n\n";
                bIsOnMainMenu = false;
                bShowInfoBox = true;
                bShowInputBox = true;
            }
            else if (mouseX >= AlgorithmBox[0] && mouseY >= AlgorithmBox[1] && mouseX <= AlgorithmBox[2] && mouseY <= AlgorithmBox[3])
            {
                // Handle Algorithm Box click

                hActiveBmp = hBmpAlgorithm;
                UserInputDisplayText = L"Use the space below to help you figure out your solution to the challenge:\n\n";
                bIsOnMainMenu = false;
                bShowInfoBox = true;
                bShowInputBox = true;
            }
            else if (mouseX >= SolveBox[0] && mouseY >= SolveBox[1] && mouseX <= SolveBox[2] && mouseY <= SolveBox[3])
            {
                // Handle Solve Box click
                
            }

        }
        else
        {
            // ---- SUB-IMAGE LOGIC ----

            if (mouseX >= backBox[0] && mouseY >= backBox[1] && mouseX <= backBox[2] && mouseY <= backBox[3])
            {
                // Handle back box click
                hActiveBmp = hBmpMainMenu;
                bIsOnMainMenu = true;
                bShowInputBox = false;
                bShowInfoBox = false;
                
            }
        }


        if (hActiveBmp) { GetObject(hActiveBmp, sizeof(bmpInfo), &bmpInfo); }
        InvalidateRect(hWnd, nullptr, TRUE);
    }
    break;
    case WM_CHAR:
    {
        // Only capture typing if the input box overlay is open 
        if (bShowInputBox)
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

            DrawInfoBox(hdc, windowWidth, windowHeight);
            DrawInputBox(hdc, windowWidth, windowHeight);
    
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
        if (hBmpSolver)      DeleteObject(hBmpSolver);
        if (hBmpCypher)      DeleteObject(hBmpCypher);
        if (hBmpMorse)       DeleteObject(hBmpMorse);
        if (hBmpAlgorithm)   DeleteObject(hBmpAlgorithm);

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
