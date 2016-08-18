#!/bin/bash

function show
{
    dot -Tsvg < $1 > ${1%.*}.svg
    display ${1%.*}.svg
}

rm *.{gv,svg}

./big1

./check1

./compare1 ; show compare1-action.gv
./compare2 ; show compare2-action.gv
./compare3 ; show compare3-action.gv
./compare4 ; show compare4-action.gv
./compare5 ; show compare5-action.gv
./compare6 ; show compare6-action.gv

./copy1
./copy2

./find1

./test1 ; show test1.gv
./test2
