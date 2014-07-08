#!/bin/sh

gcc -g typetest.c passes/*.c prims/*.c
./a.out test.txt
