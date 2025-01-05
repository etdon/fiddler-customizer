#include "pch.h"
#include <Windows.h>
#include "FiddlerCustomizer.h"
#include <uxtheme.h>
#include <dwmapi.h>
#include <vsstyle.h>
#include "ManagedListView.h"
#include "ManagedStatusBar.h"
#include "ManagedTabControl.h"
#include "UAH.h"

#pragma comment(lib, "gdi32.lib")

#define DARK_MODE true

using namespace System;
using namespace Drawing;
using namespace Reflection;
using namespace Windows::Forms;
using namespace Collections::Generic;
using namespace Runtime::InteropServices;

static WNDPROC parentWindowProcedure;
static HTHEME menuTheme = nullptr;
static HBRUSH nativeBackgroundBrush = CreateSolidBrush(RGB(32, 32, 32));

static LRESULT CALLBACK WindowHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_UAHDRAWMENU:
		{
			UAHMENU* pUDM = (UAHMENU*)lParam;
			RECT rc = {0};

			{
				MENUBARINFO mbi = {sizeof(mbi)};
				GetMenuBarInfo(hwnd, OBJID_MENU, 0, &mbi);

				RECT rcWindow;
				GetWindowRect(hwnd, &rcWindow);

				rc = mbi.rcBar;
				OffsetRect(&rc, -rcWindow.left, -rcWindow.top);
			}

			FillRect(pUDM->hdc, &rc, nativeBackgroundBrush);

			return true;
		}
	case WM_UAHDRAWMENUITEM:
		{
			UAHDRAWMENUITEM* pUDMI = (UAHDRAWMENUITEM*)lParam;

			static HBRUSH g_brItemBackgroundHot = CreateSolidBrush(RGB(179, 215, 243));
			static HBRUSH g_brItemBackgroundSelected = nativeBackgroundBrush;

			HBRUSH* pbrBackground = &nativeBackgroundBrush;
			wchar_t menuString[256] = {0};
			MENUITEMINFO mii = {sizeof(mii), MIIM_STRING};
			{
				mii.dwTypeData = menuString;
				mii.cch = (sizeof(menuString) / 2) - 1;

				GetMenuItemInfo(pUDMI->um.hmenu, pUDMI->umi.iPosition, TRUE, &mii);
			}

			DWORD dwFlags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
			int textStateId = 0;
			{
				int backgroundStateId = 0;
				if ((pUDMI->dis.itemState & ODS_INACTIVE) | (pUDMI->dis.itemState & ODS_DEFAULT))
				{
					textStateId = MPI_NORMAL;
					backgroundStateId = MPI_NORMAL;
				}
				if (pUDMI->dis.itemState & ODS_HOTLIGHT)
				{
					textStateId = MPI_HOT;
					backgroundStateId = MPI_HOT;

					pbrBackground = &g_brItemBackgroundHot;
				}
				if (pUDMI->dis.itemState & ODS_SELECTED)
				{
					textStateId = MPI_HOT;
					backgroundStateId = MPI_HOT;

					pbrBackground = &g_brItemBackgroundSelected;
				}
				if ((pUDMI->dis.itemState & ODS_GRAYED) || (pUDMI->dis.itemState & ODS_DISABLED))
				{
					textStateId = MPI_DISABLED;
					backgroundStateId = MPI_DISABLED;
				}
				if (pUDMI->dis.itemState & ODS_NOACCEL)
				{
					dwFlags |= DT_HIDEPREFIX;
				}
			}

			if (!menuTheme)
			{
				menuTheme = OpenThemeData(hwnd, L"Menu");
			}

			DTTOPTS opts = {
				sizeof(opts), DTT_TEXTCOLOR,
				textStateId != MPI_DISABLED ? RGB(176, 176, 176) : RGB(176, 176, 176)
			};

			FillRect(pUDMI->um.hdc, &pUDMI->dis.rcItem, *pbrBackground);
			DrawThemeTextEx(menuTheme, pUDMI->um.hdc, MENU_POPUPITEM, MBI_NORMAL, menuString, mii.cch, dwFlags,
			                &pUDMI->dis.rcItem, &opts);

			return true;
		}
	case WM_UAHMEASUREMENUITEM:
		{
			UAHMEASUREMENUITEM* pMmi = (UAHMEASUREMENUITEM*)lParam;
			lParam = DefWindowProc(hwnd, msg, wParam, lParam);
			pMmi->mis.itemWidth = (pMmi->mis.itemWidth * 4) / 3;

			return false;
		}
	case WM_THEMECHANGED:
		{
			if (menuTheme)
			{
				CloseThemeData(menuTheme);
				menuTheme = nullptr;
			}
			return CallWindowProc(parentWindowProcedure, hwnd, msg, wParam, lParam);
		}
	case WM_NCPAINT:
	case WM_NCACTIVATE:
		{
			lParam = DefWindowProc(hwnd, msg, wParam, lParam);
			MENUBARINFO mbi = {sizeof(mbi)};
			if (!GetMenuBarInfo(hwnd, OBJID_MENU, 0, &mbi))
				return CallWindowProc(parentWindowProcedure, hwnd, msg, wParam, lParam);

			RECT rcClient = {0};
			GetClientRect(hwnd, &rcClient);
			MapWindowPoints(hwnd, nullptr, (POINT*)&rcClient, 2);

			RECT rcWindow = {0};
			GetWindowRect(hwnd, &rcWindow);

			OffsetRect(&rcClient, -rcWindow.left, -rcWindow.top);

			RECT rcAnnoyingLine = rcClient;
			rcAnnoyingLine.bottom = rcAnnoyingLine.top;
			rcAnnoyingLine.top--;

			HDC hdc = GetWindowDC(hwnd);
			FillRect(hdc, &rcAnnoyingLine, nativeBackgroundBrush);
			ReleaseDC(hwnd, hdc);
			return true;
		}
	default:
		return CallWindowProc(parentWindowProcedure, hwnd, msg, wParam, lParam);
	}
}

