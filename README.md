# 8086 Disassembler

A program for disassembling 8086 machine code binaries.

Pass it a binary file encoded with 8086 machine code and it will output the corresponding assembly instructions.

## Example
Below xlat_lea_lds_les is a binary file containing 8086 encoded machine code instructions.
```
>hexdump.exe xlat_lea_lds_les
0xd7 0x8d 0x81 0x8c 0x05 0x8d 0x5e 0xce 
0x8d 0xa6 0x15 0xfc 0x8d 0x78 0xf9 0xc5
0x81 0x8c 0x05 0xc5 0x5e 0xce 0xc5 0xa6
0x15 0xfc 0xc5 0x78 0xf9 0xc4 0x81 0x8c
0x05 0xc4 0x5e 0xce 0xc4 0xa6 0x15 0xfc
0xc4 0x78 0xf9
```

We run the disassembler with this file as input and it will print out the disassembly:
```
>dis86.exe xlat_lea_lds_les
xlat 
lea ax, [bx + di + 1420]
lea bx, [bp - 50]
lea sp, [bp - 1003]
lea di, [bx + si - 7]
lds ax, [bx + di + 1420]
lds bx, [bp - 50]
lds sp, [bp - 1003]
lds di, [bx + si - 7]
les ax, [bx + di + 1420]
les bx, [bp - 50]
les sp, [bp - 1003]
les di, [bx + si - 7]
```

## Supported Instructions

| Currently supported instructions |
| -------------------------------- |
|         add                      |
|         sub                      |
|         cmp                      |
|         mov                      |
|         adc                      |
|         sbb                      |
|         push                     |
|         pop                      |
|         xchg                     |
|         in                       |
|         out                      |
|         xlat                     |
|         lea                      |
|         lds                      |
|         les                      |
|         lahf                     |
|         sahf                     |
|         pushf                    |
|         popf                     |
|                                  |


## Build Instructions (VSCode)

1. Confirm the CMake and C/C++ extensions are installed on VSCode.
2. Open the command bar and execute the CMake build command. The executable should be output in the `/build/Debug/` directory.

## Build Instructions (CMake)

1. Create a build directory in the project root and run CMake
```
mkdir build
cd build
cmake ..
```
2. Run CMake build in the project root with build as the build directory
```
cmake --build build
```
3. The executable will be generated in `project_root/build/Debug/`

Note: tested on Windows 11 with MSVC (VS 2022) and on Ubuntu Linux.