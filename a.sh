#!/bin/bash

cd

git clone https://github.com/sisoputnfrba/hilolay
git clone https://github.com/sisoputnfrba/linuse-tests-programs
git clone https://github.com/sisoputnfrba/so-commons-library
git clone https://gist.github.com/mesaglio/8b5902a3f2b4a5630926ab5261a490b7

cd tp-2019-2c-SOcorro/planificador/Hilolay/Debug
make
cd 
cd so-commons-library
sudo make install
cd ..
cd hilolay
sudo make
sudo make install
cd ..

cd 8b5902a3f2b4a5630926ab5261a490b7
cp Makefile ../linuse-tests-programs
cd ..

cd linuse-tests-programs
sudo make 

sudo make entrega