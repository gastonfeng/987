*version 1.0 450181721
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
u 323
U? 9
R? 11
I? 2
V? 5
@analysis
.DC 1 0 0 2 0 1
+ 0 0 i1
+ 0 4 253.2u
+ 0 5 355.5u
+ 0 6 1u
+ 1 0 V2
+ 1 4 0
+ 1 5 50
+ 1 6 5
.MC 0 2 2 1 0 2 1 1 1
+0 100
+1 V(out)
+7 10
+8 r
@attributes
@index
pageloc 1 0 3449 
@status
n 11009 98:11:22:22:48:13;914395693 e 
s 0 98:11:22:22:48:13;914395693 e 
a 0 96:03:20:20:55:09;830058909 e 
*page 1 0 970 720 iA
@ports
port  2 AGND 300 210 h
port  11 agnd 140 290 h
port  148 bubble 180 100 h
a 1 s 14 0 0 0 hcn 100 LABEL=+5V
port  150 bubble 470 140 h
a 1 s 14 0 0 0 hcn 100 LABEL=+5V
port  153 agnd 470 180 h
port  174 bubble 140 100 h
a 1 s 14 0 0 0 hcn 100 LABEL=+5V
port  288 agnd 260 120 u
@parts
part  7 r 180 290 v
a 0 x 9 0 15 0 hln 100 REFDES=R2
a 0 u 13 0 15 25 hln 100 value=5.696k
part  8 r 180 140 v
a 0 x 9 0 15 0 hln 100 REFDES=R3
a 0 u 13 0 15 25 hln 100 value=5.504k
part  9 r 140 290 v
a 0 a 9 0 15 0 hln 100 REFDES=R4
a 0 u 13 0 15 25 hln 100 value=10.1666667k
part  70 ISRC 140 110 h
a 1 a 9 0 20 10 hcn 100 refdes=I1
part  110 r 200 160 h
a 0 u 13 0 15 25 hln 100 value=100k
a 0 x 9 0 15 0 hln 100 REFDES=R5
part  111 r 220 200 h
a 0 u 13 0 15 25 hln 100 value=100k
a 0 x 9 0 15 0 hln 100 REFDES=R6
part  155 VSRC 470 140 h
a 1 u 0 0 0 0 hcn 100 DC=+5V
a 1 x 9 0 20 10 hcn 100 refdes=V2
part  182 R 300 250 u
a 0 u 13 0 15 25 hln 100 value=510k
a 1 x 9 0 15 0 hln 100 REFDES=R7
part  278 lm358 260 160 h
a 0 s 13 0 50 60 hcn 100 PART=lm358
a 0 a 13 0 50 10 hcn 100 refdes=U8
part  285 r 260 160 v
a 0 u 13 0 15 25 hln 100 value=510k
a 0 a 9 0 15 0 hln 100 REFDES=R10
part  292 vsrc 410 140 h
a 1 u 0 0 0 0 hcn 100 DC=8v
a 1 x 9 0 20 10 hcn 100 refdes=V4
part  0 titleblk 970 720 h
a 1 s 13 0 350 10 hcn 100 PageSize=A
a 1 s 13 0 180 60 hcn 100 PageTitle=
a 1 s 13 0 300 95 hrn 100 PageNO=1
a 1 s 13 0 340 95 hrn 100 PageCount=1
@conn
w 14
a 0 s 3 0 0 0 hln 100 LABEL=1
s 140 150 140 160 13
s 140 160 140 250 125
a 0 s 3 0 142 205 hln 100 LABEL=1
s 200 160 140 160 121
a 0 s 3 0 170 158 hln 100 LABEL=1
w 24
a 0 s 3 0 165 288 hln 100 LABEL=gnd
s 180 290 140 290 241
a 0 s 3 0 165 288 hln 100 LABEL=gnd
w 39
s 300 100 300 150 84
s 410 140 410 100 298
s 410 100 300 100 301
w 53
a 0 s 3 0 350 178 hln 100 LABEL=out
s 340 180 350 180 141
s 300 250 350 250 311
s 350 180 360 180 316
a 0 s 3 0 350 178 hln 100 LABEL=out
s 350 250 350 180 313
w 118
a 0 s 3 0 0 0 hln 100 LABEL=l+
s 260 160 240 160 117
a 0 s 3 0 250 158 hln 100 LABEL=l+
w 127
a 0 s 3 0 0 0 hln 100 LABEL=l-
s 260 250 260 200 213
a 0 s 3 0 262 225 hln 100 LABEL=l-
w 158
s 140 110 140 100 38
w 250
a 0 s 3 0 0 0 hln 100 LABEL=2
s 180 250 180 200 317
s 180 200 180 140 322
s 220 200 180 200 319
a 0 s 3 0 210 198 hln 100 LABEL=2
w 294
s 410 180 470 180 293
@junction
j 140 290
+ s 11
+ p 9 1
j 140 250
+ p 9 2
+ w 14
j 140 290
+ p 9 1
+ w 24
j 140 290
+ s 11
+ w 24
j 140 150
+ p 70 -
+ w 14
j 140 110
+ p 70 +
+ w 158
j 240 160
+ p 110 2
+ w 118
j 200 160
+ p 110 1
+ w 14
j 140 160
+ w 14
+ w 14
j 470 180
+ p 155 -
+ s 153
j 140 100
+ s 174
+ w 158
j 180 290
+ p 7 1
+ w 24
j 180 140
+ p 8 1
+ w 250
j 180 250
+ p 7 2
+ w 250
j 260 200
+ p 111 2
+ w 127
j 220 200
+ p 111 1
+ w 250
j 260 250
+ p 182 2
+ w 127
j 470 140
+ s 150
+ p 155 +
j 260 200
+ p 278 -
+ p 111 2
j 300 210
+ p 278 V-
+ s 2
j 300 150
+ p 278 V+
+ w 39
j 340 180
+ p 278 5
+ w 53
j 260 160
+ p 278 +
+ w 118
j 260 200
+ p 278 -
+ w 127
j 260 160
+ p 285 1
+ p 278 +
j 260 160
+ p 285 1
+ w 118
j 260 120
+ s 288
+ p 285 2
j 180 100
+ s 148
+ p 8 2
j 470 180
+ p 155 -
+ w 294
j 410 180
+ p 292 -
+ w 294
j 470 180
+ s 153
+ w 294
j 410 140
+ p 292 +
+ w 39
j 300 250
+ p 182 1
+ w 53
j 350 180
+ w 53
+ w 53
j 180 200
+ w 250
+ w 250
@attributes
a 0 s 15 0 0 0 hln 100 PAGETITLE=
a 0 s 15 0 0 0 hln 100 PAGENO=1
a 0 s 15 0 0 0 hln 100 PAGESIZE=A
a 0 s 15 0 0 0 hln 100 PAGECOUNT=1
@text
