cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build -j$(nproc) && ./build/typing_master
