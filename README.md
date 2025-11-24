# lzx
Microsoft LZX library for Windows, Linux and macOS

# Build on Windows (MinGW)
- cd lzx/decoder
- make -f GNUmakefile
- make -f GNUmakefile install
- cd ../encoder
- make -f GNUmakefile
- make -f GNUmakefile install

# Build on Linux (GCC)
- cd lzx/decoder
- make -f GNUmakefile.linux
- sudo make -f GNUmakefile.linux install
- cd ../encoder
- make -f GNUmakefile.linux
- sudo make -f GNUmakefile.linux install

# Build on macOS
- cd lzx/decoder
- make -f GNUmakefile.macos
- sudo make -f GNUmakefile.macos install
- cd ../encoder
- make -f GNUmakefile.macos
- sudo make -f GNUmakefile.macos install
