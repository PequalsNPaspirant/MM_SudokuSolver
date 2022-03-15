@ECHO OFF
SETLOCAL

if not exist "build_vs_2017" mkdir "build_vs_2017"

set CLEANBUILD=%1

IF /I "%CLEANBUILD%"=="clean" (
	echo.removing old build contents...
	del /S /Q "build_vs_2017\*.*"
)

cmake -H. -Bbuild_vs_2017 -G "Visual Studio 15 2017 Win64"

cmake --build build_vs_2017 --config RelWithDebInfo
