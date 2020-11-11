#undef UNICODE
#include "framework.h" 
#include "resource.h" 
#include <string> 
#include <commctrl.h> // tooltip
using namespace std;

HWND DialogHelp;
HWND hWndDialog;
HINSTANCE hInst;

// Описание используемой оконной процедуры 
BOOL CALLBACK PviewDlgProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogBoxHelp(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);

// Главное приложение программы 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	MSG msg;

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

void InfoFigure(HWND hWnd, string info3, string info5) {
	SendMessage(GetDlgItem(hWnd, IDC_INFO5), WM_SETTEXT, NULL, (LPARAM)to_string(0).data());
	SendMessage(GetDlgItem(hWnd, IDC_INFO3), WM_SETTEXT, NULL, (LPARAM)info3.data());
	SendMessage(GetDlgItem(hWnd, IDC_INFO7), WM_SETTEXT, NULL, (LPARAM)info5.data());
}

void CreateToolTip(HWND hwndParent, LPSTR ToolTipText) {
	// Create a tooltip.
	HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST,
		TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hwndParent, NULL, hInst, NULL);

	SetWindowPos(hwndTT, HWND_TOPMOST,
		0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	// Set up "tool" information.
	// In this case, the "tool" is the entire parent window.
	TOOLINFO ti = { 0 };
#if (_WIN32_WINNT >= 0x0501)
	ti.cbSize = TTTOOLINFO_V1_SIZE;
#else
	ti.cbSize = sizeof(TOOLINFO);
#endif
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hwndParent;
	ti.hinst = hInst;
	ti.lpszText = ToolTipText;
	GetClientRect(hwndParent, &ti.rect);

	// Associate the tooltip with the "tool" window.
	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
}

