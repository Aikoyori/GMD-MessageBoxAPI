#include "MBXAPI.h"

MsgBox::~MsgBox()
{
	memset(title, 0, 127); // clearing variables
	memset(content, 0, 127);
	memset(button, 0, 127);
	FreeLibrary(PSAPI); // freeing loaded DLLs
	FreeLibrary(OpenGl32);
}

MsgBox::MsgBox() {}

MsgBox::MsgBox(char m_title[127], char m_content[127], char m_button[127])
{
	strcpy_s(title, m_title); // copying parameter values to the public ones equivelent
	strcpy_s(content, m_content);
	strcpy_s(button, m_button);
}

void MsgBox::SetAddress(DWORD alloc_addr)
{
	allocated_addr = alloc_addr; // copying parameter value to public one
}

void MsgBox::SetTitle(char m_title[127])
{
	strcpy_s(title, m_title); // copying parameter value to public one
}

void MsgBox::SetContent(char m_content[127])
{
	strcpy_s(content, m_content); // copying parameter value to public one
}

void MsgBox::SetButton(char m_button[127])
{
	strcpy_s(button, m_button); // copying parameter value to public one
}

DWORD MsgBox::GetModuleBaseAddress(char* name, DWORD procId)
{
	HMODULE hModules[0x1000];
	DWORD outModules;
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
	if (!hProc)
		return 0; // exiting function if OpenProcess fails
	EnumProcessModules((HWND)(hProc), (UINT)(hModules), 0x1000, (LPARAM)(&outModules)); // retrieving handles for all process modules
	for (int i = 0; i < outModules / 4; i++) { // iterate per module
		char moduleName[MAX_PATH];
		if (GetModuleFileNameExA((HWND)(hProc), (UINT)(hModules[i]), (WPARAM)(moduleName), MAX_PATH)) { // getting module path
			if (!strcmp(name, strrchr(moduleName, '\\') + 1)) {
				CloseHandle(hProc);
				return (DWORD)(hModules[i]); // returning module's address if matched
			}
		}
	}
	CloseHandle(hProc); // closing process handle
	return 0;
}

bool MsgBox::SetupVars(char title[128], char content[256], char button[8], char text_length)
{
	HWND hWnd = FindWindowA(0, "Geometry Dash"); // getting window handle
	if (hWnd == 0)
		return 0; // exiting if unable to find
	DWORD pId;
	GetWindowThreadProcessId(hWnd, &pId); // getting processid from window handle
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pId);
	if (!hProc)
		return 0; // exiting function if OpenProcess fails
	allocated_addr = (DWORD)(VirtualAllocEx(hProc, 0, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)); // allocating memory to write to
	DWORD hBase = GetModuleBaseAddress("GeometryDash.exe", pId); // getting process's base address
	DWORD v1 = allocated_addr + 0x200;
	DWORD v2 = allocated_addr + 0x280;
	DWORD v3 = allocated_addr + 0x300;
	WriteProcessMemory(hProc, (LPVOID)(allocated_addr + 0x200), title, 127, NULL); // writing variables to allocated memory
	WriteProcessMemory(hProc, (LPVOID)(allocated_addr + 0x280), button, 127, NULL);
	WriteProcessMemory(hProc, (LPVOID)(allocated_addr + 0x300), content, 127, NULL);
	WriteProcessMemory(hProc, (LPVOID)(hBase + 0x24654A), &text_length, 1, NULL); // writing values to show message box function
	WriteProcessMemory(hProc, (LPVOID)(hBase + 0x24656F), &v1, 4, NULL);
	WriteProcessMemory(hProc, (LPVOID)(hBase + 0x246576), &v2, 4, NULL);
	WriteProcessMemory(hProc, (LPVOID)(hBase + 0x24655A), &v3, 4, NULL);
	CloseHandle(hProc); // closing process handle
	return 1;
}

