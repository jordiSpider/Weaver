#!/bin/bash

sudo apt update &> /dev/null

# GCC
echo -n "[0/5] Installing gcc ... "

sudo apt install gcc -y &> /dev/null

echo "OK"


# G++
echo -n "[1/5] Installing g++ ... "

sudo apt install g++ -y &> /dev/null

echo "OK"


# Make
echo -n "[2/5] Installing make ... "

sudo apt install make -y &> /dev/null

echo "OK"


# Python3
echo -n "[3/5] Installing python3 ... "

sudo apt install python3 -y &> /dev/null

echo "OK"


# CMake
echo -n "[4/5] Installing CMake, this can take several minutes ... "

version=3.25
build=1

    # OpenSSL
sudo apt install libssl-dev -y &> /dev/null

    # TBB
sudo apt install libtbb-dev -y &> /dev/null

cd ~
wget https://cmake.org/files/v$version/cmake-$version.$build.tar.gz &> /dev/null
tar -xzvf cmake-$version.$build.tar.gz &> /dev/null
cd cmake-$version.$build/

./bootstrap --parallel=$(nproc) &> /dev/null
make -j$(nproc) &> /dev/null
sudo make install &> /dev/null

echo "OK"

echo "[5/5] Finished script"
