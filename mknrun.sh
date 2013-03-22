#!/bin/sh

gcc lexer.c parser.c primorder.c main.c det_file_depends.c det_depends.c
./a.out test.txt
