#!/bin/sh

gcc lexer.c parser.c primorder.c main.c det_depends.c
./a.out test.txt
