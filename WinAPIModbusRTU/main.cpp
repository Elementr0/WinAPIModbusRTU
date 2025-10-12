#include<Windows.h>
#include"ModbusAPP.h"
#include"ModbusRTU.h"


ModbusRTU modbus;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {

	WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL, IDI_QUESTION), L"MainWndClass", SoftwareMainPRocedure);


	if (!RegisterClassW(&SoftwareMainClass)) { return -1; }

	MSG msg = { 0 };
	CreateWindow(L"MainWndClass", L"ModbusRTU", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, WindowWidth, WindowHeight, NULL, NULL, NULL, NULL);

	while (GetMessage(&msg, NULL, NULL, NULL)) { 
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}


WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure) {
	WNDCLASS NWC = { 0 };
	NWC.hCursor = Cursor;
	NWC.hIcon = Icon;
	NWC.hInstance = hInst;
	NWC.lpszClassName = Name;
	NWC.hbrBackground = BGColor;
	NWC.lpfnWndProc = Procedure;

	return NWC;
}

LRESULT CALLBACK  SoftwareMainPRocedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_COMMAND:

		if ((wp >= ComSelectIndex) && (wp <= ComSelectIndex + ComPortAmount)) {
			selectPort = wp - ComSelectIndex;
			updateSerialSelectLable();
			
			SerialUpdate();
		}

		if (LOWORD(wp) == IdCombobox && HIWORD(wp) == CBN_SELCHANGE) {
			// Получаем выбранный индекс
			selectComboBoxIndex = SendMessage(hCombobox, CB_GETCURSEL, 0, 0);
		}


		switch (wp) {
		case OnMenuClicked: MessageBox(hWnd, L"-", L"-", MB_OK);
			break;
		case OnExitMenuClicked:
			PostQuitMessage(0);
			modbus.~ModbusRTU();
			break;

		case OnSerialRefresh: SerialUpdate(); updateSerialSelectLable(); break;

		case OnClickedConnect: {
			modbus.close();
			EnableWindow(disconnectButton, TRUE);
			SlaveId = GetDlgItemInt(hWnd, ID_SLAVE_EDIT, NULL, false);
			std::string serialPorttoCoonect = "COM" + std::to_string(selectPort);

			addLineToConsole("Подключение к порту: " + serialPorttoCoonect);
			addLineToConsole("Скорость: " + std::to_string(IdndexComboxTobaudRate(selectComboBoxIndex)));
			addLineToConsole("SlaveID: " + std::to_string(SlaveId));
			bool success = modbus.open(serialPorttoCoonect.c_str(), IdndexComboxTobaudRate(selectComboBoxIndex));
			if(success){ isConectedLabel(); 
			EnableWindow(connectButton, FALSE);
			}
			addLineToConsole("Подключино");
			addHRToConsole();
			break;
		}
		case OnClickedDisconnect:
			addLineToConsole("Отключение...");
			modbus.close();
			EnableWindow(connectButton, TRUE);
			isConectedLabel();
			EnableWindow(disconnectButton, FALSE);
			addLineToConsole("Отключино");
			addHRToConsole();
			break;

		case OnClickedSand:
			GetWindowTextA(WriteToConsoleEDIT,Buffer, TextBufferSize);
			if (strlen(Buffer) > 0) {
				addLineToConsole("Ввод:");
				str = Buffer;
				addLineToConsole(str);
				addHRToConsole();
			}
			test2();

			int ts;
			break;
		default: break;
		}
		break;
	case WM_CREATE:
		{
		
		MainWndAddMenus(hWnd);
		MainWndAddWidgets(hWnd);
		EnableWindow(disconnectButton, FALSE);
		SerialUpdate();
		break;
		}




	case WM_DESTROY:
		PostQuitMessage(0);
		modbus.~ModbusRTU();
		break;

	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
}

