namespace FOConfig
{
    enum PROXYTYPE
    {
        NONE = 0,
        SOCKS4,
        SOCKS5,
        HTTP,
        MAX
    }
    enum COMBATMODE
    {
        BOTH = 0,
        REAL_TIME,
        TURN_BASED,
        MAX
    }
    enum INDICATORTYPE
    {
        LINES = 0,
        NUMS,
        LINES_NUMS,
        MAX
    }
    enum COMBATMSGTYPES
    {
        VERBOSE = 0,
        BRIEF,
        MAX
    }
    public class Settings
    {
        public string Language = "russ";
        public bool WinNotify = true;
        public bool SoundNotify = false;
        public bool InvertMessBox = false;
        public bool Logging = true;
        public bool LoggingTime = true;
        public int Sleep = 0;
        public int ScrollDelay = 4;
        public int ScrollStep = 32;
        public int MouseSpeed = 100;
        public int TextDelay = 3000;
        public bool LangChange = false;
        public bool AlwaysRun = false;
        public string RemoteHost = "localhost";
        public UInt16 RemotePort = 4000;
        public int ProxyType = (int)PROXYTYPE.NONE;
        public string ProxyHost = "localhost";
        public UInt16 ProxyPort = 1080;
        public string ProxyUser = "";
        public string ProxyPass = "";
        public int ScreenWidth = 800;
        public int ScreenHeight = 600;
        public int Light = 20;
        public int FlushValue = 100;
        public int BaseTexture = 1024;
        public bool Fullscreen = false;
        public bool BackGroundClear = false;
        public bool VSync = false;
        public bool AlwaysOnTop = false;
        public bool SoftwareSkinning = false;
        public int Animation3dFPS = 0;
        public int Animation3dSmoothTime = 250;
        public int MusicVolume = 100;
        public int SoundVolume = 100;
        public bool GlobalSound = true;
        public int DefaultCombatMode = (int)COMBATMODE.BOTH;
        public int IndicatorType = (int)INDICATORTYPE.LINES_NUMS;
        public int CombatMessagesType = (int)COMBATMSGTYPES.VERBOSE;
        public int DamageHitDelay = 0;
        public int Multisampling = -1;
    }
}