using fnFlushMenuThemes = void (WINAPI*)();
using fnSetPreferredAppMode = PreferredAppMode(WINAPI*)(PreferredAppMode appMode);
using fnAllowDarkModeForWindow = bool (WINAPI*)(HWND hWnd, bool allow);

constexpr BOOL darkMode = true;

HMODULE uxThemeModule;
fnFlushMenuThemes FlushMenuThemes;
fnSetPreferredAppMode SetPreferredAppMode;
fnAllowDarkModeForWindow AllowDarkModeForWindow;

FiddlerCustomizer::FiddlerCustomizer()
{
	backgroundColor = Color::FromArgb(32, 32, 32);
	foregroundColor = Color::FromArgb(176, 176, 176);
}

void FiddlerCustomizer::ApplyChanges()
{
	uxThemeModule = LoadLibraryExW(L"UxTheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (uxThemeModule == nullptr)
		return;

	FlushMenuThemes =
		reinterpret_cast<fnFlushMenuThemes>(GetProcAddress(uxThemeModule, MAKEINTRESOURCEA(136)));
	SetPreferredAppMode =
		reinterpret_cast<fnSetPreferredAppMode>(GetProcAddress(uxThemeModule, MAKEINTRESOURCEA(135)));
	AllowDarkModeForWindow =
		reinterpret_cast<fnAllowDarkModeForWindow>(GetProcAddress(uxThemeModule, MAKEINTRESOURCEA(133)));

	Dictionary<String^, Assembly^>^ assemblies = gcnew Dictionary<String^, Assembly^>();
	for each (Assembly^ assembly in AppDomain::CurrentDomain->GetAssemblies())
	{
		String^ name = assembly->GetName()->Name;
		assemblies->Add(name, assembly);
	}

	Assembly^ fiddlerAssembly = assemblies["Fiddler"];
	if (fiddlerAssembly == nullptr)
		return;

	Type^ fiddlerApplicationType = fiddlerAssembly->GetType("Fiddler.FiddlerApplication");
	MethodInfo^ getUIMethodInfo = fiddlerApplicationType->GetMethod("get_UI");
	Form^ mainForm = safe_cast<Form^>(getUIMethodInfo->Invoke(nullptr, nullptr));
	HWND windowHandle = static_cast<HWND>(mainForm->Handle.ToPointer());
	mainForm->ControlAdded += gcnew ControlEventHandler(
		this, &FiddlerCustomizer::OnControlAdded
	);

	SetPreferredAppMode(PreferredAppMode::ForceDark);
	FlushMenuThemes();

	parentWindowProcedure = (WNDPROC)GetWindowLongPtr(windowHandle, GWLP_WNDPROC);
	SetWindowLongPtr(windowHandle, GWLP_WNDPROC, (LONG_PTR)WindowHook);

	AllowDarkModeForWindow(windowHandle, true);
	DwmSetWindowAttribute(windowHandle, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));
	UpdateWindow(windowHandle);

	ProcessControlsRecursively(mainForm);
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	FreeLibrary(uxThemeModule);
}

