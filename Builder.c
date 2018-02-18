/*
* main.c : This File Contains Routines for Appending Zip Archive to the
* end of program. Also, this file Manages some other routines too such as
* appending an ini file at the end of stub for info.
*
* Created By : @Ice3man
*/

#include <windows.h>
#include <stdio.h>

void AppendZip(char *, char *, char*); // name and location

void AppendZip(char *name, char *loc, char *out)
{
  HANDLE hFile, hFileA, hFileO;
  DWORD dw, fs, fzs, tfs;

  /*
  * This Portion Opens Exe File For Reading
  * and appending.
  */
  hFile = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); // opening exe for appending
  if (hFile == INVALID_HANDLE_VALUE) //error checking
  {
    MessageBox(NULL, "Could Not Open EXE File for appending.", "EXEBuilder", MB_OK | MB_ICONERROR);
		CloseHandle(hFile);
		exit(1);
  }

  /*
  * This Portion Opens Zip Archive to be
  * appended at the end
  */

  hFileA = CreateFile(loc, GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE,FILE_SHARE_READ,NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFileA == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, "Could Not Open Input Zip File.", "EXEBuilder", MB_OK | MB_ICONERROR);
		CloseHandle(hFileA);
		exit(1);
	}

  /*
  * Here, we open output exe File
  * for writing.
  */

  hFileO = CreateFile(out, GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE,FILE_SHARE_READ,NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFileA == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, "Could Not Open Output EXE File.", "EXEBuilder", MB_OK | MB_ICONERROR);
		CloseHandle(hFileO);
		exit(1);
	}

  /*
  * This part gets file size of both I/O Files
  */

  fs = GetFileSize(hFile, NULL);
  fzs = GetFileSize(hFileA, NULL);

  tfs = fs + fzs; // Total File Size

  BYTE *pe[fs]; //Array for EXE data
  BYTE *zip[fzs]; //Array for ZIP data

  /*
  * First Read Input EXE
  */

  ReadFile(hFile, pe, fs, &dw, NULL);

  /*
  * Then Read Input ZIP
  */

  ReadFile(hFileA, zip, fzs, &dw, NULL);

  BYTE *ou[tfs]; // Output EXE Buffer

  /*
  * This Block Writes the data to Output
  */

  WriteFile(hFileO, pe, fs, &dw, NULL); // Writes EXE
  SetFilePointer(hFileO, fs, NULL, FILE_BEGIN); //Setting Pointer To Size
  WriteFile(hFileO, zip, fzs, &dw, NULL); // Writes ZIP data

  CloseHandle(hFile);
  CloseHandle(hFileA);
  CloseHandle(hFileO);
  MessageBox(NULL, "Written SFX Successfully", "EXEBuilder", MB_OK | MB_ICONINFORMATION);
  return;
}

int main(int argc, char *argv[])
{
	// Replace n, o, z to your own data
	char *n = "D:\\d.exe"; // EXE name
 	char *o = "D:\\new.exe";  // zip to append
	char *z = "D:\\b.exe"; //output name
	printf("\n[*] ZipSFX Builder : (C)@Iceman");
	AppendZip((char *)n, (char *)z, (char *)o);
	return 0;
}
