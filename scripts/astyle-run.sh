#!/usr/bin/env bash

ASTYLE_ARGS="-A2s4xexgk1W3xyxfjxbcxC85Y" 

astyle $ASTYLE_ARGS ../src/libgenesis13/*/*.cpp
astyle $ASTYLE_ARGS ../src/libgenesis13/*/cpp/*.cpp
astyle $ASTYLE_ARGS ../src/libgenesis13/*/cuda/*.cpp
astyle $ASTYLE_ARGS ../src/gencore/*.cpp
astyle $ASTYLE_ARGS ../src/libgenesis13/*/*.h
astyle $ASTYLE_ARGS ../src/libgenesis13/*/cpp/*.h
astyle $ASTYLE_ARGS ../src/libgenesis13/*/cuda/*.h
astyle $ASTYLE_ARGS ../src/gencore/*.cpp
astyle $ASTYLE_ARGS ../include/*.h

find . -name "*.orig" -type f -delete
