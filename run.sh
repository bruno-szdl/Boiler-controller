#!/bin/bash

java -jar aquecedor2008_1.jar 12345 &
make
make clean
path=$(pwd)
${path}/boiler_controller localhost 12345