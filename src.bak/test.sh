#!/bin/bash

valgrind --tool=memcheck --show-reachable=yes --leak-check=full  ./inspect -p "xml:filename=test.xml"
