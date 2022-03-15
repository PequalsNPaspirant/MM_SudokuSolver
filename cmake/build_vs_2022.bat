@ECHO OFF
SETLOCAL

if not exist "build_vs_2022" mkdir "build_vs_2022"

set CLEANBUILD=%1

IF /I "%CLEANBUILD%"=="clean" (
	echo.removing old build contents...
	del /S /Q "build_vs_2022\*.*"
)

cmake -H. -Bbuild_vs_2022 -G "Visual Studio 17 2022" -A x64

cmake --build build_vs_2022 --config RelWithDebInfo
