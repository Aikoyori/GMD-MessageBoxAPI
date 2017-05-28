#include "MBXAPI.h"

int main()
{
	MsgBox MSGBOX("Multiple Lines", "Line 1\nLine 2\nLine 3\nLine 4", "OK");
	DWORD allocation = MSGBOX.Setup();
	bool success = MSGBOX.Show(allocation);

	return 0;
}
