#include "GetDiskSN.h"
#include <stdio.h>
#include <winsock.h>
#include "Ntddscsi.h"

#include <tchar.h>


#pragma pack(1) 

#define  IDENTIFY_BUFFER_SIZE  512 

//  IOCTL commands 
#define  DFP_GET_VERSION                0x00074080 
#define  DFP_SEND_DRIVE_COMMAND            0x0007c084 
#define  DFP_RECEIVE_DRIVE_DATA            0x0007c088 
#define  FILE_DEVICE_SCSI                0x0000001b 
#define  IOCTL_SCSI_MINIPORT_IDENTIFY    ((FILE_DEVICE_SCSI << 16) + 0x0501) 


//  GETVERSIONOUTPARAMS contains the data returned from the  
//  Get Driver Version function. 
typedef struct _GETVERSIONOUTPARAMS
{
	BYTE bVersion;      // Binary driver version. 
	BYTE bRevision;     // Binary driver revision. 
	BYTE bReserved;     // Not used. 
	BYTE bIDEDeviceMap; // Bit map of IDE devices. 
	DWORD fCapabilities; // Bit mask of driver capabilities. 
	DWORD dwReserved[4]; // For future use. 
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;

//  Bits returned in the fCapabilities member of GETVERSIONOUTPARAMS  
#define  CAP_IDE_ID_FUNCTION             1  // ATA ID command supported 
#define  CAP_IDE_ATAPI_ID                2  // ATAPI ID command supported 
#define  CAP_IDE_EXECUTE_SMART_FUNCTION  4  // SMART commannds supported 

//  Valid values for the bCommandReg member of IDEREGS. 
#define  IDE_ATAPI_IDENTIFY  0xA1  //  Returns ID sector for ATAPI. 
#define  IDE_ATA_IDENTIFY    0xEC  //  Returns ID sector for ATA. 


// The following struct defines the interesting part of the IDENTIFY 
// buffer: 
typedef struct _IDSECTOR
{
	USHORT  wGenConfig;
	USHORT  wNumCyls;
	USHORT  wReserved;
	USHORT  wNumHeads;
	USHORT  wBytesPerTrack;
	USHORT  wBytesPerSector;
	USHORT  wSectorsPerTrack;
	USHORT  wVendorUnique[3];
	CHAR    sSerialNumber[20];
	USHORT  wBufferType;
	USHORT  wBufferSize;
	USHORT  wECCSize;
	CHAR    sFirmwareRev[8];
	CHAR    sModelNumber[40];
	USHORT  wMoreVendorUnique;
	USHORT  wDoubleWordIO;
	USHORT  wCapabilities;
	USHORT  wReserved1;
	USHORT  wPIOTiming;
	USHORT  wDMATiming;
	USHORT  wBS;
	USHORT  wNumCurrentCyls;
	USHORT  wNumCurrentHeads;
	USHORT  wNumCurrentSectorsPerTrack;
	ULONG   ulCurrentSectorCapacity;
	USHORT  wMultSectorStuff;
	ULONG   ulTotalAddressableSectors;
	USHORT  wSingleWordDMA;
	USHORT  wMultiWordDMA;
	BYTE    bReserved[128];
} IDSECTOR, *PIDSECTOR;


// Define global buffers. 
BYTE IdOutCmd[sizeof (SENDCMDOUTPARAMS)+IDENTIFY_BUFFER_SIZE - 1];

char *ConvertToString(DWORD diskdata[256], int firstIndex, int lastIndex);
void PrintIdeInfo(int drive, DWORD diskdata[256], char *sDest, int nLen);
BOOL DoIDENTIFY(HANDLE, PSENDCMDINPARAMS, PSENDCMDOUTPARAMS, BYTE, BYTE, PDWORD);

//  Max number of drives assuming primary/secondary, master/slave topology 
#define  MAX_IDE_DRIVES  16 

int ReadPhysicalDriveInNTWithAdminRights(char *sBuff, int nLen)
{
	int done = FALSE;
	HANDLE hPhysicalDriveIOCTL = 0;
	TCHAR driveName[MAX_PATH];

	for (int drive = 0; drive < MAX_IDE_DRIVES; drive++){
		//  Try to get a handle to PhysicalDrive IOCTL, report failure 
		//  and exit if can''t. 
		swprintf(driveName, L"\\\\.\\PhysicalDrive%d", drive);
		//  Windows NT, Windows 2000, must have admin rights 
		hPhysicalDriveIOCTL = CreateFile(driveName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

		// if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE) 
		//    printf ("Unable to open physical drive %d, error code: 0x%lX\n", 
		//            drive, GetLastError ()); 

		if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE){
			GETVERSIONOUTPARAMS VersionParams;
			DWORD               cbBytesReturned = 0;

			// Get the version, etc of PhysicalDrive IOCTL 
			memset((void*)&VersionParams, 0, sizeof(VersionParams));
			if (!DeviceIoControl(hPhysicalDriveIOCTL, DFP_GET_VERSION, NULL, 0, &VersionParams, sizeof(VersionParams), &cbBytesReturned, NULL)){
				// printf ("DFP_GET_VERSION failed for drive %d\n", i); 
				// continue; 
			}

			// If there is a IDE device at number "i" issue commands 
			// to the device 
			if (VersionParams.bIDEDeviceMap > 0){
				BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd 
				SENDCMDINPARAMS  scip;
				//SENDCMDOUTPARAMS OutCmd; 
				// Now, get the ID sector for all IDE devices in the system. 
				// If the device is ATAPI use the IDE_ATAPI_IDENTIFY command, 
				// otherwise use the IDE_ATA_IDENTIFY command 
				bIDCmd = (VersionParams.bIDEDeviceMap >> drive & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
				memset(&scip, 0, sizeof(scip));
				memset(IdOutCmd, 0, sizeof(IdOutCmd));
				if (DoIDENTIFY(hPhysicalDriveIOCTL, &scip, (PSENDCMDOUTPARAMS)&IdOutCmd, (BYTE)bIDCmd, (BYTE)drive, &cbBytesReturned)){
					DWORD diskdata[256];
					int ijk = 0;
					USHORT *pIdSector = (USHORT *)((PSENDCMDOUTPARAMS)IdOutCmd)->bBuffer;
					for (ijk = 0; ijk < 256; ijk++) {
						diskdata[ijk] = pIdSector[ijk];
					}
					PrintIdeInfo(drive, diskdata, sBuff, nLen);
					done = TRUE;
				}
			}

			CloseHandle(hPhysicalDriveIOCTL);
		}
	}//end for
	return done;
}

//  Required to ensure correct PhysicalDrive IOCTL structure setup 
#pragma pack(4) 

// 
// IOCTL_STORAGE_QUERY_PROPERTY 
// 
// Input Buffer: 
//      a STORAGE_PROPERTY_QUERY structure which describes what type of query 
//      is being done, what property is being queried for, and any additional 
//      parameters which a particular property query requires. 
// 
//  Output Buffer: 
//      Contains a buffer to place the results of the query into.  Since all 
//      property descriptors can be cast into a STORAGE_DESCRIPTOR_HEADER, 
//      the IOCTL can be called once with a small buffer then again using 
//      a buffer as large as the header reports is necessary. 
// 

#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS) 

// 
// Device property descriptor - this is really just a rehash of the inquiry 
// data retrieved from a scsi device 
// 
// This may only be retrieved from a target device.  Sending this to the bus 
// will result in an error 
// 
#pragma pack(4) 

//  function to decode the serial numbers of IDE hard drives 
//  using the IOCTL_STORAGE_QUERY_PROPERTY command  
char * flipAndCodeBytes(char * str)
{
	static char flipped[1000];
	int i = 0;
	int j = 0;
	int k = 0;
	int num = strlen(str);
	strcpy_s(flipped, "");
	for (i = 0; i < num; i += 4)
	{
		for (j = 1; j >= 0; j--)
		{
			int sum = 0;
			for (k = 0; k < 2; k++)
			{
				sum *= 16;
				switch (str[i + j * 2 + k])
				{
				case '0':  sum += 0; break;
				case '1':  sum += 1; break;
				case '2':  sum += 2; break;
				case '3':  sum += 3; break;
				case '4':  sum += 4; break;
				case '5':  sum += 5; break;
				case '6':  sum += 6; break;
				case '7':  sum += 7; break;
				case '8':  sum += 8; break;
				case '9':  sum += 9; break;
				case 'a':  sum += 10; break;
				case 'b':  sum += 11; break;
				case 'c':  sum += 12; break;
				case 'd':  sum += 13; break;
				case 'e':  sum += 14; break;
				case 'f':  sum += 15; break;
				case 'A':  sum += 10; break;
				case 'B':  sum += 11; break;
				case 'C':  sum += 12; break;
				case 'D':  sum += 13; break;
				case 'E':  sum += 14; break;
				case 'F':  sum += 15; break;
				}
			}
			if (sum > 0)
			{
				char sub[2];
				sub[0] = (char)sum;
				sub[1] = 0;
				strcat_s(flipped, sub);
			}
		}
	}
	return flipped;
}

typedef struct _MEDIA_SERAL_NUMBER_DATA {
	ULONG  SerialNumberLength;
	ULONG  Result;
	ULONG  Reserved[2];
	UCHAR  SerialNumberData[1];
} MEDIA_SERIAL_NUMBER_DATA, *PMEDIA_SERIAL_NUMBER_DATA;

int ReadPhysicalDriveInNTWithZeroRights(char *sBuff, int nLen)
{
	int done = FALSE;
	HANDLE hPhysicalDriveIOCTL = 0;
	TCHAR driveName[MAX_PATH];

	//获取程序路径
	TCHAR pathBuf[MAX_PATH];
	//	::GetCurrentDirectory(MAX_PATH, pathBuf);
	GetModuleFileName(NULL, pathBuf, 256);
	TCHAR diskIndex[4];
	memset(diskIndex, 0, 4);
	_tcsncpy(diskIndex, pathBuf, 1);
//	TCHAR *cp = _tcsrchr(pathBuf, _T('\\'));
//	if (cp != NULL)
	//	*(cp) = _T('\0');

	//for (int drive = 0; drive < MAX_IDE_DRIVES; drive++){
		//  Try to get a handle to PhysicalDrive IOCTL, report failure 
		//  and exit if can''t. 
		//swprintf(driveName, L"\\\\.\\PhysicalDrive%d", drive);
		//swprintf(driveName, L"\\\\.\\PhysicalDrive%d", drive);
	swprintf(driveName, L"\\\\.\\%s:", diskIndex);
		//  Windows NT, Windows 2000, Windows XP - admin rights not required 
		hPhysicalDriveIOCTL = CreateFile(driveName, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		// if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE) 
		//    printf ("Unable to open physical drive %d, error code: 0x%lX\n", 
		//            drive, GetLastError ()); 

		if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE) {
			STORAGE_PROPERTY_QUERY query;
			DWORD cbBytesReturned = 0;
			char buffer[10000];

			memset((void *)& query, 0, sizeof (query));
			query.PropertyId = StorageDeviceProperty;
			query.QueryType = PropertyStandardQuery;
			memset(buffer, 0, sizeof (buffer));
			if (DeviceIoControl(hPhysicalDriveIOCTL, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query), &buffer, sizeof(buffer), &cbBytesReturned, NULL)){
				STORAGE_DEVICE_DESCRIPTOR * descrip = (STORAGE_DEVICE_DESCRIPTOR *)& buffer;
				char serialNumber[1000];
				char modelNumber[1000];

				strcpy_s(serialNumber,
					flipAndCodeBytes(&buffer[descrip->SerialNumberOffset]));
				strcpy_s(modelNumber, &buffer[descrip->ProductIdOffset]);
				if (0 == sBuff[0] //&&
					//  serial number must be alphanumeric 
					//  (but there can be leading spaces on IBM drives) 
					/*(isalnum(serialNumber[0]) || isalnum(serialNumber[19]))*/) {
					strcpy_s(sBuff, nLen, serialNumber);
					//strcpy_s (HardDriveModelNumber, modelNumber); 
					done = TRUE;
				}
			}
			else{
				DWORD err = GetLastError();
				//printf ("\nDeviceIOControl IOCTL_STORAGE_QUERY_PROPERTY error = %d\n", err); 
			}
			//memset(buffer, 0, sizeof (buffer));
			if (DeviceIoControl(hPhysicalDriveIOCTL, IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER, NULL, 0, &buffer, sizeof(buffer), &cbBytesReturned, NULL)) {
				MEDIA_SERIAL_NUMBER_DATA * mediaSerialNumber = (MEDIA_SERIAL_NUMBER_DATA *)& buffer;
				char serialNumber[1000];
				// char modelNumber [1000]; 
				strcpy_s(serialNumber, (char *)mediaSerialNumber->SerialNumberData);
				// strcpy_s (modelNumber, & buffer [descrip -> ProductIdOffset]); 
				if (0 == sBuff[0] &&
					//  serial number must be alphanumeric 
					//  (but there can be leading spaces on IBM drives) 
					(isalnum(serialNumber[0]) || isalnum(serialNumber[19]))) {
					strcpy_s(sBuff, nLen, serialNumber);
					// strcpy_s (HardDriveModelNumber, modelNumber); 
					done = TRUE;
				}
				//printf ("\n**** MEDIA_SERIAL_NUMBER_DATA for drive %d ****\nSerial Number = %s\n", drive, serialNumber); 
			}
			else{
				//DWORD err = GetLastError (); 
				//switch (err) 
				//{ 
				//case 1:  
				//    printf ("\nDeviceIOControl IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER error = \n" 
				//        "              The request is not valid for this device.\n\n"); 
				//    break; 
				//case 50: 
				//    printf ("\nDeviceIOControl IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER error = \n" 
				//        "              The request is not supported for this device.\n\n"); 
				//    break; 
				//default: 
				//    printf ("\nDeviceIOControl IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER error = %d\n\n", err); 
				//} 
			}

			CloseHandle(hPhysicalDriveIOCTL);
		}
//	}//end for
	return done;
}

// DoIDENTIFY 
// FUNCTION: Send an IDENTIFY command to the drive 
// bDriveNum = 0-3 
// bIDCmd = IDE_ATA_IDENTIFY or IDE_ATAPI_IDENTIFY 
BOOL DoIDENTIFY(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP, PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum, PDWORD lpcbBytesReturned)
{
	// Set up data structures for IDENTIFY command. 
	pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;
	pSCIP->irDriveRegs.bFeaturesReg = 0;
	pSCIP->irDriveRegs.bSectorCountReg = 1;
	//pSCIP -> irDriveRegs.bSectorNumberReg = 1; 
	pSCIP->irDriveRegs.bCylLowReg = 0;
	pSCIP->irDriveRegs.bCylHighReg = 0;
	// Compute the drive number. 
	pSCIP->irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);
	// The command can either be IDE identify or ATAPI identify. 
	pSCIP->irDriveRegs.bCommandReg = bIDCmd;
	pSCIP->bDriveNumber = bDriveNum;
	pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;
	return (DeviceIoControl(hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA, (LPVOID)pSCIP, sizeof(SENDCMDINPARAMS)-1, (LPVOID)pSCOP, sizeof(SENDCMDOUTPARAMS)+IDENTIFY_BUFFER_SIZE - 1, lpcbBytesReturned, NULL));
}

