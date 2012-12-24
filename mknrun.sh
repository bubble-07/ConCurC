#!/bin/sh

gcc lexer.c parser.c primorder.c main.c
echo "Ready!"
./a.out
