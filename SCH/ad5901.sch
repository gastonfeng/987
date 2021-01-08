*version 8.0 446056878
u 63
U? 2
R? 4
I? 2
V? 4
@libraries
@analysis
.DC 1 0 0 2 1 1
+ 0 0 I1
+ 0 4 253.2uA
+ 0 5 355.5uA
+ 0 6 1uA
@targets
@attributes
@translators
a 0 u 13 0 0 0 hln 100 TANGO=PCB
a 0 u 13 0 0 0 hln 100 SCICARDS=PCB
a 0 u 13 0 0 0 hln 100 PROTEL=PCB
a 0 u 13 0 0 0 hln 100 PCBOARDS=PCB
a 0 u 13 0 0 0 hln 100 PCAD=PCB
a 0 u 13 0 0 0 hln 100 PADS=PCB
a 0 u 13 0 0 0 hln 100 ORCAD=PCB
a 0 u 13 0 0 0 hln 100 EDIF=PCB
a 0 u 13 0 0 0 hln 100 CADSTAR=PCB
a 0 u 13 0 0 0 hln 100 PSPICE=PSPICE
a 0 u 13 0 0 0 hln 100 XILINX=XILINX
@setup
unconnectedPins 0
connectViaLabel 0
connectViaLocalLabels 0
NoStim4ExtIFPortsWarnings 1
AutoGenStim4ExtIFPorts 1
@index
pageloc 1 0 3475 
@status
n 0 99:08:17:10:12:36;937534356 e 
s 2832 99:08:17:10:12:38;937534358 e 
*page 1 0 970 720 iA
@ports
@parts
part 2 LM324 270 130 h
a 0 sp 11 0 14 70 hcn 100 PART=LM324
a 0 s 0:13 0 0 0 hln 100 PKGTYPE=DIP14
a 0 s 0:13 0 0 0 hln 100 GATE=A
a 0 a 0:13 0 0 0 hln 100 PKGREF=U1
a 0 ap 9 0 56 8 hcn 100 REFDES=U1A
part 9 VDC 440 120 h
a 1 u 13 0 -11 18 hcn 100 DC=12V
a 0 sp 0 0 22 37 hln 100 PART=VDC
a 0 a 0:13 0 0 0 hln 100 PKGREF=V3
a 1 ap 9 0 24 7 hcn 100 REFDES=V3
part 8 VDC 440 70 h
a 1 u 13 0 -11 18 hcn 100 DC=12V
a 0 sp 0 0 22 37 hln 100 PART=VDC
a 0 a 0:13 0 0 0 hln 100 PKGREF=V2
a 1 ap 9 0 24 7 hcn 100 REFDES=V2
part 7 VDC 110 70 h
a 1 u 13 0 -11 18 hcn 100 DC=5V
a 0 sp 0 0 22 37 hln 100 PART=VDC
a 0 a 0:13 0 0 0 hln 100 PKGREF=V1
a 1 ap 9 0 24 7 hcn 100 REFDES=V1
part 6 IDC 170 240 h
a 1 u 13 0 -9 21 hcn 100 DC=253.2uA
a 0 sp 11 0 0 50 hln 100 PART=IDC
a 0 a 0:13 0 0 0 hln 100 PKGREF=I1
a 1 ap 9 0 20 10 hcn 100 REFDES=I1
part 4 R 250 110 v
a 0 u 13 0 15 25 hln 100 VALUE=7.5k
a 0 sp 0 0 0 10 hlb 100 PART=R
a 0 s 0:13 0 0 0 hln 100 PKGTYPE=RC05
a 0 s 0:13 0 0 0 hln 100 GATE=
a 0 a 0:13 0 0 0 hln 100 PKGREF=R2
a 0 ap 9 0 15 0 hln 100 REFDES=R2
part 5 R 170 110 v
a 0 u 13 0 -10 45 hln 100 VALUE=19.75k
a 0 sp 0 0 0 10 hlb 100 PART=R
a 0 s 0:13 0 0 0 hln 100 PKGTYPE=RC05
a 0 s 0:13 0 0 0 hln 100 GATE=
a 0 a 0:13 0 0 0 hln 100 PKGREF=R3
a 0 ap 9 0 15 0 hln 100 REFDES=R3
part 3 R 290 240 h
a 0 sp 0 0 0 10 hlb 100 PART=R
a 0 s 0:13 0 0 0 hln 100 PKGTYPE=RC05
a 0 s 0:13 0 0 0 hln 100 GATE=
a 0 a 0:13 0 0 0 hln 100 PKGREF=R1
a 0 ap 9 0 15 0 hln 100 REFDES=R1
a 0 u 13 0 15 25 hln 100 VALUE=51k
part 1 titleblk 970 720 h
a 1 s 13 0 350 10 hcn 100 PAGESIZE=A
a 1 s 13 0 180 60 hcn 100 PAGETITLE=
a 1 s 13 0 300 95 hrn 100 PAGENO=1
a 1 s 13 0 340 95 hrn 100 PAGECOUNT=1
@conn
w 11
a 0 up 0:33 0 0 0 hln 100 V=
s 250 110 250 130 10
s 250 130 270 130 12
a 0 up 33 0 260 129 hct 100 V=
w 15
a 0 up 0:33 0 0 0 hln 100 V=
s 170 110 170 240 14
a 0 up 33 0 172 175 hlt 100 V=
s 170 240 250 240 16
s 270 170 250 170 18
s 250 240 290 240 22
s 250 170 250 240 20
w 36
a 0 up 0:33 0 0 0 hln 100 V=
s 310 120 310 70 35
s 310 70 440 70 37
a 0 up 33 0 375 69 hct 100 V=
w 50
a 0 up 0:33 0 0 0 hln 100 V=
s 110 70 170 70 49
a 0 up 33 0 140 69 hct 100 V=
w 32
a 0 up 0:33 0 0 0 hln 100 V=
s 310 180 440 180 31
s 440 180 440 160 33
s 170 280 440 280 59
a 0 up 33 0 305 279 hct 100 V=
s 440 280 440 180 61
w 40
a 0 sr 0 0 0 0 hln 100 LABEL=0
s 360 50 250 50 45
a 0 sr 3 0 305 48 hcn 100 LABEL=0
s 440 120 440 110 39
s 440 110 360 110 41
s 360 110 360 50 43
s 250 50 250 70 47
s 110 110 40 110 51
s 40 110 40 50 53
s 40 50 250 50 55
w 26
a 0 sr 0 0 0 0 hln 100 LABEL=out
a 0 up 0:33 0 0 0 hln 100 V=
s 350 150 380 150 29
a 0 sr 3 0 365 148 hcn 100 LABEL=out
a 0 up 33 0 365 149 hct 100 V=
s 350 150 350 240 25
s 350 240 330 240 27
@junction
j 250 110
+ p 4 1
+ w 11
j 270 130
+ p 2 +
+ w 11
j 170 110
+ p 5 1
+ w 15
j 170 240
+ p 6 +
+ w 15
j 290 240
+ p 3 1
+ w 15
j 270 170
+ p 2 -
+ w 15
j 250 240
+ w 15
+ w 15
j 350 150
+ p 2 OUT
+ w 26
j 330 240
+ p 3 2
+ w 26
j 310 180
+ p 2 V-
+ w 32
j 440 160
+ p 9 -
+ w 32
j 310 120
+ p 2 V+
+ w 36
j 440 70
+ p 8 +
+ w 36
j 440 120
+ p 9 +
+ w 40
j 440 110
+ p 8 -
+ w 40
j 250 70
+ p 4 2
+ w 40
j 110 70
+ p 7 +
+ w 50
j 170 70
+ p 5 2
+ w 50
j 110 110
+ p 7 -
+ w 40
j 250 50
+ w 40
+ w 40
j 170 280
+ p 6 -
+ w 32
j 440 180
+ w 32
+ w 32
@attributes
a 0 s 0:13 0 0 0 hln 100 PAGETITLE=
a 0 s 0:13 0 0 0 hln 100 PAGENO=1
a 0 s 0:13 0 0 0 hln 100 PAGESIZE=A
a 0 s 0:13 0 0 0 hln 100 PAGECOUNT=1
@graphics
