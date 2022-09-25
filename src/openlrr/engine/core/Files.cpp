// Files.cpp : 
//

#include "../../platform/windows.h"

#include "../util/Registry.h"
#include "Errors.h"
#include "Memory.h"
#include "Utils.h"
#include "Wad.h"

#include "Files.h"


/**********************************************************************************
 ******** Globals
 **********************************************************************************/

#pragma region Globals

// <LegoRR.exe @004abf00>
sint32 & Gods98::currWadHandle = *(sint32*)0x004abf00; // = -1; // = WAD_ERROR;

// <LegoRR.exe @005349a0>
Gods98::File_Globs & Gods98::fileGlobs = *(Gods98::File_Globs*)0x005349a0; // = { 0 };

// <LegoRR.exe @005779e0>
Gods98::FileCheck_Globs & Gods98::checkGlobs = *(Gods98::FileCheck_Globs*)0x005779e0; // (no init)


// Data directory has priority over loading from WAD files.
static bool _dataFirst = false;

// CD drive will never be checked if false.
static bool _useCD = true;

// WAD files will never be checked if false.
static bool _useWads = true;

// Counterpart to fileGlobs.dataDir. For looking up files relative to the CWD when used with FileFlags::FILE_FLAG_EXEDIR.
//static char _exeDir[FILE_MAXPATH];
static std::string _exeDir = "";

// Data directory assigned by File_InitDataDir. Only used during File_Initialise.
static std::string _initDataDirectory = "";

// WAD file directory assigned by File_InitWadDir. Only used during File_Initialise.
static std::string _initWadDirectory = "";

#pragma endregion

/**********************************************************************************
 ******** Macros
 **********************************************************************************/

#pragma region Macros

#define StdFile(f) ((FILE*)((f)->std))
#define WadFile(f) ((WADFILE*)((f)->wad))

#pragma endregion

/**********************************************************************************
 ******** Functions
 **********************************************************************************/

#pragma region Functions

// multiple-uses: logf_removed
// <LegoRR.exe @00484f50>
void __cdecl Gods98::File_ErrorFile(const char* msg, ...)
{
	log_firstcall();

}

// <LegoRR.exe @0047f3f0>
void __cdecl Gods98::File_Initialise(const char* programName, bool32 insistOnCD, const char* registryLocation)
{
	log_firstcall();

	bool32 foundCD = false;
	bool32 foundWad = false;
	bool32 failed = false;

	if (_initDataDirectory.empty()) File_SetDataDir(""); // Initialise Data directory to its default setting.
	std::strcpy(fileGlobs.dataDir, _initDataDirectory.c_str());
	_exeDir = File_GetWorkingDir();

//#ifdef _GODS98_USEWAD_
	{
		if (_initWadDirectory.empty()) File_SetWadDir(""); // Initialise WAD file directory to its default setting.

		char wadFile[FILE_MAXPATH];
		for (uint32 i = 0; i < MAX_WADS; i++) {
			std::sprintf(wadFile, "%s\\%s%i.wad", _initWadDirectory.c_str(), programName, (sint32)i);
			const sint32 wad = File_LoadWad(wadFile);
			if (wad == WAD_ERROR) {
				// Reduce useless warnings:
				//Error_Warn(true, Error_Format("Cannot load %s", wadFile));
			}
			else {
//				File_SetActiveWad(wad);
				foundWad = true;
			}
		}
		File_SetBaseSearchPath(fileGlobs.dataDir);
	}
//#endif // _GODS98_USEWAD_


	while (!(foundCD = File_FindDataCD()) && insistOnCD) {

		char msgNoCD[1024];
		char msgError[1024] = "Error";

		if (Registry_GetValue(registryLocation, "CDMissing", RegistryValue::String, msgNoCD, sizeof(msgNoCD))) {

			Registry_GetValue(registryLocation, "SetupError", RegistryValue::String, msgError, sizeof(msgError));

			if (::MessageBoxA(nullptr, msgNoCD, msgError, MB_OKCANCEL) == IDCANCEL) std::exit(0);

		} else std::exit(0);
	}

	if (!foundCD && !foundWad) { // Assume that if a wad is found then it is the correct one.

		//const char* dataDirName = FILE_DATADIRNAME;
		const char* dataDirName = fileGlobs.dataDir;
		for (const char* s = dataDirName; *s != '\0'; s++) {
			if (*s == '\\') dataDirName = s + 1;
		}

		failed = true;

		_finddata32_t findData;
		intptr_t handle;
		//if ((handle = ::_findfirst32("*.*", &findData)) != -1) {
		if ((handle = ::_findfirst32(fileGlobs.dataDir, &findData)) != -1) {
			do {
				if (findData.attrib & _A_SUBDIR) {
					//if (::_stricmp(findData.name, FILE_DATADIRNAME) == 0) {
					if (::_stricmp(findData.name, dataDirName) == 0) {
						failed = false;
						break;
					}
				}
			} while (::_findnext32(handle, &findData) == 0);

			::_findclose(handle);
		}
	}

	if (insistOnCD) {
		char fname[1024];
		std::sprintf(fname, "%s\\%s", fileGlobs.dataDir, FILE_KEYFILENAME);
		//std::sprintf(fname, "%s\\%s", FILE_DATADIRNAME, FILE_KEYFILENAME);
		FILE* fp;
		if (fp = std::fopen(fname, "r")) {
			std::fclose(fp);
			failed = true;
		}
	}

	if (!failed) {

		// If everything started up ok (CD in or WAD found), then ensure the data directory exists...
		::_mkdir(fileGlobs.dataDir);
		/*::_mkdir(FILE_DATADIRNAME);

		// Remove the 'delme' file if it exists...
		char fname[1024];
		std::sprintf(fname, "%s\\%s", FILE_DATADIRNAME, FILE_DELMEFILENAME);
		
		FILE* fp;
		if (fp = std::fopen(fname, "r")) {
			std::fclose(fp);
			::_chmod(fname, _S_IWRITE);
			std::remove(fname);
		}*/

	} else {
		char msgNoData[1024];
		char msgError[1024] = "Error";

		if (Registry_GetValue(registryLocation, "DataMissing", RegistryValue::String, msgNoData, sizeof(msgNoData))) {
			Registry_GetValue(registryLocation, "SetupError", RegistryValue::String, msgError, sizeof(msgError));
			::MessageBoxA(nullptr, msgNoData, msgError, MB_OK);
		}
		std::exit(0);
	}
}

