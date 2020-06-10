﻿#include "framework.h" 
#include "resource.h" 
#include <stdlib.h> 
#include <stdio.h> 
#include <string> 
#include <shellapi.h> 
#include <commctrl.h> 

using namespace std;

HWND hWndDialog;
HINSTANCE ghInstance; // Переменная для хранения хендела процесса 
// Описание используемой оконной процедуры 

BOOL CALLBACK PviewDlgProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
// Главное приложение программы 

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	MSG msg;
	ghInstance = hInstance;

	// Создание диалогового окна 
	hWndDialog = CreateDialogParam(hInstance,
		MAKEINTRESOURCE(IDD_DIALOG1),
		NULL,
		(DLGPROC)PviewDlgProc,
		(LONG)0);
	ShowWindow(hWndDialog, nCmdShow);
	UpdateWindow(hWndDialog);
	// Стандартный цикл обработки сообщений приложения 
	while (GetMessage(&msg, NULL, 0, 0))
		if (!IsDialogMessage(hWndDialog, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	DestroyWindow(hWndDialog);
	return 0;
}

//Процедура обработки сообщений диалогового окна 
BOOL CALLBACK PviewDlgProc(HWND hWnd,
	UINT wMsg,
	WPARAM wParam,
	LPARAM lParam) {

	static string CheckDraw = "";
	static int CountPointLER = 0, CountPointCAP = 0,
		CountPointF = 0, CountPointPP = 0;
	static RECT FRect, rect, rect1, rect2;
	static POINT pt[4], ptpp[8], point;
	static HDC hdc, hdcm, hdc1, hdcm1, hdc2, hdcm2; // контекст устройтва 
	static PAINTSTRUCT ps; // структура для перерисовки окна 
	static HBITMAP hbm, hbm1, hbm2; // дескриптор изображения 
	static int red = 0, green = 0, blue = 0, r = 0, g = 0, b = 0;
	static HPEN usPen;
	static HWND hStatic, hStatic1, hStatic2;
	static int lx, ly, lx1, ly1, lx2, ly2; // координаты конца экрана 
	static int pen = 1, brush = 1, color = 1, style = 7, width = 1; // значения для всех полос стандартные

	switch (wMsg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_INITDIALOG: {

		hStatic = GetDlgItem(hWnd, BIG_STATIC);
		hdc = GetDC(hStatic); // получаем контекст устройства окна hWnd 
		GetClientRect(hStatic, &rect); // получить координаты графического окна 

		hStatic1 = GetDlgItem(hWnd, DRAW_STATIC);
		hdc1 = GetDC(hStatic1); // получаем контекст устройства окна hWnd 
		GetClientRect(hStatic1, &rect1); // получить координаты графического окна 

		hStatic2 = GetDlgItem(hWnd, SMALL_STATIC);
		hdc2 = GetDC(hStatic2); // получаем контекст устройства окна hWnd 
		GetClientRect(hStatic2, &rect2); // получить координаты графического окна 

		lx = rect.right;
		ly = rect.bottom;
		lx1 = rect1.right;
		ly1 = rect1.bottom;
		lx2 = rect2.right;
		ly2 = rect2.bottom;

		// создать в памяти контекст устройства hdcm, 
		// совместимый с текущим контекстом устройства вывода hdc. 
		hdcm = CreateCompatibleDC(hdc); // Создать холст в памяти заданного размера 
		hbm = CreateCompatibleBitmap(hdc, lx, ly);

		hdcm1 = CreateCompatibleDC(hdc1); // Создать холст в памяти заданного размера 
		hbm1 = CreateCompatibleBitmap(hdc1, lx1, ly1);

		hdcm2 = CreateCompatibleDC(hdc2); // Создать холст в памяти заданного размера 
		hbm2 = CreateCompatibleBitmap(hdc2, lx2, ly2);

		// привязать к контексту устройства: 
		SelectObject(hdcm, hbm); // холст 
		SelectObject(hdcm, (HBRUSH)GetStockObject(0)); // кисть 

		SelectObject(hdcm1, hbm1); // холст 
		SelectObject(hdcm1, (HBRUSH)GetStockObject(WHITE_BRUSH)); // кисть 
		SelectObject(hdcm1, (HPEN)GetStockObject(BLACK_PEN)); // перо 
		PatBlt(hdcm1, 0, 0, lx1, ly1, PATCOPY); // область показа примененных атрибутов
		Rectangle(hdcm1, 21, 25, 81, 70);

		// закрашивает прямоугольную область с координатами(x, y), шириной width // и высотой height, используя текущую кисть и растровую операцию dwRaster 
		// PATCOPY - область заполняется текущей кистью; 
		PatBlt(hdcm, 0, 0, lx, ly, PATCOPY); // большая область
		PatBlt(hdcm2, 0, 0, lx2, ly2, PATCOPY); // small view
		InvalidateRect(hWnd, NULL, false);

		// для InvalidateRect
		rect1.right = rect.right + 11;
		rect1.bottom = rect.bottom + rect.bottom;
		rect2.right = rect.right + rect.right;
		rect2.bottom = rect.bottom + 11;

		break;
	}
	//Обработка сообщения прокрутки по горизонтали
	case WM_HSCROLL: {
		switch (LOWORD(wParam)) {
		case SB_LINERIGHT: //На одну линию вправо
			if (GetDlgItem(hWnd, IDC_PEN) == (HWND)lParam) { pen++; }
			if (GetDlgItem(hWnd, IDC_BRUSH) == (HWND)lParam) { brush++; }
			if (GetDlgItem(hWnd, IDC_COLOR) == (HWND)lParam) { color++; }
			if (GetDlgItem(hWnd, IDC_STYLE) == (HWND)lParam) { style--; }
			if (GetDlgItem(hWnd, IDC_WIDTH) == (HWND)lParam) { width++; }
			break;

		case SB_LINELEFT: //На линию влево
			if (GetDlgItem(hWnd, IDC_PEN) == (HWND)lParam) { pen--; }
			if (GetDlgItem(hWnd, IDC_BRUSH) == (HWND)lParam) { brush--; }
			if (GetDlgItem(hWnd, IDC_COLOR) == (HWND)lParam) { color--; }
			if (GetDlgItem(hWnd, IDC_STYLE) == (HWND)lParam) { style++; }
			if (GetDlgItem(hWnd, IDC_WIDTH) == (HWND)lParam) { width--; }
			break;

		default: break;
		}

		SelectObject(hdcm1, CreateSolidBrush(RGB(255, 255, 255)));
		PatBlt(hdcm1, 0, 0, lx1, ly1, PATCOPY);
		if (brush == 1) { r = 255; g = 255; b = 255; }
		if (brush == 2) { r = 255; g = 0; b = 0; }
		if (brush == 3) { r = 0; g = 255; b = 0; }
		if (brush == 4) { r = 0; g = 0; b = 255; }
		SelectObject(hdcm, CreateHatchBrush(style - 1, RGB(r, g, b)));
		SelectObject(hdcm1, CreateHatchBrush(style - 1, RGB(r, g, b)));

		if (style == 7) { SelectObject(hdcm, CreateSolidBrush(RGB(r, g, b))); 
		SelectObject(hdcm1, CreateSolidBrush(RGB(r, g, b))); }

		if (color == 1) { red = 0; green = 0; blue = 0; }
		if (color == 2) { red = 255; green = 0; blue = 0; }
		if (color == 3) { red = 0; green = 255; blue = 0; }
		if (color == 4) { red = 0; green = 0; blue = 255; }
		usPen = CreatePen(pen - 1, width - 1, RGB(red, green, blue));
		SelectObject(hdcm, usPen);
		SelectObject(hdcm1, usPen);

		if (pen > 5) { pen = 5; } if (pen < 1) { pen = 1; }
		if (brush > 4) { brush = 4; } if (brush < 1) { brush = 1; }
		if (color > 4) { color = 4; } if (color < 1) { color = 1; }
		if (style > 7) { style = 7; } if (style < 1) { style = 1; }
		if (width > 5) { width = 5; } if (width < 1) { width = 1; }

		Rectangle(hdcm1, 21, 25, 81, 70);
		InvalidateRect(hWnd, &rect1, false);

		break;
	}
	case WM_LBUTTONDOWN: {

		if (LOWORD(lParam) >= 11 && LOWORD(lParam) <= lx + 11 && HIWORD(lParam) >= 11 && HIWORD(lParam) <= ly + 11) {
			if (CheckDraw == "") { MessageBox(hWnd, L"Выберите тип фигуры для рисования", L"Caption text", MB_OK); };
			if (CheckDraw == "ID_LINE" || CheckDraw == "ID_ELLIPS" || CheckDraw == "ID_RECTANGLE") {
				CountPointLER++;
				if (CountPointLER == 1) { pt[0].x = LOWORD(lParam) - 11; pt[0].y = HIWORD(lParam) - 11; }
				else if (CountPointLER == 2) { pt[1].x = LOWORD(lParam) - 11; pt[1].y = HIWORD(lParam) - 11; }
				else MessageBox(hWnd, L"Вы за максимальное кол-во точек (2)", L"Caption text", MB_OK);
			}
			if (CheckDraw == "ID_CHORD" || CheckDraw == "ID_ARC" || CheckDraw == "ID_PIE") {
				CountPointCAP++;
				if (CountPointCAP == 1) { pt[0].x = LOWORD(lParam) - 11; pt[0].y = HIWORD(lParam) - 11; }
				else if (CountPointCAP == 2) { pt[1].x = LOWORD(lParam) - 11; pt[1].y = HIWORD(lParam) - 11; }
				else if (CountPointCAP == 3) { pt[2].x = LOWORD(lParam) - 11; pt[2].y = HIWORD(lParam) - 11; }
				else if (CountPointCAP == 4) { pt[3].x = LOWORD(lParam) - 11; pt[3].y = HIWORD(lParam) - 11; }
				else MessageBox(hWnd, L"Вы выбрали максимальное кол-во точек (4)", L"Caption text", MB_OK);
			}
			if (CheckDraw == "ID_FOCUSRECT") {
				CountPointF++;
				if (CountPointF == 1) { FRect.left = LOWORD(lParam) - 11; FRect.top = HIWORD(lParam) - 11; }
				else if (CountPointF == 2) { FRect.right = LOWORD(lParam) - 11; FRect.bottom = HIWORD(lParam) - 11; }
				else MessageBox(hWnd, L"Вы выбрали максимальное кол-во точек (2)", L"Caption text", MB_OK);
			}
			if (CheckDraw == "ID_POLYGON" || CheckDraw == "ID_POLYLINE") {
				CountPointPP++;
				if (CountPointPP != 9) { ptpp[CountPointPP - 1].x = LOWORD(lParam) - 11; ptpp[CountPointPP - 1].y = HIWORD(lParam) - 11; }
				else MessageBox(hWnd, L"Вы выбрали максимальное кол-во точек (8)", L"Caption text", MB_OK);
			}
		}
		
		/*
		point.x = LOWORD(lParam) - 11; point.y = HIWORD(lParam) - 11;
		MessageBox(hWnd, to_wstring(point.x).data(), L"Caption text", MB_OK);
		MessageBox(hWnd, to_wstring(point.y).data(), L"Caption text", MB_OK);
		*/
		
		break;
	}
	case WM_PAINT: {
		HDC hdc = BeginPaint(hWnd, &ps);

		BitBlt(hdc, 11, 11, lx, ly, hdcm, 0, 0, SRCCOPY); // Вывод битового изображения 
		BitBlt(hdc, 186, 201, lx1, ly1, hdcm1, 0, 0, SRCCOPY); // Вывод битового изображения 
		StretchBlt(hdc, 306, 91, lx2, ly2, hdc, 11, 11, lx, ly, SRCCOPY);
		
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam))
		{
		case ID_LINE: { CheckDraw = "ID_LINE"; break; }
		case ID_ELLIPS: { CheckDraw = "ID_ELLIPS"; break; }
		case ID_FOCUSRECT: { CheckDraw = "ID_FOCUSRECT"; break; }
		case ID_CHORD: { CheckDraw = "ID_CHORD"; break; }
		case ID_PIE: { CheckDraw = "ID_PIE"; break; }
		case ID_ARC: { CheckDraw = "ID_ARC"; break; }
		case ID_POLYGON: { CheckDraw = "ID_POLYGON"; break; }
		case ID_POLYLINE: { CheckDraw = "ID_POLYLINE"; break; }
		case ID_RECTANGLE: { CheckDraw = "ID_RECTANGLE"; break; }
		case IDC_CHECK1: { 
			if (IsDlgButtonChecked(hWnd, IDC_CHECK1)) { SetPolyFillMode(hdcm, WINDING); } 
			else SetPolyFillMode(hdcm, ALTERNATE); 
			break;
		}

		case EXECUTE: {
			if (CheckDraw == "ID_LINE" && CountPointLER >= 2) { MoveToEx(hdcm, pt[1].x, pt[1].y, NULL); LineTo(hdcm, pt[0].x, pt[0].y); }
			else if (CheckDraw == "ID_ELLIPS" && CountPointLER >= 2) { Ellipse(hdcm, pt[0].x, pt[0].y, pt[1].x, pt[1].y); }
			else if (CheckDraw == "ID_RECTANGLE" && CountPointLER >= 2) { Rectangle(hdcm, pt[0].x, pt[0].y, pt[1].x, pt[1].y); }
			else if (CheckDraw == "ID_FOCUSRECT" && CountPointF >= 2) { DrawFocusRect(hdcm, &FRect); }
			else if (CheckDraw == "ID_CHORD" && CountPointCAP >= 4) { Chord(hdcm, pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y, pt[3].x, pt[3].y); }
			else if (CheckDraw == "ID_PIE" && CountPointCAP >= 4) { Pie(hdcm, pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y, pt[3].x, pt[3].y); }
			else if (CheckDraw == "ID_ARC" && CountPointCAP >= 4) { Arc(hdcm, pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y, pt[3].x, pt[3].y); }
			else if (CheckDraw == "ID_POLYLINE") { Polyline(hdcm, ptpp, CountPointPP - 1); }
			else if (CheckDraw == "ID_POLYGON") { Polygon(hdcm, ptpp, CountPointPP - 1); }
			else MessageBox(hWnd, L"Недостатоно точек для построения фигуры", L"Caption text", MB_OK);

			InvalidateRect(hWnd, &rect1, false);
			CountPointLER = 0; CountPointCAP = 0; CountPointF = 0; CountPointPP = 0; 
			break;
		}
		case SMALLV: {
			InvalidateRect(hWnd, &rect2, false);
			break;
		}
		case IDC_BUTTON2: {
			InvalidateRect(hWnd, NULL, false);
			break;
		}
		default: return FALSE; }
		break;
	}
	default: return FALSE; }
	return TRUE;
}