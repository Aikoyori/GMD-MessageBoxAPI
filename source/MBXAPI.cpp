#include "MBXAPI.h"

MsgBox::~MsgBox()
{
	FreeLibrary(PSAPI);
	FreeLibrary(OpenGl32);
}

MsgBox::MsgBox() {}

MsgBox::MsgBox(char m_title[127], char m_content[127], char m_button[127])
{
	strcpy_s(title, m_title);
	strcpy_s(content, m_content);
	strcpy_s(button, m_button);
}

void MsgBox::SetAddress(DWORD alloc_addr)
{
	allocated_addr = alloc_addr;
}

void MsgBox::SetTitle(char m_title[127])
{
	strcpy_s(title, m_title);
}

void MsgBox::SetContent(char m_content[127])
{
	strcpy_s(content, m_content);
}

void MsgBox::SetButton(char m_button[127])
{
	strcpy_s(button, m_button);
}

DWORD MsgBox::GetModuleBaseAddress(char* name, DWORD procId)
{
	HMODULE hModules[0x1000];
	DWORD outModules;
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
	if (!hProc)
		return 0;
	EnumProcModules((HWND)(hProc), (UINT)(hModules), 0x1000, (LPARAM)(&outModules));
	for (int i = 0; i < outModules / 4; i++) {
		char moduleName[MAX_PATH];
		if (GetModuleFileNameExA((HWND)(hProc), (UINT)(hModules[i]), (WPARAM)(moduleName), MAX_PATH)) {
			if (!strcmp(name, strrchr(moduleName, '\\') + 1)) {
				CloseHandle(hProc);
				return (DWORD)(hModules[i]);
			}
		}
	}
	CloseHandle(hProc);
	return 0;
}

bool MsgBox::CustomCall(DWORD alloc_addr)
{
	HWND hWnd = FindWindowA(0, "Geometry Dash");
	if (hWnd == 0)
		return 0;
	DWORD pId;
	GetWindowThreadProcessId(hWnd, &pId);
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pId);
	if (!hProc)
		return 0;
	DWORD hSwapBuffers = GetModuleBaseAddress("OPENGL32.dll", pId) + sb_offset;
	if (!WriteProcessMemory(hProc, (LPVOID)(hSwapBuffers + 0x01), &alloc_addr, 4, NULL)) {
		CloseHandle(hProc);
		return 0;
	}
	CloseHandle(hProc);
	return 1;
}

bool MsgBox::SetupVars(char title[128], char content[256], char button[8], char text_length)
{
	HWND hWnd = FindWindowA(0, "Geometry Dash");
	if (hWnd == 0)
		return 0;
	DWORD pId;
	GetWindowThreadProcessId(hWnd, &pId);
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pId);
	if (!hProc)
		return 0;
	DWORD hBase = GetModuleBaseAddress("GeometryDash.exe", pId);
	DWORD v1 = allocated_addr + 0x200;
	DWORD v2 = allocated_addr + 0x280;
	DWORD v3 = allocated_addr + 0x300;
	WriteProcessMemory(hProc, (LPVOID)(allocated_addr + 0x200), title, 128, NULL);
	WriteProcessMemory(hProc, (LPVOID)(allocated_addr + 0x280), button, 8, NULL);
	WriteProcessMemory(hProc, (LPVOID)(allocated_addr + 0x300), content, 256, NULL);
	WriteProcessMemory(hProc, (LPVOID)(hBase + 0x24654A), &text_length, 1, NULL);
	WriteProcessMemory(hProc, (LPVOID)(hBase + 0x24656F), &v1, 4, NULL);
	WriteProcessMemory(hProc, (LPVOID)(hBase + 0x246576), &v2, 4, NULL);
	WriteProcessMemory(hProc, (LPVOID)(hBase + 0x24655A), &v3, 4, NULL);
	CloseHandle(hProc);
	return 1;
}

DWORD MsgBox::Setup()
{
	HWND hWnd = FindWindowA(0, "Geometry Dash");
	if (hWnd == 0)
		return 0;
	DWORD pId;
	GetWindowThreadProcessId(hWnd, &pId);
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pId);
	if (!hProc)
		return 0;
	DWORD hBase = GetModuleBaseAddress("GeometryDash.exe", pId);
	DWORD hOpenGL = GetModuleBaseAddress("OPENGL32.dll", pId);
	DWORD hSwapBuffers = hOpenGL + sb_offset;
	DWORD bkup, oldAddr;

	DWORD allocMem = (DWORD)(VirtualAllocEx(hProc, 0, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
	if (!allocMem) {
		CloseHandle(hProc);
		return 0;
	}
	DWORD jmp1 = hSwapBuffers - allocMem - 0x10;
	DWORD jmp2 = hSwapBuffers + 1;
	DWORD cll1 = (hBase + 0x246530) - (allocMem + 0x0C) - 4;
	DWORD callMsgBox = allocMem - hSwapBuffers - 5;
	for (int i = 0; i < 0x1000; i++)
		WriteProcessMemory(hProc, (LPVOID)(allocMem + i), "\x90", 1, NULL);
	if (!VirtualProtectEx(hProc, (LPVOID)(hSwapBuffers), 0x1000, PAGE_EXECUTE_READWRITE, &bkup)) {
		CloseHandle(hProc);
		return 0;
	}
	ReadProcessMemory(hProc, (LPVOID)(hSwapBuffers + 0x01), &oldAddr, 4, NULL);
	WriteProcessMemory(hProc, (LPVOID)(allocMem + 0x10), "\xE9", 1, NULL);
	WriteProcessMemory(hProc, (LPVOID)(allocMem + 0x11), &jmp1, 4, NULL);
	WriteProcessMemory(hProc, (LPVOID)(allocMem), "\xB9\xF5\x34\x0A\x11\x89\x0D\xD1\xD4\xE9\x65", 11, NULL);
	WriteProcessMemory(hProc, (LPVOID)(allocMem + 0x07), &jmp2, 4, NULL);
	WriteProcessMemory(hProc, (LPVOID)(allocMem + 0x01), &oldAddr, 4, NULL);
	WriteProcessMemory(hProc, (LPVOID)(allocMem + 0x0B), "\xE8", 1, NULL);
	WriteProcessMemory(hProc, (LPVOID)(allocMem + 0x0C), &cll1, 4, NULL);

	allocated_addr = allocMem;
	CloseHandle(hProc);
	return callMsgBox;
}

bool MsgBox::Show(DWORD addr)
{
	char textLen = '\x00';
	for (int i = 0; i < 256; i++) {
		if (content[i])
			textLen++;
		else
			break;
	}
	if (!SetupVars(title, content, button, textLen))
		return 0;
	return CustomCall(addr);
}
