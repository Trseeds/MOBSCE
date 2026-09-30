cls
del Binaries\GAME.exe
make Debug -j8
cd Binaries
gdb GAME
cd ..