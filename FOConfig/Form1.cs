using Enichan.INIParser;
using System.Diagnostics;

namespace FOConfig
{
    public partial class Form1 : Form
    {
        readonly string[] CONFIG_NAMES = ["Client.cfg", "FOnline.cfg"];
        readonly string CLIENT_EXE_NAME = "Client64.exe";
        string CurrConfigName = "";
        readonly IniFile cfgFile = new IniFile();

        public Settings CfgSettings;

        public Form1()
        {
            CfgSettings = new Settings();
            LoadConfig();
            InitializeComponent();
            InitScreenElements();
            SaveBtn.Click += SaveBtn_Click;
        }

        private void SaveConfig()
        {
            if (CurrConfigName == "")
            {
                CurrConfigName = Path.Combine(AppContext.BaseDirectory, CONFIG_NAMES[0]);
            }

            CfgSettings.Language = LangEngl.Checked ? "engl" : "russ";
            CfgSettings.WinNotify = WinNotify.Checked;
            CfgSettings.SoundNotify = SoundNotify.Checked;
            CfgSettings.InvertMessBox = InvertMessbox.Checked;
            CfgSettings.Logging = Logging.Checked;
            CfgSettings.LoggingTime = LoggingTime.Checked;
            CfgSettings.Sleep = Decimal.ToInt32(SleepValue.Value);
            CfgSettings.ScrollDelay = Decimal.ToInt32(ScrollDelayValue.Value);
            CfgSettings.ScrollStep = Decimal.ToInt32(ScrollStepValue.Value);
            CfgSettings.MouseSpeed = Decimal.ToInt32(MouseSpeedValue.Value);
            CfgSettings.TextDelay = Decimal.ToInt32(TextDelayValue.Value);
            CfgSettings.LangChange = LangSwitchAltShift.Checked;
            CfgSettings.AlwaysRun = AlwaysRun.Checked;
            CfgSettings.RemoteHost = GameHostValue.Text;
            CfgSettings.RemotePort = Decimal.ToUInt16(GamePortValue.Value);
            CfgSettings.ProxyType = ProxyTypeHTTP.Checked ? (int)PROXYTYPE.HTTP
                : ProxyTypeSocks5.Checked ? (int)PROXYTYPE.SOCKS5
                : ProxyTypeSocks4.Checked ? (int)PROXYTYPE.SOCKS4
                : (int)PROXYTYPE.NONE;
            CfgSettings.ProxyHost = ProxyHostValue.Text;
            CfgSettings.ProxyPort = Decimal.ToUInt16(ProxyPortValue.Value);
            CfgSettings.ProxyUser = ProxyLoginValue.Text;
            CfgSettings.ProxyPass = ProxyPasswordValue.Text;
            CfgSettings.ScreenWidth = Int32.TryParse(ResolutionX.Text, out var width) ? width : CfgSettings.ScreenWidth;
            CfgSettings.ScreenHeight = Int32.TryParse(ResolutionY.Text, out var height) ? height : CfgSettings.ScreenHeight;
            CfgSettings.Light = Decimal.ToInt32(LightValue.Value);
            CfgSettings.FlushValue = Decimal.ToInt32(CacheSpritesValue.Value);
            CfgSettings.BaseTexture = Decimal.ToInt32(TextureSizeValue.Value);
            CfgSettings.Fullscreen = Fullscreen.Checked;
            CfgSettings.BackGroundClear = ScreenClear.Checked;
            CfgSettings.VSync = VSync.Checked;
            CfgSettings.AlwaysOnTop = AlwaysOnTop.Checked;
            CfgSettings.SoftwareSkinning = SoftwareSkin3D.Checked;
            CfgSettings.Animation3dFPS = Decimal.ToInt32(FPSValue.Value);
            CfgSettings.Animation3dSmoothTime = Decimal.ToInt32(SmoothTrans3DValue.Value);
            CfgSettings.MusicVolume = MusicVolValue.Value;
            CfgSettings.SoundVolume = SoundVolValue.Value;
            CfgSettings.GlobalSound = GlobalSound.Checked;
            CfgSettings.DefaultCombatMode = CombatTurnBased.Checked ? (int)COMBATMODE.TURN_BASED
                : CombatRealTime.Checked ? (int)COMBATMODE.REAL_TIME
                : (int)COMBATMODE.BOTH;
            CfgSettings.IndicatorType = AmmoNumbers.Checked ? (int)INDICATORTYPE.NUMS
                : AmmoLines.Checked ? (int)INDICATORTYPE.LINES
                : (int)INDICATORTYPE.LINES_NUMS;
            CfgSettings.CombatMessagesType = CombatBrief.Checked ? (int)COMBATMSGTYPES.BRIEF : (int)COMBATMSGTYPES.VERBOSE;
            CfgSettings.DamageHitDelay = Decimal.ToInt32(DmgDelayValue.Value);
            CfgSettings.Multisampling = MultisamplingValue.SelectedIndex - 1;

            var values = cfgFile[""];
            values["Language"] = CfgSettings.Language;
            values["WinNotify"] = CfgSettings.WinNotify;
            values["SoundNotify"] = CfgSettings.SoundNotify;
            values["InvertMessBox"] = CfgSettings.InvertMessBox;
            values["Logging"] = CfgSettings.Logging;
            values["LoggingTime"] = CfgSettings.LoggingTime;
            values["Sleep"] = CfgSettings.Sleep;
            values["ScrollDelay"] = CfgSettings.ScrollDelay;
            values["ScrollStep"] = CfgSettings.ScrollStep;
            values["MouseSpeed"] = CfgSettings.MouseSpeed;
            values["TextDelay"] = CfgSettings.TextDelay;
            values["LangChange"] = CfgSettings.LangChange;
            values["AlwaysRun"] = CfgSettings.AlwaysRun;
            values["RemoteHost"] = CfgSettings.RemoteHost;
            SaveIndexedComboBoxValues(GameHostValue, values, "RemoteHost");
            values["RemotePort"] = CfgSettings.RemotePort;
            values["ProxyType"] = CfgSettings.ProxyType;
            values["ProxyHost"] = CfgSettings.ProxyHost;
            values["ProxyPort"] = CfgSettings.ProxyPort;
            values["ProxyUser"] = CfgSettings.ProxyUser;
            values["ProxyPass"] = CfgSettings.ProxyPass;
            values["ScreenWidth"] = CfgSettings.ScreenWidth;
            values["ScreenHeight"] = CfgSettings.ScreenHeight;
            values["Light"] = CfgSettings.Light;
            values["FlushValue"] = CfgSettings.FlushValue;
            values["BaseTexture"] = CfgSettings.BaseTexture;
            values["Fullscreen"] = CfgSettings.Fullscreen;
            values["BackGroundClear"] = CfgSettings.BackGroundClear;
            values["VSync"] = CfgSettings.VSync;
            values["AlwaysOnTop"] = CfgSettings.AlwaysOnTop;
            values["SoftwareSkinning"] = CfgSettings.SoftwareSkinning;
            values["Animation3dFPS"] = CfgSettings.Animation3dFPS;
            values["Animation3dSmoothTime"] = CfgSettings.Animation3dSmoothTime;
            values["MusicVolume"] = CfgSettings.MusicVolume;
            values["SoundVolume"] = CfgSettings.SoundVolume;
            values["GlobalSound"] = CfgSettings.GlobalSound;
            values["DefaultCombatMode"] = CfgSettings.DefaultCombatMode;
            values["IndicatorType"] = CfgSettings.IndicatorType;
            values["CombatMessagesType"] = CfgSettings.CombatMessagesType;
            values["DamageHitDelay"] = CfgSettings.DamageHitDelay;
            values["Multisampling"] = CfgSettings.Multisampling;

            cfgFile.Save(CurrConfigName);

        }

