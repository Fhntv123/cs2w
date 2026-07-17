@echo off
chcp 65001 >nul
title vexium build

:: ─────────────────────────────────────────────────────────────────
::  Путь к MSBuild — меняй если VS установлена в другое место
:: ─────────────────────────────────────────────────────────────────
set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

:: Если не нашли — пробуем Enterprise / Professional
if not exist %MSBUILD% set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
if not exist %MSBUILD% set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"

if not exist %MSBUILD% (
    echo [ERROR] MSBuild не найден. Укажи путь вручную в переменной MSBUILD.
    pause
    exit /b 1
)

:: ─────────────────────────────────────────────────────────────────
::  1. Синхронизация с GitHub
:: ─────────────────────────────────────────────────────────────────
echo.
echo [1/2] Синхронизация с origin/main...
echo.

git fetch origin main
if %errorlevel% neq 0 (
    echo [ERROR] git fetch провалился. Проверь интернет / remote.
    pause
    exit /b 1
)

git reset --hard origin/main
if %errorlevel% neq 0 (
    echo [ERROR] git reset --hard провалился.
    pause
    exit /b 1
)

echo.
echo [OK] Репо синхронизировано с origin/main.
echo.

:: ─────────────────────────────────────────────────────────────────
::  2. Сборка x64 Release
:: ─────────────────────────────────────────────────────────────────
echo [2/2] Сборка x64 Release...
echo.

%MSBUILD% "vexium.sln" /p:Configuration=Release /p:Platform=x64 /m /nologo /verbosity:minimal

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Сборка провалилась. Смотри ошибки выше.
    pause
    exit /b 1
)

echo.
echo [OK] Сборка успешна!
echo.

:: Показываем где лежит DLL
echo Готовый файл:
for /r "output" %%f in (*.dll) do echo   %%f
for /r "x64\Release" %%f in (*.dll) do echo   %%f

echo.
pause
