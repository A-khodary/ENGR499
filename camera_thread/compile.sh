#!/bin/bash

mkdir images
mkdir build
cd build
cmake -DCUDA_USE_STATIC_CUDA_RUNTIME=OFF ..
make
