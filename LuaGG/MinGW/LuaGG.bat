C:\MinGW\bin\windres.exe "../Resources.rc" "../MingW/Resources.o"
C:\MinGW\bin\g++.exe "../AboutDlg.cpp" "../ChangeStatus.cpp" "../Configuration.cpp" "../Connection.cpp" "../LuaGG.cpp" "../LUAScript.cpp" "../MainWindow.cpp" "../Protocol.cpp" "../ScriptManager.cpp" "../Threads.cpp" "../Tools.cpp" "../WinGUI.cpp" -lcomctl32 -lshlwapi -lpsapi -lws2_32 -llua5.1 -lxml2 -lboost_regex -lboost_filesystem -lboost_system -lcrypto -O3 -s -lkernel32 -luser32 -lshell32 -mwindows -o "../MingW/LuaGG.exe" "../MingW/Resources.o"
DEL "../MingW/Resources.o"
PAUSE