void MainWndAddMenus(HWND hWnd) {
	HMENU RootMenu = CreateMenu();
	HMENU SubMenu = CreateMenu();

	ComPortSubMenu = CreateMenu();
	ComPortListMenu = CreateMenu();
	
	AppendMenu(SubMenu, MF_POPUP, (UINT_PTR)ComPortSubMenu, L"Подключение");

	AppendMenu(SubMenu, MF_STRING, OnAboutMenuClicked, L"Информация");
	AppendMenu(SubMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(SubMenu, MF_STRING, OnExitMenuClicked, L"Выход");

	
	AppendMenu(ComPortSubMenu, MF_POPUP, (UINT_PTR)ComPortListMenu, L"Выбрать порт");
	AppendMenu(ComPortSubMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(ComPortSubMenu, MF_STRING, OnSerialRefresh, L"Обновить порты");

	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"Меню");
	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)ComPortSubMenu, L"Подключить устройство");

	SetMenu(hWnd, RootMenu);
}

void MainWndAddWidgets(HWND hWnd) {
	CreateWindowA("static", "ModBusRTU", WS_VISIBLE | WS_CHILD | ES_LEFT, 140, 5, 350, 30, hWnd, NULL, NULL, NULL);
	
	
	CreateWindowA("static", "Выбранный порт:", WS_VISIBLE | WS_CHILD, 5, 35, 120, 30, hWnd, NULL, NULL, NULL);
	connectButton = CreateWindowA("button", "Подключиться", WS_VISIBLE | WS_CHILD, 5, 75, 120, 30, hWnd, (HMENU)OnClickedConnect, NULL, NULL);
	disconnectButton = CreateWindowA("button", "Отключится", WS_VISIBLE | WS_CHILD, 130, 75, 120, 30, hWnd, (HMENU)OnClickedDisconnect, NULL, NULL);
	CreateWindowA("static", "Состояние:", WS_VISIBLE | WS_CHILD, 5,110, 120, 30, hWnd, (HMENU)12345, NULL, NULL);
	SelecPortStateLable = CreateWindowA("static", "Не подключен", WS_VISIBLE | WS_CHILD, 100, 110, 120, 20, hWnd, NULL, NULL, NULL);

	CreateWindowA("static", "SlaveID:", WS_VISIBLE | WS_CHILD, 5, 140, 120, 30, hWnd, NULL, NULL, NULL);


	SlaveIDEdit = CreateWindowA("edit", "1", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 64, 138, 120, 20, hWnd, (HMENU)ID_SLAVE_EDIT, NULL, NULL);



	/*Console = CreateWindowA("static", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL, 5, 200, 574, 241-5, hWnd, (HMENU)ConsoleText, NULL, NULL);*/
	Console = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL | ES_AUTOVSCROLL, 5, 498, 1190, 241 - 5, hWnd, (HMENU)ConsoleText, NULL, NULL);
	WriteToConsoleEDIT = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 5, 498 - 5 -20, 1190, 20,hWnd, NULL, NULL, NULL);

	CreateWindowA("button", "Отправить!", WS_VISIBLE | WS_CHILD, 205, 138, 100, 20, hWnd, (HMENU)OnClickedSand, NULL, NULL);



	std::string selectPortStr = "COM" + std::to_string(selectPort);
	SelecPortLable = CreateWindowA("static", selectPortStr.c_str(), WS_VISIBLE | WS_CHILD, 125, 35, 45, 30, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "|  Скорость:", WS_VISIBLE | WS_CHILD, 170, 35, 200, 30, hWnd, NULL, NULL, NULL);

	CreateWindowA("static", "ReadCoils(x01)", WS_VISIBLE | WS_CHILD, 450, 0, 200, 30, hWnd, NULL, NULL, NULL);
	CreateCombobox(hWnd);
	//CreateGring(hWnd);
	
}

