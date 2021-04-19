cls
c:\ps2dev\gcc\ee\bin\ee-gcc -march=r4000 -g -mgp32 -mlong32 -O3 -c num16.c
c:\ps2dev\gcc\ee\bin\ee-gcc -march=r4000 -g -mgp32 -mlong32 -O3 -c controller.c
c:\ps2dev\gcc\ee\bin\ee-gcc -march=r4000 -g -mgp32 -mlong32 -O3 -c power.c
c:\ps2dev\gcc\ee\bin\ee-gcc -march=r4000 -g -mgp32 -mlong32 -O3 -c string.c
c:\ps2dev\gcc\ee\bin\ee-gcc -march=r4000 -g -mgp32 -mlong32 -O3 -c schedule.c
c:\ps2dev\gcc\ee\bin\ee-gcc -march=r4000 -g -mgp32 -mlong32 -O3 -c pg.c
c:\ps2dev\gcc\ee\bin\ee-gcc -march=r4000 -g -mgp32 -mlong32 -O3 -c inifile.c
c:\ps2dev\gcc\ee\bin\ee-gcc -march=r4000 -g -mgp32 -mlong32 -O3 -c main.c
c:\ps2dev\gcc\ee\bin\ee-gcc -march=r4000 -g -mgp32 -c -xassembler -O -o startup.o startup.s
c:\ps2dev\gcc\ee\bin\ee-ld -O0 startup.o main.o pg.o controller.o schedule.o string.o inifile.o power.o num16.o -M -Ttext 8900000 -q -o out > main.map
outpatch
C:\PS2Dev\share\elf2pbp.exe outp "PSP Calendar Ver0.3"
