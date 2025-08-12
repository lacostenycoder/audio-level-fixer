; Audio Processor - Inno Setup Installer Script
; Professional Windows installer for Audio Processor application

#define MyAppName "Audio Processor"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "Audio Processor Development Team"
#define MyAppURL "https://github.com/audioprocessor/audioprocessor"
#define MyAppExeName "Audio Processor.exe"
#define MyAppDescription "Professional real-time audio processing for content creators"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
AppId={{8B5F3A2C-1D4E-4F6B-9C8A-7E2F5D3B1A9C}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
AppCopyright=Copyright (C) 2025 {#MyAppPublisher}
AppComments={#MyAppDescription}

; Installation directories
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes

; Output configuration
OutputDir=Output
OutputBaseFilename=AudioProcessor-{#MyAppVersion}-Setup
SetupIconFile=..\Resources\icon.ico
UninstallDisplayIcon={app}\{#MyAppExeName}

; Compression and encryption
Compression=lzma2/ultra64
SolidCompression=yes
EncryptionUsed=no

; Privileges and compatibility
PrivilegesRequired=admin
PrivilegesRequiredOverridesAllowed=dialog
MinVersion=10.0.19041
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

; Visual appearance
WizardStyle=modern
WizardSizePercent=100,100
WizardImageFile=..\Resources\wizard-image.bmp
WizardSmallImageFile=..\Resources\wizard-small.bmp
DisableWelcomePage=no
LicenseFile=..\LICENSE.txt

; Uninstall configuration
UninstallDisplayName={#MyAppName}
UninstallFilesDir={app}\Uninstall
CreateUninstallRegKey=yes

; Additional options
AllowNoIcons=yes
AlwaysRestart=no
RestartIfNeededByRun=no
ShowLanguageDialog=auto
UsePreviousAppDir=yes
UsePreviousGroup=yes
UsePreviousSetupType=yes
UsePreviousTasks=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 6.1
Name: "associatefiles"; Description: "Associate .audiopreset files with {#MyAppName}"; GroupDescription: "File associations"; Flags: unchecked
Name: "startmenu"; Description: "Create Start Menu entry"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkedonce

[Files]
; Main application executable
Source: "..\Build\AudioProcessor_artefacts\Audio Processor.exe"; DestDir: "{app}"; Flags: ignoreversion signonce
Source: "..\Build\AudioProcessor_artefacts\*.dll"; DestDir: "{app}"; Flags: ignoreversion signonce; Check: DLLExists

; Documentation
Source: "..\README.md"; DestDir: "{app}\Documentation"; DestName: "README.txt"; Flags: ignoreversion
Source: "..\USER_MANUAL.md"; DestDir: "{app}\Documentation"; DestName: "UserManual.txt"; Flags: ignoreversion
Source: "..\LICENSE.txt"; DestDir: "{app}\Documentation"; Flags: ignoreversion

; Presets
Source: "..\Presets\*.preset"; DestDir: "{app}\Presets"; Flags: ignoreversion createallsubdirs recursesubdirs

; Resources
Source: "..\Resources\*"; DestDir: "{app}\Resources"; Flags: ignoreversion createallsubdirs recursesubdirs; Check: ResourcesExist

; Visual C++ Redistributable (if needed)
Source: "Redist\VC_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall; Check: VCRedistNeeded

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Comment: "{#MyAppDescription}"; IconFilename: "{app}\Resources\icon.ico"
Name: "{group}\User Manual"; Filename: "{app}\Documentation\UserManual.txt"; Comment: "Audio Processor User Manual"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"; Comment: "Uninstall {#MyAppName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Comment: "{#MyAppDescription}"; IconFilename: "{app}\Resources\icon.ico"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Comment: "{#MyAppDescription}"; IconFilename: "{app}\Resources\icon.ico"; Tasks: quicklaunchicon

[Registry]
; Application registration
Root: HKLM; Subkey: "SOFTWARE\{#MyAppPublisher}\{#MyAppName}"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\{#MyAppPublisher}\{#MyAppName}"; ValueType: string; ValueName: "Version"; ValueData: "{#MyAppVersion}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\{#MyAppPublisher}\{#MyAppName}"; ValueType: dword; ValueName: "Installed"; ValueData: 1; Flags: uninsdeletekey

; File associations
Root: HKCR; Subkey: ".audiopreset"; ValueType: string; ValueName: ""; ValueData: "AudioProcessorPreset"; Flags: uninsdeletevalue; Tasks: associatefiles
Root: HKCR; Subkey: "AudioProcessorPreset"; ValueType: string; ValueName: ""; ValueData: "Audio Processor Preset"; Flags: uninsdeletekey; Tasks: associatefiles
Root: HKCR; Subkey: "AudioProcessorPreset\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Tasks: associatefiles
Root: HKCR; Subkey: "AudioProcessorPreset\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: associatefiles

; Uninstall information
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppName}"; ValueType: string; ValueName: "DisplayName"; ValueData: "{#MyAppName}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppName}"; ValueType: string; ValueName: "DisplayVersion"; ValueData: "{#MyAppVersion}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppName}"; ValueType: string; ValueName: "Publisher"; ValueData: "{#MyAppPublisher}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppName}"; ValueType: string; ValueName: "UninstallString"; ValueData: "{uninstallexe}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppName}"; ValueType: string; ValueName: "InstallLocation"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppName}"; ValueType: string; ValueName: "DisplayIcon"; ValueData: "{app}\{#MyAppExeName}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppName}"; ValueType: dword; ValueName: "NoModify"; ValueData: 1; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppName}"; ValueType: dword; ValueName: "NoRepair"; ValueData: 1; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppName}"; ValueType: dword; ValueName: "EstimatedSize"; ValueData: 50000; Flags: uninsdeletekey

[Run]
; Install Visual C++ Redistributable if needed
Filename: "{tmp}\VC_redist.x64.exe"; Parameters: "/quiet /norestart"; StatusMsg: "Installing Visual C++ Redistributable..."; Check: VCRedistNeeded

; Optional: Launch application after installation
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[UninstallRun]
; Stop application if running
Filename: "{cmd}"; Parameters: "/C taskkill /F /IM ""{#MyAppExeName}"" /T"; RunOnceId: "StopApp"; Flags: runhidden

[UninstallDelete]
; Clean up user data (optional - ask user)
Type: filesandordirs; Name: "{userappdata}\{#MyAppName}"; Check: ShouldDeleteUserData

[Code]
var
  DeleteUserDataPage: TInputOptionWizardPage;

// Check if DLL files exist
function DLLExists: Boolean;
begin
  Result := FileExists(ExpandConstant('{src}\..\Build\AudioProcessor_artefacts\*.dll'));
end;

// Check if Resources directory exists
function ResourcesExist: Boolean;
begin
  Result := DirExists(ExpandConstant('{src}\..\Resources'));
end;

// Check if Visual C++ Redistributable is needed
function VCRedistNeeded: Boolean;
var
  Version: String;
begin
  Result := not RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Version', Version);
end;

// Initialize setup
procedure InitializeWizard;
begin
  // Create custom page for user data deletion option during uninstall
  DeleteUserDataPage := CreateInputOptionPage(wpWelcome,
    'User Data', 'What should be done with your settings and presets?',
    'Audio Processor stores your settings and custom presets in your user profile. ' +
    'You can choose to keep or remove this data.',
    True, False);
  DeleteUserDataPage.Add('Keep my settings and custom presets');
  DeleteUserDataPage.Add('Remove all settings and custom presets');
  DeleteUserDataPage.SelectedValueIndex := 0;
end;

// Check if user data should be deleted
function ShouldDeleteUserData: Boolean;
begin
  Result := DeleteUserDataPage.SelectedValueIndex = 1;
end;

// Pre-installation checks
function InitializeSetup(): Boolean;
var
  Version: TWindowsVersion;
begin
  GetWindowsVersionEx(Version);
  
  // Check Windows version (Windows 10 version 1903 or later)
  if (Version.Major < 10) or ((Version.Major = 10) and (Version.Build < 18362)) then
  begin
    MsgBox('Audio Processor requires Windows 10 version 1903 (build 18362) or later.' + #13#10 +
           'Your Windows version is not supported.', mbError, MB_OK);
    Result := False;
    Exit;
  end;
  
  // Check available disk space (minimum 100 MB)
  if GetSpaceOnDisk(ExpandConstant('{autopf}'), False, nil, nil, nil) < 100 * 1024 * 1024 then
  begin
    MsgBox('Insufficient disk space. Audio Processor requires at least 100 MB of free space.', mbError, MB_OK);
    Result := False;
    Exit;
  end;
  
  Result := True;
end;

// Post-installation tasks
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    // Register virtual audio device (placeholder for actual implementation)
    // This would call a helper executable or DLL to register the WASAPI virtual device
    
    // Create user data directory
    CreateDir(ExpandConstant('{userappdata}\{#MyAppName}'));
    CreateDir(ExpandConstant('{userappdata}\{#MyAppName}\Presets'));
    CreateDir(ExpandConstant('{userappdata}\{#MyAppName}\Logs'));
  end;
end;

// Pre-uninstall tasks
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usUninstall then
  begin
    // Unregister virtual audio device (placeholder for actual implementation)
    // This would call a helper executable or DLL to unregister the WASAPI virtual device
  end;
end;

// Custom messages
[Messages]
WelcomeLabel2=This will install [name/ver] on your computer.%n%nAudio Processor provides professional real-time audio processing for content creators, podcasters, and streamers. It includes boost, compression, and limiting with an integrated virtual audio cable.%n%nIt is recommended that you close all other applications before continuing.
FinishedHeadingLabel=Completing the [name] Setup Wizard
FinishedLabelNoIcons=Setup has finished installing [name] on your computer.%n%nThe application includes a virtual audio device that allows you to route processed audio to recording software like OBS, Audacity, or streaming applications.
FinishedLabel=Setup has finished installing [name] on your computer. The application may be launched by selecting the installed icons.%n%nThe virtual audio device is now available for use with recording and streaming software.

[CustomMessages]
LaunchProgram=Launch %1 after installation
AssociateFiles=Associate .audiopreset files with %1
CreateDesktopIcon=Create a &desktop icon
CreateQuickLaunchIcon=Create a &Quick Launch icon
AdditionalIcons=Additional icons:
OptionalFeatures=Optional features:

