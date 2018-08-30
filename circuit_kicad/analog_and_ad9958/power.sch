EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:balmer
LIBS:analog_and_ad9958-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 4
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L LM7805CT U14
U 1 1 58725B0E
P 2300 1200
F 0 "U14" H 2100 1400 50  0000 C CNN
F 1 "LM7805CT" H 2300 1400 50  0000 L CNN
F 2 "Power_Integrations:TO-220" H 2300 1300 50  0001 C CIN
F 3 "" H 2300 1200 50  0000 C CNN
	1    2300 1200
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P3
U 1 1 58726029
P 1000 1750
F 0 "P3" H 1000 1900 50  0000 C CNN
F 1 "+9V" V 1100 1750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02" H 1000 1750 50  0001 C CNN
F 3 "" H 1000 1750 50  0000 C CNN
	1    1000 1750
	-1   0    0    -1  
$EndComp
$Comp
L SG053-SZ U15
U 1 1 5872686D
P 2500 3200
F 0 "U15" H 2550 3400 60  0000 C CNN
F 1 "SG053-SZ" H 2550 3050 60  0000 C CNN
F 2 "balmer:SG053-SZ" H 2500 3200 60  0001 C CNN
F 3 "" H 2500 3200 60  0001 C CNN
	1    2500 3200
	1    0    0    -1  
$EndComp
$Comp
L Earth #PWR096
U 1 1 58726CE8
P 1250 1850
F 0 "#PWR096" H 1250 1600 50  0001 C CNN
F 1 "Earth" H 1250 1700 50  0001 C CNN
F 2 "" H 1250 1850 50  0000 C CNN
F 3 "" H 1250 1850 50  0000 C CNN
	1    1250 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 1800 1250 1800
Wire Wire Line
	1250 1800 1250 1850
$Comp
L +9V #PWR097
U 1 1 58726D12
P 1250 1650
F 0 "#PWR097" H 1250 1500 50  0001 C CNN
F 1 "+9V" H 1250 1790 50  0000 C CNN
F 2 "" H 1250 1650 50  0000 C CNN
F 3 "" H 1250 1650 50  0000 C CNN
	1    1250 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 1700 1250 1700
Wire Wire Line
	1250 1700 1250 1650
$Comp
L +9V #PWR098
U 1 1 587272F2
P 1800 1100
F 0 "#PWR098" H 1800 950 50  0001 C CNN
F 1 "+9V" H 1800 1240 50  0000 C CNN
F 2 "" H 1800 1100 50  0000 C CNN
F 3 "" H 1800 1100 50  0000 C CNN
	1    1800 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 1150 1800 1150
Wire Wire Line
	1800 1150 1800 1100
$Comp
L Earth #PWR099
U 1 1 58727313
P 2300 1500
F 0 "#PWR099" H 2300 1250 50  0001 C CNN
F 1 "Earth" H 2300 1350 50  0001 C CNN
F 2 "" H 2300 1500 50  0000 C CNN
F 3 "" H 2300 1500 50  0000 C CNN
	1    2300 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 1450 2300 1500
$Comp
L C C82
U 1 1 587277B4
P 2800 1300
F 0 "C82" H 2825 1400 50  0000 L CNN
F 1 "100n" H 2825 1200 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 2838 1150 50  0001 C CNN
F 3 "" H 2800 1300 50  0000 C CNN
	1    2800 1300
	1    0    0    -1  
$EndComp
$Comp
L CP C84
U 1 1 58727A55
P 3100 1300
F 0 "C84" H 3125 1400 50  0000 L CNN
F 1 "220u" H 3125 1200 50  0000 L CNN
F 2 "Capacitors_SMD:c_elec_6.3x4.5" H 3138 1150 50  0001 C CNN
F 3 "" H 3100 1300 50  0000 C CNN
	1    3100 1300
	1    0    0    -1  
$EndComp
Connection ~ 2800 1150
Connection ~ 2800 1450
Connection ~ 2300 1450
$Comp
L +5VA #PWR0100
U 1 1 58728DE3
P 3100 1100
F 0 "#PWR0100" H 3100 950 50  0001 C CNN
F 1 "+5VA" H 3100 1240 50  0000 C CNN
F 2 "" H 3100 1100 50  0000 C CNN
F 3 "" H 3100 1100 50  0000 C CNN
	1    3100 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 1150 3100 1100
