
#!/bin/bash
cmake -S . -B build/ && cmake --build build/ --config debug
./build/tests/InOneWeek