// <LegoRR.exe @0047f7b0>
bool32 __cdecl Gods98::File_FindDataCD(void)
{
	log_firstcall();

	/// COMMANDLINE OPTION: -noCD "Never try to open files from, or locate the CD."
	if (!_useCD)
		return false;

	char drive[] = "A:\\";
	char fname[FILE_MAXPATH];

	for (char letter = 'C'; letter <= 'Z'; letter++) {
		drive[0] = letter;
		if (::GetDriveTypeA(drive) == DRIVE_CDROM) {
			std::sprintf(fname, "%c:\\%s\\%s", letter, FILE_DATADIRNAME, FILE_KEYFILENAME);
			FILE* fp;
			if (fp = std::fopen(fname, "r")) {
				std::fclose(fp);
				fileGlobs.cdLetter = letter;
				return true;
			}
		}
	}
	return false;
}

// This allows the translation from File paths into Wad names.
// <LegoRR.exe @0047f850>
bool32 __cdecl Gods98::File_SetBaseSearchPath(const char* basePath)
{
	log_firstcall();

	size_t len = (basePath ? std::strlen(basePath) : 0);
	if (len > 0 && len < MAX_WAD_BASE_PATH) {
		std::strcpy(fileGlobs.wadBasePath, basePath);
		fileGlobs.basePathSet = true;
		return true;
	}
	else {
		std::memset(fileGlobs.wadBasePath, 0, sizeof(fileGlobs.wadBasePath));
		fileGlobs.basePathSet = false;
		return false;
	}
}

// <LegoRR.exe @0047f8c0>
void __cdecl Gods98::File_Error(const char* msg, ...)
{
	log_firstcall();

	std::va_list args;
	char buff[1024];
	va_start(args, msg);
	std::vsprintf(buff, msg, args);
	////////////////////////
	::OutputDebugStringA(buff);//CHANGE TO WHATEVER IS SUITABLE
	////////////////////////
	va_end(args);
}

// Extra functions for using with Wads.
// <LegoRR.exe @0047f900>
sint32 __cdecl Gods98::File_LoadWad(const char* fName)
{
	log_firstcall();

	//File_ErrorFile(0);			// Open the wad error log
	return Wad_Load(fName);
}

// <missing>
void __cdecl Gods98::File_CloseWad(sint32 hWad)
{
	//File_ErrorFile(0);			// Close the wad error log
	//if (hWad == WAD_ERROR) Wad_Close(hWad);
	/// FIX: originally == WAD_ERROR
	if (hWad != WAD_ERROR) Wad_Close(hWad);
}

// <missing>
void __cdecl Gods98::File_SetActiveWad(sint32 hWad)
{
	currWadHandle = hWad;
}

// <LegoRR.exe @0047f920>
bool32 __cdecl Gods98::File_GetCDFilePath(IN OUT char* path, const char* fname)
{
	log_firstcall();

	/// COMMANDLINE OPTION: -noCD "Never try to open files from, or locate the CD."
	if (!_useCD)
		return false;

	if (fileGlobs.cdLetter != '\0') {
		std::sprintf(path, "%c:\\%s\\%s", fileGlobs.cdLetter, FILE_DATADIRNAME, fname);
		return true;
	}

	return false;
}

// C library implementation

// <LegoRR.exe @0047f960>
void __cdecl Gods98::File_MakeDir(const char* path)
{
	File_MakeDir2(path, FileFlags::FILE_FLAGS_DEFAULT);
}

/// CUSTOM: Creates a directory, with additional flags specifying where to create it.
void Gods98::File_MakeDir2(const char* path, FileFlags fileFlags)
{
	log_firstcall();

	if (fileFlags & FileFlags::FILE_FLAG_NOSTD)
		return; // Can only create directories in the standard file system.

	char name[FILE_MAXPATH];
	std::strcpy(name, File_VerifyFilename2(path, fileFlags));
	char* start = name;

	switch (fileFlags & FileFlags::FILE_FLAGS_PATHMASK) {
	case FileFlags::FILE_FLAG_DATADIR:
		start += std::strlen(fileGlobs.dataDir); // Skip past dataDir
		break;
	case FileFlags::FILE_FLAG_EXEDIR:
		start += _exeDir.length(); // Skip past exeDir
		break;
	case FileFlags::FILE_FLAG_ABSOLUTEPATH:
	default:
		if (std::toupper(start[0]) >= 'A' && std::toupper(start[0]) <= 'Z' && start[1] == ':') {
			start += 2; // Skip volume name "C:<path>"
		}
		if (*start == '\\') {
			start++; // Skip root path slash "\<path>"
		}
		break;
	}

	for (char* s = start; *s != '\0'; s++) {
		if (*s == '\\') {
			*s = '\0';
			::_mkdir(name);
			*s = '\\';
		}
	}
}

/// CUSTOM: Subfunction of File_Open
Gods98::File* Gods98::File_FromStandardFile(FILE* stdfile)
{
	if (!stdfile) return nullptr;

	File* file = _File_Alloc(FileSys::Standard);
	if (!file) return nullptr;

	file->std = stdfile;
	return file;
}

