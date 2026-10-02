@echo off
rem Builds svufix.dll (x86) and svulaunch.exe (x86) into .\bin
rem Needs: Visual Studio with C++ (x86) tools, and .NET Framework 4.x (built into Windows).
setlocal
for /f "usebackq delims=" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -prerelease -products * -property installationPath`) do set VS=%%i
if not defined VS (echo Visual Studio not found & exit /b 1)
call "%VS%\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul || exit /b 1
if not exist bin mkdir bin
cl /nologo /LD /O1 /MT /W3 src\svufix.c /Fo:bin\ /Fe:bin\svufix.dll kernel32.lib || exit /b 1
"%WINDIR%\Microsoft.NET\Framework\v4.0.30319\csc.exe" /nologo /platform:x86 /target:winexe /out:bin\svulaunch.exe src\svulaunch.cs || exit /b 1
del bin\*.obj bin\*.lib bin\*.exp 2>nul
echo Done: bin\svufix.dll bin\svulaunch.exe