        private void LoadConfig()
        {
            foreach (string name in CONFIG_NAMES)
            {
                FileInfo fileInfo = new FileInfo(Path.Combine(AppContext.BaseDirectory, name));
                if (!fileInfo.Exists) continue;
                CurrConfigName = fileInfo.FullName;
                break;
            }

            if (CurrConfigName == "") return;

            cfgFile.Load(CurrConfigName);

            string strRead = "";

            strRead = cfgFile[""]["Language"].GetString();
            CfgSettings.Language = strRead != "" ? strRead : "russ";
            CfgSettings.WinNotify = cfgFile[""]["WinNotify"].ToBool(true);
            CfgSettings.SoundNotify = cfgFile[""]["SoundNotify"].ToBool(false);
            CfgSettings.InvertMessBox = cfgFile[""]["InvertMessBox"].ToBool(false);
            CfgSettings.Logging = cfgFile[""]["Logging"].ToBool(true);
            CfgSettings.LoggingTime = cfgFile[""]["LoggingTime"].ToBool(false);
            CfgSettings.Sleep = cfgFile[""]["Sleep"].ToInt(0);
            CfgSettings.ScrollDelay = cfgFile[""]["ScrollDelay"].ToInt(4);
            CfgSettings.ScrollStep = cfgFile[""]["ScrollStep"].ToInt(32);
            CfgSettings.MouseSpeed = cfgFile[""]["MouseSpeed"].ToInt(100);
            CfgSettings.TextDelay = cfgFile[""]["TextDelay"].ToInt(3000);
            CfgSettings.LangChange = cfgFile[""]["LangChange"].ToBool(false);
            CfgSettings.AlwaysRun = cfgFile[""]["AlwaysRun"].ToBool(false);
            strRead = cfgFile[""]["RemoteHost"].GetString();
            CfgSettings.RemoteHost = strRead != "" ? strRead : "localhost";
            CfgSettings.RemotePort = (UInt16)cfgFile[""]["RemotePort"].ToInt(4000);
            CfgSettings.ProxyType = cfgFile[""]["ProxyType"].ToInt((int)PROXYTYPE.NONE);
            strRead = cfgFile[""]["ProxyHost"].GetString();
            CfgSettings.ProxyHost = strRead != "" ? strRead : "localhost";
            CfgSettings.ProxyPort = (UInt16)cfgFile[""]["ProxyPort"].ToInt(1080);
            CfgSettings.ProxyUser = cfgFile[""]["ProxyUser"].GetString();
            CfgSettings.ProxyPass = cfgFile[""]["ProxyPass"].GetString();
            CfgSettings.ScreenWidth = cfgFile[""]["ScreenWidth"].ToInt(800);
            CfgSettings.ScreenHeight = cfgFile[""]["ScreenHeight"].ToInt(600);
            CfgSettings.Light = cfgFile[""]["Light"].ToInt(20);
            CfgSettings.FlushValue = cfgFile[""]["FlushValue"].ToInt(100);
            CfgSettings.BaseTexture = cfgFile[""]["BaseTexture"].ToInt(1024);
            CfgSettings.Fullscreen = cfgFile[""]["Fullscreen"].ToBool(false);
            CfgSettings.BackGroundClear = cfgFile[""]["BackGroundClear"].ToBool(false);
            CfgSettings.VSync = cfgFile[""]["VSync"].ToBool(false);
            CfgSettings.AlwaysOnTop = cfgFile[""]["AlwaysOnTop"].ToBool(false);
            CfgSettings.SoftwareSkinning = cfgFile[""]["SoftwareSkinning"].ToBool(false);
            CfgSettings.Animation3dFPS = cfgFile[""]["Animation3dFPS"].ToInt(0);
            CfgSettings.Animation3dSmoothTime = cfgFile[""]["Animation3dSmoothTime"].ToInt(250);
            CfgSettings.MusicVolume = cfgFile[""]["MusicVolume"].ToInt(100);
            CfgSettings.SoundVolume = cfgFile[""]["SoundVolume"].ToInt(100);
            CfgSettings.GlobalSound = cfgFile[""]["GlobalSound"].ToBool(true);
            CfgSettings.DefaultCombatMode = cfgFile[""]["DefaultCombatMode"].ToInt((int)COMBATMODE.BOTH);
            CfgSettings.IndicatorType = cfgFile[""]["IndicatorType"].ToInt((int)INDICATORTYPE.LINES_NUMS);
            CfgSettings.CombatMessagesType = cfgFile[""]["CombatMessagesType"].ToInt((int)COMBATMSGTYPES.VERBOSE);
            CfgSettings.DamageHitDelay = cfgFile[""]["DamageHitDelay"].ToInt(0);
            CfgSettings.Multisampling = cfgFile[""]["Multisampling"].ToInt(-1);
        }