// <LegoRR.exe @0047f9a0>
Gods98::File* __cdecl Gods98::File_Open(const char* fName, const char* mode)
{
	return File_Open2(fName, mode, FileFlags::FILE_FLAGS_DEFAULT);
}

/// CUSTOM: Opens a file, with additional flags specifying where and what checks are used to open it.
Gods98::File* Gods98::File_Open2(const char* fName, const char* mode, FileFlags fileFlags)
{
	log_firstcall();

	bool dataFirst = _dataFirst;
	switch (fileFlags & FileFlags::FILE_FLAGS_PRIORITYMASK) {
	case FileFlags::FILE_FLAG_DATAPRIORITY:
		dataFirst = true;
		break;
	case FileFlags::FILE_FLAG_WADPRIORITY:
		dataFirst = false;
		break;
	}

	bool useStd = !(fileFlags & FileFlags::FILE_FLAG_NOSTD);
	bool useCD  = _useCD && !(fileFlags & FileFlags::FILE_FLAG_NOCD);
	switch (fileFlags & FileFlags::FILE_FLAGS_PATHMASK) {
	case FileFlags::FILE_FLAG_DATADIR:
	case FileFlags::FILE_FLAG_EXEDIR:
		break;
	case FileFlags::FILE_FLAG_ABSOLUTEPATH:
	default:
		useCD  = false;
		break;
	}

	const char* fullName = File_VerifyFilename2(fName, fileFlags);

	/// COMMANDLINE OPTION: -datafirst "Data files have precedence over WAD files."
	if (dataFirst && useStd && Util_StrIStr(mode, "r") != nullptr) {
		// dataFirst does NOT include checking the CD.
		FILE* stdfile;
		if (stdfile = std::fopen(fullName, mode)) {
			// Don't slow things down by opening the file twice (first to verify).
			File_ErrorFile("STD Load %s\n", fullName);
			return File_FromStandardFile(stdfile);
		}
	}

	FileSys fs = _File_CheckSystem2(fullName, mode, fileFlags);
	File* file = nullptr; // dummy init

	switch (fs)
	{
	case FileSys::Standard:
		file = _File_Alloc(fs);
		if (!file) return nullptr;

		file->std = nullptr;
		if (useStd) {
			if (file->std = std::fopen(fullName, mode)) {
				File_ErrorFile("STD Load %s\n", fullName);
				return file;
			}
		}
		if (useCD && file->std == nullptr) {
			if (Util_StrIStr(mode, "w") == nullptr) { // CDROM is readonly
				char cdName[FILE_MAXPATH];
				if (File_GetCDFilePath(cdName, fName)) {
					if (file->std = std::fopen(cdName, mode)) {
						File_ErrorFile("CD Load %s\n", cdName);
						return file;
					}
				}
			}
		}
		File_ErrorFile("STD Fail %s\n", fullName);
		_File_Dealloc(file);
		return nullptr;

	case FileSys::Wad:
		file = _File_Alloc(fs);
		if (!file) return nullptr;

		if (_File_OpenWad(file->wad, _File_GetWadName(fullName))) {
			File_ErrorFile("WAD Load %s\n", _File_GetWadName(fullName));
			return file;
		}
		File_ErrorFile("WAD Fail %s\n", _File_GetWadName(fullName));
		_File_Dealloc(file);
		return nullptr;

	case FileSys::Error:
	default:
		File_Error("%s(%i) : Error in call to %s\n", __FILE__, __LINE__, "File_Open");
		break;
	}
	return nullptr;
}

// <LegoRR.exe @0047fb10>
sint32 __cdecl Gods98::File_Seek(File* f, sint32 pos, SeekOrigin mode)
{
	log_firstcall();

	FileSys fs = _File_GetSystem(f);
	switch (fs)
	{
	case FileSys::Standard:
		return (sint32)std::fseek(StdFile(f), pos, (sint32)mode);

	case FileSys::Wad:
		switch (mode)
		{
		case SeekOrigin::Set:
			WadFile(f)->streamPos = pos;
			if (WadFile(f)->streamPos > Wad_hLength(WadFile(f)->hFile))
				WadFile(f)->streamPos = Wad_hLength(WadFile(f)->hFile);
			if (WadFile(f)->streamPos < 0) WadFile(f)->streamPos = 0;
			break;
		case SeekOrigin::Current:
			WadFile(f)->streamPos += pos;
			if (WadFile(f)->streamPos > Wad_hLength(WadFile(f)->hFile))
				WadFile(f)->streamPos = Wad_hLength(WadFile(f)->hFile);
			if (WadFile(f)->streamPos < 0) WadFile(f)->streamPos = 0;
			break;
		case SeekOrigin::End:
			WadFile(f)->streamPos = Wad_hLength(WadFile(f)->hFile) + pos;
			break;
		default:
			File_Error("Uknown seek mode (%i)", (sint32)mode);
		}
		break;

	case FileSys::Error:
	default:
		File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_Seek");
		break;
	}
	return 0;
}

// <LegoRR.exe @0047fc40>
sint32 __cdecl Gods98::File_Read(OUT void* buffer, sint32 size, sint32 count, File* f)
{
	log_firstcall();

	FileSys fs = _File_GetSystem(f);
	switch (fs)
	{
	case FileSys::Standard:
		return (sint32)std::fread(buffer, size, count, StdFile(f));

	case FileSys::Wad: {
		int len = Wad_hLength(WadFile(f)->hFile);
		int amountToCopy;
		if ((WadFile(f)->streamPos + (size * count)) > len) amountToCopy = len - WadFile(f)->streamPos;
		else amountToCopy = size * count;
		std::memcpy(buffer, (char*)Wad_hData(WadFile(f)->hFile) + WadFile(f)->streamPos, amountToCopy);
		WadFile(f)->streamPos += amountToCopy;
		return amountToCopy / size;
	}
	case FileSys::Error:
	default:
		File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_Read");
		break;
	}
	return 0;
}

