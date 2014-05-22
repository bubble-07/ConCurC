#!/bin/sh

gcc -pg lexer.c parser.c primorder.c main3.c det_file_depends.c det_depends.c
./a.out test.txt