//  --------------------------------------------------- 
// (* Output Bbuffer for the VxD (rt_IdeDinfo record) *) 
typedef struct _rt_IdeDInfo_
{
	BYTE IDEExists[4];
	BYTE DiskExists[8];
	WORD DisksRawInfo[8 * 256];
} rt_IdeDInfo, *pt_IdeDInfo;

// (* IdeDinfo "data fields" *) 
typedef struct _rt_DiskInfo_
{
	BOOL DiskExists;
	BOOL ATAdevice;
	BOOL RemovableDevice;
	WORD TotLogCyl;
	WORD TotLogHeads;
	WORD TotLogSPT;
	char SerialNumber[20];
	char FirmwareRevision[8];
	char ModelNumber[40];
	WORD CurLogCyl;
	WORD CurLogHeads;
	WORD CurLogSPT;
} rt_DiskInfo;

#define  m_cVxDFunctionIdesDInfo  1 

//////////////////////////////////////////////////////////////////////////

int ReadDrivePortsInWin9X(char *sBuff, int nLen)
{
	int done = FALSE;
	HANDLE VxDHandle = 0;
	pt_IdeDInfo pOutBufVxD = 0;
	DWORD lpBytesReturned = 0;
	//  set the thread priority high so that we get exclusive access to the disk 
	BOOL status = SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	if (0 == status){
		//printf ("\nERROR: Could not SetPriorityClass, LastError: %d\n", GetLastError ()); 
	}
	// 1. Make an output buffer for the VxD 
	rt_IdeDInfo info;
	pOutBufVxD = &info;
	// ***************** 
	// KLUDGE WARNING!!! 
	// HAVE to zero out the buffer space for the IDE information! 
	// If this is NOT done then garbage could be in the memory 
	// locations indicating if a disk exists or not. 
	ZeroMemory(&info, sizeof(info));
	// 1. Try to load the VxD 
	//  must use the short file name path to open a VXD file 
	//char StartupDirectory [2048]; 
	//char shortFileNamePath [2048]; 
	//char *p = NULL; 
	//char vxd [2048]; 
	//  get the directory that the exe was started from 
	//GetModuleFileName (hInst, (LPSTR) StartupDirectory, sizeof (StartupDirectory)); 
	//  cut the exe name from string 
	//p = &(StartupDirectory [strlen (StartupDirectory) - 1]); 
	//while (p >= StartupDirectory && *p && ''\\'' != *p) p--; 
	//*p = ''\0'';    
	//GetShortPathName (StartupDirectory, shortFileNamePath, 2048); 
	//sprintf_s (vxd, "\\\\.\\%s\\IDE21201.VXD", shortFileNamePath); 
	//VxDHandle = CreateFile (vxd, 0, 0, 0, 
	//               0, FILE_FLAG_DELETE_ON_CLOSE, 0);    
	VxDHandle = CreateFile(L"\\\\.\\IDE21201.VXD", 0, 0, 0, 0, FILE_FLAG_DELETE_ON_CLOSE, 0);
	if (VxDHandle != INVALID_HANDLE_VALUE) {
		// 2. Run VxD function 
		DeviceIoControl(VxDHandle, m_cVxDFunctionIdesDInfo, 0, 0, pOutBufVxD, sizeof(pt_IdeDInfo), &lpBytesReturned, 0);
		// 3. Unload VxD 
		CloseHandle(VxDHandle);
	}
	else{
		//MessageBox (NULL, "ERROR: Could not open IDE21201.VXD file",TITLE, MB_ICONSTOP); 
	}

	// 4. Translate and store data 
	for (int i = 0; i<8; i++) {
		if ((pOutBufVxD->DiskExists[i]) && (pOutBufVxD->IDEExists[i / 2])) {
			DWORD diskinfo[256];
			for (int j = 0; j < 256; j++)
				diskinfo[j] = pOutBufVxD->DisksRawInfo[i * 256 + j];
			// process the information for this buffer 
			PrintIdeInfo(i, diskinfo, sBuff, nLen);
			done = TRUE;
		}
	}
	//  reset the thread priority back to normal 
	// SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_NORMAL); 
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
	return done;
}

