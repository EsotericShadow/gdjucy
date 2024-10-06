@echo off
cmake -S . -B ./build 
cmake --build ./build 
REM * for Relaase Build * 
REM cmake --build ./build --config Release

if %ERRORLEVEL% equ 0 (
    copy /Y "build\gdjucy\Debug\gdjucy.dll" "godot_test_project\bin\"
) else (
    echo "Build Error!!"
)

REM -- MEMO ---
REM copy /Y "build/gameplay/Debug/gameplay.dll" "godot_test_project/bin/gameplay.dll"
REM "C:\MyData\Works40s\Godot\Godot_v4.3-stable_win64.exe\Godot_v4.3-stable_win64.exe" -e godot_test_project/project.godot
REM "C:\MyData\Works40s\Godot\Godot_v4.3-stable_win64.exe\Godot_v4.3-stable_win64.exe" godot_test_project/project.godot
REM rmdir /S /Q "..\..\Dodge_Sample\.godot\"