Connection ~ 3100 1150
$Comp
L C C88
U 1 1 5872A3DB
P 4250 2100
F 0 "C88" H 4275 2200 50  0000 L CNN
F 1 "100n" H 4275 2000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 4288 1950 50  0001 C CNN
F 3 "" H 4250 2100 50  0000 C CNN
	1    4250 2100
	1    0    0    -1  
$EndComp
$Comp
L CP C89
U 1 1 5872A434
P 4550 2100
F 0 "C89" H 4575 2200 50  0000 L CNN
F 1 "220u" H 4575 2000 50  0000 L CNN
F 2 "Capacitors_SMD:c_elec_6.3x4.5" H 4588 1950 50  0001 C CNN
F 3 "" H 4550 2100 50  0000 C CNN
	1    4550 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 1450 3100 1450
Wire Wire Line
	3100 1150 2700 1150
$Comp
L Earth #PWR0101
U 1 1 5872A7B1
P 4250 1900
F 0 "#PWR0101" H 4250 1650 50  0001 C CNN
F 1 "Earth" H 4250 1750 50  0001 C CNN
F 2 "" H 4250 1900 50  0000 C CNN
F 3 "" H 4250 1900 50  0000 C CNN
	1    4250 1900
	1    0    0    1   
$EndComp
Connection ~ 4250 1950
Wire Wire Line
	3800 2250 4850 2250
Connection ~ 4250 2250
Connection ~ 4550 2250
$Comp
L +9V #PWR0102
U 1 1 5872BCD3
P 2050 3050
F 0 "#PWR0102" H 2050 2900 50  0001 C CNN
F 1 "+9V" H 2050 3190 50  0000 C CNN
F 2 "" H 2050 3050 50  0000 C CNN
F 3 "" H 2050 3050 50  0000 C CNN
	1    2050 3050
	1    0    0    -1  
$EndComp
$Comp
L Earth #PWR0103
U 1 1 5872BCF6
P 2050 3300
F 0 "#PWR0103" H 2050 3050 50  0001 C CNN
F 1 "Earth" H 2050 3150 50  0001 C CNN
F 2 "" H 2050 3300 50  0000 C CNN
F 3 "" H 2050 3300 50  0000 C CNN
	1    2050 3300
	1    0    0    -1  
$EndComp
$Comp
L Earth #PWR0104
U 1 1 5872BD19
P 3250 3450
F 0 "#PWR0104" H 3250 3200 50  0001 C CNN
F 1 "Earth" H 3250 3300 50  0001 C CNN
F 2 "" H 3250 3450 50  0000 C CNN
F 3 "" H 3250 3450 50  0000 C CNN
	1    3250 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 3250 2050 3250
Wire Wire Line
	2050 3250 2050 3300
Wire Wire Line
	2100 3100 2050 3100
Wire Wire Line
	2050 3100 2050 3050
Wire Wire Line
	3050 3250 3050 3400
$Comp
L CP C87
U 1 1 5872C36E
P 3600 3250
F 0 "C87" H 3625 3350 50  0000 L CNN
F 1 "33u" H 3625 3150 50  0000 L CNN
F 2 "Capacitors_SMD:c_elec_6.3x4.5" H 3638 3100 50  0001 C CNN
F 3 "" H 3600 3250 50  0000 C CNN
	1    3600 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 3400 4050 3400
Wire Wire Line
	3250 3400 3250 3450
Wire Wire Line
	3600 3100 3600 3050
Connection ~ 3600 3100
$Comp
L LM2663 U16
U 1 1 5872E191
P 2600 2100
F 0 "U16" H 2600 2350 60  0000 C CNN
F 1 "LM2663" H 2600 1850 60  0000 C CNN
F 2 "SMD_Packages:SOIC-8-N" H 2550 2050 60  0001 C CNN
F 3 "" H 2550 2050 60  0001 C CNN
	1    2600 2100
	1    0    0    -1  
$EndComp
$Comp
L CP C81
U 1 1 5872E73E
P 1700 2200
F 0 "C81" H 1725 2300 50  0000 L CNN
F 1 "47u 10V" H 1725 2100 50  0000 L CNN
F 2 "Capacitors_Tantalum_SMD:TantalC_SizeB_EIA-3528_HandSoldering" H 1738 2050 50  0001 C CNN
F 3 "" H 1700 2200 50  0000 C CNN
	1    1700 2200
	1    0    0    -1  