// <LegoRR.exe @0047fd10>
sint32 __cdecl Gods98::File_Write(const void* buffer, sint32 size, sint32 count, File* f)
{
	log_firstcall();

	FileSys fs = _File_GetSystem(f);
	switch (fs)
	{
	case FileSys::Standard:
		return (sint32)std::fwrite(buffer, size, count, StdFile(f));

	case FileSys::Wad:
		File_Error("Cannot write to a file stored in a Wad!");
		break;
	case FileSys::Error:
	default:
		File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_Write");
		break;
	}
	return 0;
}

// <LegoRR.exe @0047fd80>
sint32 __cdecl Gods98::File_Close(File* f)
{
	log_firstcall();

	FileSys fs = _File_GetSystem(f);
	switch (fs)
	{
	case FileSys::Standard:
	case FileSys::Wad:
		_File_Dealloc(f);
		break;
	case FileSys::Error:
	default:
		File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_Close");
		break;
	}
	return 0;
}

// <missing>
sint32 __cdecl Gods98::File_EOF(File* f)
{
	FileSys fs = _File_GetSystem(f);
	switch (fs)
	{
	case FileSys::Standard:
		return (sint32)std::feof(StdFile(f));

	case FileSys::Wad:
		return WadFile(f)->streamPos >= (Wad_hLength(WadFile(f)->hFile) - 1);

	case FileSys::Error:
	default:
		File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_EOF");
		break;
	}
	return 0;
}

// <LegoRR.exe @0047fdd0>
sint32 __cdecl Gods98::File_Tell(File* f)
{
	log_firstcall();

	FileSys fs = _File_GetSystem(f);
	switch (fs)
	{
	case FileSys::Standard:
		return (sint32)std::ftell(StdFile(f));

	case FileSys::Wad:
		return WadFile(f)->streamPos;

	case FileSys::Error:
	default:
		File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_Tell");
		break;
	}
	return 0;
}

// <missing>
sint32 __cdecl Gods98::File_Flush(File* f)
{
	FileSys fs = _File_GetSystem(f);
	switch (fs)
	{
	case FileSys::Standard:
		return (sint32)std::fflush(StdFile(f));

	case FileSys::Wad:
		return 0;

	case FileSys::Error:
	default:
		File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_Flush");
		break;
	}
	return 0;
}

// <LegoRR.exe @0047fe20>
bool32 __cdecl Gods98::File_Exists(const char* fName)
{
	return File_Exists2(fName, FileFlags::FILE_FLAGS_DEFAULT);
}

/// CUSTOM: Checks for a file's existence, with additional flags specifying where and what checks are used.
bool32 Gods98::File_Exists2(const char* fName, FileFlags fileFlags)
{
	log_firstcall();

	bool useStd = !(fileFlags & FileFlags::FILE_FLAG_NOSTD);
	bool useCD  = _useCD && !(fileFlags & FileFlags::FILE_FLAG_NOCD);
	switch (fileFlags & FileFlags::FILE_FLAGS_PATHMASK) {
	case FileFlags::FILE_FLAG_DATADIR:
	case FileFlags::FILE_FLAG_EXEDIR:
		break;
	case FileFlags::FILE_FLAG_ABSOLUTEPATH:
	default:
		useCD  = false;
		break;
	}

	const char* fullName = File_VerifyFilename2(fName, fileFlags);
	FileSys fs = _File_CheckSystem2(fullName, "r", fileFlags);
	switch (fs)
	{
	case FileSys::Standard: {
		FILE* f = nullptr;
		if (useStd) {
			if (f = std::fopen(fullName, "r")) {
				std::fclose(f);
				return true;
			}
		}
		if (useCD && f == nullptr) {
			char cdName[FILE_MAXPATH];
			if (File_GetCDFilePath(cdName, fName)) {
				if (f = std::fopen(cdName, "r")) {
					std::fclose(f);
					return true;
				}
			}
		}
		break;
	}
	case FileSys::Wad:
		/// CHANGE: This should always be true, because _File_CheckSystem2 already found the file in a wad.
		return true;
		/// FIX APPLY: Use _File_GetWadName to check for the correct name, and check against WAD_ERROR instead of non-zero.
		//return Wad_IsFileInWad(_File_GetWadName(fullName), currWadHandle) != WAD_ERROR;

	case FileSys::Error:
	default:
		break;
	}
	return false;
}

// <LegoRR.exe @0047fee0>
sint32 __cdecl Gods98::File_GetC(File* f)
{
	log_firstcall();

	FileSys fs = _File_GetSystem(f);
	switch (fs)
	{
	case FileSys::Standard:
		return (sint32)std::fgetc(StdFile(f));

	case FileSys::Wad: {
		sint32 len = Wad_hLength(WadFile(f)->hFile);
		if (WadFile(f)->streamPos >= len - 1)
		{
			return EOF;
		}
		else
		{
			int c = *((int*)((char*)Wad_hData(WadFile(f)->hFile) + WadFile(f)->streamPos));
			WadFile(f)->streamPos++;
			return c;
		}
	}
	case FileSys::Error:
	default:
		File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_GetC");
		break;
	}
	return 0;
}

// <LegoRR.exe @0047ff60>
sint32 __cdecl Gods98::File_Length(File* f)
{
	log_firstcall();

	sint32 pos = File_Tell(f);
	File_Seek(f, 0, SeekOrigin::End);
	sint32 len = File_Tell(f);
	File_Seek(f, pos, SeekOrigin::Set);
	return len;
}

