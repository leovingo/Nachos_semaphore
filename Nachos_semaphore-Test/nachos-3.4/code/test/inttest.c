#include "syscall.h"

int main()
{
	int a;
	PrintString("Nhap mot so nguyen: ");
	a = ReadInt();
	PrintString("So nguyen vua nhap: ");
	PrintInt(a);
	PrintChar('\n');
	Exit(0);
}