void SerialUpdate() {
	while (RemoveMenu(ComPortListMenu, 0, MF_BYPOSITION));
	int radioLast = 0, radioCurrent = -1;

	for (int i = 1; i < ComPortAmount; i++) {
		HANDLE port = CreateFileA(
			("\\\\.\\COM" + std::to_string(i)).c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		if (port != INVALID_HANDLE_VALUE) {
			AppendMenuA(ComPortListMenu, MF_STRING, ComSelectIndex + i, ("COM" + std::to_string(i)).c_str());
			if (i == selectPort) { radioCurrent = radioLast; }
			++radioLast;
		}
		CloseHandle(port);
	}
	if (radioLast) { --radioLast; }
	if (radioCurrent != -1) { CheckMenuItem(ComPortListMenu, radioCurrent, MF_BYPOSITION | MF_CHECKED); }
}

void updateSerialSelectLable() {
	std::string selectPortStr = "COM" + std::to_string(selectPort);
	SetWindowTextA(SelecPortLable, selectPortStr.c_str());
}

void isConectedLabel() {
	if (modbus.isConnected()) {
		std::string isConectedLabelText = "Подключен!";
		SetWindowTextA(SelecPortStateLable, isConectedLabelText.c_str());
	}
	else {
		std::string isConectedLabelText = "Не подключен!!!";
		SetWindowTextA(SelecPortStateLable, isConectedLabelText.c_str());
	}
}

void CreateCombobox(HWND hWnd) {

	hCombobox = CreateWindowA("COMBOBOX", "", WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL, 250, 32, 80, 200, hWnd, (HMENU)IdCombobox, NULL, NULL);

	SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)ComboBoxValie1.c_str());
	SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)ComboBoxValie2.c_str());
	SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)ComboBoxValie3.c_str());
	SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)ComboBoxValie4.c_str());
	SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)ComboBoxValie5.c_str());
	SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)ComboBoxValie6.c_str());
	SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)ComboBoxValie7.c_str());
	SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)ComboBoxValie8.c_str());
	SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)ComboBoxValie9.c_str());
	SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)ComboBoxValie10.c_str());
	SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)ComboBoxValie11.c_str());
	SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)ComboBoxValie12.c_str());
	SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)ComboBoxValie13.c_str());
	SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)ComboBoxValie14.c_str());
	SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)ComboBoxValie15.c_str());
	SendMessage(hCombobox, CB_SETCURSEL, 6, 0);
}

void addLineToConsole(std::string str) {
	// Добавляем символы новой строки для переноса
	str += "\r\n";
	// Получаем текущую длину текста в консоли
	int textLength = GetWindowTextLengthA(Console);

	// Устанавливаем курсор в конец текста
	SendMessageA(Console, EM_SETSEL, textLength, textLength);

	// Добавляем новый текст
	SendMessageA(Console, EM_REPLACESEL, FALSE, (LPARAM)str.c_str());

	// Прокручиваем к каретке (вниз)
	SendMessageA(Console, EM_SCROLLCARET, 0, 0);
}

void addLineToConsole1(std::string str) {
	// Добавляем символы новой строки для переноса
	str += "\r\n";

	// Получаем текущую длину текста в консоли
	int textLength = GetWindowTextLengthA(Console);

	// Устанавливаем курсор в конец текста
	SendMessageA(Console, EM_SETSEL, textLength, textLength);

	// Добавляем новый текст
	SendMessageA(Console, EM_REPLACESEL, FALSE, (LPARAM)str.c_str());

	// Прокручиваем к каретке (вниз)
	SendMessageA(Console, EM_SCROLLCARET, 0, 0);
}

void addHRToConsole() {
	addLineToConsole1(std::string(292, '-'));
}

