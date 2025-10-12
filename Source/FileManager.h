#ifndef __FILE_MANAGER__
#define __FILE_MANAGER__

#include "Defines.h"
#include "FileSystem.h"
#include "Log.h"
#include "DataFile.h"

// Client and mapper paths
enum PATH_SHARED_TYPES
{
	PT_ROOT = 0,
	PT_DATA,
	PT_ART,
	PT_ART_CRITTERS,
	PT_ART_INTRFACE,
	PT_ART_INVEN,
	PT_ART_ITEMS,
	PT_ART_MISC,
	PT_ART_SCENERY,
	PT_ART_SKILLDEX,
	PT_ART_SPLASH,
	PT_ART_TILES,
	PT_ART_WALLS,
	PT_TEXTURES,
	PT_EFFECTS,
	PT_SND_MUSIC = 16,
	PT_SND_SFX,
	PT_SCRIPTS,
	PT_VIDEO,
	PT_TEXTS,
	PT_SAVE,
	PT_FONTS,
	PT_CACHE
};

// Server paths
enum PATH_SERVER_TYPES
{
	PT_SERVER_ROOT = 30,
	PT_SERVER_DATA,
	PT_SERVER_TEXTS,
	PT_SERVER_DIALOGS,
	PT_SERVER_MAPS,
	PT_SERVER_PRO_ITEMS,
	PT_SERVER_PRO_CRITTERS,
	PT_SERVER_SCRIPTS,
	PT_SERVER_SAVE,
	PT_SERVER_CLIENTS,
	PT_SERVER_BANS,
	PT_SERVER_LOGS,
	PT_SERVER_DUMPS,
	PT_SERVER_PROFILER
};

// Other
enum PATH_MAPPER_TYPES
{
	PT_MAPPER_ROOT = 45,
	PT_MAPPER_DATA
};

#define PATH_LIST_COUNT           ( 50 )
extern const char* PathList[ PATH_LIST_COUNT ];

class FileManager
{
public:
    static void SetDataPath( const char* path );
    static void SetCacheName( const char* name );
    static void InitDataFiles( const char* path );
    static bool LoadDataFile( const char* path );
    static void EndOfWork();

    bool   LoadFile( const char* fname, int path_type, bool no_read_data = false );
    bool   LoadStream( const uchar* stream, uint length );
    void   UnloadFile();
    uchar* ReleaseBuffer();

    void SetCurPos( uint pos );
    void GoForward( uint offs );
    void GoBack( uint offs );
    bool FindFragment( const uchar* fragment, uint fragment_len, uint begin_offs );

    bool   GetLine( char* str, uint len );
    bool   CopyMem( void* ptr, uint size );
    void   GetStr( char* str );
    uchar  GetUChar();
    ushort GetBEUShort();
    ushort GetLEUShort();
    uint   GetBEUInt();
    uint   GetLEUInt();
    uint   GetLE3UChar();
    float  GetBEFloat();
    float  GetLEFloat();

    void   SwitchToRead();
    void   SwitchToWrite();
    void   ClearOutBuf();
    bool   ResizeOutBuf();
    void   SetPosOutBuf( uint pos );
    bool   SaveOutBufToFile( const char* fname, int path_type );
    uchar* GetOutBuf()    { return dataOutBuf; }
    uint   GetOutBufLen() { return endOutBuf; }

    void SetData( void* data, uint len );
    void SetStr( const char* fmt, ... );
    void SetUChar( uchar data );
    void SetBEUShort( ushort data );
    void SetLEUShort( ushort data );
    void SetBEUInt( uint data );
    void SetLEUInt( uint data );

    static const char* GetFullPath( const char* fname, int path_type );
    static void        GetFullPath( const char* fname, int path_type, char* get_path );
    static const char* GetPath( int path_type );
    static const char* GetDataPath( int path_type );
    static void        FormatPath( char* path, bool first_skipped = false );
    static void        ExtractPath( const char* fname, char* path );
    static void        ExtractFileName( const char* fname, char* name );
    static void        MakeFilePath( const char* name, const char* path, char* result );
    static void        CreateDirectoryTree( const char* path );
    static const char* GetExtension( const char* fname ); // EXT without dot
    static char*       EraseExtension( char* fname );     // Erase EXT with dot

    bool   IsLoaded()  { return fileBuf != NULL; }
    uchar* GetBuf()    { return fileBuf; }
    uchar* GetCurBuf() { return fileBuf + curPos; }
    uint   GetCurPos() { return curPos; }
    uint   GetFsize()  { return fileSize; }
    bool   IsEOF()     { return curPos >= fileSize; }
	uint64 GetWriteTime() { return timeWrite; }
    int    ParseLinesInt( const char* fname, int path_type, IntVec& lines );

    static DataFileVec& GetDataFiles() { return dataFiles; }
    static void         GetFolderFileNames( const char* path, bool include_subdirs, const char* ext, StrVec& result );
    static void         GetDatsFileNames( const char* path, bool include_subdirs, const char* ext, StrVec& result );

    FileManager(): dataOutBuf( NULL ), posOutBuf( 0 ), endOutBuf( 0 ), lenOutBuf( 0 ), fileSize( 0 ), curPos( 0 ), fileBuf( NULL ) {};
    ~FileManager()
    {
        UnloadFile();
        ClearOutBuf();
    }

private:
    static char        dataPath[ MAX_FOPATH ];
    static DataFileVec dataFiles;

    uint               fileSize;
    uchar*             fileBuf;
    uint               curPos;

    uchar*             dataOutBuf;
    uint               posOutBuf;
    uint               endOutBuf;
    uint               lenOutBuf;

    uint64             timeCreate, timeAccess, timeWrite;

    static void RecursiveDirLook( const char* init_dir, bool include_subdirs, const char* ext, StrVec& result );
};

#endif // __FILE_MANAGER__