// <LegoRR.exe @0047ffa0>
char* __cdecl Gods98::File_InternalFGetS(OUT char* fgetsBuffer, sint32 num, File* f)
{
	log_firstcall();

	sint32 pos = 0;
	while ((fgetsBuffer[pos] = (char)File_GetC(f)) != '\0' && fgetsBuffer[pos] != '\n' && fgetsBuffer[pos] != EOF && pos != num) {
		pos++;
	}
	fgetsBuffer[pos + 1] = '\0';
	if (pos == 0) return nullptr;
	else return fgetsBuffer;
}

// <LegoRR.exe @00480000>
char* __cdecl Gods98::File_GetS(OUT char* fgetsBuffer, sint32 num, File* f)
{
	log_firstcall();

	FileSys fs = _File_GetSystem(f);
	switch (fs)
	{
	case FileSys::Standard:
		return std::fgets(fgetsBuffer, num, StdFile(f));

	case FileSys::Wad:
		return File_InternalFGetS(fgetsBuffer, num, f);

	case FileSys::Error:
	default:
		File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_GetS");
		break;
	}
	return 0;
}

// <LegoRR.exe @00480070>
sint32 __cdecl Gods98::File_PrintF(File* f, const char* msg, ...)
{
	log_firstcall();

	FileSys fs = _File_GetSystem(f);

	std::va_list arg;
	va_start(arg, msg);
	/// FIXME: other FileSys types do not call va_end(?)

	switch (fs)
	{
	case FileSys::Standard: {
		/// FIXME: ret is not returned
		sint32 ret = std::vfprintf(StdFile(f), msg, arg);
		va_end(arg);
		break;
	}
	case FileSys::Wad:
		File_Error("\"fprintf\" is unsupprted for wad files");
		break;
	case FileSys::Error:
	default:
		File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_PrintF");
		break;
	}
	return 0;
}

// <missing>
uint32 __cdecl Gods98::File_VPrintF(File* f, const char* msg, std::va_list args)
{
	FileSys fs = _File_GetSystem(f);

	if (FileSys::Standard == fs)
	{
		char buffer[1024];

		uint32 len = (uint32)std::vsprintf(buffer, msg, args);
		Error_Fatal(len >= sizeof(buffer), "Buffer too small");
		File_Write(buffer, sizeof(char), len, f);

		return len;
	}

	return 0;
}

// <missing>
sint32 __cdecl Gods98::File_ScanF(File* f, const char* msg, ...)
{
	/// FIXME: not implemented by GODS98
	return 0;
}


// <LegoRR.exe @004800e0>
Gods98::FileSys __cdecl Gods98::_File_GetSystem(File* f)
{
	log_firstcall();

	return f->type;
}

// <LegoRR.exe @004800f0>
Gods98::FileSys __cdecl Gods98::_File_CheckSystem(const char* fName, const char* mode)
{
	return _File_CheckSystem2(fName, mode, FileFlags::FILE_FLAGS_DEFAULT);
}

/// CUSTOM: Checks which file system a file is found in, with additional flags specifying where to check.
Gods98::FileSys Gods98::_File_CheckSystem2(const char* fName, const char* mode, FileFlags fileFlags)
{
	log_firstcall();

	if (!fName || !mode || !std::strlen(fName) || !std::strlen(mode)) return FileSys::Error;

	bool dataFirst = _dataFirst;
	switch (fileFlags & FileFlags::FILE_FLAGS_PRIORITYMASK) {
	case FileFlags::FILE_FLAG_DATAPRIORITY:
		dataFirst = true;
		break;
	case FileFlags::FILE_FLAG_WADPRIORITY:
		dataFirst = false;
		break;
	}

	bool useStd = !(fileFlags & FileFlags::FILE_FLAG_NOSTD);
	bool useWad = _useWads && !(fileFlags & FileFlags::FILE_FLAG_NOWAD);
	bool useCD  = _useCD   && !(fileFlags & FileFlags::FILE_FLAG_NOCD);
	switch (fileFlags & FileFlags::FILE_FLAGS_PATHMASK) {
	case FileFlags::FILE_FLAG_DATADIR:
		break;
	case FileFlags::FILE_FLAG_EXEDIR:
		useWad = false;
		break;
	case FileFlags::FILE_FLAG_ABSOLUTEPATH:
	default:
		useWad = false;
		useCD  = false;
		break;
	}

	if (Util_StrIStr(mode, "w") != nullptr) { // WAD is readonly
		if (useStd) {
			// File must be opened as stdC
			return FileSys::Standard;
		}
	}
	else {
		/// COMMANDLINE OPTION: Data files have precedence over WAD files.
		if (dataFirst && useStd && useWad) { // Only perform the check if wads are in-use.
			FILE* f;
			if (f = std::fopen(fName, mode)) {
				std::fclose(f);
				return FileSys::Standard;
			}
		}

		if (useWad && Wad_IsFileInWad(_File_GetWadName(fName), currWadHandle) != WAD_ERROR) {
			// The file is in the wad so we can use the wad version
			return FileSys::Wad;
		}
		else if (useStd || useCD) {
			// Otherwise we will try the normal file system
			return FileSys::Standard;
		}
	}
	return FileSys::Error;
}

// <LegoRR.exe @00480160>
bool32 __cdecl Gods98::_File_OpenWad(WADFILE* wad, const char* fName)
{
	log_firstcall();

	wad->eof = false;
	wad->streamPos = 0;
	wad->hFile = Wad_FileOpen(fName, currWadHandle);
	return (bool32)(wad->hFile == WAD_ERROR ? false : true);
}

// <LegoRR.exe @00480190>
Gods98::File* __cdecl Gods98::_File_Alloc(FileSys fType)
{
	log_firstcall();

	if (fType == FileSys::Standard)
	{
		File* f;
		f = (File*)_File_Malloc(sizeof(File));
		f->type = fType;
		return f;
	}
	else if (fType == FileSys::Wad)
	{
		File* f = (File*)_File_Malloc(sizeof(File));
		if (f)
		{
			f->type = fType;
			f->wad = (WADFILE*)_File_Malloc(sizeof(WADFILE));
			if (f->wad) return f;
			else _File_Free(f);
		}
	}
	return nullptr;
}

