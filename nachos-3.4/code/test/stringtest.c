#include "syscall.h"

int main()
{
	char str[100];
	int length;
	PrintString("Nhap do dai chuoi muon in ra (toi da nen la 100): ");
	length = ReadInt();
	PrintString("Nhap chuoi: ");
	ReadString(str, length);
	PrintString("Chuoi vua nhap: ");
	PrintString(str);
	PrintString("\n");
	Exit(0);
}