#define  SENDIDLENGTH  sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE 

int ReadIdeDriveAsScsiDriveInNT(char *sBuff, int nLen)
{
	int done = FALSE;
	int controller = 0;
	for (controller = 0; controller < 16; controller++)
	{
		HANDLE hScsiDriveIOCTL = 0;
		TCHAR  driveName[MAX_PATH];
		//  Try to get a handle to PhysicalDrive IOCTL, report failure 
		//  and exit if can''t. 
		swprintf(driveName, L"\\\\.\\Scsi%d:", controller);
		//  Windows NT, Windows 2000, any rights should do 
		hScsiDriveIOCTL = CreateFile(driveName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);
		// if (hScsiDriveIOCTL == INVALID_HANDLE_VALUE) 
		//    printf ("Unable to open SCSI controller %d, error code: 0x%lX\n", 
		//            controller, GetLastError ()); 
		if (hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
		{
			int drive = 0;
			for (drive = 0; drive < 2; drive++)
			{
				char buffer[sizeof (SRB_IO_CONTROL)+SENDIDLENGTH];
				SRB_IO_CONTROL *p = (SRB_IO_CONTROL *)buffer;
				SENDCMDINPARAMS *pin =
					(SENDCMDINPARAMS *)(buffer + sizeof (SRB_IO_CONTROL));
				DWORD dummy;

				memset(buffer, 0, sizeof (buffer));
				p->HeaderLength = sizeof (SRB_IO_CONTROL);
				p->Timeout = 10000;
				p->Length = SENDIDLENGTH;
				p->ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
				strncpy_s((char *)p->Signature, 9, "SCSIDISK", 8);

				pin->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
				pin->bDriveNumber = drive;
				if (DeviceIoControl(hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT,
					buffer,
					sizeof (SRB_IO_CONTROL)+
					sizeof (SENDCMDINPARAMS)-1,
					buffer,
					sizeof (SRB_IO_CONTROL)+SENDIDLENGTH,
					&dummy, NULL))
				{
					SENDCMDOUTPARAMS *pOut =
						(SENDCMDOUTPARAMS *)(buffer + sizeof (SRB_IO_CONTROL));
					IDSECTOR *pId = (IDSECTOR *)(pOut->bBuffer);
					if (pId->sModelNumber[0])
					{
						DWORD diskdata[256];
						int ijk = 0;
						USHORT *pIdSector = (USHORT *)pId;

						for (ijk = 0; ijk < 256; ijk++)
							diskdata[ijk] = pIdSector[ijk];
						PrintIdeInfo(controller * 2 + drive, diskdata, sBuff, nLen);
						done = TRUE;
					}
				}
			}
			CloseHandle(hScsiDriveIOCTL);
		}
	}
	return done;
}


//////////////////////////////////////////////////////////////////////////


//
// IDENTIFY data (from ATAPI driver source)
//

#pragma pack(1)

typedef struct _IDENTIFY_DATA {
	USHORT GeneralConfiguration;            // 00 00
	USHORT NumberOfCylinders;               // 02  1
	USHORT Reserved1;                       // 04  2
	USHORT NumberOfHeads;                   // 06  3
	USHORT UnformattedBytesPerTrack;        // 08  4
	USHORT UnformattedBytesPerSector;       // 0A  5
	USHORT SectorsPerTrack;                 // 0C  6
	USHORT VendorUnique1[3];                // 0E  7-9
	USHORT SerialNumber[10];                // 14  10-19
	USHORT BufferType;                      // 28  20
	USHORT BufferSectorSize;                // 2A  21
	USHORT NumberOfEccBytes;                // 2C  22
	USHORT FirmwareRevision[4];             // 2E  23-26
	USHORT ModelNumber[20];                 // 36  27-46
	UCHAR  MaximumBlockTransfer;            // 5E  47
	UCHAR  VendorUnique2;                   // 5F
	USHORT DoubleWordIo;                    // 60  48
	USHORT Capabilities;                    // 62  49
	USHORT Reserved2;                       // 64  50
	UCHAR  VendorUnique3;                   // 66  51
	UCHAR  PioCycleTimingMode;              // 67
	UCHAR  VendorUnique4;                   // 68  52
	UCHAR  DmaCycleTimingMode;              // 69
	USHORT TranslationFieldsValid : 1;        // 6A  53
	USHORT Reserved3 : 15;
	USHORT NumberOfCurrentCylinders;        // 6C  54
	USHORT NumberOfCurrentHeads;            // 6E  55
	USHORT CurrentSectorsPerTrack;          // 70  56
	ULONG  CurrentSectorCapacity;           // 72  57-58
	USHORT CurrentMultiSectorSetting;       //     59
	ULONG  UserAddressableSectors;          //     60-61
	USHORT SingleWordDMASupport : 8;        //     62
	USHORT SingleWordDMAActive : 8;
	USHORT MultiWordDMASupport : 8;         //     63
	USHORT MultiWordDMAActive : 8;
	USHORT AdvancedPIOModes : 8;            //     64
	USHORT Reserved4 : 8;
	USHORT MinimumMWXferCycleTime;          //     65
	USHORT RecommendedMWXferCycleTime;      //     66
	USHORT MinimumPIOCycleTime;             //     67
	USHORT MinimumPIOCycleTimeIORDY;        //     68
	USHORT Reserved5[2];                    //     69-70
	USHORT ReleaseTimeOverlapped;           //     71
	USHORT ReleaseTimeServiceCommand;       //     72
	USHORT MajorRevision;                   //     73
	USHORT MinorRevision;                   //     74
	USHORT Reserved6[50];                   //     75-126
	USHORT SpecialFunctionsEnabled;         //     127
	USHORT Reserved7[128];                  //     128-255
} IDENTIFY_DATA, *PIDENTIFY_DATA;

#pragma pack()


int ReadPhysicalDriveInNTUsingSmart(char *sBuff, int nLen)
{
	int done = FALSE;
	int drive = 0;

	for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
	{
		HANDLE hPhysicalDriveIOCTL = 0;

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		TCHAR driveName[MAX_PATH];

		swprintf(driveName, L"\\\\.\\PhysicalDrive%d", drive);

		//  Windows NT, Windows 2000, Windows Server 2003, Vista
		hPhysicalDriveIOCTL = CreateFile(driveName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		// if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
		//    printf ("Unable to open physical drive %d, error code: 0x%lX\n",
		//            drive, GetLastError ());

		if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE){
			//if (PRINT_DEBUG) 
			//    printf ("\n%d ReadPhysicalDriveInNTUsingSmart ERROR"
			//    "\nCreateFile(%s) returned INVALID_HANDLE_VALUE\n"
			//    "Error Code %d\n",
			//    __LINE__, driveName, GetLastError ());
		}
		else{
			GETVERSIONINPARAMS GetVersionParams;
			DWORD cbBytesReturned = 0;

			// Get the version, etc of PhysicalDrive IOCTL
			memset((void*)& GetVersionParams, 0, sizeof(GetVersionParams));

			if (!DeviceIoControl(hPhysicalDriveIOCTL, SMART_GET_VERSION, NULL, 0, &GetVersionParams, sizeof (GETVERSIONINPARAMS), &cbBytesReturned, NULL)){
				//if (PRINT_DEBUG)
				//{
				//    DWORD err = GetLastError ();
				//    printf ("\n%d ReadPhysicalDriveInNTUsingSmart ERROR"
				//        "\nDeviceIoControl(%d, SMART_GET_VERSION) returned 0, error is %d\n",
				//        __LINE__, (int) hPhysicalDriveIOCTL, (int) err);
				//}
			}
			else{
				// Print the SMART version
				// PrintVersion (& GetVersionParams);
				// Allocate the command buffer
				ULONG CommandSize = sizeof(SENDCMDINPARAMS)+IDENTIFY_BUFFER_SIZE;
				PSENDCMDINPARAMS Command = (PSENDCMDINPARAMS)new BYTE[CommandSize];
				// Retrieve the IDENTIFY data
				// Prepare the command
#define ID_CMD          0xEC            // Returns ID sector for ATA
				Command->irDriveRegs.bCommandReg = ID_CMD;
				DWORD BytesReturned = 0;
				if (!DeviceIoControl(hPhysicalDriveIOCTL, SMART_RCV_DRIVE_DATA, Command, sizeof(SENDCMDINPARAMS), Command, CommandSize, &BytesReturned, NULL)){
					// Print the error
					//PrintError ("SMART_RCV_DRIVE_DATA IOCTL", GetLastError());
				}
				else{
					// Print the IDENTIFY data
					DWORD diskdata[256];
					USHORT *pIdSector = (USHORT *)(PIDENTIFY_DATA)((PSENDCMDOUTPARAMS)Command)->bBuffer;

					for (int ijk = 0; ijk < 256; ijk++){
						diskdata[ijk] = pIdSector[ijk];
					}

					PrintIdeInfo(drive, diskdata, sBuff, nLen);
					done = TRUE;
				}
				// Done
				CloseHandle(hPhysicalDriveIOCTL);
				delete[]Command;
			}
		}
	}

	return done;
}

void PrintIdeInfo(int drive, DWORD diskdata[256], char *sDest, int nLen)
{
	char string1[1024];
	__int64 sectors = 0;
	__int64 bytes = 0;
	//  copy the hard drive serial number to the buffer 
	strcpy_s(string1, ConvertToString(diskdata, 10, 19));
	if (0 == sDest[0] &&
		//  serial number must be alphanumeric 
		//  (but there can be leading spaces on IBM drives) 
		(isalnum(string1[0]) || isalnum(string1[19])))
	{
		strcpy_s(sDest, nLen, string1);
		//strcpy_s (HardDriveModelNumber, ConvertToString (diskdata, 27, 46)); 
	}
}

char *ConvertToString(DWORD diskdata[256], int firstIndex, int lastIndex)
{
	static char string[1024];
	int position = 0;
	//  each integer has two characters stored in it backwards 
	for (int index = firstIndex; index <= lastIndex; index++)
	{
		//  get high byte for 1st character 
		string[position] = (char)(diskdata[index] / 256);
		position++;
		//  get low byte for 2nd character 
		string[position] = (char)(diskdata[index] % 256);
		position++;
	}
	//  end the string  
	string[position] = '\0';
	//  cut off the trailing blanks 
	for (int index = position - 1; index > 0 && ' ' == string[index]; index--)
		string[index] = '\0';
	return string;
}
int GetHardDriveSerialNumber(char *sBuff, int nLen)
{
	int done = FALSE;
	// char string [1024];
	__int64 id = 0;
	OSVERSIONINFO version = { 0 };
	version.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx(&version);

	memset(sBuff, 0, nLen);

	if (version.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		done = ReadPhysicalDriveInNTWithZeroRights(sBuff, nLen);
		//  this works under WinNT4 or Win2K if you have admin rights
		//printf ("\nTrying to read the drive IDs using physical access with admin rights\n");
	/*	done = ReadPhysicalDriveInNTWithAdminRights(sBuff, nLen);

		//  this should work in WinNT or Win2K if previous did not work
		//  this is kind of a backdoor via the SCSI mini port driver into
		//     the IDE drives
		//printf ("\nTrying to read the drive IDs using the SCSI back door\n");
		if (!done){
			done = ReadIdeDriveAsScsiDriveInNT(sBuff, nLen);
		}

		//  this works under WinNT4 or Win2K or WinXP if you have any rights
		//printf ("\nTrying to read the drive IDs using physical access with zero rights\n");
		if (!done){
			done = ReadPhysicalDriveInNTWithZeroRights(sBuff, nLen);
		}

		//  this works under WinNT4 or Win2K or WinXP or Windows Server 2003 or Vista if you have any rights
		//printf ("\nTrying to read the drive IDs using Smart\n");
		if (!done){
			done = ReadPhysicalDriveInNTUsingSmart(sBuff, nLen);
		}
	}
	else{
		//  this works under Win9X and calls a VXD
		//  try this up to 10 times to get a hard drive serial number
		for (int attempt = 0; attempt < 10 && !done && 0 == sBuff[0]; attempt++) {
			done = ReadDrivePortsInWin9X(sBuff, nLen);
		}*/
	}

	return done;
}




