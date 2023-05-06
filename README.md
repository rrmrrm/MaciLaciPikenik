# MaciLaciPikenik

## how download build tools for running qt and testing:
```
sudo apt update 
sudo apt upgrade
sudo apt install build-essential
sudo apt install qmake6
sudo apt install qt6-base-dev

```
if you installed qt creator there is a chance that a bad qmake toolchain came with it. in this case open Qt Creator, go to Projects menu, click "Manage kits", and modify the toolchain(the Kit): at Qt Versions tab set the path of the qmake (for me the right path is /usr/bin/qmake6 which is a link to/lib/qt6/bin/qmake6).

## how to install and run on ubuntu:

go to ... /MaciLaciPikenik/
```
mkdir build
cd build

qmake6 -makefile ..
make
```

to run:

```
./untitled
```

## how to build and run test
go to ... /MaciLaciPikenik/
```
mkdir buildTest
cd buildTest
```
```
qmake6 ../test.pro
make
```

To run the test:
```
./test
```

## notes
apparently some QImages can't be draw if they are defined in the global space like in a namespace or as a static class member.
Specifically:
according to this post
https://stackoverflow.com/a/64863927/19251074
qt resources(image file in my case) cant be loaded at the time that static storage duration variables (a static QImage in my case) are initialized
