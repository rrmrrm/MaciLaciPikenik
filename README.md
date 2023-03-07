# MaciLaciPikenik

## how to install and run on ubuntu:
```
sudo apt update 
sudo apt upgrade
sudo apt install build-essential
```

TODO install qmake if not present.

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
