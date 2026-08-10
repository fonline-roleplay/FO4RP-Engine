#include "StdAfx.h"
#include "Server.h"
#include "Exception.h"
#include "Version.h"
#include "Access.h"
#include "BufferManager.h"
#include <locale.h>
#ifndef FO_WINDOWS
# include <signal.h>
#else
# include "BugTrap/BugTrap.h"
#endif

// ImGui & SDL2 / OpenGL3
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

# ifdef FO_MSVC
#  pragma comment( lib, "opengl32.lib" )
#  pragma comment( lib, "glu32.lib" )
#  pragma comment( lib, "SDL2.lib" )
#  pragma comment( lib, "SDL2main.lib" )
# endif

enum GUIActions
{
    GUI_RELOAD_CLIENT_SCRIPTS = 0,
    GUI_SAVE_WORLD,
    GUI_SAVE_LOG,
    GUI_SAVE_INFO,
    GUI_SHOW_MEMORY,
    GUI_SHOW_PLAYERS,
    GUI_SHOW_LOCS_MAPS,
    GUI_SHOW_TIME_EVENTS,
    GUI_SHOW_ANYDATA,
    GUI_SHOW_ITEMS_COUNT,
    GUI_SHOW_PROFILER,
    GUI_START_STOP,
    GUI_SCRIPT_DEBUG,
    GUI_LOGGING,
    GUI_LOG_WITH_TIME,
    GUI_LOG_WITH_THREAD,
    GUI_AUTOUPDATE
};

void InitAdminManager( IniParser* cfg );

/************************************************************************/
/* GUI & Windows service version                                        */
/************************************************************************/

#ifndef SERVER_DAEMON
void GUIInit( IniParser& cfg );
void GUIDrawFrame();
void GUICallback(GUIActions action);
void UpdateInfo();
void UpdateLog();
void GameLoopThread( void* );

Thread     LoopThread;
MutexEvent GameInitEvent;
FOServer   Server;
string     UpdateLogName;

SDL_Window* GuiWindow = nullptr;
SDL_GLContext GuiGlContext = nullptr;

char labelGameTime[128] = "Time: n/a";
char labelClients[128]  = "Connections: n/a";
char labelIngame[128]   = "Players in game: n/a";
char labelNPC[128]      = "NPC in game: n/a";
char labelLocCount[128] = "Locations: n/a";
char labelItemsCount[128] = "Items: n/a";
char labelVarsCount[128]  = "Vars: n/a";
char labelAnyDataCount[128] = "Any data: n/a";
char labelTECount[128]    = "Time events: n/a";
char labelFPS[128]        = "Cycles per second: n/a";
char labelDelta[128]      = "Cycle time: n/a";
char labelUptime[128]     = "Uptime: n/a";
char labelSend[128]       = "KBytes send: n/a";
char labelRecv[128]       = "KBytes recv: n/a";
char labelCompress[128]   = "Compress ratio: n/a";

bool GuiCBtnAutoUpdate = false;
bool GuiCBtnLogging = true;
bool GuiCBtnLoggingTime = true;
bool GuiCBtnLoggingThread = true;
bool GuiCBtnScriptDebug = true;

ImGuiTextBuffer GuiLogBuffer;
ImGuiTextBuffer GuiInfoBuffer;

int  SplitProcent = 80;
int  GUISizeMod = 0;
bool ForceScrollLog = false;

// Windows service
# ifdef FO_WINDOWS
void ServiceMain( bool as_service );
# endif

