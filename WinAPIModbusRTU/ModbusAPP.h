#pragma once

#define OnMenuClicked 1
#define OnConnetMenuClicked 2
#define OnAboutMenuClicked 3
#define OnExitMenuClicked 4

#define OnSerialRefresh 5

#define OnClickedConnect 6
#define OnClickedDisconnect 7
#define ConsoleText 8
#define OnClickedSand 9


#define DlgIndexNumber 254

#define ComPortAmount 255
#define ComSelectIndex 1120
#define ComSelectIndexMAX 1375
#define IdCombobox 1376

#define TextBufferSize 4096
#define ID_SLAVE_EDIT 1377


#include<string>
#include<vector>

#include<Windows.h>
#include"ModbusRTU.h"
#include <chrono>
#include <ctime>

char Buffer[TextBufferSize];
std::vector<uint8_t> data;

HWND  hCombobox;
HMENU ComPortSubMenu;
HMENU ComPortListMenu;


int selectComboBoxIndex =-1;
int lineCount;
int selectPort = 1;
int const WindowHeight = 800;
int const WindowWidth = 1216;

std::vector<uint8_t> dataRequest;
std::vector<uint8_t> dataResponse;

std::string str;


std::wstring ComboBoxValie1 = L"110";
std::wstring ComboBoxValie2 = L"300";
std::wstring ComboBoxValie3 = L"600";
std::wstring ComboBoxValie4 = L"1200";
std::wstring ComboBoxValie5 = L"2400";
std::wstring ComboBoxValie6 = L"4800";
std::wstring ComboBoxValie7 = L"9600";
std::wstring ComboBoxValie8 = L"14400";
std::wstring ComboBoxValie9 = L"19200";
std::wstring ComboBoxValie10 = L"38400";
std::wstring ComboBoxValie11 = L"56000";
std::wstring ComboBoxValie12 = L"57600";
std::wstring ComboBoxValie13 = L"115200";
std::wstring ComboBoxValie14 = L"128000";
std::wstring ComboBoxValie15 = L"256000";

HWND connectButton;
HWND disconnectButton;
HWND SlaveIDEdit;

UINT SlaveId;

HWND SelecPortLable;
HWND SelecPortStateLable;

HWND Console;
HWND WriteToConsoleEDIT;

WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure);
LRESULT CALLBACK  SoftwareMainPRocedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

void MainWndAddMenus(HWND hWnd);
void MainWndAddWidgets(HWND hWnd);
void SerialUpdate();
void updateSerialSelectLable();
void isConectedLabel();
void CreateCombobox(HWND hWnd);
void addLineToConsole(std::string str);
void addLineToConsole1(std::string str);
void addHRToConsole();
int IdndexComboxTobaudRate(int index);
std::string test();
void test2();
void CreateGring(HWND hWnd);