$EndComp
$Comp
L Earth #PWR0105
U 1 1 5872EC4E
P 2000 2150
F 0 "#PWR0105" H 2000 1900 50  0001 C CNN
F 1 "Earth" H 2000 2000 50  0001 C CNN
F 2 "" H 2000 2150 50  0000 C CNN
F 3 "" H 2000 2150 50  0000 C CNN
	1    2000 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 2150 2000 2150
Wire Wire Line
	1700 2350 2150 2350
Wire Wire Line
	2150 2350 2150 2250
Wire Wire Line
	2150 2050 1700 2050
Wire Wire Line
	2000 2150 2000 1950
Wire Wire Line
	2000 1950 2150 1950
Connection ~ 2000 2150
$Comp
L CP C86
U 1 1 5872F37A
P 3350 2600
F 0 "C86" H 3375 2700 50  0000 L CNN
F 1 "47u 10V" H 3375 2500 50  0000 L CNN
F 2 "Capacitors_Tantalum_SMD:TantalC_SizeB_EIA-3528_HandSoldering" H 3388 2450 50  0001 C CNN
F 3 "" H 3350 2600 50  0000 C CNN
	1    3350 2600
	-1   0    0    1   
$EndComp
$Comp
L Earth #PWR0106
U 1 1 5872FA1F
P 3100 2800
F 0 "#PWR0106" H 3100 2550 50  0001 C CNN
F 1 "Earth" H 3100 2650 50  0001 C CNN
F 2 "" H 3100 2800 50  0000 C CNN
F 3 "" H 3100 2800 50  0000 C CNN
	1    3100 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 2250 3500 2250
$Comp
L C C83
U 1 1 5873001B
P 2850 2600
F 0 "C83" H 2875 2700 50  0000 L CNN
F 1 "100n" H 2875 2500 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 2888 2450 50  0001 C CNN
F 3 "" H 2850 2600 50  0000 C CNN
	1    2850 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 2450 3350 2450
Wire Wire Line
	2850 2750 3350 2750
Wire Wire Line
	3100 2800 3100 2750
Connection ~ 3100 2750
Wire Wire Line
	3100 2450 3100 2250
Connection ~ 3100 2250
Connection ~ 3100 2450
Connection ~ 3600 3400
$Comp
L -5VA #PWR108
U 1 1 58734A98
P 4850 2250
F 0 "#PWR108" H 4850 2350 50  0001 C CNN
F 1 "-5VA" H 4850 2400 50  0000 C CNN
F 2 "" H 4850 2250 50  0000 C CNN
F 3 "" H 4850 2250 50  0000 C CNN
	1    4850 2250
	1    0    0    -1  
$EndComp
$Comp
L LM117K U13
U 1 1 58746E70
P 2550 4000
F 0 "U13" H 2350 4200 50  0000 C CNN
F 1 "HT7333" H 2550 4200 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT89-3_Housing" H 2550 4100 50  0001 C CIN
F 3 "" H 2550 4000 50  0000 C CNN
	1    2550 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 3950 2050 3950
Wire Wire Line
	2050 3950 2050 3900
Text GLabel 2950 3950 2    60   Input ~ 0
3.3VA
Wire Wire Line
	3050 3250 3000 3250
Wire Wire Line
	3000 3100 4050 3100
$Comp
L +3V3 #PWR0107
U 1 1 587417BE
P 3600 3050
F 0 "#PWR0107" H 3600 2900 50  0001 C CNN
F 1 "+3V3" H 3600 3190 50  0000 C CNN
F 2 "" H 3600 3050 50  0000 C CNN
F 3 "" H 3600 3050 50  0000 C CNN
	1    3600 3050
	1    0    0    -1  
$EndComp
$Comp
L +5VA #PWR0108
U 1 1 587424B0
P 2050 3900
F 0 "#PWR0108" H 2050 3750 50  0001 C CNN
F 1 "+5VA" H 2050 4040 50  0000 C CNN
F 2 "" H 2050 3900 50  0000 C CNN
F 3 "" H 2050 3900 50  0000 C CNN
	1    2050 3900
	1    0    0    -1  
$EndComp
$Comp
L Earth #PWR0109
U 1 1 5874292C
P 2550 4300
F 0 "#PWR0109" H 2550 4050 50  0001 C CNN
F 1 "Earth" H 2550 4150 50  0001 C CNN
F 2 "" H 2550 4300 50  0000 C CNN
F 3 "" H 2550 4300 50  0000 C CNN
	1    2550 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 4250 2550 4300