bool MsgBox::JMPCLL(HANDLE hProc, DWORD StartPoint, DWORD EndPoint) {
	DWORD AddressValue = EndPoint - StartPoint - 5; // calculating jmp/call value
	return WriteProcessMemory(hProc, (PVOID)(StartPoint + 1), &AddressValue, 4, NULL); // writing calculated value to address (+1)
}

DWORD MsgBox::Setup()
{
	HWND hWnd = FindWindowA(0, "Geometry Dash"); // getting window handle
	if (hWnd == 0)
		return 0; // exiting if unable to find
	DWORD pId;
	GetWindowThreadProcessId(hWnd, &pId); // getting processid from window handle
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pId);
	if (!hProc)
		return 0; // exiting function if OpenProcess fails
	DWORD pBase = GetModuleBaseAddress("GeometryDash.exe", pId); // getting process's base address
	DWORD pOpenGlBase = GetModuleBaseAddress("OPENGL32.dll", pId); // getting opengl base
	DWORD pSwapBuffers = pOpenGlBase + sb_offset;
	DWORD pSBInc = pSwapBuffers + 1;
	DWORD bkup, jmp_addr, cc_addr;

	VirtualProtectEx(hProc, (LPVOID)(pSwapBuffers), 0x1000, PAGE_EXECUTE_READWRITE, &bkup); // changing memory protect to avoid crash
	ReadProcessMemory(hProc, (LPVOID)(pSwapBuffers + 0x01), &jmp_addr, 4, NULL); // getting current jmp value
	cc_addr = pSwapBuffers + jmp_addr + 5;
	
	WriteProcessMemory(hProc, (LPVOID)(cc_addr + 0x05), "\xE8\x00\x00\x00\x00", 5, NULL); // call {null} (to be message box func)
	JMPCLL(hProc, cc_addr + 0x05, pBase + 0x00246530);

	WriteProcessMemory(hProc, (LPVOID)(cc_addr + 0x0A), "\xB9\x00\x00\x00\x00", 5, NULL); // mov ecx,{null} (to be wglSwapBuffers+0x01)
	WriteProcessMemory(hProc, (LPVOID)(cc_addr + 0x0B), &pSBInc, 4, NULL);
	
	WriteProcessMemory(hProc, (LPVOID)(cc_addr + 0x0F), "\xC7\x01\x00\x00\x00\x00", 6, NULL); // mov [ecx],{null} (to be overlay steam overlay jump)
	WriteProcessMemory(hProc, (LPVOID)(cc_addr + 0x11), &jmp_addr, 4, NULL);
	
	WriteProcessMemory(hProc, (LPVOID)(cc_addr + 0x15), "\xE9\x00\x00\x00\x00", 5, NULL); // jmp {null} (to be wglSwapBuffers+0x05)
	JMPCLL(hProc, cc_addr + 0x15, pSwapBuffers + 0x05);

	return jmp_addr+5;
}

bool MsgBox::Show(DWORD addr)
{
	char textLen = 0;
	for (int i = 0; i < 255; i++) {
		if (content[i])
			textLen++; // calculating text length (plus one for every non-zero character)
		else
			break; // breaking upon null terminator
	}
	if (!SetupVars(title, content, button, textLen)) // setting up text variables
		return 0;

	HWND hWnd = FindWindowA(0, "Geometry Dash"); // getting window handle
	if (hWnd == 0)
		return 0; // exiting if unable to find
	DWORD pId;
	GetWindowThreadProcessId(hWnd, &pId); // getting processid from window handle
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pId);
	if (!hProc)
		return 0; // exiting function if OpenProcess fails
	DWORD pOpenGL = GetModuleBaseAddress("OPENGL32.dll", pId); // getting opengl base
	DWORD pSwapBuffers = pOpenGL + sb_offset;
	if (!WriteProcessMemory(hProc, (LPVOID)(pSwapBuffers + 0x01), &addr, 4, NULL)) { // writing address to toggle function call
		CloseHandle(hProc); // closing process handle if failed
		return 0;
	}
	CloseHandle(hProc); // closing process handle
	return 1;
}
