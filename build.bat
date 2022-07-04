@echo off

rem Zi generates debug info
set outfile=bin\tetris.exe
set cflags=/Zi /nologo /DDEBUG /DWIN32
set incdirs=/Ilib-win\SDL2-2.0.10\include /Ilib\glad-gl3.3\include /Ilib\glm /Ilib /Ilib-win\freetype-2.3.5-1-lib\include /Ilib-win\freetype-2.3.5-1-lib\include\freetype2
set ldflags=/SUBSYSTEM:CONSOLE
set ldlibs=lib-win\SDL2-2.0.10\lib\x64\SDL2main.lib lib-win\SDL2-2.0.10\lib\x64\SDL2.lib lib-win\freetype-2.3.5-1-lib\lib\freetype.lib

rem if vcvars not initialized, initialize it (x64 mode)
WHERE cl >nul 2>nul
IF %ERRORLEVEL% NEQ 0 (
  call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall" x64
)

rem if that didn't work, try to initialize msvc 2017
WHERE cl >nul 2>nul
IF %ERRORLEVEL% NEQ 0 (
  call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall" x64
)

rem Compiling glad:
if not exist glad.obj (
cl /c lib/glad-gl3.3/src/glad.c -Ilib/glad-gl3.3/include/ /nologo
)

rem creating bin folder if it doesn't exist
if not exist "bin\" (
mkdir bin
)

rem copying required DLL's to bin folder

rem TODO: for every file in bin\lib-win\dlls that is not in bin\...
rem      copy them to bin\

if not exist "bin\freetype.dll" (
  xcopy lib-win\dlls\freetype.dll bin\ 
)

if not exist "bin\SDL2.dll" (
  xcopy lib-win\dlls\SDL2.dll bin\ 
)


cl %cflags% %incdirs% main.cpp %ldlibs% glad.obj /Fe"bin\tetris.exe" /link %ldflags% 

