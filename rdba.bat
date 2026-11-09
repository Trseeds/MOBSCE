cls
del Binaries\GAME.exe
make DebugASM -j8
cd Binaries
gdb GAME
cd ..