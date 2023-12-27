#include "syscall.h"

int main()
{
	int i;
	PrintString("=== BANG MA ASCII ===\n");
	for (i = 0; i <= 127; i++)
	{
		PrintInt(i);
		PrintString("\t");
		PrintChar((char)i);
		PrintString("\n");
	}
	Exit(0);
}