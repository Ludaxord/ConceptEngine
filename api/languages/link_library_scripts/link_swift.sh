# Linking Swift with C++
# Sources:
# https://gist.github.com/HiImJulien/c79f07a8a619431b88ea33cca51de787
# TODO: Add Swift Library to CMakeLists.txt
# show working directory
echo "working directory:"
pwd
cd ../../../
pwd
#build swift library
swiftc api/languages/bridges/AppleAPISwiftBridge.swift -emit-library -o bin/apple_swift_lib.dylib
#build C++ executable from main.cpp
gcc -std=c++17 -c main.cpp -o bin/main.o
#link swift library with C++
gcc bin/main.o bin/apple_swift_lib.dylib -lstdc++ -o bin/concept_engine