        private void InitScreenElements()
        {
            #region Language
            if (CfgSettings.Language == "russ")
            {
                LangRuss.Checked = true;
            }
            else if (CfgSettings.Language == "engl")
            {
                LangEngl.Checked = true;
            }
            #endregion
            #region WinNotify
            WinNotify.Checked = CfgSettings.WinNotify;
            #endregion
            #region SoundNotify
            SoundNotify.Checked = CfgSettings.SoundNotify;
            #endregion
            #region InvertMessbox
            InvertMessbox.Checked = CfgSettings.InvertMessBox;
            #endregion
            #region Logging
            Logging.Checked = CfgSettings.Logging;
            #endregion
            #region LoggingTime
            LoggingTime.Checked = CfgSettings.LoggingTime;
            #endregion
            #region Sleep
            SleepValue.Value = CfgSettings.Sleep;
            #endregion
            #region ScrollDelay
            ScrollDelayValue.Value = CfgSettings.ScrollDelay;
            #endregion
            #region ScrollStep
            ScrollStepValue.Value = CfgSettings.ScrollStep;
            #endregion
            #region MouseSpeed
            MouseSpeedValue.Value = CfgSettings.MouseSpeed;
            #endregion
            #region TextDelay
            TextDelayValue.Value = CfgSettings.TextDelay;
            #endregion
            #region LangChange
            if (CfgSettings.LangChange)
            {
                LangSwitchAltShift.Checked = true;
            }
            else
            {
                LangSwitchCtrlShift.Checked = true;
            }
            #endregion
            #region AlwaysRun
            AlwaysRun.Checked = CfgSettings.AlwaysRun;
            #endregion
            #region RemoteHost
            SetComboBoxValue(GameHostValue, CfgSettings.RemoteHost);
            AddIndexedComboBoxValues(GameHostValue, cfgFile[""], "RemoteHost");
            #endregion
            #region RemotePort
            GamePortValue.Value = CfgSettings.RemotePort;
            #endregion
            #region ProxyType
            switch ((PROXYTYPE)CfgSettings.ProxyType)
            {
                case (PROXYTYPE.NONE):
                    {
                        ProxyTypeNone.Checked = true;
                    }
                    break;
                case (PROXYTYPE.SOCKS4):
                    {
                        ProxyTypeSocks4.Checked = true;
                    }
                    break;
                case (PROXYTYPE.SOCKS5):
                    {
                        ProxyTypeSocks5.Checked = true;
                    }
                    break;
                case (PROXYTYPE.HTTP):
                    {
                        ProxyTypeHTTP.Checked = true;
                    }
                    break;
            }
            #endregion
            #region ProxyHost
            SetComboBoxValue(ProxyHostValue, CfgSettings.ProxyHost);
            #endregion
            #region ProxyPort
            ProxyPortValue.Value = CfgSettings.ProxyPort;
            #endregion
            #region ProxyUser
            ProxyLoginValue.Text = CfgSettings.ProxyUser;
            #endregion
            #region ProxyPass
            ProxyPasswordValue.Text = CfgSettings.ProxyPass;
            #endregion
            #region ScreenWidth
            SetComboBoxValue(ResolutionX, CfgSettings.ScreenWidth.ToString());
            #endregion
            #region ScreenHeight
            SetComboBoxValue(ResolutionY, CfgSettings.ScreenHeight.ToString());
            #endregion
            #region Light
            LightValue.Value = CfgSettings.Light;
            #endregion
            #region FlushValue
            CacheSpritesValue.Value = CfgSettings.FlushValue;
            #endregion
            #region BaseTexture
            TextureSizeValue.Value = CfgSettings.BaseTexture;
            #endregion
            #region Fullscreen
            Fullscreen.Checked = CfgSettings.Fullscreen;
            #endregion
            #region BackGroundClear
            ScreenClear.Checked = CfgSettings.BackGroundClear;
            #endregion
            #region VSync
            VSync.Checked = CfgSettings.VSync;
            #endregion
            #region AlwaysOnTop
            AlwaysOnTop.Checked = CfgSettings.AlwaysOnTop;
            #endregion
            #region SoftwareSkinning
            SoftwareSkin3D.Checked = CfgSettings.SoftwareSkinning;
            #endregion
            #region Animation3dFPS
            FPSValue.Value = CfgSettings.Animation3dFPS;
            #endregion
            #region Animation3dSmoothTime
            SmoothTrans3DValue.Value = CfgSettings.Animation3dSmoothTime;
            #endregion
            #region MusicVolume
            MusicVolValue.Value = CfgSettings.MusicVolume;
            #endregion
            #region SoundVolume
            SoundVolValue.Value = CfgSettings.SoundVolume;
            #endregion
            #region GlobalSound
            GlobalSound.Checked = CfgSettings.GlobalSound;
            #endregion
            #region DefaultCombatMode
            switch ((COMBATMODE)CfgSettings.DefaultCombatMode)
            {
                case (COMBATMODE.BOTH):
                    {
                        CombatBothModes.Checked = true;
                    }
                    break;
                case (COMBATMODE.REAL_TIME):
                    {
                        CombatRealTime.Checked = true;
                    }
                    break;
                case (COMBATMODE.TURN_BASED):
                    {
                        CombatTurnBased.Checked = true;
                    }
                    break;
            }
            #endregion
            #region IndicatorType
            switch ((INDICATORTYPE)CfgSettings.IndicatorType)
            {
                case (INDICATORTYPE.LINES):
                    {
                        AmmoLines.Checked = true;
                    }
                    break;
                case (INDICATORTYPE.NUMS):
                    {
                        AmmoNumbers.Checked = true;
                    }
                    break;
                case (INDICATORTYPE.LINES_NUMS):
                    {
                        AmmoLinesNumbers.Checked = true;
                    }
                    break;
            }
            #endregion
            #region Multisampling
            MultisamplingValue.SelectedIndex = 1 + CfgSettings.Multisampling;
            #endregion
            #region CombatMessagesType
            if ((COMBATMSGTYPES)CfgSettings.CombatMessagesType == COMBATMSGTYPES.BRIEF)
            {
                CombatBrief.Checked = true;
            }
            else
            {
                CombatVerbose.Checked = true;
            }
            #endregion
            #region DamageHitDelay
            DmgDelayValue.Value = CfgSettings.DamageHitDelay;
            #endregion

            ApplyTranslation();
        }

