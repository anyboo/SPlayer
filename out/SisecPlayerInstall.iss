; 脚本由 Inno Setup 脚本向导 生成！
; 有关创建 Inno Setup 脚本文件的详细资料请查阅帮助文档！

#define MyAppName "OVPlayer"
#define MyAppVersion "1.31"
#define MyAppPublisher "深圳市龙脉安信软件技术有限公司"
#define MyAppURL "http://www.example.com/"
#define MyAppExeName "OVPlayer.exe"

[Setup]
; 注: AppId的值为单独标识该应用程序。
; 不要为其他安装程序使用相同的AppId值。
; (生成新的GUID，点击 工具|在IDE中生成GUID。)
AppId={{B6041B43-56EF-40B4-AF92-D28AC13416F3}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\OVPlayer
DefaultGroupName=OVPlayer
AllowNoIcons=yes
OutputBaseFilename=OVPlayer_V1.31
SetupIconFile=E:\work\sisec\PlayerDemo\out\bin\sisecplayer.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "chinesesimp"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkablealone;
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags:checkablealone


[Files]
Source: "E:\work\sisec\PlayerDemo\out\bin\OVPlayer.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\update.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\update.bat"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\update.xml"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\sisecplayer.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\Enginio.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\Enginiod.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\icudt53.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\icuin53.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\icuuc53.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\msvcp120.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\msvcr120.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\PlayerFactory.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\Qt5Network.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\Qt5Sql.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\Qt5Xml.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\work\sisec\PlayerDemo\out\bin\7-Zip\*"; DestDir: "{app}\7-Zip"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "E:\work\sisec\PlayerDemo\out\bin\factorys\*"; DestDir: "{app}\factorys"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "E:\work\sisec\PlayerDemo\out\bin\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
; 注意: 不要在任何共享系统文件上使用“Flags: ignoreversion”

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

