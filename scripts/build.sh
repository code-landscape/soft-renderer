
#!/bin/bash
mkdir -p build
cmake -S . -B build/ -DCMAKE_BUILD_TYPR=Release
cmake --build build/
./build/tests/InOneWeek
