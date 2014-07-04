#!/bin/sh

gcc -g main.c passes/*.c prims/*.c
./a.out test.txt
