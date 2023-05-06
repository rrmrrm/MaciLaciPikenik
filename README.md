# MaciLaciPikenik

## how download build tools for running qt and testing:
```
sudo apt update 
sudo apt upgrade
sudo apt install build-essential
sudo apt install qmake6
sudo apt install qt6-base-dev

```


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
