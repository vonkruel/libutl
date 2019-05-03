@echo off

@rem require admin
net session >nul 2>&1
if not %errorLevel% == 0 (
  echo Please run this script as administrator.
  goto:EOF
)

@rem for invoking PowerShell
set POWERSHELL_EXE=%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe
set POWERSHELL_ARGS=-NoProfile -InputFormat None -ExecutionPolicy Bypass
set POWERSHELL=%POWERSHELL_EXE% %POWERSHELL_ARGS%

@rem enable developer mode
%POWERSHELL% .\enable-developer-mode.ps1

@rem chocolatey: install
where choco >nul 2>&1
if not %errorLevel% == 0 (
  %POWERSHELL% -Command "iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))"
  SET "PATH=%PATH%;%ALLUSERSPROFILE%\chocolatey\bin"
)

@rem Git: install
if not exist "%PROGRAMFILES%\Git" (
  choco install git --params "/GitAndUnixToolsOnPath"
  set PATH=%PATH%;%PROGRAMFILES%\Git\mingw64\bin
)

@rem Git: enable symlinks (system-wide)
git config --system core.symlinks true

@rem Git: unix-style newlines in repositories (LF not CRLF)
git config --global core.autocrlf input