// Main
int main( int argc, char** argv )
{
    RestoreMainDirectory();

    // Threading
    # ifdef FO_WINDOWS
    pthread_win32_process_attach_np();
    # endif
    Thread::SetCurrentName( "GUI" );

    // Disable SIGPIPE signal
    # ifndef FO_WINDOWS
    signal( SIGPIPE, SIG_IGN );
    # endif

    // Exceptions catcher
    SetupExceptionHandler( "Server", SERVER_VERSION );

    // Timer
    Timer::Init();

    // Config
    IniParser& cfg = IniParser::GetServerConfig();

    // Memory debugging
    MemoryDebugLevel = cfg.GetInt( "MemoryDebugLevel", 0 );
    /*if( MemoryDebugLevel >= 3 )
        Debugger::StartTraceMemory();*/

    // Make command line
    SetCommandLine( argc, argv );

    // Logging
    LogWithTime( cfg.GetInt( "LoggingTime", 1 ) == 0 ? false : true );
    LogWithThread( cfg.GetInt( "LoggingThread", 1 ) == 0 ? false : true );
    if( strstr( CommandLine, "-logdebugoutput" ) || strstr( CommandLine, "-LoggingDebugOutput" ) || cfg.GetInt( "LoggingDebugOutput", 0 ) != 0 )
        LogToDebugOutput( true );

    // Init event
    GameInitEvent.Disallow();

    // Service
    if( strstr( CommandLine, "-service" ) )
    {
        # ifdef FO_WINDOWS
        ServiceMain( strstr( CommandLine, "--service" ) != NULL );
        # endif
        return 0;
    }

	// Logging
    char log_path[MAX_FOPATH] = { 0 };
    if (!strstr(CommandLine, "-nologpath") && strstr(CommandLine, "-logpath "))
    {
        const char* ptr = strstr(CommandLine, "-logpath ") + Str::Length("-logpath ");
        Str::Copy(log_path, ptr);
    }
    Str::EraseFrontBackSpecificChars(log_path);
    Str::Append(log_path, "Server.log");
    LogToFile(log_path);

    // Check single player parameters
    if( strstr( CommandLine, "-singleplayer " ) )
    {
        # ifdef FO_WINDOWS
        Singleplayer = true;
        Timer::SetGamePause( true );

        WriteLog( "Singleplayer mode.\n" );

        // Shared data
        const char* ptr = strstr( CommandLine, "-singleplayer " ) + Str::Length( "-singleplayer " );
        HANDLE      map_file = NULL;
        if( sscanf( ptr, "%p%p", &map_file, &SingleplayerClientProcess ) != 2 || !SingleplayerData.Attach( map_file ) )
        {
            WriteLog( "Can't attach to mapped file<%p>.\n", map_file );
            return 0;
        }
        # else
        return 0;
        # endif
    }

    // GUI
    if( !Singleplayer || strstr( CommandLine, "-showgui" ) )
    {
        GUIInit( cfg );
        LogToBuffer( true );
    }

    WriteLog( "FOnline server, version %04X-%02X.\n", SERVER_VERSION, FO_PROTOCOL_VERSION & 0xFF );

    FOQuit = true;
    Script::SetLogDebugInfo( true );

    if( GuiWindow )
    {
        GuiCBtnAutoUpdate = false;
        GuiCBtnLogging = (cfg.GetInt( "Logging", 1 ) != 0);
        GuiCBtnLoggingTime = (cfg.GetInt( "LoggingTime", 1 ) != 0);
        GuiCBtnLoggingThread = (cfg.GetInt( "LoggingThread", 1 ) != 0);
        GuiCBtnScriptDebug = true;
    }

    // Command line
    if( Str::Length( CommandLine ) > 0 )
        WriteLog( "Command line<%s>.\n", CommandLine );

    // Autostart
    if( strstr( CommandLine, "-start" ) || Singleplayer )
    {
        GUICallback(GUI_START_STOP);
    }

    // Start admin manager
    InitAdminManager( &cfg );

    // Loop
    if( GuiWindow )
    {
        bool done = false;
        ImGuiIO& io = ImGui::GetIO();

        while( !done )
        {
            SDL_Event event;
            while( SDL_PollEvent(&event) )
            {
                ImGui_ImplSDL2_ProcessEvent(&event);
                if( event.type == SDL_QUIT )
                    done = true;
                if( event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(GuiWindow) )
                    done = true;
            }

            UpdateLog();
            UpdateInfo();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            GUIDrawFrame();

            ImGui::Render();
            glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            SDL_GL_SwapWindow(GuiWindow);

            Thread::Sleep(10);
        }

        FOQuit = true;
        LoopThread.Wait();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(GuiGlContext);
        SDL_DestroyWindow(GuiWindow);
        SDL_Quit();
    }
    else
    {
        while( !FOQuit )
            Thread::Sleep( 100 );
    }

    // Finish
    Timer::Finish();
    return 0;
}