        private void ApplyTranslation()
        {
            var russian = CfgSettings.Language == "russ";

            Text = russian ? "FOnline Конфигурация" : "FOnline Config";
            tabPage1.Text = russian ? "Разное" : "Other";
            LoggingTime.Text = russian ? "Запись в лог с указанием времени." : "Logging with time.";
            Logging.Text = russian ? "Ведение лога в 'FOnline.log'." : "Logging in 'FOnline.log'.";
            InvertMessbox.Text = russian ? "Инвертирование текста\r\nв окне сообщений." : "Invert text in messbox.";
            SoundNotify.Text = russian ? "Звуковое извещение о сообщениях\r\nпри неактивном окне." : "Beep sound on not active game.";
            WinNotify.Text = russian ? "Извещение о сообщениях\r\nпри неактивном окне." : "Flush window on not active game.";

            GamePage.Text = russian ? "Игра" : "Game";
            LangSwitchGroup.Text = russian ? "Переключение раскладки" : "Keyboard Language Switch";
            GameGroup.Text = russian ? "Игра" : "Game";
            AlwaysRun.Text = russian ? "Постоянный бег" : "Always run";
            TextDelayLabel.Text = russian ? "Задержка текста (мс)" : "Text Delay (ms)";
            MouseSpeedLabel.Text = russian ? "Скорость мышки (%)" : "Mouse Speed (%)";
            ScrollStepLabel.Text = russian ? "Шаг скроллинга" : "Scroll Step";
            ScrollDelayLabel.Text = russian ? "Задержка скроллинга" : "Scroll Delay";

            tabPage3.Text = russian ? "Боевка" : "Combat";
            DmgDelayGroup.Text = russian ? "Отображение урона над головой" : "Damage indicators on head";
            DmgDelayLabel.Text = russian ? "Задержка (мс)" : "Delay (ms)";
            CombatMessagesGroup.Text = russian ? "Боевые сообщения" : "Combat messages";
            CombatBrief.Text = russian ? "Краткие" : "Brief";
            CombatVerbose.Text = russian ? "Полные" : "Verbose";
            AmmoDisplGroup.Text = russian ? "Индикатор патронов" : "Ammo amount display";
            AmmoNumbers.Text = russian ? "Номера" : "Numbers";
            AmmoLines.Text = russian ? "Линии" : "Lines";
            AmmoLinesNumbers.Text = russian ? "Линии и номера" : "Lines and Numbers";
            CombatModeGroup.Text = russian ? "Режим боя по-умолчанию" : "Default combat mode";
            CombatTurnBased.Text = russian ? "Пошаговый режим" : "Turn-based";
            CombatRealTime.Text = russian ? "Реальное время" : "Real-time";
            CombatBothModes.Text = russian ? "Оба режима" : "Both modes";

            tabPage4.Text = russian ? "Сеть" : "Net";
            ProxyServerGroup.Text = russian ? "Прокси" : "Proxy";
            ProxyPasswordLabel.Text = russian ? "Пароль" : "Password";
            ProxyLoginLabel.Text = russian ? "Логин" : "Login";
            ProxyPortLabel.Text = russian ? "Порт" : "Port";
            ProxyTypeLabel.Text = russian ? "Тип" : "Type";
            ProxyHostLabel.Text = russian ? "Хост" : "Host";
            ProxyTypeNone.Text = russian ? "Нет" : "None";
            GameServerGroup.Text = russian ? "Игровой сервер" : "Game server";
            GamePortLabel.Text = russian ? "Порт" : "Port";
            GameHostLabel.Text = russian ? "Адрес" : "Host";

            tabPage5.Text = russian ? "Видео" : "Video";
            SoftwareSkin3D.Text = russian ? "Софтварный скиннинг 3D" : "Software skinning 3D";
            AlwaysOnTop.Text = russian ? "Поверх всех окон" : "Always on top";
            ScreenClear.Text = russian ? "Очистка экрана" : "Screen clear";
            Fullscreen.Text = russian ? "Полноэкранный режим" : "Fullscreen";
            SmoothTrans3DLabel.Text = russian ? "Мягкость 3D\r\nпереходов" : "3D Smooth\r\ntransition";
            MultisamplingLabel.Text = russian ? "Мультисэмпл 3D" : "Multisampling 3D";
            TextureSizeLabel.Text = russian ? "Размер текстур" : "Texture size";
            CacheSpritesLabel.Text = russian ? "Кэш спрайтов" : "Cache sprites";
            LightLable.Text = russian ? "Яркость" : "Light";
            ResolutionGroup.Text = russian ? "Разрешение" : "Resolution";

            tabPage6.Text = russian ? "Звук" : "Sound";
            GlobalSound.Text = russian ? "Общий звук" : "Global sound";
            VolumeGroup.Text = russian ? "Громкость" : "Volume";
            SoundVolLabel.Text = russian ? "Звуки" : "Sound";
            MusicVolLabel.Text = russian ? "Музыка" : "Music";

            PlayBtn.Text = russian ? "Играть" : "Play";
            SaveBtn.Text = russian ? "Сохранить" : "Save";
            ExitBtn.Text = russian ? "Выход" : "Exit";
        }