void FiddlerCustomizer::ProcessControlsRecursively(Control^ control)
{
	ProcessControl(control);
	for each (Control^ subControl in control->Controls)
	{
		ProcessControlsRecursively(subControl);
	}
}

void FiddlerCustomizer::ProcessControl(Control^ control)
{
	if (control->IsHandleCreated)
	{
		control->BackColor = backgroundColor;
		control->ForeColor = foregroundColor;

		IntPtr^ handlePointer = control->Handle;
		HWND controlHandle = static_cast<HWND>(handlePointer->ToPointer());
		if (IsWindow(controlHandle))
		{
			AllowDarkModeForWindow(controlHandle, true);
			if (dynamic_cast<Button^>(control) != nullptr)
			{
				SetWindowTheme(controlHandle, L"Explorer", nullptr);
				SendMessageW(controlHandle, WM_THEMECHANGED, 0, 0);
			}
			else if (dynamic_cast<ComboBox^>(control) != nullptr)
			{
				SetWindowTheme(controlHandle, L"CFD", nullptr);
				SendMessageW(controlHandle, WM_THEMECHANGED, 0, 0);
			}
			else if (dynamic_cast<ListView^>(control) != nullptr)
			{
				SetWindowTheme(controlHandle, L"Explorer", nullptr);
				SendMessageW(controlHandle, WM_THEMECHANGED, 0, 0);
				gcnew ManagedListView(this, safe_cast<ListView^>(control));
			}
			else if (dynamic_cast<TabControl^>(control) != nullptr)
			{
				SetWindowTheme(controlHandle, L"DarkMode_Explorer", nullptr);
				SendMessageW(controlHandle, WM_THEMECHANGED, 0, 0);
				gcnew ManagedTabControl(this, safe_cast<TabControl^>(control));
			}
			else if (dynamic_cast<StatusBar^>(control) != nullptr)
			{
				SetWindowTheme(controlHandle, L"DarkMode_Explorer", nullptr);
				SendMessageW(controlHandle, WM_THEMECHANGED, 0, 0);
				gcnew ManagedStatusBar(this, safe_cast<StatusBar^>(control));
			}
			else
			{
				SetWindowTheme(controlHandle, L"DarkMode_Explorer", nullptr);
				SendMessageW(controlHandle, WM_THEMECHANGED, 0, 0);
			}
			control->Invalidate();
			control->Update();
		}
	}
}

void FiddlerCustomizer::OnControlAdded(Object^ sender, ControlEventArgs^ args)
{
	ProcessControlsRecursively(args->Control);
}

Color FiddlerCustomizer::GetBackgroundColor()
{
	return backgroundColor;
}

Color FiddlerCustomizer::GetForegroundColor()
{
	return foregroundColor;
}