int IdndexComboxTobaudRate(int index) {
	switch (index) {
	case 0:
		return 110;
		break;
	case 1:
		return 300;
		break;
	case 2:
		return 600;
		break;
	case 3:
		return 1200;
		break;
	case 4:
		return 2400;
		break;
	case 5:
		return 4800;
		break;
	case 6:
		return 9600;
		break;
	case 7:
		return 14400;
		break;
	case 8:
		return 19200;
		break;
	case 9:
		return 38400;
		break;
	case 10:
		return 56000;
		break;
	case 11:
		return 57600;
		break;
	case 12:
		return 115200;
		break;
	case 13:
		return 128000;
		break;
	case 14:
		return 256000;
		break;
	default: return 9600; break;
	}
}

std::string test() {
	auto request = ModbusRTU::createReadCoils(SlaveId, 0, 8);
	modbus.query(request, dataResponse);
	return ModbusRTU::toHexString(dataResponse);
}


void test2() {

	addLineToConsole("Функция ModbusRTU::createReadCoils(1, 0, 8)");
	auto request = ModbusRTU::createReadCoils(1, 0, 8);
	addLineToConsole(ModbusRTU::toHexString(request));

	addHRToConsole();

	addLineToConsole("Функция createWriteSingleRegister(1, 5, 1234)");
	request = ModbusRTU::createWriteSingleRegister(1, 5, 1234);
	addLineToConsole(ModbusRTU::toHexString(request));

	addHRToConsole();

	addLineToConsole("Функция createWriteSingleCoil(1, 3, true)");
	request = ModbusRTU::createWriteSingleCoil(1, 3, true);
	addLineToConsole(ModbusRTU::toHexString(request));

	addHRToConsole();

	std::vector<uint16_t> values = { 100, 200, 300, 400 };
	addLineToConsole("Функция createWriteMultipleRegisters(1, 10, values)");
	request = ModbusRTU::createWriteMultipleRegisters(1, 10, values);
	addLineToConsole(ModbusRTU::toHexString(request));

	addHRToConsole();

	addLineToConsole("Функция createReadRequest(1, 1, 1)");
	request = ModbusRTU::createReadRequest(1, 1, 1);
	addLineToConsole(ModbusRTU::toHexString(request));
}

void CreateGring(HWND hWnd) {
	CreateWindowA("static", "x0", WS_VISIBLE | WS_CHILD, 0, 0, 25, 20, hWnd, NULL, NULL, NULL);

	CreateWindowA("static", "x100", WS_VISIBLE | WS_CHILD, 100, 0, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "x200", WS_VISIBLE | WS_CHILD, 200, 0, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "x300", WS_VISIBLE | WS_CHILD, 300, 0, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "x400", WS_VISIBLE | WS_CHILD, 400, 0, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "x500", WS_VISIBLE | WS_CHILD, 500, 0, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "x600", WS_VISIBLE | WS_CHILD, 600, 0, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "x700", WS_VISIBLE | WS_CHILD, 700, 0, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "x800", WS_VISIBLE | WS_CHILD, 800, 0, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "x900", WS_VISIBLE | WS_CHILD, 900, 0, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "x1000", WS_VISIBLE | WS_CHILD, 1000, 0, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "x1100", WS_VISIBLE | WS_CHILD, 1100, 0, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "x1200", WS_VISIBLE | WS_CHILD, 1200, 0, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "x1300", WS_VISIBLE | WS_CHILD, 1300, 0, 200, 20, hWnd, NULL, NULL, NULL);


	CreateWindowA("static", "y100", WS_VISIBLE | WS_CHILD, 0, 100, 26, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "y200", WS_VISIBLE | WS_CHILD, 0, 200, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "y300", WS_VISIBLE | WS_CHILD, 0, 300, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "y400", WS_VISIBLE | WS_CHILD, 0, 400, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "y500", WS_VISIBLE | WS_CHILD, 0, 500, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "y600", WS_VISIBLE | WS_CHILD, 0, 600, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "y700", WS_VISIBLE | WS_CHILD, 0, 700, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "y800", WS_VISIBLE | WS_CHILD, 0, 800, 200, 20, hWnd, NULL, NULL, NULL);
}