// <LegoRR.exe @004801f0>
void* __cdecl Gods98::_File_Malloc(uint32 size)
{
	log_firstcall();

	return Mem_Alloc(size);	// Change to whatever is required
}

// <LegoRR.exe @00480200>
void __cdecl Gods98::_File_Free(void* ptr)
{
	log_firstcall();

	Mem_Free(ptr);				// Change to whatever is required
}

// <LegoRR.exe @00480210>
void __cdecl Gods98::_File_Dealloc(File* file)
{
	log_firstcall();

	if (file)
	{
		if (_File_GetSystem(file) == FileSys::Standard)
		{
			if (file->std) std::fclose(StdFile(file));
			_File_Free(file);
		}
		else if (_File_GetSystem(file) == FileSys::Wad)
		{
			if (file->wad)
			{
				Wad_FileClose(file->wad->hFile);
				_File_Free(file->wad);
			}
			_File_Free(file);
		}
	}
}

// <LegoRR.exe @00480280>
const char* __cdecl Gods98::_File_GetWadName(const char* fName)
{
	log_firstcall();

	//static char wadedName[MAX_WAD_BASE_PATH];
	//fileGlobs.s_GetWadName_wadedName

	if (fileGlobs.basePathSet) {
		int fLen = std::strlen(fName);
		int wLen = std::strlen(fileGlobs.wadBasePath);
		if (fLen <= wLen) {
			return fName;
		}
		else {
			char copy = fName[wLen];
			const_cast<char*>(fName)[wLen] = '\0';
			if (!::_stricmp(fName, fileGlobs.wadBasePath)) {
				const char* ptr = fName + wLen + 1;
				std::sprintf(fileGlobs.s_GetWadName_wadedName, "%s", ptr);
				const_cast<char*>(fName)[wLen] = copy;
				return fileGlobs.s_GetWadName_wadedName;
			}
			else {
				const_cast<char*>(fName)[wLen] = copy;
				return fName;
			}
		}
	}
	else return fName;
}

// <LegoRR.exe @00480310>
char* __cdecl Gods98::File_GetLine(OUT char* buffer, uint32 size, File* file)
{
	log_firstcall();

	// Same as File_GetS() but without the return character.

	char* res = File_GetS(buffer, size, file);
	uint32 length = std::strlen(buffer) - 1;
	if (buffer[length] == '\n') buffer[length] = '\0';
	if (length && buffer[length - 1] == '\r') buffer[length - 1] = '\0';

	return res;
}

// <LegoRR.exe @00480360>
void* __cdecl Gods98::File_LoadBinary(const char* filename, OPTIONAL OUT uint32* sizeptr)
{
	log_firstcall();

	return File_LoadBinary2(filename, sizeptr, FileFlags::FILE_FLAGS_DEFAULT);
}

/// CUSTOM: Support for FileFlags.
void* __cdecl Gods98::File_LoadBinary2(const char* filename, OPTIONAL OUT uint32* sizeptr, FileFlags fileFlags)
{
	log_firstcall();

	return File_Load2(filename, sizeptr, true, 0, fileFlags);
}

// <missing>
void* __cdecl Gods98::File_LoadASCII(const char* filename, OPTIONAL OUT uint32* sizeptr)
{
	return File_LoadASCII2(filename, sizeptr, FileFlags::FILE_FLAGS_DEFAULT);
}

/// CUSTOM: Support for FileFlags.
void* Gods98::File_LoadASCII2(const char* filename, OPTIONAL OUT uint32* sizeptr, FileFlags fileFlags)
{
	return File_Load2(filename, sizeptr, false, 0, fileFlags);
}

// <LegoRR.exe @00480380>
void* __cdecl Gods98::File_Load(const char* filename, OPTIONAL OUT uint32* sizeptr, bool32 binary)
{
	log_firstcall();

	return File_Load2(filename, sizeptr, binary, 0, FileFlags::FILE_FLAGS_DEFAULT);
}


/// CUSTOM: Extension of File_LoadBinary that allocates one extra byte and null-terminates the end of the buffer.
///         Extra byte IS NOT included in the value returned by sizeptr.
void* __cdecl Gods98::File_LoadBinaryString(const char* filename, OPTIONAL OUT uint32* sizeptr)
{
	return File_LoadBinaryString2(filename, sizeptr, FileFlags::FILE_FLAGS_DEFAULT);
}

/// CUSTOM: Extension of File_LoadBinary that allocates one extra byte and null-terminates the end of the buffer.
///         Extra byte IS NOT included in the value returned by sizeptr.
///         Includes additional flags specifying where and what checks are used to load.
void* Gods98::File_LoadBinaryString2(const char* filename, OPTIONAL OUT uint32* sizeptr, FileFlags fileFlags)
{
	log_firstcall();

	return File_Load2(filename, sizeptr, true, 1, fileFlags);
}

/// CUSTOM: Extension of File_Load to allow terminating the end of the buffer with extra bytes.
///         extraSize IS NOT included in the value returned by sizeptr.
///         Includes additional flags specifying where and what checks are used to load.
void* Gods98::File_Load2(const char* filename, OPTIONAL OUT uint32* sizeptr, bool32 binary, uint32 extraSize, FileFlags fileFlags)
{
	log_firstcall();

	File* file;
	if (file = File_Open2(filename, binary ? "rb" : "r", fileFlags)) {
		File_Seek(file, 0, SeekOrigin::End);
		uint32 size = File_Tell(file);

		if (fileGlobs.loadCallback) fileGlobs.loadCallback(filename, size, fileGlobs.loadCallbackData);

		/// CUSTOM: Add extraSize to null-terminate buffer with.
		uint8* buffer;
		if (buffer = (uint8*)Mem_Alloc(size + extraSize)) {
			File_Seek(file, 0, SeekOrigin::Set);
			File_Read(buffer, sizeof(uint8), size, file);

			/// CUSTOM: Null-terminate extraSize of buffer.
			std::memset(buffer + size, 0, extraSize);

			if (sizeptr) *sizeptr = size;

			File_Close(file);
			return buffer;
		}
		File_Close(file);
	}

	return nullptr;
}


