## Prerequisites
- Make
- MinGW (or some other C++ compiler, using MySyS on windows)
- Cmake

## Recomended
- Clangd
    - Run with arguments:
        - `--enable-config`
        - `-log=verbose`
        - `--background-index`
- clang-format
    - Installed on windows using LLVM

# Included libraries
- (dr_mp3)[https://github.com/mackron/dr_libs/blob/master/dr_mp3.h]
- (miniaudio)[https://miniaud.io/]
- (spline)[https://github.com/ttk592/spline]
- (nlohmann/json)[https://github.com/nlohmann/json#quick-reference]