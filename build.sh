#!/bin/bash


bison -y -d lure.y; flex lure.l; gcc y.tab.c lex.yy.c -o lure
