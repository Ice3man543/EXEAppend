/*
* stub.c : This file contains stub routines for ZipSFX Program.
* Some Portions of this code was taken from Peter Kankowski's
* Article. So, appropriate credits for those parts go to him.
* Other Parts are copyrights of Me (Sh4d0w-l0rd)
*
* Created By : @Ice3man
*/

#include <windows.h>
#include <stdio.h>


const TCHAR AppName[] = TEXT("EXEStub");
const TCHAR OutOfMemory[] = TEXT("Out of memory");

HANDLE heap;
HANDLE h; //for output file

/*
* Memory Allocation Function Replacement.
*/
void* malloc2(size_t n) {
	void* mem = HeapAlloc(heap, HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY, n);
	if(!mem) {
		MessageBox(0, OutOfMemory, AppName, MB_ICONERROR);
		ExitProcess(1);
	}
	return mem;
}

/*
* Heap Clearing Function Replacement.
*/
void free2(void* p) {
    if(p == NULL) return;
    HeapFree(heap, HEAP_NO_SERIALIZE, p);
}

#define malloc(n) malloc2(n)
#define free(n) free2(n)


const int ERR_OK = 0, ERR_READFAILED = 1, ERR_NOINFO = 2, ERR_BADFORMAT = 3;

/*
* This Function writes the output file to disk.
* It Also has some error checkings. (First Time !)
*/

int ProcessData(BYTE* data, int datasize) {
	DWORD dw;
	*(data + datasize) = '\0';
	// Replace with your own name (Out File)
	h = CreateFile("G:\\Out.zip", GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE,FILE_SHARE_READ,NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if(h == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, "Could Not Create Output File for extraction.", "EXEStub", MB_OK | MB_ICONERROR);
		CloseHandle(h);
		exit(1);
	}
	WriteFile(h, data, datasize, &dw, NULL);
	return ERR_OK;
}

int ReadFromExeFile(void) {
/* Reads data attached to the exe file and calls
   ProcessData(pointertodata, datasize).
   Return values:
	  * ERR_READFAILED - read from exe file had failed;
	  * ERR_BADFORMAT  - invalid format of the exe file;
	  * ERR_NOINFO     - no info was attached.
   If the data were read OK, it returns the return value of ProcessData.

	 (c) Peter Kankowski, 2006 http://smallcode.weblogs.us kankowski@narod.ru
*/
#define ErrIf(a) if(a) goto HANDLE_BADFORMAT;

	BYTE buff[4096]; DWORD read; BYTE* data;

	// Open exe file
	GetModuleFileName(NULL, (CHAR*)buff, sizeof(buff));
	HANDLE hFile = CreateFile((CHAR*)buff, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == hFile) return ERR_READFAILED;
	if(!ReadFile(hFile, buff, sizeof(buff), &read, NULL)) goto HANDLE_READFAILED;
	IMAGE_DOS_HEADER* dosheader = (IMAGE_DOS_HEADER*)buff;
	ErrIf(dosheader->e_magic != IMAGE_DOS_SIGNATURE);
	ErrIf((ULONG)(dosheader->e_lfanew) >= (ULONG)(sizeof(buff) - sizeof(IMAGE_NT_HEADERS32)));

	// Locate PE header
	IMAGE_NT_HEADERS32* header = (IMAGE_NT_HEADERS32*)(buff + dosheader->e_lfanew);
	ErrIf(header->Signature != IMAGE_NT_SIGNATURE);
	IMAGE_SECTION_HEADER* sectiontable =
		(IMAGE_SECTION_HEADER*)((BYTE*)header + sizeof(IMAGE_NT_HEADERS32));
	ErrIf((BYTE*)sectiontable >= buff + sizeof(buff));
	DWORD maxpointer = 0, exesize = 0;

	/*
	* This portion gets exe size.
	* It's not so pretty, but efficient enough.
	*/

	// For each section
	for(int i = 0; i < header->FileHeader.NumberOfSections; ++i) {
		if(sectiontable->PointerToRawData > maxpointer) {
			maxpointer = sectiontable->PointerToRawData;
			exesize = sectiontable->PointerToRawData + sectiontable->SizeOfRawData;
		}
		sectiontable++;
	}

	// Seek to the overlay
	DWORD filesize = GetFileSize(hFile, NULL);
	if(exesize == filesize) goto HANDLE_NOINFO;
	ErrIf(filesize == INVALID_FILE_SIZE || exesize > filesize);
	if(SetFilePointer(hFile, exesize, NULL, FILE_BEGIN) ==
		INVALID_SET_FILE_POINTER) goto HANDLE_READFAILED;
	data = (BYTE*)malloc(filesize - exesize + 8);
	if(!ReadFile(hFile, data, filesize - exesize, &read, NULL)) goto HANDLE_WITHFREE;
	CloseHandle(hFile);

	// Process the data
	int result = ProcessData(data, filesize - exesize);
	free(data);
	return result;

	/*
	* Handler Routines for Program.
	*/

	HANDLE_WITHFREE:
		free(data);
	HANDLE_READFAILED:
		CloseHandle(hFile);
		return ERR_READFAILED;
  HANDLE_BADFORMAT:
	  CloseHandle(hFile);
		return ERR_BADFORMAT;
	HANDLE_NOINFO:
		CloseHandle(hFile);
		return ERR_NOINFO;

#undef ErrIf
}

/*int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, INT nCmdShow) {*/
int main(int argc, char *argv[])
{
	int err;
	/*UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);*/
	heap = GetProcessHeap();
	if((err = ReadFromExeFile()) != ERR_OK) {
		if(err == ERR_NOINFO)
			MessageBox(0, TEXT("No data were attached"), AppName, MB_ICONINFORMATION);
		else
			MessageBox(0, TEXT("Can't read from exe file"), AppName, MB_ICONERROR);
	}
	return 0;
}
