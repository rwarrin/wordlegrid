@echo off

SET ProjectPath=%cd:~0,-15%

SUBST Y: /D
SUBST Y: %ProjectPath%

pushd Y:

call emsdk_env.bat
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

start D:\Development\Vim\Vim90\gvim.exe

echo Shell Started

