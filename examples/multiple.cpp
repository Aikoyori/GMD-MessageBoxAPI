#include "MBXAPI.h"

int main()
{
	MsgBox MSGBOX_1("First Box", "<cj>Colour</c>", "OK");
	DWORD allocation = MSGBOX_1.Setup();

	MsgBox MSGBOX_2("Second Box", "Line 1\nLine 2\x0ALine 3", "OK");
	MSGBOX_2.Setup();
	
	while (1) {
		system("pause");
		MSGBOX_1.Show(allocation);
		system("pause");
		MSGBOX_2.Show(allocation);
	}

	return 0;
}
