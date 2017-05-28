#ifndef MBXAPI_H
#define MBXAPI_H

#include <Windows.h>

class MsgBox;

class MsgBox
{
private:
	DWORD allocated_addr;
	char title[127];
	char content[127];
	char button[127];

	typedef DWORD(*opengl_wglSwapBuffers)();
	HMODULE PSAPI = LoadLibraryA("psapi.dll");
	WNDPROC EnumProcModules = (WNDPROC)GetProcAddress(PSAPI, "EnumProcessModules");
	WNDPROC GetModuleFileNameExA = (WNDPROC)GetProcAddress(PSAPI, "GetModuleFileNameExA");

	HINSTANCE OpenGl32 = LoadLibraryA("OPENGL32.dll");
	opengl_wglSwapBuffers wgl_SwapBuffers = (opengl_wglSwapBuffers)GetProcAddress(OpenGl32, "wglSwapBuffers");
	DWORD sb_offset = (DWORD)wgl_SwapBuffers - (DWORD)OpenGl32;

	DWORD GetModuleBaseAddress(char* name, DWORD procId);
	bool CustomCall(DWORD alloc_addr);
	bool SetupVars(char title[128], char content[256], char button[8], char text_length);
public:
	~MsgBox();
	MsgBox();
	MsgBox(char m_title[127], char m_content[127], char m_button[127]);

	void SetAddress(DWORD allocated_address);
	void SetTitle(char m_title[127]);
	void SetContent(char m_content[127]);
	void SetButton(char m_button[127]);

	DWORD Setup();
	bool Show(DWORD addr);
};

#endif
