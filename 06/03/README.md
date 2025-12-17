```bash
$ cd ../../
$ cmake -S . -B build -G Ninja 
$ cmake --build build --target rational_06_01
$ cmake --build build --target rational_06_02
$ cd build
$ python3.11 ../script.py
```