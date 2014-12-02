#!/bin/bash

rm *.{dot,png}

./big1

./check1

./compare1 ; display compare1-action.png
./compare2 ; display compare2-action.png
./compare3 ; display compare3-action.png
./compare4 ; display compare4-action.png
./compare5 ; display compare5-action.png
./compare6 ; display compare6-action.png

./copy1
./copy2

./find1

./test1 ; display test1.png
./test2

