#include "syscall.h"

/* size of physical memory; with code, we'll run out of space!*/

int main()
{
	int arr[100];
	int i, j;
	int swapped;
	int n;
	int tmp;
	do
	{
		PrintString("Nhap do dai mang (1 <= n <= 100): ");
		n = ReadInt();
	} while (n < 1 || n > 100);
	PrintString("Nhap mang: \n");
	for (i = 0; i < n; i++)
	{
		PrintString("Nhap so thu ");
		PrintInt(i + 1);
		PrintString(": ");
		arr[i] = ReadInt();
	}
	PrintString("\nMang vua nhap: ");
	for (i = 0; i < n; i++)
	{
		PrintInt(arr[i]);
		PrintString("  ");
	}
	for (i = 0; i < n - 1; i++)
	{
		swapped = 0;
		for (j = 0; j < n - i - 1; j++)
		{
			if (arr[j] > arr[j + 1])
			{
				tmp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = tmp;
				swapped = 1;
			}
		}
		if (swapped == 0)
			break;
	}
	PrintString("\nMang da sap xep: ");
	for (i = 0; i < n; i++)
	{
		PrintInt(arr[i]);
		PrintString("  ");
	}
	PrintChar('\n');
	Exit(0);
}