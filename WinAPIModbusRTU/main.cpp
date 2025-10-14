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

			EnableWindow(ReadCoilsValueAddres, TRUE);
			EnableWindow(ReadCoilsValueCount, TRUE);
			EnableWindow(addReadCoilsButton, TRUE);

			EnableWindow(readHoldingRegistersAddres, TRUE);
			EnableWindow(readHoldingRegistersCount, TRUE);
			EnableWindow(readHoldingRegistersButton, TRUE);

			EnableWindow(writeSingleRegisterAddres, TRUE);
			EnableWindow(writeSingleRegisterValue, TRUE);
			EnableWindow(writeSingleRegisterButton, TRUE);

			EnableWindow(writeSingleCoilAddres, TRUE);
			EnableWindow(writeSingleCoilButtonTrue, TRUE);
			EnableWindow(writeSingleCoilButtonFalse, TRUE);

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


			EnableWindow(ReadCoilsValueAddres, FALSE);
			EnableWindow(ReadCoilsValueCount, FALSE);
			EnableWindow(addReadCoilsButton, FALSE);

			EnableWindow(readHoldingRegistersAddres, FALSE);
			EnableWindow(readHoldingRegistersCount, FALSE);
			EnableWindow(readHoldingRegistersButton, FALSE);

			EnableWindow(writeSingleRegisterAddres, FALSE);
			EnableWindow(writeSingleRegisterValue, FALSE);
			EnableWindow(writeSingleRegisterButton, FALSE);

			EnableWindow(writeSingleCoilAddres, FALSE);
			EnableWindow(writeSingleCoilButtonTrue, FALSE);
			EnableWindow(writeSingleCoilButtonFalse, FALSE);
			//HWND writeSingleRegisterAddres;
//HWND writeSingleRegisterValue;
//HWND writeSingleRegisterButton;

			break;

		case OnClickedSand:
			GetWindowTextA(WriteToConsoleEDIT,Buffer, TextBufferSize);
			if (strlen(Buffer) > 0) {
				addLineToConsole("Ввод:");
				str = Buffer;
				addLineToConsole(str);
				addHRToConsole();
			}

			int ts;
			break;

		case onClickedreadHoldingRegistersButton:


//#define readHoldingRegistersAddresEDIT 13
//#define readHoldingRegistersCountEDIT 14
//#define onClickedreadHoldingRegistersButton 15

			addLineToConsole("Функция Read Holding Registers(x03)");
			addres = GetDlgItemInt(hWnd, readHoldingRegistersAddresEDIT,NULL, false);
			count =  GetDlgItemInt(hWnd, readHoldingRegistersCountEDIT, NULL, false);
			request = ModbusRTU::createReadRequest(SlaveId, addres, count);
			addLineToConsole("Запрос: [" + ModbusRTU::toHexString(request) + " ]");
			if (!modbus.query(request, response, 1000)) { addLineToConsole("Ошибка чтения");  addHRToConsole(); break; };
			addLineToConsole("Ответ: [" + ModbusRTU::toHexString(response) + " ]");
			coils = { 0 };
			registers;
			modbus.parseReadResponse(response, registers);

			addTildaToConsole();
			addLineToConsole("Расшифровка:");
			str = "Регистры:";
			for (auto reg : registers) {
				str += " " + std::to_string(reg);
			}
			addLineToConsole(str);
			addHRToConsole();

			break;

		case onClickedwriteSingleCoilButtonTrue:
			addLineToConsole("Функция Write Single Coil(x05)");
			addres = GetDlgItemInt(hWnd, writeSingleCoilAddresEDIT, NULL, false) ;
			request = ModbusRTU::createWriteSingleCoil(SlaveId, addres, true);
			addLineToConsole("Запрос: [" + ModbusRTU::toHexString(request) + " ]");
			if (!modbus.query(request, response, 1000)) { addLineToConsole("Ошибка чтения");  addHRToConsole(); break; };
			addLineToConsole("Ответ: [" + ModbusRTU::toHexString(response) + " ]");
			addLineToConsole("Значение записанно!");
			addHRToConsole();
			


			break;

		case onClickedwriteSingleCoilButtonFalse:
			addLineToConsole("Функция Write Single Coil(x05)");
			addres = GetDlgItemInt(hWnd, writeSingleCoilAddresEDIT, NULL, false) ;
			request = ModbusRTU::createWriteSingleCoil(SlaveId, addres, false);
			addLineToConsole("Запрос: [" + ModbusRTU::toHexString(request) + " ]");
			if (!modbus.query(request, response, 1000)) { addLineToConsole("Ошибка чтения");  addHRToConsole(); break; };
			addLineToConsole("Ответ: [" + ModbusRTU::toHexString(response) + " ]");
			addLineToConsole("Значение записанно!");
			addHRToConsole();
			break;

		case OnClickedReadCoils:
			addLineToConsole("Функция ReadCoils 0x01");
			addres = GetDlgItemInt(hWnd, ReadCoilsValueAddresEDIT, NULL, false) ;
			count = GetDlgItemInt(hWnd, ReadCoilsValueCountEDIT, NULL, false);
			request = ModbusRTU::createReadCoils(SlaveId, addres, count);
			addLineToConsole("Запрос: [" + ModbusRTU::toHexString(request) + " ]");
			if (!modbus.query(request, response, 1000)) { addLineToConsole("Ошибка чтения");  addHRToConsole(); break;};
			addLineToConsole("Ответ: [" + ModbusRTU::toHexString(response) + " ]");
			coils={ 0 };
			ModbusRTU::parseReadCoilsResponse(response, coils);
			addTildaToConsole();
			addLineToConsole("Расшифровка:");
			
			addLineToConsole(vectorBoolToCoilsString(coils));
			addHRToConsole();
			addres = 0;
			count = 0;
			request = { 0 };
			response = { 0 };

			break;

		case onClickedwriteSingleRegisterButton:

			//HWND writeSingleRegisterAddres;
			//HWND writeSingleRegisterValue;
			//HWND writeSingleRegisterButton;
			//
			//#define writeSingleRegisterAddresEDIT 19
			//#define writeSingleRegisterValueEDIT 20
			//#define onClickedwriteSingleRegisterButton 21

			addLineToConsole("Функция Write Single Register 0x06");
			addres = GetDlgItemInt(hWnd, writeSingleRegisterAddresEDIT, NULL, false);
			value = GetDlgItemInt(hWnd, writeSingleRegisterValueEDIT, NULL, false);
			request = ModbusRTU::createWriteSingleRegister(SlaveId, addres, value);
			addLineToConsole("Запрос: [" + ModbusRTU::toHexString(request) + " ]");
			if (!modbus.query(request, response, 1000)) { addLineToConsole("Ошибка чтения");  addHRToConsole(); break; };
			addLineToConsole("Ответ: [" + ModbusRTU::toHexString(response) + " ]");
			uint16_t uintAddres = static_cast<uint16_t>(addres);
			uint16_t uintValue = static_cast<uint16_t>(value);
			if (ModbusRTU::parseWriteResponse(response, uintAddres, uintValue)) {
				addLineToConsole("Регистры записанны!");
			}
			else {
				addLineToConsole("Регистры не записанны!");
			}
			addHRToConsole();

			break;

		}
		break;
	case WM_CREATE:
		{
		
		MainWndAddMenus(hWnd);
		MainWndAddWidgets(hWnd);
		EnableWindow(disconnectButton, FALSE);

		EnableWindow(ReadCoilsValueAddres, FALSE);
		EnableWindow(ReadCoilsValueCount, FALSE);
		EnableWindow(addReadCoilsButton, FALSE);

		//HWND readHoldingRegistersAddres;
		//HWND readHoldingRegistersCount;
		//HWND readHoldingRegistersButton;

		//HWND writeSingleCoilAddres;
		//HWND writeSingleCoilButtonTrue;
		//HWND writeSingleCoilButtonFalse;

		EnableWindow(readHoldingRegistersAddres, FALSE);
		EnableWindow(readHoldingRegistersCount, FALSE);
		EnableWindow(readHoldingRegistersButton, FALSE);

		EnableWindow(writeSingleRegisterAddres, FALSE);
		EnableWindow(writeSingleRegisterValue, FALSE);
		EnableWindow(writeSingleRegisterButton, FALSE);

		EnableWindow(writeSingleCoilAddres, FALSE);
		EnableWindow(writeSingleCoilButtonTrue, FALSE);
		EnableWindow(writeSingleCoilButtonFalse, FALSE);

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
	//WriteToConsoleEDIT = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 5, 498 - 5 -20, 1190, 20,hWnd, NULL, NULL, NULL);

	//CreateWindowA("button", "Отправить!", WS_VISIBLE | WS_CHILD, 205, 138, 100, 20, hWnd, (HMENU)OnClickedSand, NULL, NULL);


	writeSingleCoil(hWnd);
	std::string selectPortStr = "COM" + std::to_string(selectPort);
	SelecPortLable = CreateWindowA("static", selectPortStr.c_str(), WS_VISIBLE | WS_CHILD, 125, 35, 45, 30, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "|  Скорость:", WS_VISIBLE | WS_CHILD, 170, 35, 200, 30, hWnd, NULL, NULL, NULL);

	CreateWindowA("static", "ReadCoils(x01)", WS_VISIBLE | WS_CHILD, 450, 0, 200, 30, hWnd, NULL, NULL, NULL);
	addReadCoils(hWnd);
	CreateCombobox(hWnd);

	readHoldingRegisters(hWnd);

	writeSingleRegister(hWnd);

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

void addTildaToConsole() {
	addLineToConsole1(std::string(233, '~'));
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


void addReadCoils(HWND hWnd) {
	CreateWindowA("static", "Read Coils(x01)", WS_VISIBLE | WS_CHILD, 450, 0, 120, 30, hWnd, NULL, NULL, NULL);
	ReadCoilsValueAddres = CreateWindowA("edit", "1", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 450, 30, 120, 20, hWnd, (HMENU)ReadCoilsValueAddresEDIT, NULL, NULL);
	CreateWindowA("static", "Адрес:", WS_VISIBLE | WS_CHILD, 400, 30, 50, 30, hWnd, NULL, NULL, NULL);
	ReadCoilsValueCount = CreateWindowA("edit", "8", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 450, 55, 120, 20, hWnd, (HMENU)ReadCoilsValueCountEDIT, NULL, NULL);
	CreateWindowA("static", "Количество:", WS_VISIBLE | WS_CHILD, 363, 55, 84, 30, hWnd, NULL, NULL, NULL);
	addReadCoilsButton = CreateWindowA("button", "Отправить!", WS_VISIBLE | WS_CHILD, 450, 85, 129, 20, hWnd, (HMENU)OnClickedReadCoils, NULL, NULL);
}

void readHoldingRegisters(HWND hWnd) {

	//HWND readHoldingRegistersAddres;
	//HWND readHoldingRegistersCount;
	//HWND readHoldingRegistersButton;

	CreateWindowA("static", "Read Holding Registers(x03)", WS_VISIBLE | WS_CHILD, 450, 150, 125, 32, hWnd, NULL, NULL, NULL);
	readHoldingRegistersAddres = CreateWindowA("edit", "1", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 450, 187, 120, 20, hWnd, (HMENU)readHoldingRegistersAddresEDIT, NULL, NULL);
	CreateWindowA("static", "Адрес:", WS_VISIBLE | WS_CHILD, 400, 187, 50, 30, hWnd, NULL, NULL, NULL);
	readHoldingRegistersCount = CreateWindowA("edit", "8", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 450, 212, 120, 20, hWnd, (HMENU)readHoldingRegistersCountEDIT, NULL, NULL);
	CreateWindowA("static", "Количество:", WS_VISIBLE | WS_CHILD, 363, 212, 84, 30, hWnd, NULL, NULL, NULL);
	readHoldingRegistersButton = CreateWindowA("button", "Отправить!", WS_VISIBLE | WS_CHILD, 450, 212+30, 129, 20, hWnd, (HMENU)onClickedreadHoldingRegistersButton, NULL, NULL);
}

std::string vectorBoolToCoilsString(const std::vector<bool>& vec) {
	std::string result;

	for (size_t i = 0; i < vec.size(); ++i) {
		result += "coil[" + std::to_string(i+1) + "] = " + (vec[i] ? "1" : "0");
		if (i < vec.size() - 1) {
			result += "; \r\n";
		}
	}

	return result;
}

void writeSingleCoil(HWND hWnd) {
	//HWND writeSingleCoilAddres;
	//HWND writeSingleCoilButtonTrue;
	//HWND writeSingleCoilButtonFalse;

	CreateWindowA("static", "Write Single Coil(x05)", WS_VISIBLE | WS_CHILD, 700, 0, 160, 30, hWnd, NULL, NULL, NULL);
	writeSingleCoilAddres = CreateWindowA("edit", "1", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 700, 30, 120, 20, hWnd, (HMENU)writeSingleCoilAddresEDIT, NULL, NULL);
	writeSingleCoilButtonTrue = CreateWindowA("button", "Вкл.", WS_VISIBLE | WS_CHILD, 700, 55, 50, 21, hWnd, (HMENU)onClickedwriteSingleCoilButtonTrue, NULL, NULL);
	writeSingleCoilButtonFalse = CreateWindowA("button", "Выкл.", WS_VISIBLE | WS_CHILD, 769, 55, 51, 20, hWnd, (HMENU)onClickedwriteSingleCoilButtonFalse, NULL, NULL);


}

//HWND writeSingleRegisterAddres;
//HWND writeSingleRegisterValue;
//HWND writeSingleRegisterButton;
//
//#define writeSingleRegisterAddresEDIT 19
//#define writeSingleRegisterValueEDIT 20
//#define onClickedwriteSingleRegisterButton 21

void writeSingleRegister(HWND hWnd) {
	CreateWindowA("static", "Write Single Register(x06)", WS_VISIBLE | WS_CHILD, 700, 150, 160, 32, hWnd, NULL, NULL, NULL);
	writeSingleRegisterAddres = CreateWindowA("edit", "1", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 700, 187, 120, 20, hWnd, (HMENU)writeSingleRegisterAddresEDIT, NULL, NULL);
	CreateWindowA("static", "Адрес:", WS_VISIBLE | WS_CHILD, 650, 187, 50, 30, hWnd, NULL, NULL, NULL);
	writeSingleRegisterValue = CreateWindowA("edit", "110", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 700, 212, 120, 20, hWnd, (HMENU)writeSingleRegisterValueEDIT, NULL, NULL);
	CreateWindowA("static", "Значение:", WS_VISIBLE | WS_CHILD, 637-10-2, 212, 80-5-1, 30, hWnd, NULL, NULL, NULL);
	writeSingleRegisterButton = CreateWindowA("button", "Отправить!", WS_VISIBLE | WS_CHILD, 700, 212 + 30, 129, 20, hWnd, (HMENU)onClickedwriteSingleRegisterButton, NULL, NULL);
}