// <LegoRR.exe @00480430>
uint32 __cdecl Gods98::File_LoadBinaryHandle(const char* filename, OPTIONAL OUT uint32* sizeptr)
{
	return File_LoadBinaryHandle2(filename, sizeptr, FileFlags::FILE_FLAGS_DEFAULT);
}

/// CUSTOM: Support for FileFlags.
uint32 Gods98::File_LoadBinaryHandle2(const char* filename, OPTIONAL OUT uint32* sizeptr, FileFlags fileFlags)
{
	log_firstcall();

	return File_LoadHandle2(filename, sizeptr, true, 0, fileFlags);
}


/// CUSTOM: Extension of File_LoadBinaryHandle to allow ASCII and terminating the end of the buffer with extra bytes.
///         extraSize IS NOT included in the value returned by sizeptr.
///         Includes additional flags specifying where and what checks are used to load.
uint32 Gods98::File_LoadHandle2(const char* filename, OPTIONAL OUT uint32* sizeptr, bool32 binary, uint32 extraSize, FileFlags fileFlags)
{
	log_firstcall();

	File* file;
	if (file = File_Open2(filename, binary ? "rb" : "r", fileFlags)) {
		File_Seek(file, 0, SeekOrigin::End);
		uint32 size = File_Tell(file);

		if (fileGlobs.loadCallback) fileGlobs.loadCallback(filename, size, fileGlobs.loadCallbackData);

		/// CUSTOM: Add extraSize to null-terminate buffer with.
		Mem_HandleValue handle;
		if ((handle = Mem_AllocHandle(size + extraSize)) != -1) {
			uint8* buffer = (uint8*)Mem_AddressHandle(handle);
			File_Seek(file, 0, SeekOrigin::Set);
			File_Read(buffer, sizeof(uint8), size, file);

			/// CUSTOM: Null-terminate extraSize of buffer.
			std::memset(buffer + size, 0, extraSize);

			if (sizeptr) *sizeptr = size;

			File_Close(file);
			return handle;
		}
		File_Close(file);
	}

	return (uint32)-1;
}


// <LegoRR.exe @004804e0>
const char* __cdecl Gods98::File_VerifyFilename(const char* filename)
{
	return File_VerifyFilename2(filename, FileFlags::FILE_FLAGS_DEFAULT);
}

/// CUSTOM: Creates a full path filename, with additional flags specifying where the base directory is and whether to verify or not.
const char* Gods98::File_VerifyFilename2(const char* filename, FileFlags fileFlags)
{
	log_firstcall();

	//static char full[_MAX_PATH];
	//fileGlobs.s_VerifyFilename_full

	char part[_MAX_PATH];

	if (filename != nullptr) {
		const char* temp = filename;
		const char* fileDir = nullptr;
		bool noVerify = (fileFlags & FileFlags::FILE_FLAG_NOVERIFY);

		switch (fileFlags & FileFlags::FILE_FLAGS_PATHMASK) {
		case FileFlags::FILE_FLAG_DATADIR:
			fileDir = fileGlobs.dataDir;
			// Remove the leading slash if any.
			if (*temp == '\\') temp++;
			std::sprintf(part, "%s\\%s", fileDir, temp);
			break;
		case FileFlags::FILE_FLAG_EXEDIR:
			fileDir = _exeDir.c_str();
			// Remove the leading slash if any.
			if (*temp == '\\') temp++;
			std::sprintf(part, "%s\\%s", fileDir, temp);
			break;
		case FileFlags::FILE_FLAG_ABSOLUTEPATH:
		default:
			noVerify = true; // Can't verify absolute paths.
			std::strcpy(part, temp);
			break;
		}

		if (::_fullpath(fileGlobs.s_VerifyFilename_full, part, sizeof(fileGlobs.s_VerifyFilename_full)) != nullptr) {

			if (noVerify || std::strncmp(fileGlobs.s_VerifyFilename_full, fileDir, std::strlen(fileDir)) == 0) {
				return fileGlobs.s_VerifyFilename_full;
			}
		}
	}

	Error_Warn(true, Error_Format("Cannot verify file name \"%s\".", filename));
	Error_LogLoadError(true, Error_Format("%d\t%s", (sint32)Error_LoadError::UnableToVerifyName, filename));

	return nullptr;
}

// <LegoRR.exe @00480570>
void __cdecl Gods98::File_SetLoadCallback(FileLoadCallback Callback, void* data)
{
	log_firstcall();

	fileGlobs.loadCallback = Callback;
	fileGlobs.loadCallbackData = data;
}

// <LegoRR.exe @00480590>
void __cdecl Gods98::File_CheckRedundantFiles(const char* logName)
{
	log_firstcall();

	char fileName[1024] = { '\0' }; // dummy init

	/// FIX APPLY: Properly use C file API, instead of File_Open combined with f-functions.
	FILE* fileList;
	if (fileList = std::fopen(logName, "r")) {
		checkGlobs.numInList = 0;

		while (std::fscanf(fileList, "%s", fileName) != EOF) {
			Error_Fatal(checkGlobs.numInList == FILE_DEBUG_RLISTSIZE, "FILE_DEBUG_RLISTSIZE too small");
			std::strcpy(checkGlobs.loadedList[checkGlobs.numInList++], fileName);
			fileName[0] = '\0'; // dummy null terminate
		}

		File_CheckDirectory(fileGlobs.dataDir);

		std::fclose(fileList);
	}
}

