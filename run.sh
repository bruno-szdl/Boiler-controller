#!/bin/bash

java -jar aquecedor2008_1.jar 12345 &
make
path=$(pwd)
${path}/bin/controller localhost 12345