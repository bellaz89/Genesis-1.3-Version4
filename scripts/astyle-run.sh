#!/usr/bin/env bash

BASEDIR=$(dirname "$0")

ASTYLE_ARGS="-A2s4xexgk1W3xyxfjxbcxC85Y" 

astyle $ASTYLE_ARGS "$BASEDIR"/../src/libgenesis13/*/*.cpp
astyle $ASTYLE_ARGS "$BASEDIR"/../src/gencore/*.cpp
astyle $ASTYLE_ARGS "$BASEDIR"/../src/libgenesis13/*/*.h
astyle $ASTYLE_ARGS "$BASEDIR"/../src/gencore/*.cpp
astyle $ASTYLE_ARGS "$BASEDIR/../test"/*.cpp
astyle $ASTYLE_ARGS "$BASEDIR"/../include/libgenesis13/*.h

find . -name "*.orig" -type f -delete
