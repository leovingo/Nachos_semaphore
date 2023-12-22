// ******************************************************************************
// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.
// ******************************************************************************

// CODE:
#include "copyright.h"
#include "system.h"
#include "syscall.h"

#define MAX 200

// --------------------------------------------------------------------------
// ExceptionHandler:
// 	-   Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
// 	-   For system calls, the following is the calling convention:
// 		system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//	-   The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!)
//
// "which" is the kind of exception.  The list of possible exceptions
// are in machine.h.
// --------------------------------------------------------------------------

// FUNCTION:
// Copy buffer from User memory space to System memory space
char *User2System(int virtAddr, int limit)
{
	int i;
	int oneChar;
	char *kernelBuf = NULL;

	kernelBuf = new char[limit + 1];
	if (kernelBuf == NULL)
		return kernelBuf;

	memset(kernelBuf, 0, limit + 1);

	for (i = 0; i < limit; i++)
	{
		machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}
// Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char *buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return len;
	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);

	return i;
}
// Increase PC
void IncreasePC()
{
	int counter = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg, counter);

	counter = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg, counter);
	machine->WriteRegister(NextPCReg, counter + 4);
}
// Count the number of digits
int countDigits(long int n)
{
	if (n == 0)
	{
		return 1;
	}
	else if (n < 0)
	{
		n = -n;
	}

	int count = 0;
	while (n > 0)
	{
		count++;
		n /= 10;
	}

	return count;
}
// Handle exception
void ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);
	switch (which)
	{
	// khong phai syscall exception
	case NoException:
		DEBUG('a', "\n\nEverything ok!\n");
		printf("Everything ok!\n");
		interrupt->Halt();
		return;
	case PageFaultException:
		DEBUG('a', "\n\nNo valid translation found.\n");
		printf("No valid translation found.\n");
		interrupt->Halt();
		break;
	case ReadOnlyException:
		DEBUG('a', "\n\nWrite attempted to page marked read-only.\n");
		printf("Write attempted to page marked read-only.\n");
		interrupt->Halt();
		break;
	case BusErrorException:
		DEBUG('a', "\n\nTranslation resulted in an invalid physical address.\n");
		printf("Translation resulted in an invalid physical address.\n");
		interrupt->Halt();
		break;
	case AddressErrorException:
		DEBUG('a', "\n\nUnaligned reference or one that was beyond the end of the address space.\n");
		printf("Unaligned reference or one that was beyond the end of the address space.\n");
		interrupt->Halt();
		break;
	case OverflowException:
		DEBUG('a', "\n\nInteger overflow in add or sub.\n");
		printf("Integer overflow in add or sub.\n");
		interrupt->Halt();
		break;
	case IllegalInstrException:
		DEBUG('a', "\n\nUnimplemented or reserved instr.\n");
		printf("Unimplemented or reserved instr.\n");
		interrupt->Halt();
		break;
	case NumExceptionTypes:
		DEBUG('a', "\n\nNumber Exception Types.");
		printf("Number Exception Types.");
		interrupt->Halt();
		break;

	// la syscall exception
	case SyscallException:
	{
		DEBUG('a', "\n\nA program executed a system call.\n");
		// switch(type)
		switch (type)
		{
			// Halt
		case SC_Halt:
		{
			// Tat HDH
			DEBUG('a', "\n\nShutdown, initiated by user program.\n");
			printf("Shutdown, initiated by user program.\n");
			interrupt->Halt();
			return;
		}
		case SC_Exit:
		{
			int num = machine->ReadRegister(4);
			DEBUG('a', "\n\nProcess return %d.\n", num);
			printf("Process return %d.\n", num);
			Exit(num);
			break;
		}
		
		{
			DEBUG('a', "\n\nUnimplemented!\n");
			printf("Unimplemented!\n");
			interrupt->Halt();
			break;
		}
		case SC_Create:
		{
			int virtAddr;
			char *filename;
			DEBUG('a', "\n\nSC_Create call ...\n");
			DEBUG('a', "\n\nReading virtual address of filename.\n");

			virtAddr = machine->ReadRegister(4);
			DEBUG('a', "\n\nReading filename.\n");
			int maxFileLength;
			maxFileLength = 32;
			filename = User2System(virtAddr, maxFileLength + 1);

			if (filename == NULL)
			{
				printf("Not enough memory in system.");
				DEBUG('a', "\n\nNot enough memory in system.\n");
				machine->WriteRegister(2, -1);
				delete filename;
				return;
			}
			DEBUG('a', "\n\nFinish reading filename.\n");
			if (!fileSystem->Create(filename, 0))
			{
				printf("Error create file '%s'.\n", filename);
				machine->WriteRegister(2, -1);
				delete filename;
				return;
			}
			machine->WriteRegister(2, 0);
			delete filename;
			break;
		}
			// ReadInt
		case SC_ReadInt:
		{
			// Doc so nguyen tu man hinh console
			// Output: So nguyen doc duoc tu man hinh console
			DEBUG('a', "\n\nRead Int.\n");
			// Ket qua luu vao num
			int num = 0;
			bool isFloat = false;
			bool isNegative = false;
			char *buf = new char[MAX + 1];

			// Nguoi dung nhap du lieu vao man hinh console
			// Khi thuc hien xong, du lieu truyen vao buf
			int count = gSynchConsole->Read(buf, MAX);
			if (count == -1)
			{
				num = 0;
				machine->WriteRegister(2, num);
				delete[] buf;
				break;
			}
			// Chuyen chuoi cac ky tu nguoi dung nhap thanh so nguyen
			// Neu khong hop le: num = 0
			for (int i = 0; i < count; i++)
			{
				char c = buf[i];
				// Dau '-' xuat hien lan dau tien
				if (i == 0 && c == '-')
				{
					isNegative = true;
					continue;
				}
				// Dau '.' xuat hien lan dau tien
				if (!isFloat && c == '.')
				{
					isFloat = true;
					continue;
				}
				// Ky tu xet phai la ky so
				if (c < '0' || c > '9')
				{
					num = 0;
					break;
				}
				int digit = c - '0';
				if (!isFloat)
				{
					// Phan nguyen
					num = num * 10 + digit;
				}
				else if (digit != 0)
				{
					// Phan thap phan (hop le neu sau dau cham luon la 0)
					num = 0;
					break;
				}
			}
			if (isNegative)
				num *= -1;
			// Tra so nguyen ve thong qua thanh ghi so 2
			machine->WriteRegister(2, num);
			delete[] buf;
			break;
		}
		// PrintInt
		case SC_PrintInt:
		{
			// In so nguyen ra man hinh console
			// Input: So nguyen
			DEBUG('a', "\n\nPrint Int.\n");
			// Lay so nguyen tu thanh ghi so 4
			int num = machine->ReadRegister(4);
			if (num == 0)
			{
				gSynchConsole->Write("0", 1);
				break;
			}
			if (num == 2147483648)
			{
				gSynchConsole->Write("-2147483648", 11);
				break;
			}
			// Chuyen so nguyen thanh mang cac ky tu
			int count = countDigits(num);
			if (num == -2147483648)
			{
				count = 10;
			}
			if (num < 0)
			{
				count++;
				num = -num;
			}
			char *buf = new char[count + 1];
			int i = count - 1;
			buf[0] = '-';

			while (num > 0)
			{
				char c = (num % 10) + '0';
				num /= 10;
				buf[i--] = c;
			}

			// Xuat chuoi ky tu (dang so nguyen) ra man hinh console
			gSynchConsole->Write(buf, count);
			delete[] buf;
			break;
		}
		// ReadChar
		case SC_ReadChar:
		{
			// Doc 1 ky tu
			// Output: Mot ky tu duoc nhap tu man hinh console
			DEBUG('a', "\n\nRead Char.\n");
			char *buf = new char[MAX + 1];

			// Doc cac ky tu tu man hinh console do nguoi dung nhap
			// Lay ky tu dau va tra ve thong qua thanh ghi so 2
			if (gSynchConsole->Read(buf, MAX) > 0)
			{
				machine->WriteRegister(2, buf[0]);
			}
			else
			{
				machine->WriteRegister(2, 0);
			}

			delete[] buf;
			break;
		}
		// PrintChar
		case SC_PrintChar:
		{
			// In 1 ky tu ra man hinh console
			// Input: 1 ky tu
			DEBUG('a', "\n\nPrint Char.\n");
			// Lay ky tu tu thanh ghi so 4
			char c = char(machine->ReadRegister(4));
			// In ky tu ra man hinh console
			gSynchConsole->Write(&c, 1);
			break;
		}
		// ReadString
		case SC_ReadString:
		{
			// Doc chuoi ky tu tu man hinh console va luu vao vung nho nguoi dung
			// Input: Dia chi vung nho nguoi dung, do dai chuoi
			DEBUG('a', "\n\nRead String.\n");
			int vAddr = machine->ReadRegister(4);
			int length = machine->ReadRegister(5);
			char *buf = new char[length + 1];
			// Kiem tra do dai hop le
			if (length < 1)
			{
				buf[0] = '\0';
				System2User(vAddr, 1, buf);
				delete[] buf;
				break;
			}
			// Nguoi dung nhap du lieu vao man hinh console
			// Khi nhap xong, du lieu duoc truyen vao buf (nam tren vung nho he thong)
			int count = gSynchConsole->Read(buf, length);
			if (count == -1)
			{
				buf[0] = '\0';
				System2User(vAddr, 1, buf);
				delete[] buf;
				break;
			}
			// Chuyen du lieu tu vung nho he thong sang vung nho nguoi dung
			buf[length] = '\0';
			System2User(vAddr, count + 1, buf);
			delete[] buf;
			break;
		}
		// PrintString
		case SC_PrintString:
		{
			// In ra man hinh console chuoi cac ky tu luu trong vung nho nguoi dung
			// Input: Dia chi vung nho nguoi dung
			DEBUG('a', "\n\nPrint String.\n");
			int vAddr = machine->ReadRegister(4);
			int count = 0;
			int oneChar;
			// Tinh toan do dai chuoi nam trong vung nho nguoi dung
			// Ket thuc khi gap ky tu '\0'
			do
			{
				machine->ReadMem(vAddr + count, 1, &oneChar);
				count++;
			} while (oneChar != (int)'\0');
			count--;
			// Chuyen chuoi tu vung nho nguoi dung sang vung nho he thong
			char *buf = User2System(vAddr, count);
			// In chuoi ra man hinh console
			gSynchConsole->Write(buf, count);
			delete[] buf;
			break;
		}
		default:
		{
			DEBUG('a', "\n\nUnexpected system call (%d %d).\n", which, type);
			printf("\nUnexpected system call (%d %d).\n", which, type);
			interrupt->Halt();
		}
			
		case SC_Open:
		{
			// Input: arg1: Dia chi cua chuoi name, arg2: type
			// Output: Tra ve OpenFileID neu thanh, -1 neu loi
			// Chuc nang: Tra ve ID cua file.
	 
			//OpenFileID Open(char *name, int type)
			int virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so name tu thanh ghi so 4
			int type = machine->ReadRegister(5); // Lay tham so type tu thanh ghi so 5
			char* filename;
			filename = User2System(virtAddr, MaxFileLength); // Copy chuoi tu vung nho User Space sang System Space voi bo dem name dai MaxFileLength
			//Kiem tra xem OS con mo dc file khong
			
			// update 4/1/2018
			int freeSlot = fileSystem->FindFreeSlot();
			if (freeSlot != -1) //Chi xu li khi con slot trong
			{
				if (type == 0 || type == 1) //chi xu li khi type = 0 hoac 1
				{
					
					if ((fileSystem->openf[freeSlot] = fileSystem->Open(filename, type)) != NULL) //Mo file thanh cong
					{
						machine->WriteRegister(2, freeSlot); //tra ve OpenFileID
					}
				}
				else if (type == 2) // xu li stdin voi type quy uoc la 2
				{
					machine->WriteRegister(2, 0); //tra ve OpenFileID
				}
				else // xu li stdout voi type quy uoc la 3
				{
					machine->WriteRegister(2, 1); //tra ve OpenFileID
				}
				delete[] filename;
				break;
			}
			machine->WriteRegister(2, -1); //Khong mo duoc file return -1
			
			delete[] filename;
			break;
		}

		case SC_Close:
		{
			//Input id cua file(OpenFileID)
			// Output: 0: thanh cong, -1 that bai
			int fid = machine->ReadRegister(4); // Lay id cua file tu thanh ghi so 4
			if (fid >= 0 && fid <= 14) //Chi xu li khi fid nam trong [0, 14]
			{
				if (fileSystem->openf[fid]) //neu mo file thanh cong
				{
					delete fileSystem->openf[fid]; //Xoa vung nho luu tru file
					fileSystem->openf[fid] = NULL; //Gan vung nho NULL
					machine->WriteRegister(2, 0);
					break;
				}
			}
			machine->WriteRegister(2, -1);
			break;
		}

		case SC_Read:
		{
			// Input: buffer(char*), so ky tu(int), id cua file(OpenFileID)
			// Output: -1: Loi, So byte read thuc su: Thanh cong, -2: Thanh cong
			// Cong dung: Doc file voi tham so la buffer, so ky tu cho phep va id cua file
			int virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
			int charcount = machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
			int id = machine->ReadRegister(6); // Lay id cua file tu thanh ghi so 6 
			int OldPos;
			int NewPos;
			char *buf;
			// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
			if (id < 0 || id > 14)
			{
				printf("\nKhong the read vi id nam ngoai bang mo ta file.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Kiem tra file co ton tai khong
			if (fileSystem->openf[id] == NULL)
			{
				printf("\nKhong the read vi file nay khong ton tai.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			if (fileSystem->openf[id]->type == 3) // Xet truong hop doc file stdout (type quy uoc la 3) thi tra ve -1
			{
				printf("\nKhong the read file stdout.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			OldPos = fileSystem->openf[id]->GetCurrentPos(); // Kiem tra thanh cong thi lay vi tri OldPos
			buf = User2System(virtAddr, charcount); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
			// Xet truong hop doc file stdin (type quy uoc la 2)
			if (fileSystem->openf[id]->type == 2)
			{
				// Su dung ham Read cua lop SynchConsole de tra ve so byte thuc su doc duoc
				int size = gSynchConsole->Read(buf, charcount); 
				System2User(virtAddr, size, buf); // Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer co do dai la so byte thuc su
				machine->WriteRegister(2, size); // Tra ve so byte thuc su doc duoc
				delete buf;
				IncreasePC();
				return;
			}
			// Xet truong hop doc file binh thuong thi tra ve so byte thuc su
			if ((fileSystem->openf[id]->Read(buf, charcount)) > 0)
			{
				// So byte thuc su = NewPos - OldPos
				NewPos = fileSystem->openf[id]->GetCurrentPos();
				// Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer co do dai la so byte thuc su 
				System2User(virtAddr, NewPos - OldPos, buf); 
				machine->WriteRegister(2, NewPos - OldPos);
			}
			else
			{
				// Truong hop con lai la doc file co noi dung la NULL tra ve -2
				//printf("\nDoc file rong.");
				machine->WriteRegister(2, -2);
			}
			delete buf;
			IncreasePC();
			return;
		}

		case SC_Write:
		{
			// Input: buffer(char*), so ky tu(int), id cua file(OpenFileID)
			// Output: -1: Loi, So byte write thuc su: Thanh cong, -2: Thanh cong
			// Cong dung: Ghi file voi tham so la buffer, so ky tu cho phep va id cua file
			int virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
			int charcount = machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
			int id = machine->ReadRegister(6); // Lay id cua file tu thanh ghi so 6
			int OldPos;
			int NewPos;
			char *buf;
			// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
			if (id < 0 || id > 14)
			{
				printf("\nKhong the write vi id nam ngoai bang mo ta file.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Kiem tra file co ton tai khong
			if (fileSystem->openf[id] == NULL)
			{
				printf("\nKhong the write vi file nay khong ton tai.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Xet truong hop ghi file only read (type quy uoc la 1) hoac file stdin (type quy uoc la 2) thi tra ve -1
			if (fileSystem->openf[id]->type == 1 || fileSystem->openf[id]->type == 2)
			{
				printf("\nKhong the write file stdin hoac file only read.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			OldPos = fileSystem->openf[id]->GetCurrentPos(); // Kiem tra thanh cong thi lay vi tri OldPos
			buf = User2System(virtAddr, charcount);  // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
			// Xet truong hop ghi file read & write (type quy uoc la 0) thi tra ve so byte thuc su
			if (fileSystem->openf[id]->type == 0)
			{
				if ((fileSystem->openf[id]->Write(buf, charcount)) > 0)
				{
					// So byte thuc su = NewPos - OldPos
					NewPos = fileSystem->openf[id]->GetCurrentPos();
					machine->WriteRegister(2, NewPos - OldPos);
					delete buf;
					IncreasePC();
					return;
				}
			}
			if (fileSystem->openf[id]->type == 3) // Xet truong hop con lai ghi file stdout (type quy uoc la 3)
			{
				int i = 0;
				while (buf[i] != 0 && buf[i] != '\n') // Vong lap de write den khi gap ky tu '\n'
				{
					gSynchConsole->Write(buf + i, 1); // Su dung ham Write cua lop SynchConsole 
					i++;
				}
				buf[i] = '\n';
				gSynchConsole->Write(buf + i, 1); // Write ky tu '\n'
				machine->WriteRegister(2, i - 1); // Tra ve so byte thuc su write duoc
				delete buf;
				IncreasePC();
				return;
			}
		}

		case SC_Seek:
		{
			// Input: Vi tri(int), id cua file(OpenFileID)
			// Output: -1: Loi, Vi tri thuc su: Thanh cong
			// Cong dung: Di chuyen con tro den vi tri thich hop trong file voi tham so la vi tri can chuyen va id cua file
			int pos = machine->ReadRegister(4); // Lay vi tri can chuyen con tro den trong file
			int id = machine->ReadRegister(5); // Lay id cua file
			// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
			if (id < 0 || id > 14)
			{
				printf("\nKhong the seek vi id nam ngoai bang mo ta file.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Kiem tra file co ton tai khong
			if (fileSystem->openf[id] == NULL)
			{
				printf("\nKhong the seek vi file nay khong ton tai.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Kiem tra co goi Seek tren console khong
			if (id == 0 || id == 1)
			{
				printf("\nKhong the seek tren file console.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Neu pos = -1 thi gan pos = Length nguoc lai thi giu nguyen pos
			pos = (pos == -1) ? fileSystem->openf[id]->Length() : pos;
			if (pos > fileSystem->openf[id]->Length() || pos < 0) // Kiem tra lai vi tri pos co hop le khong
			{
				printf("\nKhong the seek file den vi tri nay.");
				machine->WriteRegister(2, -1);
			}
			else
			{
				// Neu hop le thi tra ve vi tri di chuyen thuc su trong file
				fileSystem->openf[id]->Seek(pos);
				machine->WriteRegister(2, pos);
			}
			IncreasePC();
			return;
		}
		case SC_Exec:
		{
			// Input: vi tri int
			// Output: Fail return -1, Success: return id cua thread dang chay
			// SpaceId Exec(char *name);
			int virtAddr;
			virtAddr = machine->ReadRegister(4);	// doc dia chi ten chuong trinh tu thanh ghi r4
			char* name;
			name = User2System(virtAddr, MaxFileLength + 1); // Lay ten chuong trinh, nap vao kernel
	
			if(name == NULL)
			{
				DEBUG('a', "\n Not enough memory in System");
				printf("\n Not enough memory in System");
				machine->WriteRegister(2, -1);
				//IncreasePC();
				return;
			}
			OpenFile *oFile = fileSystem->Open(name);
			if (oFile == NULL)
			{
				printf("\nExec:: Can't open this file.");
				machine->WriteRegister(2,-1);
				IncreasePC();
				return;
			}

			delete oFile;

			// Return child process id
			int id = pTab->ExecUpdate(name); 
			machine->WriteRegister(2,id);

			delete[] name;	
			IncreasePC();
			return;
		}
		case SC_Join:
		{       
			// int Join(SpaceId id)
			// Input: id dia chi cua thread
			// Output: 
			int id = machine->ReadRegister(4);
			
			int res = pTab->JoinUpdate(id);
			
			machine->WriteRegister(2, res);
			IncreasePC();
			return;
		}
		case SC_Exit:
		{
			//void Exit(int status);
			// Input: status code
			int exitStatus = machine->ReadRegister(4);

			if(exitStatus != 0)
			{
				IncreasePC();
				return;
				
			}			
			
			int res = pTab->ExitUpdate(exitStatus);
			//machine->WriteRegister(2, res);

			currentThread->FreeSpace();
			currentThread->Finish();
			IncreasePC();
			return; 
				
		}
		case SC_CreateSemaphore:
		{
			// int CreateSemaphore(char* name, int semval).
			int virtAddr = machine->ReadRegister(4);
			int semval = machine->ReadRegister(5);

			char *name = User2System(virtAddr, MaxFileLength + 1);
			if(name == NULL)
			{
				DEBUG('a', "\n Not enough memory in System");
				printf("\n Not enough memory in System");
				machine->WriteRegister(2, -1);
				delete[] name;
				IncreasePC();
				return;
			}
			
			int res = semTab->Create(name, semval);

			if(res == -1)
			{
				DEBUG('a', "\n Khong the khoi tao semaphore");
				printf("\n Khong the khoi tao semaphore");
				machine->WriteRegister(2, -1);
				delete[] name;
				IncreasePC();
				return;				
			}
			
			delete[] name;
			machine->WriteRegister(2, res);
			IncreasePC();
			return;
		}
		}
		// ket thuc switch(type)
		// Tang PC sau khi thuc hien cac system call
		IncreasePC();
		break;
	}
	// ket thuc case syscall exception
	default:
	{
		DEBUG('a', "\n\nUnexpected user mode exception (%d %d).\n", which, type);
		printf("\nUnexpected user mode exception (%d %d).\n", which, type);
		interrupt->Halt();
	}
	}
	// ket thuc switch(which)
}