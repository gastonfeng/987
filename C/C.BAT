set C51INC=e:\c51\inc
set C51LIB=e:\c51\lib
\c51\bin\c51 2000.c large ot(0,speed)
\c51\bin\c51 sbuf.c large ot(0,speed)
\c51\bin\l51 2000.obj,sbuf.obj to 2000.abs XDATA(8000H) NOOVERLAY
\3h\oh 2000.abs
rem \3h\cvf51 2000.abs