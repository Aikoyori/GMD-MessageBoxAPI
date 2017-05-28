#include "MBXAPI.h"

int main()
{
	MsgBox MSGBOX("Title here", "Content Here", "Button here");
	DWORD allocation = MSGBOX.Setup();
	bool success = MSGBOX.Show(allocation);

	return 0;
}