        private static void SetComboBoxValue(ComboBox comboBox, string value)
        {
            if (!comboBox.Items.Contains(value))
            {
                comboBox.Items.Add(value);
            }

            comboBox.SelectedItem = value;
        }

        private static void AddIndexedComboBoxValues(ComboBox comboBox, IniSection section, string keyPrefix)
        {
            for (int index = 0; ; index++)
            {
                if (!section.TryGetValue($"{keyPrefix}_{index}", out var iniValue))
                {
                    break;
                }

                var value = iniValue.GetString();
                if (value.Length > 0 && !comboBox.Items.Contains(value))
                {
                    comboBox.Items.Add(value);
                }
            }
        }

        private static void SaveIndexedComboBoxValues(ComboBox comboBox, IniSection section, string keyPrefix)
        {
            var indexedKeyPrefix = keyPrefix + "_";
            foreach (var key in section.Keys.ToList())
            {
                if (key.StartsWith(indexedKeyPrefix, StringComparison.OrdinalIgnoreCase)
                    && Int32.TryParse(key.Substring(indexedKeyPrefix.Length), out _))
                {
                    section.Remove(key);
                }
            }

            var index = 0;
            foreach (var item in comboBox.Items)
            {
                var value = item?.ToString()?.Trim();
                if (!string.IsNullOrEmpty(value))
                {
                    section[$"{keyPrefix}_{index}"] = value;
                    index++;
                }
            }
        }

        private void SaveBtn_Click(object? sender, EventArgs e)
        {
            SaveConfig();
        }

        private void ExitBtn_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void Lang_CheckedChanged(object sender, EventArgs e)
        {
            if (!(sender is RadioButton)) return;
            RadioButton langBtn = (RadioButton)sender;
            if (!(langBtn.Tag is string)) return;

            if (langBtn.Checked)
            {
                CfgSettings.Language = (string)langBtn.Tag;
                ApplyTranslation();
            }
        }

        private void PlayBtn_Click(object sender, EventArgs e)
        {
            SaveConfig();
            Process.Start(CLIENT_EXE_NAME);
            Close();
        }
    }
}
