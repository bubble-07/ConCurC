#!/bin/sh

gcc -g main4.c passes/*.c prims/*.c
./a.out test3.txt
