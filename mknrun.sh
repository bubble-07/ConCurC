#!/bin/sh

gcc lexer.c parser.c primorder.c main.c
./a.out test.txt
