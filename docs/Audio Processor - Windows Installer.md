# Audio Processor - Windows Installer

This directory contains the Windows installer configuration and build scripts for Audio Processor.

## Overview

The installer is built using **Inno Setup 6**, a professional, free installer creation tool. It creates a single executable installer that includes:

- Complete application installation
- Registry entries for proper Windows integration
- Start Menu and Desktop shortcuts
- File associations for preset files
- Professional uninstaller
- Multi-language support

## Prerequisites

### Required Software
1. **Inno Setup 6** - Download from https://jrsoftware.org/isinfo.php
2. **Built Audio Processor application** - Must be compiled first using `build_windows.bat`

### Optional Components
1. **Visual C++ Redistributable** - Place `VC_redist.x64.exe` in `Redist\` directory
2. **Code Signing Certificate** - For digital signing (recommended for distribution)

## File Structure

```
installer/
├── AudioProcessor.iss          # Main Inno Setup script
├── build_installer.bat         # Automated build script
├── README.md                   # This documentation
├── Output/                     # Generated installer output
├── Redist/                     # Redistributable components
│   └── VC_redist.x64.exe      # Visual C++ Redistributable (optional)
└── Resources/                  # Installer resources
    ├── icon.ico               # Application icon
    ├── wizard-image.bmp       # Installer wizard image
    └── wizard-small.bmp       # Small wizard image
```

## Building the Installer

### Automated Build (Recommended)
```cmd
cd installer
build_installer.bat
```

### Manual Build
1. Open Inno Setup 6
2. Open `AudioProcessor.iss`
3. Click "Build" or press F9
4. Installer will be created in `Output\` directory

## Installer Features

### Installation Options
- **Installation Directory**: Customizable (default: `C:\Program Files\Audio Processor\`)
- **Components**: Core application, documentation, presets
- **Shortcuts**: Start Menu, Desktop (optional), Quick Launch (optional)
- **File Associations**: .audiopreset files (optional)

### System Integration
- **Registry Entries**: Proper Windows registration
- **Uninstall Support**: Complete removal via Add/Remove Programs
- **User Data**: Preserved during uninstall (user choice)
- **Virtual Audio Device**: Automatic registration

### Professional Features
- **Multi-language Support**: English, Spanish, French, German, Italian, Portuguese, Japanese
- **Digital Signing**: Ready for code signing certificate
- **Silent Installation**: Command-line support
- **Upgrade Handling**: Proper version management

## Installer Script Configuration

### Key Settings in AudioProcessor.iss

#### Application Information
```pascal
#define MyAppName "Audio Processor"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "Audio Processor Development Team"
#define MyAppExeName "Audio Processor.exe"
```

#### Installation Requirements
- **Minimum Windows Version**: Windows 10 version 1903 (build 18362)
- **Architecture**: x64 only
- **Privileges**: Administrator required for system integration
- **Disk Space**: Minimum 100 MB

#### Registry Entries
- Application registration under `HKLM\SOFTWARE\Audio Processor Development Team\Audio Processor`
- Uninstall information under `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall`
- File associations for .audiopreset files (optional)

## Customization

### Changing Application Information
Edit the `#define` statements at the top of `AudioProcessor.iss`:
```pascal
#define MyAppName "Your App Name"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "Your Company"
```

### Adding Files
Add entries to the `[Files]` section:
```pascal
Source: "path\to\your\file.ext"; DestDir: "{app}"; Flags: ignoreversion
```

### Custom Registry Entries
Add entries to the `[Registry]` section:
```pascal
Root: HKLM; Subkey: "SOFTWARE\YourApp"; ValueType: string; ValueName: "Setting"; ValueData: "Value"
```

### Installation Tasks
Modify the `[Tasks]` section to add/remove optional components:
```pascal
Name: "mytask"; Description: "My custom task"; GroupDescription: "Custom options"
```

## Digital Signing

### Code Signing Certificate
For professional distribution, sign both the application and installer:

1. **Sign Application**: Sign `Audio Processor.exe` before building installer
2. **Sign Installer**: Sign the final installer executable

### Signing Commands
```cmd
REM Sign application
signtool sign /f "certificate.pfx" /p "password" /t "http://timestamp.digicert.com" "Audio Processor.exe"

REM Sign installer
signtool sign /f "certificate.pfx" /p "password" /t "http://timestamp.digicert.com" "AudioProcessor-1.0.0-Setup.exe"
```

## Testing

### Test Environments
1. **Clean Windows 10/11**: Fresh installation
2. **Windows 10 version 1903+**: Minimum supported version
3. **Different User Accounts**: Administrator and standard user
4. **Antivirus Software**: Test with Windows Defender and third-party antivirus

### Test Scenarios
1. **Fresh Installation**: Install on clean system
2. **Upgrade Installation**: Install over previous version
3. **Uninstallation**: Verify complete removal
4. **Silent Installation**: Test command-line installation
5. **Application Functionality**: Verify all features work after installation

### Command-Line Testing
```cmd
REM Silent installation
AudioProcessor-1.0.0-Setup.exe /SILENT

REM Very silent installation (no UI)
AudioProcessor-1.0.0-Setup.exe /VERYSILENT

REM Installation with custom directory
AudioProcessor-1.0.0-Setup.exe /DIR="C:\MyApps\AudioProcessor"

REM Silent uninstallation
"C:\Program Files\Audio Processor\unins000.exe" /SILENT
```

## Distribution

### Installer Verification
Before distribution, verify:
- [ ] Installer is digitally signed
- [ ] Application launches correctly after installation
- [ ] Virtual audio device is registered
- [ ] Uninstaller removes all components
- [ ] No false positives from antivirus software

### Distribution Channels
- **Direct Download**: Host on your website
- **Software Repositories**: Submit to software download sites
- **Microsoft Store**: Consider Store packaging for wider distribution
- **Enterprise Distribution**: MSI wrapper for Group Policy deployment

## Troubleshooting

### Common Build Issues

#### "Application not found"
- Ensure Audio Processor is built using `build_windows.bat`
- Check path: `..\Build\AudioProcessor_artefacts\Audio Processor.exe`

#### "Inno Setup not found"
- Install Inno Setup 6 from https://jrsoftware.org/isinfo.php
- Verify installation path in `build_installer.bat`

#### "Access denied" during build
- Run as Administrator
- Check file permissions
- Ensure no antivirus interference

### Runtime Issues

#### "Application won't start"
- Install Visual C++ Redistributable
- Check Windows version compatibility
- Verify digital signature

#### "Virtual audio device not working"
- Run installer as Administrator
- Check Windows Audio service
- Verify WASAPI support

## Advanced Configuration

### Custom Actions
Add custom installation logic in the `[Code]` section:
```pascal
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    // Custom post-installation code
  end;
end;
```

### Conditional Installation
Use conditional compilation for different builds:
```pascal
#ifdef TRIAL_VERSION
  #define MyAppName "Audio Processor Trial"
#else
  #define MyAppName "Audio Processor"
#endif
```

### Multiple Languages
Add language files to the `[Languages]` section:
```pascal
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
```

This installer provides a professional, reliable installation experience for Audio Processor users on Windows systems.

