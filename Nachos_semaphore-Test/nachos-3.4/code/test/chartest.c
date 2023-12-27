#include "syscall.h"

int main()
{
	char c;
	PrintString("Nhap mot ky tu: ");
	c = ReadChar();
	PrintString("Ky tu vua nhap: ");
	PrintChar(c);
	PrintChar('\n');
	Exit(0);
}