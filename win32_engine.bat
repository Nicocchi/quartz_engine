@echo off

if not exist "env.bat" (
  echo Config file not found.
  exit /b 1
)
call env.bat

setlocal EnableDelayedExpansion

set BUILD_CACHE=%~dp0\_build_cache.cmd

if exist "!BUILD_CACHE!" (
  rem cache file exists, so call it to set env variables very fast
  call "!BUILD_CACHE!"
) else (
  echo "%MSVC_PATH%\VC\Auxiliary\Build\vcvarsall.bat"
  if not exist "%MSVC_PATH%\VC\Auxiliary\Build\vcvarsall.bat" (
    echo Visual Studio 2019 not found or installed version is to old.
    exit /b 1
  )
  rem cache file does not exist, get env variables in slow way
  call "%MSVC_PATH%\VC\Auxiliary\Build\vcvarsall.bat" amd64
  echo set PATH=!PATH!> "!BUILD_CACHE!"
  echo set INCLUDE=!INCLUDE!>> "!BUILD_CACHE!"
  echo set LIB=!LIB!>> "!BUILD_CACHE!"

  rem Depending on whether you are build .NET or other stuff, there are more
  rem env variables you might want to add to cache, like:
  rem Platform, FrameworkDir, NETFXSDKDir, WindowsSdkDir, WindowsSDKVersion, VCINSTALLDIR, ...
)

set BUILD_MODE=%1
set OUT_DIR=build\win64
set OUT_EXE=quartz
set HEADER_INCLUDES= ^
    /Ivendor\ ^
    /Ivendor\imgui ^
    /Ivendor\glfw

set SOURCES=engine\src\win32_main.cpp
set LIBS=User32.lib gdi32.lib opengl32.lib engine/lib/glfw3dll.lib engine/lib/freetype.lib
set COMMON_FLAGS=/std:c++17 -wd4091 -wd4293 /utf-8 /DUNICODE /D_UNICODE /Fd%OUT_DIR%\
set DEBUG_FLAGS=/Od /GS-
set PRODUCTION_FLAGS=/O2 /DNDEBUG
@set PCH=engine/src/pch.hpp.pch
@set PCHH=engine/src/pch.hpp

@set FLAGS=%COMMON_FLAGS% %DEBUG_FLAGS%
if "%BUILD_MODE%" == "prod" (
    @set FLAGS=%COMMON_FLAGS% %PRODUCTION_FLAGS%
)

IF NOT EXIST %OUT_DIR%\ MKDIR %OUT_DIR%

rem Get start time:
set "startTime=%time: =0%"

cl /nologo /FC /Zi /EHsc /DEBUG:FULL %FLAGS% %HEADER_INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link /LIBPATH:lib\win32 %LIBS%

rem Get end time:
set "endTime=%time: =0%"

rem Get elapsed time:
set "end=!endTime:%time:~8,1%=%%100)*100+1!"  &  set "start=!startTime:%time:~8,1%=%%100)*100+1!"
set /A "elap=((((10!end:%time:~2,1%=%%100)*60+1!%%100)-((((10!start:%time:~2,1%=%%100)*60+1!%%100), elap-=(elap>>31)*24*60*60*100"

rem Convert elapsed time to HH:MM:SS:CC format:
set /A "cc=elap%%100+100,elap/=100,ss=elap%%60+100,elap/=60,mm=elap%%60+100,hh=elap/60+100"

@REM set Start:    %startTime%
@REM set End:      %endTime%
echo Engine compilation time:  %hh:~1%%time:~2,1%%mm:~1%%time:~2,1%%ss:~1%%time:~8,1%%cc:~1%
