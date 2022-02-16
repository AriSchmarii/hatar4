#pragma once
#include <windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <functional>
#include <basetsd.h>
#include <mutex>

#include <MinHook.h>
#include "MinHook/include/MinHook.h"
#pragma comment(lib, "libMinHook-x64-v140-md.lib")

#include "scanner.hpp"

#pragma warning(disable : 4996) //due to std::wcscpy

class input {
public:
	constexpr explicit input(const char* info = { }, const std::int32_t key = { }) : info(info), key(key) { }
	~input() noexcept = default;
	void add_call(std::function<void()> callback) noexcept {
		if (GetAsyncKeyState(key) & 0x1) {
			callback();
			std::cout << info << std::endl;
		}
	}
public:
	const char* info;
	std::int32_t key;
};

#define create_hook(name, target, hk, og) \
if (MH_CreateHook(target, hk, (void**)&og) != MH_OK) \
   std::cout << "cant hook " << name << std::endl; \
else \
   std::cout << "hooked " << name << std::endl; \

bool init_console() { //ugly peace of shit
	if (!AllocConsole())
		return false;
	const HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	const HWND console_window = GetConsoleWindow();
	if (!console_handle || console_window)
		return false;
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
	SetConsoleTitleW(L"hatar4.cheats");
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	//size of the console
	RECT lp_rect{};
	GetWindowRect(console_window, &lp_rect);
	MoveWindow(console_window, lp_rect.left, lp_rect.top, 900, 500, TRUE);
	//color + alpha; credits to aki for this part
	CONSOLE_SCREEN_BUFFER_INFOEX csbiInfo{};
	csbiInfo.cbSize = sizeof(csbiInfo);
	GetConsoleScreenBufferInfoEx(console_handle, &csbiInfo);
	csbiInfo.ColorTable[1] = RGB(229, 137, 226);
	SetConsoleScreenBufferInfoEx(console_handle, &csbiInfo);
	SetConsoleTextAttribute(console_handle, 1);
	SetLayeredWindowAttributes(console_window, NULL, 245, LWA_ALPHA);
	return true;
}
/*
x-man: pattern scanner
senpai ari: easy-hook
senpai ari: bing chilling crash
senpai ari: yes im the one who made this (at 2am idk if some parts are cursed)
aki: custom console color
hi phobos :3
*/
