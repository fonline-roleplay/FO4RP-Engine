#include "StdAfx.h"
#include "Exception.h"

static string AppName;
static string AppVer;
static string ManualDumpAppendix;

#ifdef FO_WINDOWS

# include <windows.h>
# include <stdio.h>
# include <DbgHelp.h>
# pragma comment(lib, "Dbghelp.lib")
# include "BugTrap/BugTrap.h"
# include "FileManager.h"

void CALLBACK PreErrorHandler(INT_PTR)
{
	char        dump_path[ MAX_FOPATH ];
	char        dump_path_dir[ MAX_FOPATH ];

	DateTime    dt;
	Timer::GetCurrentDateTime( dt );
	FileManager::GetFullPath( NULL, PT_SERVER_DUMPS, dump_path_dir );
	Str::Format( dump_path, "%s%s_%s_%s_%04d.%02d.%02d_%02d-%02d-%02d.txt",
				 dump_path_dir, "BugTrap", AppName.c_str(), AppVer.c_str( ), dt.Year, dt.Month, dt.Day, dt.Hour, dt.Minute, dt.Second );
	BT_AddLogFile(dump_path);
}

void SetupExceptionHandler( const string& app_name, int app_ver )
{
	BT_InstallSehFilter();
	if ( !Str::Substring(CommandLine, "-SilentErrorMode") )
		BT_SetActivityType(BTA_SHOWUI);
	else
		BT_SetActivityType(BTA_SAVEREPORT);

	BT_SetDialogMessage(
	BTDM_INTRO2,
	"\
This is FOnline Engine crash reporting client. \
To help the development process, \
please Submit Bug or save report and email it manually (button More...).\
\r\nMany thanks in advance and sorry for the inconvenience."
	);
	BT_SetPreErrHandler(PreErrorHandler,0);

	BT_SetAppName(app_name.c_str());
	BT_SetReportFormat(BTRF_TEXT);
	BT_SetSupportEMail("support@fonline.ru");
	BT_SetFlags(BTF_DETAILEDMODE | BTF_ATTACHREPORT);
	BT_SetDumpType(MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithFullMemory | MiniDumpIgnoreInaccessibleMemory);

	AppName = app_name;
	AppVer = Str::FormatBuf( "%i", app_ver );
}

#else

void SetupExceptionHandler( const string& app_name, int app_ver )
{
	// No Exception Handler on Non-Windows
}

#endif
