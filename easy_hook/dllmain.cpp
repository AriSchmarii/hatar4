#include "pch.hpp"

bool g_running = { true }, g_crash = { false };
unsigned short g_tampered_buffer_count = { 0 };

using uWriteBitBufferQword = bool(*)(__int64 bit_buffer, __int64 value, int bits);
uWriteBitBufferQword write_bit_buffer_qword{};

using uWriteBitBufferDword = bool(*)(__int64 bit_buffer, int value, int bits);
uWriteBitBufferDword write_bit_buffer_dword{};

bool (*ogWriteBitBufferDword)(__int64 bit_buffer, int value, int bits) = { nullptr };
bool hkWriteBitBufferDword(__int64 bit_buffer, int value, int bits) { //nighfall devs hate this trick >:(
	if (!g_crash) 
		return ogWriteBitBufferDword(bit_buffer, value, bits);
	g_tampered_buffer_count++;
	if (g_tampered_buffer_count >= 6969) { //sex
		g_crash = false;
		std::cout << "hatar4 automatically disabled" << std::endl;
	}
	for (int i = 0; i < 5; i++) {
		write_bit_buffer_qword(bit_buffer, 0x7fff, rand() % 0xff);
		ogWriteBitBufferDword(bit_buffer, value, bits);
	}
	return true;
}

DWORD mainThread(LPVOID lp_param) {
	if (!init_console())
		return EXIT_FAILURE;
	MH_Initialize();
	std::cout << R"(
 _           _             _  _        _                _       
| |__   __ _| |_ __ _ _ __| || |   ___| |__   ___  __ _| |_ ___ 
| '_ \ / _` | __/ _` | '__| || |_ / __| '_ \ / _ \/ _` | __/ __|
| | | | (_| | || (_| | |  |__   _| (__| | | |  __/ (_| | |_\__ \
|_| |_|\__,_|\__\__,_|_|     |_|(_)___|_| |_|\___|\__,_|\__|___/
                                                                
----------------------------------------------------------------------------)" << std::endl;
	try {
		pattern_batch batch;
		batch.add("wbq", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 41 8B F0 48 8B EA 48 8B D9 41 83 F8 20", [=](ptr_manage ptr) {
			write_bit_buffer_qword = ptr.as<uWriteBitBufferQword>();
		});
		batch.add("wbd", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 20 8B EA BF 01 ? ? ?", [=](ptr_manage ptr) {
			write_bit_buffer_dword = ptr.as<uWriteBitBufferDword>();
		});
		batch.run();
		create_hook("wbd", write_bit_buffer_dword, hkWriteBitBufferDword, ogWriteBitBufferDword);
		MH_EnableHook(MH_ALL_HOOKS);
		std::cout << "----------------------------------------------------------------------------" << std::endl;
		std::cout << "f5 - OMG HATAR4!!11! (DON'T USE TO OP)" << std::endl; //is crash?
		std::cout << "f6 - unload" << std::endl;
		while (g_running) {
			input("hatar4 enabled", VK_F5).add_call([]() -> void {  g_tampered_buffer_count = 0; g_crash = true; });
			input("unloaded", VK_F6).add_call([]() -> void { g_running = false; });
		}
		MH_DisableHook(MH_ALL_HOOKS);
		MH_RemoveHook(MH_ALL_HOOKS);
		MH_Uninitialize();
		FreeConsole();
		FreeLibraryAndExitThread(static_cast<HMODULE>(lp_param), EXIT_SUCCESS);
	}
	catch (std::exception& exc) {
		std::cout << exc.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

BOOL APIENTRY DllMain(HMODULE h_module, DWORD  ul_reason_for_call, LPVOID lp_reserved) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(h_module);
		CreateThread(NULL, NULL, static_cast<LPTHREAD_START_ROUTINE>(mainThread), h_module, NULL, NULL);
	}
	return TRUE;
}