// <LegoRR.exe @00480650>
void __cdecl Gods98::File_CheckDirectory(const char* dirName)
{
	log_firstcall();

	_finddata32_t c_file;
	intptr_t hFile;
	char name[1024];


	std::strcpy(name, dirName);
	std::strcat(name, "\\*.*");

	if ((hFile = _findfirst32(name, &c_file)) == -1L)
		return;
	else
	{
		do
		{
			if (std::strcmp(c_file.name, ".") == 0 || std::strcmp(c_file.name, "..") == 0)
				continue;

			std::strcpy(name, dirName);
			std::strcat(name, "\\");
			std::strcat(name, c_file.name);

			if (c_file.attrib & _A_SUBDIR)
				File_CheckDirectory(name);
			else
				File_CheckFile(name);

		} while (_findnext32(hFile, &c_file) == 0);

		_findclose(hFile);
	}
}

// <LegoRR.exe @00480830>
void __cdecl Gods98::File_CheckFile(const char* fileName)
{
	log_firstcall();

	for (uint32 loop = 0; loop < checkGlobs.numInList; loop++)
		if (!::_stricmp(fileName, checkGlobs.loadedList[loop]))
			return;

	Error_LogRedundantFile(true, fileName);
}



/// CUSTOM: Shorthand for normalizing path separators, and other optional changes.
bool Gods98::File_NormalizePath(IN OUT std::string& path, bool fullpath, bool stripTrailingSlash)
{
	if (!path.empty()) {
		// Normalize path separators.
		for (size_t i = 0; i < path.length(); i++) {
			if (path[i] == '/') path[i] = '\\';
		}

		if (stripTrailingSlash && path.back() == '\\') {
			path.pop_back();
			if (path.empty())
				return false;
		}

		if (fullpath) {
			char buffer[_MAX_PATH] = { '\0' };
			if (!::_fullpath(buffer, path.c_str(), sizeof(buffer)))
				return false;

			path = buffer;
		}
		return true;
	}
	return false;
}

/// CUSTOM: Shorthand for normalizing path separators, and other optional changes.
bool Gods98::File_NormalizePath(IN OUT char* path, bool fullpath, bool stripTrailingSlash)
{
	size_t length = std::strlen(path);
	if (length > 0) {
		// Normalize path separators.
		for (size_t i = 0; i < length; i++) {
			if (path[i] == '/') path[i] = '\\';
		}

		if (stripTrailingSlash && path[length - 1] == '\\') {
			path[length - 1] = '\0';
			length--;
			if (length == 0)
				return false;
		}

		if (fullpath) {
			char buffer[_MAX_PATH] = { '\0' };
			if (!::_fullpath(buffer, path, sizeof(buffer)))
				return false;

			std::strcpy(buffer, path);
		}
		return true;
	}
	return false;
}


/// CUSTOM:
std::string Gods98::File_GetWorkingDir()
{
	char cwd[_MAX_PATH] = { '\0' };
	::_getcwd(cwd, sizeof(cwd));
	if (cwd[std::strlen(cwd) - 1] == '\\') cwd[std::strlen(cwd) - 1] = '\0';
	return cwd;
}

/// CUSTOM:
std::string Gods98::File_GetDataDir()
{
	return _initDataDirectory; //fileGlobs.dataDir;
}

/// CUSTOM:
std::string Gods98::File_GetExeDir()
{
	return _exeDir;
}

/// CUSTOM: Sets the Data directory used for loose files lookup.
///         The argument will be converted to a full path.
///         This must be called before File_Initialise.
void Gods98::File_SetDataDir(const std::string& initDataDir)
{
	if (!initDataDir.empty()) {
		// Normalize separators, convert to fullpath, and strip trailing slash.
		std::string s = initDataDir;
		if (File_NormalizePath(s, true, true) && !s.empty()) {
			_initDataDirectory = s;
			return;
		}

		// Don't support root directory. Fallthrough to setting the default Data directory.
	}

	// Set the default Data directory, when none is explicitly specified.
	{
		_initDataDirectory = File_GetWorkingDir();
		_initDataDirectory.append("\\" FILE_DATADIRNAME);
	}
}

/// CUSTOM: Sets the directory location to use when searching for WAD files.
///         This must be called before File_Initialise.
void Gods98::File_SetWadDir(const std::string& initWadDir)
{
	if (!initWadDir.empty()) {
		// Normalize separators, convert to fullpath, and strip trailing slash.
		std::string s = initWadDir;
		if (File_NormalizePath(s, true, true) && !s.empty()) {
			_initWadDirectory = s;
			return;
		}

		// Don't support root directory. Fallthrough to setting the default Data directory.
	}

	// Set the default WAD directory as the current directory, when none is explicitly specified.
	{
		_initWadDirectory = File_GetWorkingDir();
	}
}

/// CUSTOM: Gets if files should be looked for in the Data directory before WAD files.
bool Gods98::File_IsDataPriority()
{
	return _dataFirst;
}

/// CUSTOM: Sets if files should be looked for in the Data directory before WAD files.
void Gods98::File_SetDataPriority(bool dataFirst)
{
	_dataFirst = dataFirst;
}

/// CUSTOM: Gets if the WAD files should be used for file lookup.
bool Gods98::File_IsWadsEnabled()
{
	return _useWads;
}

/// CUSTOM: Sets if the WAD files should be used for file lookup.
void Gods98::File_SetWadsEnabled(bool useWads)
{
	_useWads = useWads;
}

/// CUSTOM: Gets if the CD should be used for file lookup.
bool Gods98::File_IsCDEnabled()
{
	return _useCD;
}

/// CUSTOM: Sets if the CD should be used for file lookup.
void Gods98::File_SetCDEnabled(bool useCD)
{
	_useCD = useCD;
}

#pragma endregion
