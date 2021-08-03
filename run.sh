#!/bin/bash

java/java -jar aquecedor2008_1.jar 12345 &
make
path=$(pwd)
${path}/controller localhost 12345