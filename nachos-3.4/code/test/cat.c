#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
	int openFileId;
	int fileSize;
	char c; //Ky tu de in ra
	char fileName[MAX_LENGTH];
	int i; //Index for loop
	PrintString("\n\t\t\t-----HIEN THI NOI DUNG FILE-----\n\n");
	PrintString(" - Nhap vao ten file can doc: ");
	
	//Goi ham ReadString de doc vao ten file
	//Co the su dung Open(stdin), nhung de tiet kiem thoi gian test ta dung ReadString
	ReadString(fileName, MAX_LENGTH);
	
	openFileId = Open(fileName, 1); // Goi ham Open de mo file 
	
	if (openFileId != -1) //Kiem tra Open co loi khong
	{
		PrintString(" -> Mo file thanh cong!!\n\n");
		Close(openFileId); // Goi ham Close de dong file
	}
	else
	{
		PrintString(" -> Mo file khong thanh cong!!\n\n");
	}
	return 0;
}