//Процедура обработки сообщений диалогового окна 
BOOL CALLBACK PviewDlgProc(HWND hWnd,
	UINT wMsg,
	WPARAM wParam,
	LPARAM lParam) {

	static string CheckDraw = "";
	static int CountPoint = 0;
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
		
		CreateToolTip(GetDlgItem(hWnd, IDC_PEN), LPSTR("Изменить стиль пера"));
		CreateToolTip(GetDlgItem(hWnd, IDC_COLOR), LPSTR("Изменить цвет пера"));
		CreateToolTip(GetDlgItem(hWnd, IDC_STYLE), LPSTR("Изменить стиль заливки"));
		CreateToolTip(GetDlgItem(hWnd, IDC_BRUSH), LPSTR("Изменить цвет заливки"));
		CreateToolTip(GetDlgItem(hWnd, IDC_WIDTH), LPSTR("Изменить ширину пера"));
		CreateToolTip(GetDlgItem(hWnd, EXECUTE), LPSTR("Нарисовать выбранную фигуру"));
		CreateToolTip(GetDlgItem(hWnd, SMALLV), LPSTR("Отобразить содержимое большого окна в маленьком"));
		break;
	}
	//Обработка сообщения прокрутки по горизонтали
	case WM_HSCROLL: {
		switch (LOWORD(wParam)) {
		case SB_LINERIGHT: // На одну линию вправо
			if (GetDlgItem(hWnd, IDC_PEN) == (HWND)lParam) { pen++; }
			if (GetDlgItem(hWnd, IDC_BRUSH) == (HWND)lParam) { brush++; }
			if (GetDlgItem(hWnd, IDC_COLOR) == (HWND)lParam) { color++; }
			if (GetDlgItem(hWnd, IDC_STYLE) == (HWND)lParam) { style--; }
			if (GetDlgItem(hWnd, IDC_WIDTH) == (HWND)lParam) { width++; }
			break;

		case SB_LINELEFT: // На линию влево
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
			if (CheckDraw == "") { MessageBox(hWnd, "Выберите тип фигуры для рисования", "Caption text", MB_OK); };
			if (CheckDraw == "ID_LINE" || CheckDraw == "ID_ELLIPS" || CheckDraw == "ID_RECTANGLE") { CountPoint++;
				if (CountPoint < 3) {
					SendMessage(GetDlgItem(hWnd, IDC_INFO5), WM_SETTEXT, NULL, (LPARAM)to_string(CountPoint).data());
					pt[CountPoint - 1].x = LOWORD(lParam) - 11; pt[CountPoint - 1].y = HIWORD(lParam) - 11;
				} else MessageBox(hWnd, "Вы выбрали максимальное кол-во точек (2)", "Caption text", MB_OK);
			}
			if (CheckDraw == "ID_CHORD" || CheckDraw == "ID_ARC" || CheckDraw == "ID_PIE") { CountPoint++;
				if (CountPoint < 5) {
					SendMessage(GetDlgItem(hWnd, IDC_INFO5), WM_SETTEXT, NULL, (LPARAM)to_string(CountPoint).data());
					pt[CountPoint - 1].x = LOWORD(lParam) - 11; pt[CountPoint - 1].y = HIWORD(lParam) - 11;
				} else MessageBox(hWnd, "Вы выбрали максимальное кол-во точек (4)", "Caption text", MB_OK);
			}
			if (CheckDraw == "ID_FOCUSRECT") { CountPoint++;
				if (CountPoint < 3) SendMessage(GetDlgItem(hWnd, IDC_INFO5), WM_SETTEXT, NULL, (LPARAM)to_string(CountPoint).data());
				if (CountPoint == 1) { FRect.left = LOWORD(lParam) - 11; FRect.top = HIWORD(lParam) - 11; }
				else if (CountPoint == 2) { FRect.right = LOWORD(lParam) - 11; FRect.bottom = HIWORD(lParam) - 11; }
				else MessageBox(hWnd, "Вы выбрали максимальное кол-во точек (2)", "Caption text", MB_OK);
			}
			if (CheckDraw == "ID_POLYGON" || CheckDraw == "ID_POLYLINE") { CountPoint++;
				if (CountPoint < 9) { 
					SendMessage(GetDlgItem(hWnd, IDC_INFO5), WM_SETTEXT, NULL, (LPARAM)to_string(CountPoint).data()); 
					ptpp[CountPoint - 1].x = LOWORD(lParam) - 11; ptpp[CountPoint - 1].y = HIWORD(lParam) - 11; 
				}
				else MessageBox(hWnd, "Вы выбрали максимальное кол-во точек (8)", "Caption text", MB_OK);
			}
		}
		
		/*
		// для отладки
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
		switch (LOWORD(wParam)) {
		case ID_LINE: { CheckDraw = "ID_LINE"; InfoFigure(hWnd, "Линия", "2"); break; }
		case ID_ELLIPS: { CheckDraw = "ID_ELLIPS"; InfoFigure(hWnd, "Эллипс", "2"); break; }
		case ID_RECTANGLE: { CheckDraw = "ID_RECTANGLE"; InfoFigure(hWnd, "Прямоугольник", "2"); break; }
		case ID_FOCUSRECT: { CheckDraw = "ID_FOCUSRECT"; InfoFigure(hWnd, "Пунктирный прямоугольник", "2"); break; }
		case ID_CHORD: { CheckDraw = "ID_CHORD"; InfoFigure(hWnd, "Сегмент эллипса", "4"); break; }
		case ID_PIE: { CheckDraw = "ID_PIE"; InfoFigure(hWnd, "Сектор эллипса", "4"); break; }
		case ID_ARC: { CheckDraw = "ID_ARC"; InfoFigure(hWnd, "Дуга (часть эллипса)", "4"); break; }
		case ID_POLYGON: { CheckDraw = "ID_POLYGON"; InfoFigure(hWnd, "Произвольный многоугольник", "8"); break; }
		case ID_POLYLINE: { CheckDraw = "ID_POLYLINE"; InfoFigure(hWnd, "Ломанные линии", "8"); break; }
		case ID_HELP: { DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, DialogBoxHelp, 0); break; }
		case ID_CLEAN: { 
			SelectObject(hdcm, (HBRUSH)GetStockObject(WHITE_BRUSH)); // кисть 
			SelectObject(hdcm1, (HBRUSH)GetStockObject(WHITE_BRUSH)); // кисть 
			pen = 1, brush = 1, color = 1, style = 7, width = 1;
			usPen = CreatePen(pen - 1, width - 1, RGB(0, 0, 0));
			SelectObject(hdcm, usPen); SelectObject(hdcm1, usPen);
			PatBlt(hdcm, 0, 0, lx, ly, PATCOPY); // большая область
			PatBlt(hdcm1, 0, 0, lx, ly, PATCOPY); // область атрибутов
			PatBlt(hdcm2, 0, 0, lx2, ly2, PATCOPY); // small view
			Rectangle(hdcm1, 21, 25, 81, 70);
			InvalidateRect(hWnd, NULL, false);
			break; }
		case IDC_CHECK: { 
			if (IsDlgButtonChecked(hWnd, IDC_CHECK)) { SetPolyFillMode(hdcm, WINDING); } 
			else SetPolyFillMode(hdcm, ALTERNATE); 
			break;
		}
		case EXECUTE: {
			if (CheckDraw == "ID_LINE" && CountPoint >= 2) { MoveToEx(hdcm, pt[1].x, pt[1].y, NULL); LineTo(hdcm, pt[0].x, pt[0].y); }
			else if (CheckDraw == "ID_ELLIPS" && CountPoint >= 2) { Ellipse(hdcm, pt[0].x, pt[0].y, pt[1].x, pt[1].y); }
			else if (CheckDraw == "ID_RECTANGLE" && CountPoint >= 2) { Rectangle(hdcm, pt[0].x, pt[0].y, pt[1].x, pt[1].y); }
			else if (CheckDraw == "ID_FOCUSRECT" && CountPoint >= 2) { DrawFocusRect(hdcm, &FRect); }
			else if (CheckDraw == "ID_CHORD" && CountPoint >= 4) { Chord(hdcm, pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y, pt[3].x, pt[3].y); }
			else if (CheckDraw == "ID_PIE" && CountPoint >= 4) { Pie(hdcm, pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y, pt[3].x, pt[3].y); }
			else if (CheckDraw == "ID_ARC" && CountPoint >= 4) { Arc(hdcm, pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y, pt[3].x, pt[3].y); }
			else if (CheckDraw == "ID_POLYLINE") { Polyline(hdcm, ptpp, CountPoint); }
			else if (CheckDraw == "ID_POLYGON") { Polygon(hdcm, ptpp, CountPoint); }
			else MessageBox(hWnd, "Недостатоно точек для построения фигуры", "Caption text", MB_OK);

			InvalidateRect(hWnd, &rect1, false);
			CountPoint = 0; //CountPoint = 0; CountPoint = 0; CountPoint = 0; 
			SendMessage(GetDlgItem(hWnd, IDC_INFO5), WM_SETTEXT, NULL, (LPARAM)to_string(0).data());
			break;
		}
		case SMALLV: { InvalidateRect(hWnd, &rect2, false); break; }
		case IDC_BUTTON2: { InvalidateRect(hWnd, NULL, false); break; }
		default: return FALSE; }
		break;
	}
	default: return FALSE; }
	return TRUE;
}


//Процедура обработки сообщений диалогового окна 
BOOL CALLBACK DialogBoxHelp(HWND hWnd,
	UINT wMsg,
	WPARAM wParam,
	LPARAM lParam) {

	switch (wMsg) {
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;

	case WM_INITDIALOG: {
		//
		break;
	}

	case WM_PAINT: {
		//HDC hdc = BeginPaint(hWnd, &ps);
		//EndPaint(hWnd, &ps);
		break;
	}
	case WM_COMMAND: {
		//
		break;
	}
	default: return FALSE;
	}
	return TRUE;
}