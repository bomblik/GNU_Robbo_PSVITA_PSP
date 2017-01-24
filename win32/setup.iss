; -- Example1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!


#define MyAppName "Gnurobbo"
#define MyAppPublisher "GNU Robbo Team"
#define MyAppURL "http://gnurobbo.sf.net"
#define MyAppVerName "Gnurobbo 0.65"

[Setup]
AppName={#MyAppName}
AppVerName={#MyAppVerName}
AppPublisher={#MyAppPublisher}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\Gnurobbo
DefaultGroupName=GNU Robbo
UninstallDisplayIcon={app}\gnurobbo.exe
Compression=lzma
SolidCompression=true
WizardImageFile=gnurobbo_tlo.bmp
OutputBaseFilename=gnurobbo
AppID={{1F6F2370-B667-4DA8-85A0-FE5603872038}

[Files]
Source: gnurobbo.exe; DestDir: {app}
Source: data/*; DestDir: {app}/data; Flags: recursesubdirs createallsubdirs
Source: SDL.dll; DestDir: {app}
Source: SDL_image.dll; DestDir: {app}
Source: SDL_mixer.dll; DestDir: {app}
Source: SDL_ttf.dll; DestDir: {app}
Source: zlib1.dll; DestDir: {app}
Source: libogg-0.dll; DestDir: {app}
Source: libpng12-0.dll; DestDir: {app}
Source: libogg-0.dll; DestDir: {app}
Source: libfreetype-6.dll; DestDir: {app}
Source: mikmod.dll; DestDir: {app}
Source: gnurobbo.ico; DestDir: {app}
Source: README; DestDir: {app}
Source: LICENSE.txt; DestDir: {app}

[Icons]
Name: {group}\gnurobbo; Filename: {app}\gnurobbo.exe; WorkingDir: {app}; IconFilename: {app}/gnurobbo.ico
Name: {group}\{cm:UninstallProgram, GNU Robbo}; Filename: {uninstallexe}

[Run]
Filename: {app}\LICENSE.txt; Description: View the GPL license file; Flags: postinstall shellexec skipifsilent unchecked
Filename: {app}\gnurobbo.exe; Description: Launch application; Flags: postinstall nowait skipifsilent