void GUIInit( IniParser& cfg )
{
    GUISizeMod = cfg.GetInt( "GUISize", 0 );

    #ifdef _WIN32
    SetProcessDPIAware();
    #endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        WriteLog("Error: %s\n", SDL_GetError());
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    #ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
    #endif

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    float main_scale = 1.0f;
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    
    GuiWindow = SDL_CreateWindow( GetWindowName(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (int)(890 * main_scale), (int)(720 * main_scale), window_flags);
    if (GuiWindow == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return;
    }

    GuiGlContext = SDL_GL_CreateContext(GuiWindow);
    if (GuiGlContext == nullptr)
    {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        return;
    }
    SDL_GL_MakeCurrent(GuiWindow, GuiGlContext);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(GuiWindow, GuiGlContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    UpdateInfo();
}

void GUIDrawFrame()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->WorkSize);
    ImGui::Begin("MasterPanel", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    if (ImGui::BeginTable("MainSplit", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV))
    {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoSort, 0.25f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoSort, 0.75f);

        ImGui::TableNextColumn();
        ImGui::BeginChild("LeftPanel");
        ImGui::TextUnformatted(labelGameTime);
        ImGui::TextUnformatted(labelClients);
        ImGui::TextUnformatted(labelIngame);
        ImGui::TextUnformatted(labelNPC);
        ImGui::TextUnformatted(labelLocCount);
        ImGui::TextUnformatted(labelItemsCount);
        ImGui::TextUnformatted(labelVarsCount);
        ImGui::TextUnformatted(labelAnyDataCount);
        ImGui::TextUnformatted(labelTECount);
        ImGui::TextUnformatted(labelFPS);
        ImGui::TextUnformatted(labelDelta);
        ImGui::TextUnformatted(labelUptime);
        ImGui::TextUnformatted(labelSend);
        ImGui::TextUnformatted(labelRecv);
        ImGui::TextUnformatted(labelCompress);

        ImGui::Separator();

        bool serverRunning = Server.Started();

        ImGui::BeginDisabled(!serverRunning);
        if (ImGui::Button("Reload client scripts", ImVec2(-1, 0))) { GUICallback(GUI_RELOAD_CLIENT_SCRIPTS); }
        if (ImGui::Button("Save world", ImVec2(-1, 0))) { GUICallback(GUI_SAVE_WORLD); }
        if (ImGui::Button("Save info", ImVec2(-1, 0))) { GUICallback(GUI_SAVE_INFO); }
        if (ImGui::Button("Players", ImVec2(-1, 0))) { GUICallback(GUI_SHOW_PLAYERS); }
        if (ImGui::Button("Locations and maps", ImVec2(-1, 0))) { GUICallback(GUI_SHOW_LOCS_MAPS); }
        if (ImGui::Button("Time events", ImVec2(-1, 0))) { GUICallback(GUI_SHOW_TIME_EVENTS); }
        if (ImGui::Button("Any data", ImVec2(-1, 0))) { GUICallback(GUI_SHOW_ANYDATA); }
        if (ImGui::Button("Items count", ImVec2(-1, 0))) { GUICallback(GUI_SHOW_ITEMS_COUNT); }
        if (ImGui::Button("Profiler", ImVec2(-1, 0))) { GUICallback(GUI_SHOW_PROFILER); }
        ImGui::EndDisabled();

        ImGui::Separator();
        if (ImGui::Button("Save log", ImVec2(-1, 0))) { GUICallback(GUI_SAVE_LOG); }
        if (ImGui::Button("Memory usage", ImVec2(-1, 0))) { GUICallback(GUI_SHOW_MEMORY); }

        ImGui::Separator();

        if (ImGui::Checkbox("Update info every second", &GuiCBtnAutoUpdate)) { GUICallback(GUI_AUTOUPDATE); }
        if (ImGui::Checkbox("Logging", &GuiCBtnLogging)) { GUICallback(GUI_LOGGING); }
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 30.0f);
        if (ImGui::ArrowButton("##Up", ImGuiDir_Up)) {
            if (SplitProcent >= 20) SplitProcent -= 15;
            ForceScrollLog = true;
        }
        if (ImGui::Checkbox("Logging with time", &GuiCBtnLoggingTime)) { GUICallback(GUI_LOG_WITH_TIME); }
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 30.0f);
        if (ImGui::ArrowButton("##Down", ImGuiDir_Down)) {
            if (SplitProcent <= 80) SplitProcent += 15;
            ForceScrollLog = true;
        }

        if (ImGui::Checkbox("Logging with thread", &GuiCBtnLoggingThread)) { GUICallback(GUI_LOG_WITH_THREAD); }
        if (ImGui::Checkbox("Script debug info", &GuiCBtnScriptDebug)) { GUICallback(GUI_SCRIPT_DEBUG); }

        const char* startStopLabel = !FOQuit ? "Stop server" : "Start server";
        if (ImGui::Button(startStopLabel, ImVec2(-1, 25))) {
            GUICallback(GUI_START_STOP);
        }
        ImGui::EndChild();

        ImGui::TableNextColumn();
        ImGui::BeginChild("RightPanel");

        float availableHeight = ImGui::GetContentRegionAvail().y;
        float logWindowHeight = availableHeight * (SplitProcent / 100.0f);


        ImGui::Text("Server Log Output:");
        ImGui::BeginChild("LogRegion", ImVec2(-1, logWindowHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::TextUnformatted(GuiLogBuffer.begin(), GuiLogBuffer.end());
        if (ForceScrollLog || ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();

        ImGui::Text("Diagnostic & Stats Info:");
        ImGui::BeginChild("InfoRegion", ImVec2(-1, -1), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::TextUnformatted(GuiInfoBuffer.begin(), GuiInfoBuffer.end());
        ImGui::EndChild();
        ImGui::EndChild();

        ImGui::EndTable();
    }

    ForceScrollLog = false;
    ImGui::End();
}

void GUICallback(GUIActions action)
{
    if( action == GUI_RELOAD_CLIENT_SCRIPTS )
    {
        if( Server.Started() )
            Server.RequestReloadClientScripts = true;
    }
    else if( action == GUI_SAVE_WORLD )
    {
        if( Server.Started() )
            Server.SaveWorldNextTick = Timer::FastTick();
    }
    else if( action == GUI_SAVE_LOG || action == GUI_SAVE_INFO )
    {
        DateTime dt;
        Timer::GetCurrentDateTime( dt );
        char     log_name[ MAX_FOTEXT ];
        char     log_name_dir[ MAX_FOTEXT ];
        
        bool isLog = (action == GUI_SAVE_LOG);
        const char* currentText = isLog ? GuiLogBuffer.c_str() : GuiInfoBuffer.c_str();

        FileManager::GetFullPath( NULL, PT_SERVER_LOGS, log_name_dir );
        Str::Format( log_name, "%sServer_%s_%04u.%02u.%02u_%02u-%02u-%02u.log", 
                     log_name_dir, isLog ? "Log" : UpdateLogName.c_str(), 
                     dt.Year, dt.Month, dt.Day, dt.Hour, dt.Minute, dt.Second );

        FILE* f = fopen(log_name, "w");
        if (f) {
            fputs(currentText, f);
            fclose(f);
        }
    }
    else if( action == GUI_SHOW_MEMORY )
    {
        FOServer::UpdateIndex = 0;
        FOServer::UpdateLastIndex = 0;
        if( !Server.Started() )
            UpdateInfo();
    }
    else if( action == GUI_SHOW_PLAYERS )
    {
        FOServer::UpdateIndex = 1;
        FOServer::UpdateLastIndex = 1;
    }
    else if( action == GUI_SHOW_LOCS_MAPS )
    {
        FOServer::UpdateIndex = 2;
        FOServer::UpdateLastIndex = 2;
    }
    else if( action == GUI_SHOW_TIME_EVENTS )
    {
        FOServer::UpdateIndex = 3;
        FOServer::UpdateLastIndex = 3;
    }
    else if( action == GUI_SHOW_ANYDATA )
    {
        FOServer::UpdateIndex = 4;
        FOServer::UpdateLastIndex = 4;
    }
    else if( action == GUI_SHOW_ITEMS_COUNT )
    {
        FOServer::UpdateIndex = 5;
        FOServer::UpdateLastIndex = 5;
    }
    else if( action == GUI_SHOW_PROFILER )
    {
        FOServer::UpdateIndex = 6;
        FOServer::UpdateLastIndex = 6;
    }
    else if( action == GUI_START_STOP )
    {
        if( !FOQuit )
        {
            FOQuit = true;
        }
        else
        {
            FOQuit = false;
            LoopThread.Start( GameLoopThread, "Main" );
        }
    }
    else if( action == GUI_SCRIPT_DEBUG )
    {
        Script::SetLogDebugInfo( GuiCBtnScriptDebug );
    }
    else if( action == GUI_LOGGING )
    {
        LogToBuffer( GuiCBtnLogging );
    }
    else if( action == GUI_LOG_WITH_TIME )
    {
        LogWithTime( GuiCBtnLoggingTime );
    }
    else if( action == GUI_LOG_WITH_THREAD )
    {
        LogWithThread( GuiCBtnLoggingThread );
    }
    else if( action == GUI_AUTOUPDATE )
    {
        if( GuiCBtnAutoUpdate )
            FOServer::UpdateLastTick = Timer::FastTick();
        else
            FOServer::UpdateLastTick = 0;
    }
}

void UpdateInfo()
{
    static char str[ MAX_FOTEXT ];
    static string std_str;

    if( Server.Started() )
    {
        DateTime st = Timer::GetGameTime( GameOpt.FullSecond );
        Str::Format( labelGameTime, "Time: %02u.%02u.%04u %02u:%02u:%02u x%u", st.Day, st.Month, st.Year, st.Hour, st.Minute, st.Second, GameOpt.TimeMultiplier );
        Str::Format( labelClients, "Connections: %u", Server.Statistics.CurOnline );
        Str::Format( labelIngame, "Players in game: %u", Server.PlayersInGame() );
        Str::Format( labelNPC, "NPC in game: %u", Server.NpcInGame() );
        Str::Format( labelLocCount, "Locations: %u (%u)", MapMngr.GetLocationsCount(), MapMngr.GetMapsCount() );
        Str::Format( labelItemsCount, "Items: %u", ItemMngr.GetItemsCount() );
        Str::Format( labelVarsCount, "Vars: %u", VarMngr.GetVarsCount() );
        Str::Format( labelAnyDataCount, "Any data: %u", Server.AnyData.size() );
        Str::Format( labelTECount, "Time events: %u", Server.GetTimeEventsCount() );
        Str::Format( labelFPS, "Cycles per second: %u", Server.Statistics.FPS );
        Str::Format( labelDelta, "Cycle time: %d", Server.Statistics.CycleTime );
    }
    else
    {
        Str::Copy( labelGameTime, "Time: n/a" );
        Str::Copy( labelClients, "Connections: n/a" );
        Str::Copy( labelIngame, "Players in game: n/a" );
        Str::Copy( labelNPC, "NPC in game: n/a" );
        Str::Copy( labelLocCount, "Locations: n/a" );
        Str::Copy( labelItemsCount, "Items: n/a" );
        Str::Copy( labelVarsCount, "Vars: n/a" );
        Str::Copy( labelAnyDataCount, "Any data: n/a" );
        Str::Copy( labelTECount, "Time events: n/a" );
        Str::Copy( labelFPS, "Cycles per second: n/a" );
        Str::Copy( labelDelta, "Cycle time: n/a" );
    }

    uint seconds = Server.Statistics.Uptime;
    Str::Format( labelUptime, "Uptime: %2u:%2u:%2u", seconds / 60 / 60, seconds / 60 % 60, seconds % 60 );
    Str::Format( labelSend, "KBytes Send: %u", Server.Statistics.BytesSend / 1024 );
    Str::Format( labelRecv, "KBytes Recv: %u", Server.Statistics.BytesRecv / 1024 );
    Str::Format( labelCompress, "Compress ratio: %g", (double) Server.Statistics.DataReal / ( Server.Statistics.DataCompressed ? Server.Statistics.DataCompressed : 1 ) );

    if( FOServer::UpdateIndex == -1 && FOServer::UpdateLastTick && FOServer::UpdateLastTick + 1000 < Timer::FastTick() )
    {
        FOServer::UpdateIndex = FOServer::UpdateLastIndex;
        FOServer::UpdateLastTick = Timer::FastTick();
    }

    if( FOServer::UpdateIndex != -1 )
    {
        switch( FOServer::UpdateIndex )
        {
        case 0:
            std_str = Debugger::GetMemoryStatistics();
            UpdateLogName = "Memory";
            break;
        case 1:
            if( Server.Started() ) std_str = Server.GetIngamePlayersStatistics();
            UpdateLogName = "Players";
            break;
        case 2:
            if( Server.Started() ) std_str = MapMngr.GetLocationsMapsStatistics();
            UpdateLogName = "LocationsAndMaps";
            break;
        case 3:
            if( Server.Started() ) std_str = Server.GetTimeEventsStatistics();
            UpdateLogName = "TimeEvents";
            break;
        case 4:
            if( Server.Started() ) std_str = Server.GetAnyDataStatistics();
            UpdateLogName = "AnyData";
            break;
        case 5:
            if( Server.Started() ) std_str = ItemMngr.GetItemsStatistics();
            UpdateLogName = "ItemsCount";
            break;
        case 6:
            std_str = Script::Profiler::GetStatistics();
            UpdateLogName = "Profiler";
            break;
        default:
            UpdateLogName = "";
            break;
        }
        
        GuiInfoBuffer.clear();
        GuiInfoBuffer.append(std_str.c_str());
        FOServer::UpdateIndex = -1;
    }
}

void UpdateLog()
{
    string str;
    LogGetBuffer( str );
    if( str.length() )
    {
        GuiLogBuffer.append( str.c_str() );
        ForceScrollLog = true;
    }
}

void GameLoopThread( void* )
{
    # ifdef FO_WINDOWS
    BT_SetTerminate();
    # endif
    GetServerOptions();

    if( Server.Init() )
    {
        if( GuiWindow )
        {
            if( !GuiCBtnLogging )
                LogToTextBox( NULL );
        }

        GameInitEvent.Allow();
        Server.MainLoop();
        Server.Finish();
        UpdateInfo();
    }
    else
    {
        WriteLog( "Initialization fail!\n" );
        GameInitEvent.Allow();
    }

    if( GuiWindow )
        UpdateLog();
    LogFinish();
    if( Singleplayer )
        ExitProcess( 0 );
}

#endif // !SERVER_DAEMON

/************************************************************************/
/* Windows service                                                      */
/************************************************************************/
#ifdef FO_WINDOWS

SERVICE_STATUS_HANDLE FOServiceStatusHandle;
VOID WINAPI FOServiceStart( DWORD argc, LPTSTR* argv );
VOID WINAPI FOServiceCtrlHandler( DWORD opcode );
void        SetFOServiceStatus( uint state );

void ServiceMain( bool as_service )
{
    // Binary started as service
    if( as_service )
    {
        // Start
        SERVICE_TABLE_ENTRY dispatch_table[] = { { Str::Duplicate( "FOnlineServer" ), FOServiceStart }, { NULL, NULL } };
        StartServiceCtrlDispatcher( dispatch_table );
        return;
    }

    // Open service manager
    SC_HANDLE manager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
    if( !manager )
    {
        MessageBox( NULL, "Can't open service manager.", "Server", MB_OK | MB_ICONHAND );
        return;
    }

    // Delete service
    if( strstr( CommandLine, "-delete" ) )
    {
        SC_HANDLE service = OpenService( manager, "FOnlineServer", DELETE );

        if( service && DeleteService( service ) )
            MessageBox( NULL, "Service deleted.", "Server", MB_OK | MB_ICONASTERISK );
        else
            MessageBox( NULL, "Can't delete service.", "Server", MB_OK | MB_ICONHAND );

        CloseServiceHandle( service );
        CloseServiceHandle( manager );
        return;
    }

    // Manage service
    SC_HANDLE service = OpenService( manager, "FOnlineServer", SERVICE_QUERY_CONFIG | SERVICE_CHANGE_CONFIG | SERVICE_QUERY_STATUS | SERVICE_START );

    // Compile service path
    char path1[ MAX_FOPATH ];
    GetModuleFileName( GetModuleHandle( NULL ), path1, MAX_FOPATH );
    char path2[ MAX_FOPATH ];
    Str::Format( path2, "\"%s\" --service", path1 );

    // Change executable path, if changed
    if( service )
    {
        LPQUERY_SERVICE_CONFIG service_cfg = (LPQUERY_SERVICE_CONFIG) calloc( 8192, 1 );
        DWORD                  dw;
        if( QueryServiceConfig( service, service_cfg, 8192, &dw ) && !Str::CompareCase( service_cfg->lpBinaryPathName, path2 ) )
            ChangeServiceConfig( service, SERVICE_NO_CHANGE, SERVICE_NO_CHANGE, SERVICE_NO_CHANGE, path2, NULL, NULL, NULL, NULL, NULL, NULL );
        free( service_cfg );
    }

    // Register service
    if( !service )
    {
        service = CreateService( manager, "FOnlineServer", "FOnlineServer", SERVICE_ALL_ACCESS,
                                 SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, path2, NULL, NULL, NULL, NULL, NULL );

        if( service )
            MessageBox( NULL, "\'FOnlineServer\' service registered.", "Server", MB_OK | MB_ICONASTERISK );
        else
            MessageBox( NULL, "Can't register \'FOnlineServer\' service.", "Server", MB_OK | MB_ICONHAND );
    }
    // Start service
    else
    {
        SERVICE_STATUS status;
        if( service && QueryServiceStatus( service, &status ) && status.dwCurrentState != SERVICE_STOPPED )
            MessageBox( NULL, "Service already running.", "Server", MB_OK | MB_ICONASTERISK );
        else if( service && !StartService( service, 0, NULL ) )
            MessageBox( NULL, "Can't start service.", "Server", MB_OK | MB_ICONHAND );
    }

    // Close handles
    if( service )
        CloseServiceHandle( service );
    if( manager )
        CloseServiceHandle( manager );
}

VOID WINAPI FOServiceStart( DWORD argc, LPTSTR* argv )
{
    Thread::SetCurrentName( "Service" );
    LogToFile( "Server.log" );
    WriteLog( "FOnline server service, version %04X-%02X.\n", SERVER_VERSION, FO_PROTOCOL_VERSION & 0xFF );

    FOServiceStatusHandle = RegisterServiceCtrlHandler( "FOnlineServer", FOServiceCtrlHandler );
    if( !FOServiceStatusHandle )
        return;

    // Start admin manager
    InitAdminManager( NULL );

    // Start game
    SetFOServiceStatus( SERVICE_START_PENDING );

    FOQuit = false;
    LoopThread.Start( GameLoopThread, "Main" );
    GameInitEvent.Wait();

    if( Server.Started() )
        SetFOServiceStatus( SERVICE_RUNNING );
    else
        SetFOServiceStatus( SERVICE_STOPPED );
}

VOID WINAPI FOServiceCtrlHandler( DWORD opcode )
{
    switch( opcode )
    {
    case SERVICE_CONTROL_STOP:
        SetFOServiceStatus( SERVICE_STOP_PENDING );
        FOQuit = true;

        LoopThread.Wait();
        SetFOServiceStatus( SERVICE_STOPPED );
        return;
    case SERVICE_CONTROL_INTERROGATE:
        // Fall through to send current status
        break;
    default:
        break;
    }

    // Send current status
    SetFOServiceStatus( 0 );
}

void SetFOServiceStatus( uint state )
{
    static uint last_state = 0;
    static uint check_point = 0;

    if( !state )
        state = last_state;
    else
        last_state = state;

    SERVICE_STATUS srv_status;
    srv_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    srv_status.dwCurrentState = state;
    srv_status.dwWin32ExitCode = 0;
    srv_status.dwServiceSpecificExitCode = 0;
    srv_status.dwWaitHint = 0;
    srv_status.dwCheckPoint = 0;
    srv_status.dwControlsAccepted = 0;

    if( state == SERVICE_RUNNING )
        srv_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    if( !( state == SERVICE_RUNNING || state == SERVICE_STOPPED ) )
        srv_status.dwCheckPoint = ++check_point;

    SetServiceStatus( FOServiceStatusHandle, &srv_status );
}

#endif // FO_WINDOWS

/************************************************************************/
/* Linux daemon                                                         */
/************************************************************************/
#ifdef SERVER_DAEMON

# include <sys/stat.h>

void DaemonLoop();
void GameLoopThread( void* );
FOServer Server;
Thread   LoopThread;

int main( int argc, char** argv )
{
    // Start daemon
    pid_t parpid = fork();
    if( parpid < 0 )
    {
        WriteLog( "Create child process (fork) fail, error<%s>.", ERRORSTR );
        return 1;
    }
    else if( parpid != 0 )
    {
        // Close parent process
        return 0;
    }

    umask( 0 );

    if( setsid() < 0 )
    {
        WriteLog( "Generate process index (setsid) fail, error<%s>.\n", ERRORSTR );
        return 1;
    }

    close( STDIN_FILENO );
    close( STDOUT_FILENO );
    close( STDERR_FILENO );

    // Stuff
    setlocale( LC_ALL, "Russian" );
    RestoreMainDirectory();

    // Threading
    Thread::SetCurrentName( "Daemon" );

    // Disable SIGPIPE signal
    # ifndef FO_WINDOWS
    signal( SIGPIPE, SIG_IGN );
    # endif

    // Exceptions catcher
    SetupExceptionHandler( "Server", SERVER_VERSION );

    // Timer
    Timer::Init();

    // Config
	IniParser& cfg = IniParser::GetServerConfig();

    // Memory debugging
    MemoryDebugLevel = cfg.GetInt( "MemoryDebugLevel", 0 );
    /*if( MemoryDebugLevel >= 3 )
        Debugger::StartTraceMemory();*/

    // Make command line
    SetCommandLine( argc, argv );

    // Logging
    LogWithTime( cfg.GetInt( "LoggingTime", 1 ) == 0 ? false : true );
    LogWithThread( cfg.GetInt( "LoggingThread", 1 ) == 0 ? false : true );
    if( strstr( CommandLine, "-logdebugoutput" ) || strstr( CommandLine, "-LoggingDebugOutput" ) || cfg.GetInt( "LoggingDebugOutput", 0 ) != 0 )
        LogToDebugOutput( true );
    LogToFile( "./ServerDaemon.log" );

    // Log version
    WriteLog( "FOnline server daemon, version %04X-%02X.\n", SERVER_VERSION, FO_PROTOCOL_VERSION & 0xFF );
    if( CommandLineArgCount > 1 )
        WriteLog( "Command line<%s>.\n", CommandLine );

    DaemonLoop(); // Never out from here
    return 0;
}

void DaemonLoop()
{
    // Autostart server
    LoopThread.Start( GameLoopThread, "Main" );

    // Start admin manager
    InitAdminManager( NULL );

    // Daemon loop
    while( true )
        Thread::Sleep( 1000 );
}

void GameLoopThread( void* )
{
    GetServerOptions();

    if( Server.Init() )
    {
        FOQuit = false;
        Server.MainLoop();
        Server.Finish();
    }
    else
    {
        WriteLog( "Initialization fail!\n" );
    }
}

#endif // SERVER_DAEMON


/************************************************************************/
/* Admin panel                                                          */
/************************************************************************/

#define MAX_SESSIONS    ( 10 )

struct Session
{
    int         RefCount;
    SOCKET      Sock;
    sockaddr_in From;
    Thread      WorkThread;
    DateTime    StartWork;
    bool        Authorized;
};
typedef vector< Session* > SessionVec;

void AdminWork( void* );
void AdminManager( void* );
Thread AdminManagerThread;

void InitAdminManager( IniParser* cfg )
{
    uint port = 0;
    if( !cfg )
    {
        IniParser& cfg_ = IniParser::GetServerConfig();
        if( !cfg_.IsLoaded() )
        {
            WriteLogF( _FUNC_, "Can't access to config file.\n" );
            return;
        }
        port = cfg_.GetInt( "AdminPanelPort", 0 );
    }
    else
    {
        port = cfg->GetInt( "AdminPanelPort", 0 );
    }

    if( port )
    {
        AdminManagerThread.Finish();
        AdminManagerThread.Start( AdminManager, "AdminPanelManager", (void*) port );
    }
}

void AdminManager( void* port_ )
{
    # ifdef FO_WINDOWS
    BT_SetTerminate();
    # endif
    // Listen socket
    #ifdef FO_WINDOWS
    WSADATA wsa;
    if( WSAStartup( MAKEWORD( 2, 2 ), &wsa ) )
    {
        WriteLog( "WSAStartup fail on creation listen socket for admin manager.\n" );
        return;
    }
    #endif
    SOCKET listen_sock = socket( AF_INET, SOCK_STREAM, 0 );
    if( listen_sock == INVALID_SOCKET )
    {
        WriteLog( "Can't create listen socket for admin manager.\n" );
        return;
    }
    const int   opt = 1;
    setsockopt( listen_sock, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof( opt ) );
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons( (ushort) (size_t) port_ );
    sin.sin_addr.s_addr = INADDR_ANY;
    if( bind( listen_sock, (sockaddr*) &sin, sizeof( sin ) ) == SOCKET_ERROR )
    {
        WriteLog( "Can't bind listen socket for admin manager.\n" );
        closesocket( listen_sock );
        return;
    }
    if( listen( listen_sock, SOMAXCONN ) == SOCKET_ERROR )
    {
        WriteLog( "Can't listen listen socket for admin manager.\n" );
        closesocket( listen_sock );
        return;
    }

    // Accept clients
    SessionVec sessions;
    while( true )
    {
        // Wait connection
        timeval tv = { 1, 0 };
        fd_set  sock_set;
        FD_ZERO( &sock_set );
        FD_SET( listen_sock, &sock_set );
        if( select( listen_sock + 1, &sock_set, NULL, NULL, &tv ) > 0 )
        {
            sockaddr_in from;
            socklen_t   len = sizeof( from );
            SOCKET      sock = accept( listen_sock, (sockaddr*) &from, &len );
            if( sock != INVALID_SOCKET )
            {
                // Found already connected from this IP
                bool refuse = false;
                for( auto it = sessions.begin(); it != sessions.end(); ++it )
                {
                    Session* s = *it;
                    if( s->From.sin_addr.s_addr == from.sin_addr.s_addr )
                    {
                        refuse = true;
                        break;
                    }
                }
                if( refuse || sessions.size() > MAX_SESSIONS )
                {
                    shutdown( sock, SD_BOTH );
                    closesocket( sock );
                }

                // Add new session
                if( !refuse )
                {
                    Session* s = new Session();
                    s->RefCount = 2;
                    s->Sock = sock;
                    s->From = from;
                    Timer::GetCurrentDateTime( s->StartWork );
                    s->Authorized = false;
                    s->WorkThread.Start( AdminWork, "AdminPanel", (void*) s );
                    sessions.push_back( s );
                }
            }
        }

        // Manage sessions
        if( !sessions.empty() )
        {
            DateTime cur_dt;
            Timer::GetCurrentDateTime( cur_dt );
            for( auto it = sessions.begin(); it != sessions.end();)
            {
                Session* s = *it;
                bool     erase = false;

                // Erase closed sessions
                if( s->Sock == INVALID_SOCKET )
                    erase = true;

                // Drop long not authorized connections
                if( !s->Authorized && Timer::GetTimeDifference( cur_dt, s->StartWork ) > 60 ) // 1 minute
                    erase = true;

                // Erase
                if( erase )
                {
                    if( s->Sock != INVALID_SOCKET )
                        shutdown( s->Sock, SD_BOTH );
                    if( --s->RefCount == 0 )
                        delete s;
                    it = sessions.erase( it );
                }
                else
                {
                    ++it;
                }
            }
        }

        // Sleep to prevent panel DDOS or keys brute force
        Thread::Sleep( 1000 );
    }
}

#define ADMIN_PREFIX    "Admin panel (%s): "
#define ADMIN_LOG( format, ... )                                                  \
    do {                                                                          \
        WriteLog( ADMIN_PREFIX format, admin_name, ## __VA_ARGS__ );              \
        char buf[ MAX_FOTEXT ];                                                   \
        Str::Format( buf, format, ## __VA_ARGS__ );                               \
        uint buf_len = Str::Length( buf ) + 1;                                    \
        if( send( s->Sock, buf, buf_len, 0 ) != (int) buf_len )                   \
        {                                                                         \
            WriteLog( ADMIN_PREFIX "Send data fail, disconnect.\n", admin_name ); \
            goto label_Finish;                                                    \
        }                                                                         \
    } while( 0 )

void AdminWork( void* session_ )
{
    # ifdef FO_WINDOWS
    BT_SetTerminate();
    # endif
    // Data
    Session* s = (Session*) session_;
    char     admin_name[ MAX_FOTEXT ] = { "Not authorized" };

    // Welcome string
    char welcome[] = { "Welcome to FOnline admin panel.\nEnter access key: " };
    uint welcome_len = Str::Length( welcome ) + 1;
    if( send( s->Sock, welcome, welcome_len, 0 ) != (int) welcome_len )
    {
        WriteLog( "Admin connection first send fail, disconnect.\n" );
        goto label_Finish;
    }

    // Commands loop
    while( true )
    {
        // Get command
        char cmd[ MAX_FOTEXT ];
        memzero( cmd, sizeof( cmd ) );
        int  len = recv( s->Sock, cmd, sizeof( cmd ), 0 );
        if( len <= 0 || len == MAX_FOTEXT )
        {
            if( !len )
                WriteLog( ADMIN_PREFIX "Socket closed, disconnect.\n", admin_name );
            else
                WriteLog( ADMIN_PREFIX "Socket error, disconnect.\n", admin_name );
            goto label_Finish;
        }
        if( len > 200 )
            len = 200;
        cmd[ len ] = 0;
        Str::EraseFrontBackSpecificChars( cmd );

        // Authorization
        if( !s->Authorized )
        {
            StrVec client, tester, moder, admin, admin_names;
            FOServer::GetAccesses( client, tester, moder, admin, admin_names );
            int    pos = -1;
            for( size_t i = 0, j = admin.size(); i < j; i++ )
            {
                if( Str::Compare( admin[ i ].c_str(), cmd ) )
                {
                    pos = (int) i;
                    break;
                }
            }
            if( pos != -1 )
            {
                if( pos < (int) admin_names.size() )
                    Str::Copy( admin_name, admin_names[ pos ].c_str() );
                else
                    Str::Format( admin_name, "%d", pos );

                s->Authorized = true;
                ADMIN_LOG( "Authorized for admin '%s', IP '%s'.\n", admin_name, inet_ntoa( s->From.sin_addr ) );
                continue;
            }
            else
            {
                WriteLog( "Wrong access key entered in admin panel from IP '%s', disconnect.\n", inet_ntoa( s->From.sin_addr ) );
                char failstr[] = { "Wrong access key!\n" };
                send( s->Sock, failstr, Str::Length( failstr ) + 1, 0 );
                goto label_Finish;
            }
        }

        // Process commands
        if( Str::CompareCase( cmd, "exit" ) )
        {
            ADMIN_LOG( "Disconnect from admin panel.\n" );
            goto label_Finish;
        }
        else if( Str::CompareCase( cmd, "kill" ) )
        {
            ADMIN_LOG( "Kill whole process.\n" );
            ExitProcess( 0 );
        }
        else if( Str::CompareCaseCount( cmd, "log ", 4 ) )
        {
            if( !Str::CompareCase( &cmd[ 4 ], "disable" ) )
            {
                LogToFile( &cmd[ 4 ] );
                ADMIN_LOG( "Logging to file '%s'.\n", &cmd[ 4 ] );
            }
            else
            {
                LogToFile( NULL );
                ADMIN_LOG( "Logging disabled.\n" );
            }
        }
        else if( Str::CompareCase( cmd, "start" ) )
        {
            if( Server.Starting() )
                ADMIN_LOG( "Server already starting.\n" );
            else if( Server.Started() )
                ADMIN_LOG( "Server already started.\n" );
            else if( Server.Stopping() )
                ADMIN_LOG( "Server stopping, wait.\n" );
            else if( Server.Stopped() )
            {
                if( !Server.ActiveOnce )
                {
                    ADMIN_LOG( "Starting server.\n" );
                    #ifndef SERVER_DAEMON
                    if( GuiWindow )
                    {
                        GUICallback(GUI_START_STOP);
                    }
                    else
                    #endif
                    {
                        LoopThread.Start( GameLoopThread, "Main" );
                    }
                }
                else
                {
                    ADMIN_LOG( "Can't start server more than one time. Restart server process.\n" );
                    #pragma MESSAGE( "Allow multiple server starting in one process session." )
                }
            }
        }
        else if( Str::CompareCase( cmd, "stop" ) )
        {
            if( Server.Starting() )
                ADMIN_LOG( "Server starting, wait.\n" );
            else if( Server.Stopped() )
                ADMIN_LOG( "Server already stopped.\n" );
            else if( Server.Stopping() )
                ADMIN_LOG( "Server already stopping.\n" );
            else if( Server.Started() )
            {
                ADMIN_LOG( "Stopping server.\n" );
                #ifndef SERVER_DAEMON
                if( GuiWindow )
                {
                    GUICallback(GUI_START_STOP);
                }
                else
                #endif
                {
                    FOQuit = true;
                }
            }
        }
        else if( Str::CompareCase( cmd, "state" ) )
        {
            if( Server.Starting() )
                ADMIN_LOG( "Server starting.\n" );
            else if( Server.Started() )
                ADMIN_LOG( "Server started.\n" );
            else if( Server.Stopping() )
                ADMIN_LOG( "Server stopping.\n" );
            else if( Server.Stopped() )
                ADMIN_LOG( "Server stopped.\n" );
            else
                ADMIN_LOG( "Unknown state.\n" );
        }
        else if( cmd[ 0 ] == '~' )
        {
            if( Server.Started() )
            {
                static THREAD char*  admin_name_ptr;
                static THREAD SOCKET sock;
                static THREAD bool   send_fail;
                struct LogCB
                {
                    static void Message( const char* str )
                    {
                        char buf[ MAX_FOTEXT ];
                        Str::Copy( buf, str );
                        uint buf_len = Str::Length( buf );
                        if( !buf_len || buf[ buf_len - 1 ] != '\n' )
                        {
                            buf[ buf_len ] = '\n';
                            buf[ buf_len + 1 ] = 0;
                            buf_len++;
                        }
                        buf_len++;

                        if( !send_fail && send( sock, buf, buf_len, 0 ) != (int) buf_len )
                        {
                            WriteLog( ADMIN_PREFIX "Send data fail, disconnect.\n", admin_name_ptr );
                            send_fail = true;
                        }
                    }
                };
                admin_name_ptr = admin_name;
                sock = s->Sock;
                send_fail = false;

                BufferManager buf;
                PackCommand( &cmd[ 1 ], buf, LogCB::Message, NULL );
                if( !buf.IsEmpty() )
                {
                    if( Script::InitThread() )
                    {
                        uint msg;
                        buf >> msg;
                        WriteLog( ADMIN_PREFIX "Execute command '%s'.\n", admin_name, cmd );
                        Server.Process_Command( buf, LogCB::Message, NULL, admin_name );
                        Script::FinishThread();
                    }
                    else
                    {
                        ADMIN_LOG( "Can't initialize script stuff for thread." );
                    }
                }

                if( send_fail )
                    goto label_Finish;
            }
            else
            {
                ADMIN_LOG( "Can't run command for not started server.\n" );
            }
        }
        else if( Str::Length( cmd ) > 0 )
        {
            ADMIN_LOG( "Unknown command '%s'.\n", cmd );
        }
    }

label_Finish:
    shutdown( s->Sock, SD_BOTH );
    closesocket( s->Sock );
    s->Sock = INVALID_SOCKET;
    if( --s->RefCount == 0 )
        delete s;
}