$Comp
L CP2102 U18
U 1 1 58781814
P 7850 2700
F 0 "U18" H 7850 2600 50  0000 C CNN
F 1 "CP2102" H 7850 2800 50  0000 C CNN
F 2 "balmer:MLP-28" H 7850 2700 60  0001 C CNN
F 3 "" H 7850 2700 60  0001 C CNN
	1    7850 2700
	1    0    0    -1  
$EndComp
$Comp
L C C92
U 1 1 58781B2F
P 7650 1700
F 0 "C92" H 7675 1800 50  0000 L CNN
F 1 "1u" H 7675 1600 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 7688 1550 50  0001 C CNN
F 3 "" H 7650 1700 50  0000 C CNN
	1    7650 1700
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X04 P4
U 1 1 58781FBC
P 5900 2300
F 0 "P4" H 5900 2550 50  0000 C CNN
F 1 "USB" V 6000 2300 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04" H 5900 2300 50  0001 C CNN
F 3 "" H 5900 2300 50  0000 C CNN
	1    5900 2300
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6500 2250 6100 2250
Wire Wire Line
	6500 2350 6100 2350
$Comp
L Earth #PWR0110
U 1 1 58782B42
P 6150 2500
F 0 "#PWR0110" H 6150 2250 50  0001 C CNN
F 1 "Earth" H 6150 2350 50  0001 C CNN
F 2 "" H 6150 2500 50  0000 C CNN
F 3 "" H 6150 2500 50  0000 C CNN
	1    6150 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 2450 6150 2450
Wire Wire Line
	6150 2450 6150 2500
$Comp
L C C93
U 1 1 58782F1A
P 8700 1700
F 0 "C93" H 8725 1800 50  0000 L CNN
F 1 "100n" H 8725 1600 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 8738 1550 50  0001 C CNN
F 3 "" H 8700 1700 50  0000 C CNN
	1    8700 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 2150 6500 2150
Wire Wire Line
	6500 2150 6500 1550
Wire Wire Line
	8200 1550 8200 1750
Connection ~ 7650 1550
Wire Wire Line
	8300 1550 8300 1750
Connection ~ 8200 1550
$Comp
L Earth #PWR0111
U 1 1 587830C7
P 7650 1850
F 0 "#PWR0111" H 7650 1600 50  0001 C CNN
F 1 "Earth" H 7650 1700 50  0001 C CNN
F 2 "" H 7650 1850 50  0000 C CNN
F 3 "" H 7650 1850 50  0000 C CNN
	1    7650 1850
	1    0    0    -1  
$EndComp
$Comp
L Earth #PWR0112
U 1 1 5878317E
P 8700 1850
F 0 "#PWR0112" H 8700 1600 50  0001 C CNN
F 1 "Earth" H 8700 1700 50  0001 C CNN
F 2 "" H 8700 1850 50  0000 C CNN
F 3 "" H 8700 1850 50  0000 C CNN
	1    8700 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	8400 1750 8400 1550
Text Label 6100 2150 0    60   ~ 0
+5VUSB
$Comp
L Earth #PWR0113
U 1 1 587839A3
P 7300 3650
F 0 "#PWR0113" H 7300 3400 50  0001 C CNN
F 1 "Earth" H 7300 3500 50  0001 C CNN
F 2 "" H 7300 3650 50  0000 C CNN
F 3 "" H 7300 3650 50  0000 C CNN
	1    7300 3650
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X04 P6
U 1 1 587842FB
P 10250 2500
F 0 "P6" H 10250 2750 50  0000 C CNN
F 1 "UART" V 10350 2500 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04" H 10250 2500 50  0001 C CNN
F 3 "" H 10250 2500 50  0000 C CNN
	1    10250 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	9200 2450 9700 2450
Wire Wire Line
	9200 2550 9800 2550
$Comp
L Earth #PWR0114
U 1 1 5878455C
P 9650 2700
F 0 "#PWR0114" H 9650 2450 50  0001 C CNN
F 1 "Earth" H 9650 2550 50  0001 C CNN
F 2 "" H 9650 2700 50  0000 C CNN
F 3 "" H 9650 2700 50  0000 C CNN
	1    9650 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	9650 2650 10050 2650
Wire Wire Line
	9650 2650 9650 2700
Wire Wire Line
	9300 2350 10050 2350
Connection ~ 8700 1550
Text Label 9300 2350 0    60   ~ 0
+3V3USB
Wire Wire Line
	9300 1550 9300 2350
Wire Wire Line
	8400 1550 9300 1550
