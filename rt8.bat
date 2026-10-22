cls
del Binaries\*.o
del Binaries\Non-Engine\*.o
del Binaries\GAME.exe
make -j8
del Binaries\*.o
del Binaries\Non-Engine\*.o
cd Binaries
GAME
cd ..