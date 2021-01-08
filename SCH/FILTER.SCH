*version 1.0 750090190
lib E:\MSIM\LIB\abm.slb 0
lib E:\MSIM\LIB\analog.slb 0
lib E:\MSIM\LIB\anlg_dev.slb 0
lib E:\MSIM\LIB\bipolar.slb 0
lib E:\MSIM\LIB\breakout.slb 0
lib E:\MSIM\LIB\burr_brn.slb 0
lib E:\MSIM\LIB\cd4000.slb 0
lib E:\MSIM\LIB\dig_1.slb 1709796238
lib E:\MSIM\LIB\dig_2.slb 0
lib E:\MSIM\LIB\dig_3.slb 0
lib E:\MSIM\LIB\dig_4.slb 0
lib E:\MSIM\LIB\dig_ecl.slb 0
lib E:\MSIM\LIB\dig_gal.slb 0
lib E:\MSIM\LIB\dig_misc.slb 0
lib E:\MSIM\LIB\dig_pal.slb 0
lib E:\MSIM\LIB\diode.slb 0
lib E:\MSIM\LIB\europe.slb 0
lib E:\MSIM\LIB\global.slb 0
lib E:\MSIM\LIB\harris.slb 0
lib E:\MSIM\LIB\jfet.slb 0
lib E:\MSIM\LIB\lin_tech.slb 0
lib E:\MSIM\LIB\marker.slb 0
lib E:\MSIM\LIB\misc.slb 0
lib E:\MSIM\LIB\mosfet.slb 0
lib E:\MSIM\LIB\opamp.slb 0
lib E:\MSIM\LIB\opto.slb 0
lib E:\MSIM\LIB\pwrbjt.slb 0
lib E:\MSIM\LIB\source.slb 0
lib E:\MSIM\LIB\special.slb 0
lib E:\MSIM\LIB\swit_rav.slb 0
lib E:\MSIM\LIB\swit_reg.slb 0
lib E:\MSIM\LIB\tex_inst.slb 0
lib E:\MSIM\LIB\thyristr.slb 0
lib E:\MSIM\LIB\xtal.slb 0
u 31
D? 2
C? 2
R? 2
V? 2
@analysis
@attributes
@index
pageloc 1 0 1340 
@status
n 11009 98:10:03:21:32:53;910157573 e 
c 1 98:10:03:21:33:28;910157608 e 
*page 1 0 970 720 iA
@ports
@parts
part  1 D1N4148 170 150 u
a 0 sp 13 0 15 25 hln 100 PART=D1N4148
a 0 a 9 0 15 0 hln 100 refdes=D1
part  2 c 180 190 v
a 0 a 9 0 15 0 hln 100 REFDES=C1
part  3 r 210 200 v
a 0 a 9 0 15 0 hln 100 REFDES=R1
part  4 VPULSE 110 150 h
a 1 u 0 0 0 0 hcn 100 DC=16v
a 1 u 0 0 0 0 hcn 100 td=1ms
a 1 u 0 0 0 0 hcn 100 tr=0.5
a 1 u 0 0 0 0 hcn 100 tf=0.5ms
a 1 x 9 0 20 10 hcn 100 refdes=V1
part  0 titleblk 970 720 h
a 1 s 13 0 350 10 hcn 100 PageSize=A
a 1 s 13 0 180 60 hcn 100 PageTitle=
a 1 s 13 0 300 95 hrn 100 PageNO=1
a 1 s 13 0 340 95 hrn 100 PageCount=1
@conn
w 6
s 110 150 140 150 5
w 10
a 0 s 3 0 0 0 hln 100 LABEL=vout
s 170 150 180 150 9
s 210 150 210 160 12
a 0 s 3 0 212 155 hln 100 LABEL=vout
s 180 150 210 150 19
s 180 150 180 160 15
w 21
a 0 s 3 0 0 0 hln 100 LABEL=0
s 110 190 110 200 20
s 110 200 180 200 23
a 0 s 3 0 145 198 hln 100 LABEL=0
s 180 200 210 200 30
s 180 190 180 200 26
@junction
j 140 150
+ p 1 2
+ w 6
j 110 150
+ p 4 +
+ w 6
j 170 150
+ p 1 1
+ w 10
j 210 160
+ p 3 2
+ w 10
j 180 160
+ p 2 2
+ w 10
j 180 150
+ w 10
+ w 10
j 110 190
+ p 4 -
+ w 21
j 210 200
+ p 3 1
+ w 21
j 180 190
+ p 2 1
+ w 21
j 180 200
+ w 21
+ w 21
@attributes
a 0 s 15 0 0 0 hln 100 PAGETITLE=
a 0 s 15 0 0 0 hln 100 PAGENO=1
a 0 s 15 0 0 0 hln 100 PAGESIZE=A
a 0 s 15 0 0 0 hln 100 PAGECOUNT=1
@text