Wire Wire Line
	6500 1550 8300 1550
Text Label 6150 2250 0    60   ~ 0
D+
Text Label 6150 2350 0    60   ~ 0
D-
Text Label 9400 2450 0    60   ~ 0
TX
Text Label 9400 2550 0    60   ~ 0
RX
Connection ~ 3250 3400
Wire Wire Line
	9800 2550 9800 2450
Wire Wire Line
	9800 2450 10050 2450
Wire Wire Line
	9700 2450 9700 2500
Wire Wire Line
	9700 2500 9850 2500
Wire Wire Line
	9850 2500 9850 2550
Wire Wire Line
	9850 2550 10050 2550
Wire Wire Line
	4250 1900 4250 1950
Wire Wire Line
	4250 1950 4550 1950
$Comp
L R R60
U 1 1 5962890B
P 3650 2250
F 0 "R60" V 3730 2250 50  0000 C CNN
F 1 "4.7" V 3650 2250 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 3580 2250 50  0001 C CNN
F 3 "" H 3650 2250 50  0001 C CNN
	1    3650 2250
	0    1    1    0   
$EndComp
$Comp
L LM7805CT U17
U 1 1 59628D4D
P 4050 1200
F 0 "U17" H 3850 1400 50  0000 C CNN
F 1 "LM7805CT" H 4050 1400 50  0000 L CNN
F 2 "Power_Integrations:TO-220" H 4050 1300 50  0001 C CIN
F 3 "" H 4050 1200 50  0000 C CNN
	1    4050 1200
	1    0    0    -1  
$EndComp
$Comp
L +9V #PWR0115
U 1 1 59628DE8
P 3550 1100
F 0 "#PWR0115" H 3550 950 50  0001 C CNN
F 1 "+9V" H 3550 1240 50  0000 C CNN
F 2 "" H 3550 1100 50  0000 C CNN
F 3 "" H 3550 1100 50  0000 C CNN
	1    3550 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 1150 3550 1150
Wire Wire Line
	3550 1150 3550 1100
$Comp
L Earth #PWR0116
U 1 1 59628E9A
P 4050 1500
F 0 "#PWR0116" H 4050 1250 50  0001 C CNN
F 1 "Earth" H 4050 1350 50  0001 C CNN
F 2 "" H 4050 1500 50  0000 C CNN
F 3 "" H 4050 1500 50  0000 C CNN
	1    4050 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 1500 4050 1450
$Comp
L C C95
U 1 1 59628F4E
P 4550 1300
F 0 "C95" H 4575 1400 50  0000 L CNN
F 1 "100n" H 4575 1200 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 4588 1150 50  0001 C CNN
F 3 "" H 4550 1300 50  0000 C CNN
	1    4550 1300
	1    0    0    -1  
$EndComp
$Comp
L CP C96
U 1 1 59628FD8
P 4900 1300
F 0 "C96" H 4925 1400 50  0000 L CNN
F 1 "47u 10V" H 4925 1200 50  0000 L CNN
F 2 "Capacitors_Tantalum_SMD:TantalC_SizeB_EIA-3528_HandSoldering" H 4938 1150 50  0001 C CNN
F 3 "" H 4900 1300 50  0000 C CNN
	1    4900 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 1150 5300 1150
Connection ~ 4550 1150
Wire Wire Line
	4050 1450 4900 1450
Connection ~ 4550 1450
Connection ~ 4050 1450
Wire Wire Line
	5300 1150 5300 1750
Wire Wire Line
	5300 1750 3000 1750
Wire Wire Line
	3000 1750 3000 1950
Connection ~ 4900 1150
$Comp
L CONN_01X04 P5
U 1 1 59629F0A
P 4250 3150
F 0 "P5" H 4250 3400 50  0000 C CNN
F 1 "POUT" V 4350 3150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04" H 4250 3150 50  0001 C CNN
F 3 "" H 4250 3150 50  0000 C CNN
	1    4250 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 3400 4050 3200
Connection ~ 4050 3300
$Comp
L +5VA #PWR0117
U 1 1 5962A7DF
P 4000 2950
F 0 "#PWR0117" H 4000 2800 50  0001 C CNN
F 1 "+5VA" H 4000 3090 50  0000 C CNN
F 2 "" H 4000 2950 50  0000 C CNN
F 3 "" H 4000 2950 50  0000 C CNN
	1    4000 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 2950 4000 3000
Wire Wire Line
	4000 3000 4050 3000
$EndSCHEMATC
