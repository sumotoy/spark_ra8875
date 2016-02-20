/*
	--------------------------------------------------
	RA8875 LCD/TFT Graphic Controller Driver Library
	--------------------------------------------------
	Version:0.70b11p9
	++++++++++++++++++++++++++++++++++++++++++++++++++
	Written by: Max MC Costa for s.u.m.o.t.o.y
	++++++++++++++++++++++++++++++++++++++++++++++++++
An attemp to create a full featured library support for RA8875 chip from RAiO.
Works with many SPI drived RA8875 Boards included Adafruit, Eastrising(buydisplay).
NOT work if you have a I2C or Parallel based display!

-------------------------------------------------------------------------------------
					>>>>>>>>>>>> About Copyrights <<<<<<<<<<<<<<<
-------------------------------------------------------------------------------------
License:GNU General Public License v3.0

    RA8875 fast SPI library for RAiO SPI RA8875 drived TFT
    Copyright (C) 2014  egidio massimo costa sumotoy (a t) gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------
					     >>>>>>>>>>>> Thanks to <<<<<<<<<<<<<<<
-------------------------------------------------------------------------------------
Teensy 3.1, a tiny MCU but monster of performances at tiny price, Arduino should learn...
Paul Stoffregen, the 'guru' behind many arduino magic, the father of Teensy
Bill Greyman, another 'maestro', greatly inspired many coders
Jnmattern & Marek Buriak for drawArc
Last but not less important the contributors and beta tester of this library:
M.Sandercrock, the experimentalist, MrTom and many others
-------------------------------------------------------------------------------------
				>>>>>>>>>>>> Current version notes <<<<<<<<<<<<<<<
-------------------------------------------------------------------------------------
	0.70b11p8 changes: Fixed some small error (thanks Mr TOM) that affects drawLine
	and drawLineAngle. Initial support for SPARK devices (very early development)
	Fixed a couple of examples.
-------------------------------------------------------------------------------------
				>>>>>>>>>>>>>>>>>>>>> Wiring <<<<<<<<<<<<<<<<<<<<<<<<<
-------------------------------------------------------------------------------------
TFT side	Teensy/Uno
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
CLK:		pin 13 UNO
MOSI:		pin 11 UNO
MISO:		pin 12 UNO
CS:			pin 10 (selectable but note 1*!)
INT:		pin 2  (selectable 2*)
RESET:		pin 9  (selectable and optional 0*)
*(0) In some modules you can leave unconnected but check if it's needed!
*(1) On Teensy3.x not all pin are usable for CS! Read the printed paper
that come with your Teensy3.x for more informations!
*(2) Arduino cannot use any pin for interrupts! In my examples I don't use
interrupts but just reading pin state so you can use any pin :)
-------------------------------------------------------------------------------------
TFT side	Stellaris (LM4F120XL) module=0 (still not checked)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
CLK:		xxx
MOSI:		xxx
MISO:		xxx
CS:			xxx  (selectable)
INT:		xxx  (selectable)
RESET:		xxx  (selectable and optional)

*if you use an [optional] SD card here's connections...
-------------------------------------------------------------------------------------
TFT side	Teensy2/Teensy3/Uno
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
SD CLK:		pin 13 [shared with RA]
SD MOSI:	pin 11 [shared with RA]
SD MISO:	pin 12 [shared with RA]
SD CS:		pin 4  (selectable 3*)
SD CARD ID: pin xx (selectable and optional)
*(3) On Teensy3.x not all pin are usable for CS! 
can be used: 2,6,9,10,15,20,21,22,23
-------------------------------------------------------------------------------------
TFT side	Stellaris (LM4F120XL) module=0 (still not checked)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
SD CLK:		xxx shared
SD MOSI:	xxx shared
SD MISO:	xxx shared
SD CS:		xxx  (selectable)
SD CARD ID: pin xx (selectable and optional)
-------------------------------------------------------------------------------------
TFT side	Teensy3 alternative setup
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
There are special cases on Teensy 3.x since it can be used for audio shield that remap
some pin. Using SPI in that situation will be impossible but you can use the alternative
pins definition feature of Teensy 3!
SD CLK:		pin 13(normal),pin 14(alt)  [shared with RA]
SD MOSI:	pin 11(normal),pin 7(alt) 	[shared with RA]
SD MISO:	pin 12(normal),pin 8(alt) 	[shared with RA]
SD CS:		pin 2  (selectable 3*)
SD CARD ID: pin xx (selectable and optional)
*(3) On Teensy3.x not all pin are usable for CS's! 
can be used: 2,6,9,10,15,20,21,22,23
-------------------------------------------------------------------------------------
TFT side	Particle Spark  (caution, never tested!)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
CLK:		 	A3
MOSI:			A5
MISO:			A4
CS:				A2
INT:		
SDA:			D0
SCL:			D1

Benchmark results for reference --------------------------------------
Screen fill              16635		16585		16561		8326
Test Pixel               60			77			63			53
Test Pixels              39780		52085		41885		35412
Text                     4500		4389		4634		3409
Lines                    96705		101951		97943		55986
Horiz/Vert Lines         66656		72470		67732		42703
Rectangles (outline)     269786		271251		269987		137691
Rectangles (filled)      269498		271415		270002		137858
Circles (filled)         110285		195011		181003		117279
Circles (outline)        109604		191746		177459		115584
Triangles (outline)      24521		26320		24886		14728
Triangles (filled)       64947		66801		65367		35226
Rounded rects (outline)  146391		167477		151142		103027
Rounded rects (filled)   935691		953280		940016		493179


PIN     UNO		MEGA       CD4050     RA8875
SCK      13		52           YES       SCK
MOSI     11		51           YES       MOSI
MISO     12		50            NO       MISO
RST      9		 5           YES       RST
CS       10		53           YES       CS
*/

#ifndef _SPARKRA8875MC_H_
#define _SPARKRA8875MC_H_

//#include "spark_ra8875/_settings/RA8875_CPU_commons.h"
#if defined(ENERGIA)
	#include "Energia.h"
	#undef byte
	#define byte uint8_t
	#if defined(__TM4C129XNCZAD__) || defined(__TM4C1294NCPDT__)//tiva???
		#define NEEDS_SET_MODULE
		#define _FASTCPU
	#elif defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM__)//stellaris first version
		#define NEEDS_SET_MODULE
		#define _FASTCPU
	#elif defined(__MSP430MCU__)//MSP430???
		// don't know
	#elif defined(TMS320F28069)//C2000???
		// don't know
	#elif defined(__CC3200R1M1RGC__)//CC3200???
		// don't know
	#endif
	static uint8_t SPImodule;
	static uint8_t SPDR;
#elif defined(__AVR__)
/* 
--------------------------------------------------------------
			8 BIT AVR BOARDS (UNO,YUN,LEONARDO,ETC.)
	Fully supported (tested)
--------------------------------------------------------------
*/
	#if !defined(_FORCE_PROGMEM__)
		#define _FORCE_PROGMEM__
	#endif
	#define __PRGMTAG_	PROGMEM
	#include "Arduino.h"
	#include <pins_arduino.h>
	#include <math.h>
	#include <avr/pgmspace.h>
#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
/* 
--------------------------------------------------------------
			TEENSY 3, TEENSY 3.1, TEENSY 3.2, TEENSY LC
	Fully supported (tested)
--------------------------------------------------------------
*/
	#define ___TEENSYES
	#define _FASTCPU//It's a fast CPU with a fast SPI
	#include "Arduino.h"
	#include <avr/pgmspace.h>//Teensy3 and AVR arduinos can use pgmspace.h (maybe not needed)
	#if defined(_FORCE_PROGMEM__)
		#undef _FORCE_PROGMEM__//force library not use PROGMEM
		#define PROGMEM __attribute__((section(".progmem.data")))//Mmm... Maybe not needed
	#endif
	#define __PRGMTAG_	
#elif defined(__32MX320F128H__) || defined(__32MX795F512L__) //chipkit uno, chipkit max
/* 
--------------------------------------------------------------
			CHIPKIT UNO, CHIPKIT MAX
	Partially supported and never tested
	the following defines need some changes!
--------------------------------------------------------------
*/
	#define ___CHIPKIT
	#define _FASTCPU//It's a fast CPU with a fast SPI
	#include "Arduino.h"
	#if defined(_FORCE_PROGMEM__)
		#undef _FORCE_PROGMEM__
	#endif
	#ifndef __PGMSPACE_H_
	  #define __PGMSPACE_H_ 1
	  #define PROGMEM
	  #define PGM_P  const char *
	  #define PSTR(str) (str)
	  #define pgm_read_byte_near(addr) pgm_read_byte(addr)
	  #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
	  #define pgm_read_word(addr) (*(const unsigned short *)(addr))
	#endif
	#define __PRGMTAG_	
#elif defined (__arm__) && defined(ARDUINO_ARCH_SAM)
/* 
--------------------------------------------------------------
			ARDUINO DUE
	Fully supported (tested)
--------------------------------------------------------------
*/
	#define ___DUESTUFF
	#define _FASTCPU//It's a fast CPU with a fast SPI
	#include "Arduino.h"
	#include <pins_arduino.h>
	#ifndef __PGMSPACE_H_
	  #define __PGMSPACE_H_ 1
	  #define PROGMEM
	  #define PGM_P  const char *
	  #define PSTR(str) (str)
	  #define pgm_read_byte_near(addr) pgm_read_byte(addr)
	  #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
	  #define pgm_read_word(addr) (*(const unsigned short *)(addr))
	#endif
	#define __PRGMTAG_	
#elif defined (__arm__) && defined(ARDUINO_ARCH_SAMD)
/* 
--------------------------------------------------------------
			ARDUINO ZERO
	Actually NOT supported
--------------------------------------------------------------
*/
	#include "Arduino.h"
	#include <pins_arduino.h>
	#define ___ZEROSTUFF
	#error "your board it's not supported yet!"
#elif defined (__arm__) && defined(__SAM3X8E__)
/* 
--------------------------------------------------------------
			ARDUINO DUE COMPATIBLE
	Fully supported (as DUE, tested)
--------------------------------------------------------------
*/
	#define ___DUESTUFF
	#define _FASTCPU
	#include "Arduino.h"
	#include <pins_arduino.h>
	#ifndef __PGMSPACE_H_
	  #define __PGMSPACE_H_ 1
	  #define PROGMEM
	  #define PGM_P  const char *
	  #define PSTR(str) (str)
	  #define pgm_read_byte_near(addr) pgm_read_byte(addr)
	  #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
	  #define pgm_read_word(addr) (*(const unsigned short *)(addr))
	#endif
#elif defined(STM32F2XX) || defined(STM32F10X_MD) || defined(STM32_SERIES_F1) || defined(STM32_SERIES_F2)
/* 
--------------------------------------------------------------
			STM32 BOARDS
	Initial support and actually won't compile
--------------------------------------------------------------
*/
	#define ___STM32STUFF
	#include "Arduino.h"
	#if defined(_FORCE_PROGMEM__)
		#undef _FORCE_PROGMEM__
	#endif
	#define __PRGMTAG_	
#elif defined(ESP8266)
/* 
--------------------------------------------------------------
			XTENSA (ESP8266)
	It compiles but never tested
--------------------------------------------------------------
*/
	#include "Arduino.h"
	#include <pins_arduino.h>
	//#define _FASTCPU
	#if defined(_FORCE_PROGMEM__)
		//#undef _FORCE_PROGMEM__
		#define PROGMEM __attribute__((section(".progmem.data")))
	#endif
	#define __PRGMTAG_	
#elif defined (__arm__) && defined(SPARK)
/* 
--------------------------------------------------------------
			PARTICLE PHOTON, ETC.
	Still in development
--------------------------------------------------------------
*/
	#include "application.h"
	#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
	#define pgm_read_byte_near(addr) (*(const unsigned char *)(addr))
	#define pgm_read_word(addr) (*(const unsigned short *)(addr))
	#define pgm_read_word_near(addr) (*(const unsigned short *)(addr))
	#if defined(_FORCE_PROGMEM__)
		#undef _FORCE_PROGMEM__
	#endif
	#undef PROGMEM
	#define PROGMEM __attribute__((section(".progmem.data")))//fix the compiler warning
	#define __PRGMTAG_	
#elif defined(__arm__) && !defined(ESP8266) && !defined(___TEENSYES) && !defined(SPARK) && !defined(STM32F2XX) && !defined(STM32F10X_MD) && !defined(STM32_SERIES_F1) && !defined(STM32_SERIES_F2) && !defined(ESP8266)
/* 
--------------------------------------------------------------
			ARM generic
	Mistery....
--------------------------------------------------------------
*/
	#if defined(_FORCE_PROGMEM__)
		#undef _FORCE_PROGMEM__
	#endif
	#include "Arduino.h"
	#define __PRGMTAG_	
	#warning "Generic Arm detected, not sure if your board it's compatible!"


/* 
--------------------------------------------------------------
			NOT SUPPORTED
--------------------------------------------------------------
*/
#else
	#error "your board it's not supported yet!"
#endif

#include "Print.h"
//end _settings/RA8875_CPU_commons.h

#if !defined(swapvals)
	#if defined(ESP8266)
		#define swapvals(a, b) { int16_t t = a; a = b; b = t; }
		//#define swapvals(a, b) { typeid(a) t = a; a = b; b = t; }
	#else
		#define swapvals(a, b) { typeof(a) t = a; a = b; b = t; }
	#endif
#endif

enum RA8875sizes { 			RA8875_480x272, RA8875_800x480, RA8875_800x480ALT, Adafruit_480x272, Adafruit_800x480 };
enum RA8875tcursor { 		NOCURSOR=0,IBEAM,UNDER,BLOCK };//0,1,2,3
enum RA8875tsize { 			X16=0,X24,X32 };//0,1,2
enum RA8875fontSource { 	INT=0, EXT };//0,1
enum RA8875fontCoding { 	ISO_IEC_8859_1, ISO_IEC_8859_2, ISO_IEC_8859_3, ISO_IEC_8859_4 };
enum RA8875extRomType { 	GT21L16T1W, GT21H16T1W, GT23L16U2W, GT30L16U2W, GT30H24T3Y, GT23L24T3Y, GT23L24M1Z, GT23L32S4W, GT30H32S4W, GT30L32S4W, ER3303_1, ER3304_1, ER3301_1 };
enum RA8875extRomCoding { 	GB2312, GB12345, BIG5, UNICODE, ASCII, UNIJIS, JIS0208, LATIN };
enum RA8875extRomFamily { 	STANDARD, ARIAL, ROMAN, BOLD };
enum RA8875boolean { 		LAYER1, LAYER2, TRANSPARENT, LIGHTEN, OR, AND, FLOATING };
enum RA8875writes { 		L1=0, L2, CGRAM, PATTERN, CURSOR };
enum RA8875scrollMode{ 		SIMULTANEOUS, LAYER1ONLY, LAYER2ONLY, BUFFERED };
enum RA8875pattern{ 		P8X8, P16X16 };
enum RA8875btedatam{ 		CONT, RECT };
enum RA8875btelayer{ 		SOURCE, DEST };
enum RA8875intlist{ 		BTE=1,TOUCH=2, DMA=3,KEY=4 };

/*
-------------- UNICODE decode (2 byte char) ---------------------
Latin:      \u0000 -> \u007F	/u00
Greek:		\u0370 -> \u03FF	/u03
Cyrillic:   \u0400 -> \u04FF	/u04
Hebrew:     \u0590 -> \u05FF	/u05
Arabic: 	\u0600 -> \u06FF	/u06
Hiragana:	\u3040 -> \u309F	/u30
Katakana:   \u30A0 -> \u30FF	/u30
CJK-Uni:	\u4E00 -> \u9FD5	/u4E ... /u9F
*/
/* ----------------------------DO NOT TOUCH ANITHING FROM HERE ------------------------*/

//#include "spark_ra8875/_settings/font.h"
	typedef struct __PRGMTAG_ {
			const uint8_t 	*data;
			uint8_t 		image_width;
			int				image_datalen;
	} tImage;

	typedef struct {
			uint8_t 		char_code;
			const tImage 	*image;
	} tChar;

	typedef struct {
			uint8_t 		length;
			const tChar 	*chars;
			uint8_t			font_width;
			uint8_t			font_height;
			bool 			rle;
	} tFont;
	//end spark_ra8875/_settings/font.h
//#include "spark_ra8875/_settings/RA8875Registers.h"

	#define CENTER 				9998
	#define ARC_ANGLE_MAX 		360		
	#define ARC_ANGLE_OFFSET 	-90	
	#define ANGLE_OFFSET		-90
	
	static const uint8_t _RA8875colorMask[6] = {11,5,0,13,8,3};//for color masking, first 3 byte for 65K
	//initialization parameters---------------------------------------------------------------------
	const static uint8_t initStrings[3][15] = {
	//{0x07,0x03,0x03,0x27,0x00,0x05,0x04,0x03,0xEF,0x00,0x05,0x00,0x0E,0x00,0x02},//0 -> 320x240 (0A)
	{0x07,0x03,0x82,0x3B,0x00,0x01,0x00,0x05,0x0F,0x01,0x02,0x00,0x07,0x00,0x09},//1 -> 480x272 (10)   -> 0
	//{0x07,0x03,0x01,0x4F,0x05,0x0F,0x01,0x00,0xDF,0x01,0x0A,0x00,0x0E,0x00,0x01},//2 -> 640x480
	{0x07,0x03,0x81,0x63,0x00,0x03,0x03,0x0B,0xDF,0x01,0x1F,0x00,0x16,0x00,0x01},//3 -> 800x480        -> 1
	{0x07,0x03,0x81,0x63,0x00,0x03,0x03,0x0B,0xDF,0x01,0x1F,0x00,0x16,0x00,0x01} //4 -> 800x480_ALT    -> 2
	//0    1    2    3    4    5    6    7    8    9    10   11   12   13   14
	};
	/*
	0: - sys clock -
	1: - sys clock -
	2: - sys clock -
	3:LCD Horizontal Display Width
	4:Horizontal Non-Display Period Fine Tuning Option
	5:LCD Horizontal Non-Display Period
	6:HSYNC Start Position
	7:HSYNC Pulse Width
	8:LCD Vertical Display Height 1
	9:LCD Vertical Display Height 2
	10:LCD Vertical Non-Display Period 1
	11:LCD Vertical Non-Display Period 2
	12:VSYNC Start Position Register 1
	13:VSYNC Start Position Register 2
	14:VSYNC Pulse Width Register
	*/
	//PostBurner PLL parameters --------------------------------------------------------------
	const static uint8_t sysClockPar[3][2] = {
	//{0x0B,0x01},//0 -> 320x240		->
	{0x0B,0x01},//1 -> 480x272		    -> 0
	//{0x0B,0x01},//2 -> 640x480			
	{0x0B,0x01},//3 -> 800x480			-> 1
	{0x0B,0x01} //4 -> 800x480_ALT		-> 2
	};
	
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    Font Parameters
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//index:x -> w,h,baselineLowOffset,baselineTopOffset,variableWidth
	const static uint8_t fontDimPar[4][5] = {
		{8,16,2,4,0},// INT font
		{8,16,3,0,0},// ROM X16
		{12,24,2,2,0},//ROM X24
		{16,32,2,2,0},//ROM X32
	};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            System & Configuration Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* Power and Display Control Register [0x01]
----- Bit 7 (LCD Display Off)
0:off, 1:on
----- Bit 6,5,4,3,2, (na)
----- Bit 1 (Sleep Mode)
0:normal, 1:sleep
----- Bit 0 (Software Reset)
0:no action, 1:reset */
#define RA8875_PWRR             		0x01//Power and Display Control Register
	#define RA8875_PWRR_DISPON      0x80
	#define RA8875_PWRR_DISPOFF     0x00
	#define RA8875_PWRR_SLEEP       0x02
	#define RA8875_PWRR_NORMAL      0x00
	#define RA8875_PWRR_SOFTRESET   0x01
/* REG[02h] Memory Read/Write Command (MRWC)
Data to write in memory corresponding to the setting of
MWCR1[3:2]. Continuous data write cycle can be accepted in
bulk data write case.
*/ 
#define RA8875_MRWC             		0x02//Memory Read/Write Command
	#define RA8875_CMDWRITE         	0x80
	#define RA8875_CMDREAD          	0xC0
	#define RA8875_DATAWRITE        	0x00
	#define RA8875_DATAREAD         	0x40
	#define RA8875_STATREG				0x40
/* Pixel Clock Setting Register 	[0x04]
----- Bit 7 (PCLK Inversion)
0:PDAT at PLCK rising , 1:PDAT at PLCK falling
----- Bit 6,5,4,3,2 (na)
----- Bit 1,0 (PCLK Period Setting)
00: PCLK period = System Clock period.
01: PCLK period = 2 times of System Clock period
10: PCLK period = 4 times of System Clock period
11: PCLK period = 8 times of System Clock period*/
#define RA8875_PCSR             	  0x04//Pixel Clock Setting Register
/* Serial Flash/ROM Configuration 	 [0x05]
----- Bit 7 (Serial Flash/ROM I/F # Select)
0:Serial Flash/ROM 0 , 1:Serial Flash/ROM 1
----- Bit 6 (Serial Flash/ROM Address Mode)
0: 24 bits address mode
----- Bit 5 (Serial Flash/ROM Waveform Mode)
----- Bit 4,3 (Serial Flash /ROM Read Cycle)
00: 4 bus -> no dummy cycle
01: 5 bus -> 1 byte dummy cycle
1x: 6 bus -> 2 byte dummy cycle
----- Bit 2 (Serial Flash /ROM Access Mode)
0:Font mode, 1:DMA mode
----- Bit 1,0 (Serial Flash /ROM I/F Data Latch Mode Select)
0x: Single Mode
10: Dual Mode 0
11: Dual Mode 1*/

	#define RA8875_SROC         		  0x05//Serial Flash/ROM Configuration
/* Serial Flash/ROM CLK			     [0x06]
----- Bit 7,6,5,4,3,2 (na) 
----- Bit 1,0 (Serial Flash/ROM Clock Frequency Setting) 
0x: SFCL frequency = System clock frequency(DMA on and 256 clr)
10: SFCL frequency = System clock frequency / 2
11: SFCL frequency = System clock frequency / 4 */
#define RA8875_SFCLR         		  0x06//Serial Flash/ROM CLK
	#define EXTROM_SFCLSPEED	0b00000011// /4 0b00000010 /2
/* System Configuration Register		 [0x10]
----- Bit 7,6,5,4 (na) 
----- Bit 3,2 (Color Depth Setting) 
00: 8-bpp generic TFT, i.e. 256 colors
1x: 16-bpp generic TFT, i.e. 65K colors
----- Bit 1,0 (MCUIF Selection) 
00: 8-bit MCU Interface
1x: 16-bit MCU Interface */
#define RA8875_SYSR             	  0x10//System Configuration Register
/* LCD Horizontal Display Width Register [0x14]
----- Bit 7 (na)
----- Bit 6,5,4,3,2,1,0 (Horizontal Display Width Setting Bit)
no more than 0x64( max with = 800)
note: Horizontal display width(pixels) = (HDWR + 1) * 8 */
#define RA8875_HDWR             	  0x14//LCD Horizontal Display Width Register
/* Horizontal Non-Display Period Fine Tuning Option Register [0x15]
----- Bit 7 (DE polarity)
0:High, 1:Low
----- Bit 6,5,4 (na)
----- Bit 3,2,1,0 (Horizontal Non-Display Period Fine Tuning(HNDFT)) */
#define RA8875_HNDFTR           	  0x15//Horizontal Non-Display Period Fine Tuning Option Register
/* LCD Horizontal Non-Display Period Register [0x16]
----- Bit 7,6,5 (na)
----- Bit 4,0 (HSYNC Start Position)
note: HSYNC Start Position(pixels) = (HSTR + 1) * 8 */
#define RA8875_HNDR             	  0x16//LCD Horizontal Non-Display Period Register
/* HSYNC Start Position Register	 [0x17]
----- Bit 7,6,5 (na)
----- Bit 4,0 (HSYNC Start Position)
note: HSYNC Start Position(pixels) = (HSTR + 1) * 8 */
#define RA8875_HSTR             	  0x17//HSYNC Start Position Register
/* HSYNC Pulse Width Register		 [0x18]
----- Bit 7 (HSYNC Polarity)
0:Low, 1:High
----- Bit 6,5 (na)
----- Bit 4,0 (HSYNC Pulse Width(HPW))
note: HSYNC Pulse Width(pixels) = (HPW + 1) * 8 */
#define RA8875_HPWR             	  0x18//HSYNC Pulse Width Register
#define RA8875_VDHR0            	  0x19//LCD Vertical Display Height Register 0
//#define RA8875_VDHR1            	  0x1A//LCD Vertical Display Height Register 1
#define RA8875_VNDR0            	  0x1B//LCD Vertical Non-Display Period Register 0
//#define RA8875_VNDR1            	  0x1C//LCD Vertical Non-Display Period Register 1
#define RA8875_VSTR0            	  0x1D//VSYNC Start Position Register 0
//#define RA8875_VSTR1            	  0x1E//VSYNC Start Position Register 1
#define RA8875_VPWR             	  0x1F//VSYNC Pulse Width Register
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                           LCD Display Control Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* Display Configuration Register	  [0x20]
----- Bit 7 (Layer Setting Control)
0:one Layer, 1:two Layers
----- Bit 6,5,4 (na)
----- Bit 3 (Horizontal Scan Direction, for n = SEG number)
0: SEG0 to SEG(n-1), 1: SEG(n-1) to SEG0
----- Bit 2 (Vertical Scan direction, for n = COM number)
0: COM0 to COM(n-1), 1: COM(n-1) to COM0
----- Bit 1,0 (na) */
#define RA8875_DPCR				  	  0x20//Display Configuration Register
/* Font Control Register 0			  [0x21]
----- Bit 7 (CGRAM/CGROM Font Selection Bit in Text Mode)
0:CGROM font, 1:CGRAM font
----- Bit 6 (na)
----- Bit 5 (External/Internal CGROM)
0:Internal CGROM (RA8875_SFRSET=0), 1:External CGROM(RA8875_FWTSET bit6,7 = 0)
----- Bit 4,3,2 (na)
----- Bit 1,0 (Font Selection for internal CGROM)
00: ISO/IEC 8859-1
01: ISO/IEC 8859-2
10: ISO/IEC 8859-3
11: ISO/IEC 8859-4 */
#define RA8875_FNCR0				  0x21//Font Control Register 0
/* Font Control Register 1			 [0x22]
----- Bit 7 (Full Alignment)
0:disabled, 1:enabled
----- Bit 6 (Font Transparency)
0:disabled, 1:enabled
----- Bit 5 (na)
----- Bit 4 (Font Rotation)
0:normal, 1:90 degrees
----- Bit 3,2 (Horizontal Font Enlargement)
00:normal, 01:x2, 10:x3, 11:x4
----- Bit 1,0 (Vertical Font Enlargement)
00:normal, 01:x2, 10:x3, 11:x4 */
#define RA8875_FNCR1				  0x22//Font Control Register 1
/* CGRAM Select Register			  [0x23]
----- Bit 7,6,5,4,3,2,1,0 ------------- */
#define RA8875_CGSR				      0x23//CGRAM Select Register
/* Horizontal Scroll Offset Register 0 [0x24]
----- Bit 7,6,5,4,3,2,1,0 ------------- */
#define RA8875_HOFS0				  0x24//Horizontal Scroll Offset Register 0
/* Horizontal Scroll Offset Register 1 [0x25]
----- Bit 7,6,5,4,3 (na) ------------- 
----- Bit 2,0 (Horizontal Display Scroll Offset) */
#define RA8875_HOFS1				  0x25//Horizontal Scroll Offset Register 1
/* Vertical Scroll Offset Register 0 [0x26]
----- Bit 7,6,5,4,3,2,1,0 ------------- */
#define RA8875_VOFS0				  0x26//Vertical Scroll Offset Register 0
/* Vertical Scroll Offset Register 1 [0x27]
----- Bit 7,6,5,4,3,2 (na) ------------- 
----- Bit 1,0 (Vertical Display Scroll Offset) ------------- */
#define RA8875_VOFS1				  0x27//Vertical Scroll Offset Register 1
/* Font Line Distance Setting Register[0x29]
----- Bit 7,6,5 (na) ------------- 
----- Bit 4,0 (Font Line Distance Setting) */
#define RA8875_FLDR				  	  0x29//Font Line Distance Setting Register

/* Font Write Cursor Horizontal Position Register 0 [0x2A]
----- Bit 7,6,5,4,3,2,1,0 ------------- */
#define RA8875_F_CURXL				  0x2A//Font Write Cursor Horizontal Position Register 0
/* Font Write Cursor Horizontal Position Register 1 [0x2B]
----- Bit 7,2 (na) ------------- 
----- Bit 1,0 (Font Write Cursor Horizontal Position) */
#define RA8875_F_CURXH				  0x2B//Font Write Cursor Horizontal Position Register 1
/* Font Write Cursor Vertical Position Register 0 [0x2C]
----- Bit 7,6,5,4,3,2,1,0 ------------- */
#define RA8875_F_CURYL				  0x2C//Font Write Cursor Vertical Position Register 0
/* Font Write Cursor Vertical Position Register 1 [0x2D]
----- Bit 7,1 (na) ------------- 
----- Bit 0 (Font Write Cursor Vertical Position Register 1) */
#define RA8875_F_CURYH				  0x2D//Font Write Cursor Vertical Position Register 1
/* Font Write Type Setting Register [0x2E]
----- Bit 7,6 -------------
00: 16x16(full) - 8x16(half) - nX16(var)
01: 24x24(full) - 12x24(half) - nX24(var)
1x: 32x32(full) - 16x32(half) - nX32(var)
----- Bit 5,0 -------------
Font to Font Width Setting
00 --> 3F (0 to 63 pixels) */
#define RA8875_FWTSET         		  0x2E//Font Write Type Setting Register
/* Serial Font ROM Setting 			 [0x2F]
----- Bit 7,6,5 -------------
000: GT21L16TW / GT21H16T1W
001: GT30L16U2W
010: GT30L24T3Y / GT30H24T3Y
011: GT30L24M1Z
100: GT30L32S4W / GT30H32S4W
----- Bit 4,3,2 -------------
000: GB2312
001: GB12345/GB18030
010: BIG5
011: UNICODE
100: ASCII
101: UNI-Japanese
110: JIS0208
111: Latin/Greek/ Cyrillic / Arabic
----- Bit 1,0 -------------
00: normal(ASCII) - normal(LGC) -    na(arabic)
01: Arial(ASCII) -  var width(LGC) - PresFormA(arabic)
10: Roman(ASCII) -  na(LGC) -        na(arabic) */
#define RA8875_SFRSET         		  0x2F//Serial Font ROM Setting
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    Active Window & Scroll Window Setting Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_HSAW0            	  0x30//Horizontal Start Point 0 of Active Window
//#define RA8875_HSAW1            	  0x31//Horizontal Start Point 1 of Active Window
#define RA8875_VSAW0            	  0x32//Vertical   Start Point 0 of Active Window
//#define RA8875_VSAW1            	  0x33//Vertical   Start Point 1 of Active Window
#define RA8875_HEAW0            	  0x34//Horizontal End   Point 0 of Active Window
//#define RA8875_HEAW1            	  0x35//Horizontal End   Point 1 of Active Window
#define RA8875_VEAW0           		  0x36//Vertical   End   Point of Active Window 0
//#define RA8875_VEAW1            	  0x37//Vertical   End   Point of Active Window 1

#define RA8875_HSSW0            	  0x38//Horizontal Start Point 0 of Scroll Window
//#define RA8875_HSSW1            	  0x39//Horizontal Start Point 1 of Scroll Window
#define RA8875_VSSW0            	  0x3A//Vertical 	 Start Point 0 of Scroll Window
//#define RA8875_VSSW1            	  0x3B//Vertical 	 Start Point 1 of Scroll Window
#define RA8875_HESW0            	  0x3C//Horizontal End   Point 0 of Scroll Window
//#define RA8875_HESW1            	  0x3D//Horizontal End   Point 1 of Scroll Window
#define RA8875_VESW0            	  0x3E//Vertical 	 End   Point 0 of Scroll Window
//#define RA8875_VESW1            	  0x3F//Vertical 	 End   Point 1 of Scroll Window
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    Cursor Setting Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* Memory Write Control Register 0   [0x40]
----- Bit 7 (Select Mode)
0: Graphic Mode
1: Text Mode
----- Bit 6 (Font Write Cursor/ Memory Write Cursor Enable)
0: Font write cursor/ Memory Write Cursor is not visible
1: Font write cursor/ Memory Write Cursor is visible
----- Bit 5 (Font Write Cursor/ Memory Write Cursor Blink Enable)
0: Normal display
1: Blink display
----- Bit 4 (na)
----- Bit 3,2 (Memory Write Direction (Only for Graphic Mode)
00: Left -> Right then Top -> Down
01: Right -> Left then Top -> Down
10: Top -> Down then Left -> Right
11: Down -> Top then Left -> Right
----- Bit 1 (Memory Write Cursor Auto-Increase Disable)
0: Cursor auto-increases when memory write
1: Cursor doesn’t auto-increases when memory write
----- Bit 0(Memory Read Cursor Auto-Increase Disable) 
0: Cursor auto-increases when memory read
1: Cursor doesn’t auto-increases when memory read */
#define RA8875_MWCR0            	  0x40//Memory Write Control Register 0

/* Memory Write Control Register 1   [0x41]
----- Bit 7 (Graphic Cursor Enable)
0:disable, 1:enable
----- Bit 6,5,4 (Graphic Cursor Selection)
000: Graphic Cursor Set 1
...
111: Graphic Cursor Set 8
----- Bit 3,2 (Write Destination Selection)
00: Layer 1~2
01: CGRAM
10: Graphic Cursor
11: Pattern
Note : When CGRAM is selected , RA8875_FNCR0 bit 7 must be set as 0.
----- Bit 1 (na)
----- Bit 0 (Layer No. for Read/Write Selection)
When resolution =< 480x400 or color depth = 8bpp:
0: Layer 1
1: Layer 2
When resolution > 480x400 and color depth > 8bpp:
na */
#define RA8875_MWCR1            	  0x41//Memory Write Control Register 1
/*
from 0 to 255
*/
#define RA8875_BTCR            	  	  0x44//Blink Time Control Register
/* Memory Read Cursor Direction      [0x45]
----- Bit 7,6,5,4,3,2(na)
----- Bit 1,0(Memory Read Direction (Only for Graphic Mode))
00: Left -> Right then Top -> Down
01: Right -> Left then Top -> Down
10: Top -> Down then Left -> Right
11: Down -> Top then Left -> Right */
#define RA8875_MRCD            	  	  0x45//Memory Read Cursor Direction
#define RA8875_CURH0            	  0x46//Memory Write Cursor Horizontal Position Register 0
//#define RA8875_CURH1            	  0x47//Memory Write Cursor Horizontal Position Register 1
#define RA8875_CURV0            	  0x48//Memory Write Cursor Vertical Position Register 0
//#define RA8875_CURV1            	  0x49//Memory Write Cursor Vertical Position Register 1

//#define RA8875_RCURH0           	  0x4A//Memory Read Cursor Horizontal Position Register 0
//#define RA8875_RCURH1           	  0x4B//Memory Read Cursor Horizontal Position Register 1
//#define RA8875_RCURV0           	  0x4C//Memory Read Cursor Vertical Position Register 0
//#define RA8875_RCURV1           	  0x4D//Memory Read Cursor Vertical Position Register 1


/* Font Write Cursor and Memory Write Cursor Horizontal Size  [0x4E]
----- Bit 7,6,5 (na)
----- Bit 4,0(Font Write Cursor Horizontal Size in pixels) */
#define RA8875_CURHS            	  0x4E//Font Write Cursor and Memory Write Cursor Horizontal Size Register
/* Font Write Cursor Vertical Size Register   [0x4F]
----- Bit 7,6,5 (na)
----- Bit 4,0(Font Write Cursor Vertical Size in pixels) */
#define RA8875_CURVS            	  0x4F//Font Write Cursor Vertical Size Register
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//              Block Transfer Engine(BTE) Control Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_BECR0            	  0x50//BTE Function Control Register 0
#define RA8875_BECR1            	  0x51//BTE Function Control Register 1
/* Layer Transparency Register 0     [0x52]
----- Bit 7,6 (Layer1/2 Scroll Mode)
00: Layer 1/2 scroll simultaneously
01: Only Layer 1 scroll
10: Only Layer 2 scroll
11: Buffer scroll (using Layer 2 as scroll buffer)
----- Bit 5 (Floating Windows Transparency Display With BGTR)
0:disable, 1:enable
----- Bit 4,3 (na)
----- Bit 2,1,0(Layer1/2 Display Mode) 
000: Only Layer 1 is visible
001: Only Layer 2 is visible
010: Lighten-overlay mode
011: Transparent mode
100: Boolean OR
101: Boolean AND
110: Floating window mode
111: Reserve */
#define RA8875_LTPR0            	  0x52//Layer Transparency Register 0
/* Layer Transparency Register 1     [0x53]
----- Bit 7,6,5,4 (Layer Transparency Setting for Layer 2)
0000: Total display
0001: 7/8 display
0010: 3/4 display
0011: 5/8 display
0100: 1/2 display
0101: 3/8 display
0110: 1/4 display
0111: 1/8 display
1000: Display disable
----- Bit 3,2,1,0 (Layer Transparency Setting for Layer 1)
0000: Total display
0001: 7/8 display
0010: 3/4 display
0011: 5/8 display
0100: 1/2 display
0101: 3/8 display
0110: 1/4 display
0111: 1/8 display
1000: Display disable */
#define RA8875_LTPR1            	  0x53//Layer Transparency Register 1
#define RA8875_HSBE0				  0x54//Horizontal Source Point 0 of BTE
//#define RA8875_HSBE1				  0x55//Horizontal Source Point 1 of BTE
#define RA8875_VSBE0				  0x56//Vertical Source Point 0 of BTE
//#define RA8875_VSBE1				  0x57//Vertical Source Point 1 of BTE
#define RA8875_HDBE0				  0x58//Horizontal Destination Point 0 of BTE
//#define RA8875_HDBE1				  0x59//Horizontal Destination Point 1 of BTE
#define RA8875_VDBE0				  0x5A//Vertical Destination Point 0 of BTE
//#define RA8875_VDBE1				  0x5B//Vertical Destination Point 1 of BTE
#define RA8875_BEWR0				  0x5C//BTE Width Register 0
//#define RA8875_BEWR1				  0x5D//BTE Width Register 1
#define RA8875_BEHR0				  0x5E//BTE Height Register 0
//#define RA8875_BEHR1				  0x5F//BTE Height Register 1

/* Pattern Set No for BTE            [0x66]
----- Bit 7 (Pattern Format)
0: 8x8
1: 16x16
----- Bit 6,5,4 (na)
----- Bit 3,2,1,0 (Pattern Set No)
If pattern Format = 8x8 then Pattern Set [3:0]
If pattern Format = 16x16 then Pattern Set [1:0] is valid */
#define RA8875_PTNO				  	  0x66//Pattern Set No for BTE

//BTE Raster OPerations - there's 16 possible operations but these are the main ones likely to be useful
#define RA8875_BTEROP_SOURCE	0xC0	//Overwrite dest with source (no mixing) *****THIS IS THE DEFAULT OPTION****
#define RA8875_BTEROP_BLACK		0xo0	//all black
#define RA8875_BTEROP_WHITE		0xf0	//all white
#define RA8875_BTEROP_DEST		0xA0    //destination unchanged
#define RA8875_BTEROP_ADD		0xE0    //ADD (brighter)
#define RA8875_BTEROP_SUBTRACT	0x20	//SUBTRACT (darker)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            Color Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_BGCR0				  0x60//Background Color Register 0 (R)
//#define RA8875_BGCR1				  0x61//Background Color Register 1 (G)
//#define RA8875_BGCR2				  0x62//Background Color Register 2 (B)
#define RA8875_FGCR0				  0x63//Foreground Color Register 0 (R)
//#define RA8875_FGCR1				  0x64//Foreground Color Register 1 (G)
//#define RA8875_FGCR2				  0x65//Foreground Color Register 2 (B)
#define RA8875_BGTR0				  0x67//Background Color Register for Transparent 0 (R)
//#define RA8875_BGTR1				  0x68//Background Color Register for Transparent 1 (G)
//#define RA8875_BGTR2				  0x69//Background Color Register for Transparent 2 (B)

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            TOUCH SCREEN REGISTERS
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if !defined(USE_EXTERNALTOUCH)
#define RA8875_TPCR0                  0x70//Touch Panel Control Register 0
	//#define RA8875_TPCR0_ENABLE           0x80
	//#define RA8875_TPCR0_DISABLE          0x00
	#define RA8875_TPCR0_WAIT_512CLK      0x00
	#define RA8875_TPCR0_WAIT_1024CLK     0x10
	#define RA8875_TPCR0_WAIT_2048CLK     0x20
	#define RA8875_TPCR0_WAIT_4096CLK     0x30
	#define RA8875_TPCR0_WAIT_8192CLK     0x40
	#define RA8875_TPCR0_WAIT_16384CLK    0x50
	#define RA8875_TPCR0_WAIT_32768CLK    0x60
	#define RA8875_TPCR0_WAIT_65536CLK    0x70
	#define RA8875_TPCR0_WAKEENABLE       0x08
	#define RA8875_TPCR0_WAKEDISABLE      0x00
	#define RA8875_TPCR0_ADCCLK_DIV1      0x00
	#define RA8875_TPCR0_ADCCLK_DIV2      0x01
	#define RA8875_TPCR0_ADCCLK_DIV4      0x02
	#define RA8875_TPCR0_ADCCLK_DIV8      0x03
	#define RA8875_TPCR0_ADCCLK_DIV16     0x04
	#define RA8875_TPCR0_ADCCLK_DIV32     0x05
	#define RA8875_TPCR0_ADCCLK_DIV64     0x06
	#define RA8875_TPCR0_ADCCLK_DIV128    0x07
/*
bits
0,1: 00(idle), 10(TP event), 01(latch x) , 11(latch y)
2:   1(Debounce enabled), 0(debounce disabled)
3,4: NA
5:   0(internal VREF), 1(Ext VREF)
6:   0(AUTO mode, 1(MANUAL mode)
*/
#define RA8875_TPCR1            	  0x71//Touch Panel Control Register 1
	#define RA8875_TPCR1_AUTO       0x00
	#define RA8875_TPCR1_MANUAL     0x40
	#define RA8875_TPCR1_VREFINT    0x00
	#define RA8875_TPCR1_VREFEXT    0x20
	#define RA8875_TPCR1_DEBOUNCE   0x04
	#define RA8875_TPCR1_NODEBOUNCE 0x00
	#define RA8875_TPCR1_IDLE       0x00
	#define RA8875_TPCR1_WAIT       0x01
	#define RA8875_TPCR1_LATCHX     0x02
	#define RA8875_TPCR1_LATCHY     0x03

#define RA8875_TPXH             	  0x72//Touch Panel X High Byte Data Register
#define RA8875_TPYH             	  0x73//Touch Panel Y High Byte Data Register
#define RA8875_TPXYL            	  0x74//Touch Panel X/Y Low Byte Data Register
#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            Graphic Cursor Setting Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//#define RA8875_GCHP0            	  0x80//Graphic Cursor Horizontal Position Register 0
//#define RA8875_GCHP1            	  0x81//Graphic Cursor Horizontal Position Register 1
//#define RA8875_GCVP0            	  0x82//Graphic Cursor Vertical Position Register 0
//#define RA8875_GCVP1            	  0x83//Graphic Cursor Vertical Position Register 0
//#define RA8875_GCC0            	      0x84//Graphic Cursor Color 0
//#define RA8875_GCC1            	      0x85//Graphic Cursor Color 1
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            PLL Setting Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_PLLC1            	  0x88//PLL Control Register 1
//#define RA8875_PLLC2            	  0x89//PLL Control Register 2

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            PWM Control Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_P1CR             	  0x8A//PWM1 Control Register
#define RA8875_P1DCR            	  0x8B//PWM1 Duty Cycle Register

#define RA8875_P2CR             	  0x8C//PWM2 Control Register
#define RA8875_P2DCR            	  0x8D//PWM2 Control Register

	#define RA8875_PxCR_ENABLE      0x80
	#define RA8875_PxCR_DISABLE     0x00
	#define RA8875_PxCR_CLKOUT      0x10
	#define RA8875_PxCR_PWMOUT      0x00



 	#define RA8875_PWM_CLK_DIV1     0x00
	#define RA8875_PWM_CLK_DIV2     0x01
	#define RA8875_PWM_CLK_DIV4     0x02
	#define RA8875_PWM_CLK_DIV8     0x03
	#define RA8875_PWM_CLK_DIV16    0x04
	#define RA8875_PWM_CLK_DIV32    0x05
	#define RA8875_PWM_CLK_DIV64    0x06
	#define RA8875_PWM_CLK_DIV128   0x07
	#define RA8875_PWM_CLK_DIV256   0x08
	#define RA8875_PWM_CLK_DIV512   0x09
	#define RA8875_PWM_CLK_DIV1024  0x0A
	#define RA8875_PWM_CLK_DIV2048  0x0B
	#define RA8875_PWM_CLK_DIV4096  0x0C
	#define RA8875_PWM_CLK_DIV8192  0x0D
	#define RA8875_PWM_CLK_DIV16384 0x0E
	#define RA8875_PWM_CLK_DIV32768 0x0F 

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            Memory Clear
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* Memory Clear Control Register     [0x8E]
----- Bit 7 (Memory Clear Function)
0: End or Stop (if read this bit and it's 0, clear completed)
1: Start the memory clear function
----- Bit 6 (Memory Clear Area Setting)
0: Clear the full window (ref. RA8875_HDWR,RA8875_VDHR0,RA8875_VDHR0ì1)
1: Clear the active window
----- Bit 5,4,3,2,1,0 (na)  */
#define RA8875_MCLR             	  0x8E//Memory Clear Control Register

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            Drawing Control Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_DCR                    0x90//Draw Line/Circle/Square Control Register
	#define RA8875_DCR_LINESQUTRI_START   0x80
	#define RA8875_DCR_LINESQUTRI_STOP    0x00
	#define RA8875_DCR_LINESQUTRI_STATUS  0x80
	#define RA8875_DCR_CIRCLE_START       0x40
	#define RA8875_DCR_CIRCLE_STATUS      0x40
	#define RA8875_DCR_CIRCLE_STOP        0x00
	#define RA8875_DCR_FILL               0x20
	#define RA8875_DCR_NOFILL             0x00
	#define RA8875_DCR_DRAWLINE           0x00
	#define RA8875_DCR_DRAWTRIANGLE       0x01
	#define RA8875_DCR_DRAWSQUARE         0x10

#define RA8875_DLHSR0         		  0x91//Draw Line/Square Horizontal Start Address Register0
//#define RA8875_DLHSR1         		  0x92//Draw Line/Square Horizontal Start Address Register1
#define RA8875_DLVSR0         		  0x93//Draw Line/Square Vertical Start Address Register0
//#define RA8875_DLVSR1         		  0x94//Draw Line/Square Vertical Start Address Register1
#define RA8875_DLHER0         		  0x95//Draw Line/Square Horizontal End Address Register0
//#define RA8875_DLHER1         		  0x96//Draw Line/Square Horizontal End Address Register1
#define RA8875_DLVER0         		  0x97//Draw Line/Square Vertical End Address Register0
//#define RA8875_DLVER1         		  0x98//Draw Line/Square Vertical End Address Register0

#define RA8875_DCHR0         		  0x99//Draw Circle Center Horizontal Address Register0
//#define RA8875_DCHR1         		  0x9A//Draw Circle Center Horizontal Address Register1
#define RA8875_DCVR0         		  0x9B//Draw Circle Center Vertical Address Register0
//#define RA8875_DCVR1         		  0x9C//Draw Circle Center Vertical Address Register1
#define RA8875_DCRR         		  0x9D//Draw Circle Radius Register

#define RA8875_ELLIPSE                0xA0//Draw Ellipse/Ellipse Curve/Circle Square Control Register
	#define RA8875_ELLIPSE_STATUS         0x80

#define RA8875_ELL_A0         		  0xA1//Draw Ellipse/Circle Square Long axis Setting Register0
//#define RA8875_ELL_A1         		  0xA2//Draw Ellipse/Circle Square Long axis Setting Register1
#define RA8875_ELL_B0         		  0xA3//Draw Ellipse/Circle Square Short axis Setting Register0
//#define RA8875_ELL_B1         		  0xA4//Draw Ellipse/Circle Square Short axis Setting Register1

#define RA8875_DEHR0         		  0xA5//Draw Ellipse/Circle Square Center Horizontal Address Register0
//#define RA8875_DEHR1         		  0xA6//Draw Ellipse/Circle Square Center Horizontal Address Register1
#define RA8875_DEVR0         		  0xA7//Draw Ellipse/Circle Square Center Vertical Address Register0
//#define RA8875_DEVR1         		  0xA8//Draw Ellipse/Circle Square Center Vertical Address Register1

#define RA8875_DTPH0         		  0xA9//Draw Triangle Point 2 Horizontal Address Register0
//#define RA8875_DTPH1         		  0xAA//Draw Triangle Point 2 Horizontal Address Register1
#define RA8875_DTPV0         		  0xAB//Draw Triangle Point 2 Vertical Address Register0
//#define RA8875_DTPV1         		  0xAC//Draw Triangle Point 2 Vertical Address Register1

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            DMA REGISTERS
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define RA8875_SSAR0				  0xB0//Source Starting Address REG 0
//#define RA8875_SSAR1				  0xB1//Source Starting Address REG 1
//#define RA8875_SSAR2				  0xB2//Source Starting Address REG 2
//#define RA8875_????					0xB3//???????????

#define RA8875_DTNR0				  0xB4//Block Width REG 0(BWR0) / DMA Transfer Number REG 0
#define RA8875_BWR1					  0xB5//Block Width REG 1
#define RA8875_DTNR1				  0xB6//Block Height REG 0(BHR0) /DMA Transfer Number REG 1
#define RA8875_BHR1					  0xB7//Block Height REG 1
#define RA8875_DTNR2				  0xB8//Source Picture Width REG 0(SPWR0) / DMA Transfer Number REG 2
#define RA8875_SPWR1				  0xB9//Source Picture Width REG 1
#define RA8875_DMACR				  0xBF//DMA Configuration REG


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            GPIO
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_GPIOX            	  0xC7
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            KEY-MATRIX
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_KSCR1            	  0xC0 //Key-Scan Control Register 1 (KSCR1)
#define RA8875_KSCR2            	  0xC1 //Key-Scan Controller Register 2 (KSCR2)
#define RA8875_KSDR0            	  0xC2 //Key-Scan Data Register (KSDR0)
#define RA8875_KSDR1            	  0xC3 //Key-Scan Data Register (KSDR1)
#define RA8875_KSDR2            	  0xC4 //Key-Scan Data Register (KSDR2)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                         Interrupt Control Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_INTC1            	  0xF0//Interrupt Control Register1
#define RA8875_INTC2            	  0xF1//Interrupt Control Register2
	#define RA8875_INTCx_KEY        	  0x10
	#define RA8875_INTCx_DMA        	  0x08
	#define RA8875_INTCx_TP         	  0x04
	#define RA8875_INTCx_BTE        	  0x02
	
	#define RA8875_ENABLE_INT_TP        ((uint8_t)(1<<2)) 
	#define RA8875_DISABLE_INT_TP       ((uint8_t)(0<<2)) 
	
    /* Touch Panel Enable/Disable Reg TPCR0[7] */
    #define TP_ENABLE   				((uint8_t)(1<<7))
    #define TP_DISABLE  				((uint8_t)(0<<7))
    #define TP_MODE_AUTO    			((uint8_t)(0<<6))   
    #define TP_MODE_MANUAL  			((uint8_t)(1<<6))
    #define TP_DEBOUNCE_OFF 			((uint8_t)(0<<2))
    #define TP_DEBOUNCE_ON  			((uint8_t)(1<<2))
	
	
    #define TP_ADC_CLKDIV_1             0
    #define TP_ADC_CLKDIV_2             1        
    #define TP_ADC_CLKDIV_4             2        
    #define TP_ADC_CLKDIV_8             3      
    #define TP_ADC_CLKDIV_16            4        
    #define TP_ADC_CLKDIV_32            5        
    #define TP_ADC_CLKDIV_64            6        
    #define TP_ADC_CLKDIV_128           7
    #define TP_ADC_SAMPLE_512_CLKS     ((uint8_t)(0<<4))
    #define TP_ADC_SAMPLE_1024_CLKS    ((uint8_t)(1<<4))
    #define TP_ADC_SAMPLE_2048_CLKS    ((uint8_t)(2<<4))
    #define TP_ADC_SAMPLE_4096_CLKS    ((uint8_t)(3<<4))
    #define TP_ADC_SAMPLE_8192_CLKS    ((uint8_t)(4<<4))
    #define TP_ADC_SAMPLE_16384_CLKS   ((uint8_t)(5<<4))
    #define TP_ADC_SAMPLE_32768_CLKS   ((uint8_t)(6<<4))
    #define TP_ADC_SAMPLE_65536_CLKS   ((uint8_t)(7<<4))
	//end spark_ra8875/_settings/RA8875Registers.h
//#include "spark_ra8875/_settings/RA8875ColorPresets.h"
const uint16_t	RA8875_BLACK            = 0x0000;
const uint16_t 	RA8875_WHITE            = 0xFFFF;

const uint16_t	RA8875_RED              = 0xF800;
const uint16_t	RA8875_GREEN            = 0x07E0;
const uint16_t	RA8875_BLUE             = 0x001F;

const uint16_t 	RA8875_CYAN             = RA8875_GREEN | RA8875_BLUE;//0x07FF;
const uint16_t 	RA8875_MAGENTA          = 0xF81F;
const uint16_t 	RA8875_YELLOW           = RA8875_RED | RA8875_GREEN;//0xFFE0;  
const uint16_t 	RA8875_LIGHT_GREY 		= 0xB5B2; // the experimentalist
const uint16_t 	RA8875_LIGHT_ORANGE 	= 0xFC80; // the experimentalist
const uint16_t 	RA8875_DARK_ORANGE 		= 0xFB60; // the experimentalist
const uint16_t 	RA8875_PINK 			= 0xFCFF; // M.Sandercock
const uint16_t 	RA8875_PURPLE 			= 0x8017; // M.Sandercock
const uint16_t 	RA8875_GRAYSCALE 		= 2113;//grayscale30 = RA8875_GRAYSCALE*30
//end spark_ra8875/_settings/RA8875ColorPresets.h
//#include "spark_ra8875/_settings/RA8875UserSettings.h"
/* ---------------------------- USER SETTINGS --------------------------------------*/
/* [RENDER TEXT OPTIMIZATIONS] +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
From 0.70b11 the Font Rendering Engine has some optimizations for render font faster but this require much more code.
Not all users need this so you can select if include Render Text Optimizations or not by comment the following line. */
#define _RA8875_TXTRNDOPTIMIZER								// [default uncommented]

/* [VISUALIZE RENDER TEXT OPTIMIZATIONS - >>> ONLY FOR DEBUG <<<] +++++++++++++++++++++++++++++++++++++++ 
The Text Render optimizations introduced in b11 it's the result of testing several algorithms so it's in continue evolution.
RA8875_VISPIXDEBUG force the rendering engine to show how the font it's rendered (Only for debug!!!).
RA8875_TXTBENCH enable the font rendering engine to measure the time  (Only for debug!!!), this
option works ONLY when  RA8875_VISPIXDEBUG it's uncommented or values are falsed*/
//#define RA8875_VISPIXDEBUG 								// [default commented]
//#define RA8875_TXTBENCH 									// [default commented]

/* [USE_RA8875_SEPARATE_TEXT_COLOR] +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
RA8875 chip set color globally, this mean that if you set text color white and after you create
a red circle, every text after that will result red text. 
Enabling this feature your text will always maintain the choosed color. */
#define USE_RA8875_SEPARATE_TEXT_COLOR 						// [default uncommented]

/* [RENDERED FONTS FOLLOWS INTERNAL FONTS CURSORS] +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
The library internal tracking the x,y position of the text cursors but not the rendered fonts, by
enabling this option you will force library to inform RA8875 to keep track of the x,y text cursor.
This option will slow down a bit the text rendering (just a bit) */
#define FORCE_RA8875_TXTREND_FOLLOW_CURS 					// [default uncommented]

/* [DISABLE ALL TOUCH SCREEN LIBRARY CAPABILITIES AND RELATIVE CODE] +++++++++++++++++++++++++++++++++++++++
If your project don't need Touch Screen or you choosed to use an external library you can comment out
the following line and ALL resources related will not loaded at all. 
If this is uncommented the CHOOSE YOUR TOUCH SCREEN TYPE has no effect. */
//#define _AVOID_TOUCHSCREEN 								// [default commented]

/* [CHOOSE YOUR TOUCH SCREEN TYPE] +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
This library supports the RA8875 internal resistive touch screen driver or a FT5206
based capacitive touch screen driver. Below, you have to choose one of them.
The Resistive Touch screen it's drived directly by RA8875.
The Capacitive Touch Screen use a I2C chip called FT5206 (hardwired to address 0x38).
Please choose at list one (NOT both), if you comment both it's the same as _AVOID_TOUCHSCREEN*/

//#define USE_RA8875_TOUCH//resistive touch screen
//#define USE_FT5206_TOUCH//capacitive touch screen

/* [USE ALTERNATIVE I2C/WIRE ON ARDUINO DUE] +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Arduino DUE has 2 I2C interfaces, default use Wire but you can force library to use Wire1.
Just uncomment the line below, this option works only for Arduino DUE and has NO effects
on other CPU!
*/

//#define USE_DUE_WIRE1_INTERFACE//force use Wire1 (only for DUE, no effects on other CPU)

/* [Default Interrupt Number for Touchscreen] [RA8875 & FT5206] ++++++++++++++++++++++++++++++++++
Some microcontroller like Teensy 3,3.1 and LC automatically assign ISR number (since they
can virtually assign ISR to any pin) but some old one like Arduino UNO have ISR hardcoded */
#if !defined(_AVOID_TOUCHSCREEN)
	#define __RA8875ISRINT 0// 								// [default 0, pin 2 on Arduino UNO]
#endif

/* [INTERNAL KEY MATRIX] +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
RA8875 has a 5x6 Key Matrix controller onboard, if you are not plan to use it
better leave commented the following define since it will share some registers
with several functions, otherwise de-comment it! */
//#define USE_RA8875_KEYMATRIX 								// [default commented]

/* [DEFAULT CURSOR BLINK RATE] +++++++++++++++++++++++++++++++++++++++++++++++++++++++
Nothing special here, you can set the default blink rate */
#define DEFAULTCURSORBLINKRATE			10 					// [default 10]

/* [DEFAULT INTERNAL FONT ENCODING] ++++++++++++++++++++++++++++++++++++++++++++++++++
RA8875 has 4 different font set, same shape but suitable for most languages
please look at RA8875 datasheet and choose the correct one for your language!
ISO_IEC_8859_1 (default), 
ISO_IEC_8859_2, 
ISO_IEC_8859_3, 
ISO_IEC_8859_4
The default one it's the most common one and should work in most situations */
#define DEFAULTINTENCODING				ISO_IEC_8859_1 		// [default ISO_IEC_8859_1]

/* [STARTUP SETTINGS] +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
#define _DFT_RA8875_TEXTWRAP			true 				// [default true]

/* [DEFAULT EXTERNAL FONT-ROM TYPE] ++++++++++++++++++++++++++++++++++++++++++++
If your card has a FONT-ROM installed, here you have to write wich one!
I support both Genicomp and Eastrising since they are the same chip named differently!
Possible solutions:
GT21L16T1W	(Genicomp)
GT21H16T1W	(Genicomp)
GT23L16U2W	(Genicomp)
GT30H24T3Y	(Genicomp)
GT23L24T3Y	(Genicomp)
GT23L24M1Z	(Genicomp)
GT23L32S4W	(Genicomp)
GT30H32S4W	(Genicomp)
GT30L32S4W	(Genicomp)
ER3303_1 	(Eastrising) *tested
ER3304_1 	(Eastrising) *tested
*/
#define	_DFT_RA8875_EXTFONTROMTYPE		GT21L16T1W 			// [default GT21L16T1W]

/* [DEFAULT EXTERNAL FONT-ROM ENCODING] ++++++++++++++++++++++++++++++++++++++++++++
Having an external FONT-ROM mean choose the desidered encoding (supported by ROM-CHIP!)
Possible solutions:
GB2312, 
GB12345, 
BIG5, 
UNICODE, 
ASCII, 
UNIJIS, 
JIS0208, 
LATIN/GREEK/ARABIC */
#define	_DFT_RA8875_EXTFONTROMCODING	GB2312 				// [default GB2312]

/* [Default Screen Rotation] ++++++++++++++++++++++++++++++++++++++++++++
*/
#define	_RA8875_DEFAULTSCRROT			0 					// [default 0]
/* [Default Backlight Color] ++++++++++++++++++++++++++++++++++++++++++++
*/
#define	_RA8875_DEFAULTBACKLIGHT		RA8875_BLACK 		// [default RA8875_BLACK]

/* [Default foreground Text Color] ++++++++++++++++++++++++++++++++++++++++++++
*/
#define	_RA8875_DEFAULTTXTFRGRND		RA8875_WHITE 		// [default RA8875_WHITE]

/* [Default background Text Color] ++++++++++++++++++++++++++++++++++++++++++++
*/
#define	_RA8875_DEFAULTTXTBKGRND		RA8875_BLACK 		// [default RA8875_BLACK]

/* [ARDUINO DUE SPI MODE] ++++++++++++++++++++++++++++++++++++++++++++
This library support DUE SPI Extended mode, by decommenting the line below
you have to choose from pin 4,10,52 for CS pin in DUE or you will get an error!
*/
//#define SPI_DUE_MODE_EXTENDED

/*----------------------------------------------------------------------------------
									SPI SPEED
----------------------------------------------------------------------------------*/
/*
From here it's better don't touch anithing, everithing has been tuned for maximum speed.
Only DUE or any AVR 8bit (like uno) can have some troubles with particular old libraries
because the use of fast port handle, in that case....
On Arduino DUE and other 8 bit Arduino MCU you can disable the fast CS port 
by commenting #define _FASTSSPORT, this will force to use the classic digitalWrite.

*/
/* Accordly RA8875 datasheet the READ cycles and WRITE cycles have different speed:
System clock/3(only write cycle), System clock/6(with read cycle)
MAXSPISPEED parameters it's also related to MCU features so it probably need to be tuned.
Not all MCU are capable to work at those speeds. Following parameters worked with both board I have.
After som mail exchange with RAiO I solved the dilemma behind SPI speed limit:
The RA8875 has limitation of 12Mhz SPI but this has been set because not all internal macros
can run over that speed, the library automatically deal with this  so I was able to go over 20Mhz!
At that speed you need to short cables as much you can, provide clean supply and good decoupling!
DO NOT Exceed 23Mhz for RA8875! It will result in garbage on screen or run very slow.
*/

#if defined(SPI_HAS_TRANSACTION)
//SPI transaction enabled library----------------------
	#if defined(__MK20DX128__) || defined(__MK20DX256__) //[Teensy 3.0 , 3.1 , 3.2]
		const static uint32_t MAXSPISPEED	= 22000000UL;  //don't go higher than 22000000!;
	#elif defined(__MKL26Z64__)							 //[Teensy LC] (12 or 24 Mhz max)
		const static uint32_t MAXSPISPEED	= 12000000UL;	 //default SPI main speed TeensyLC
		const static uint32_t MAXSPISPEED2	= 22000000UL;  //don't go higher than 22000000!;
	#elif defined(___DUESTUFF)							 //[DUE]
		const static uint32_t MAXSPISPEED	= 15000000UL;  // try experiment higher values but NOT over 22000000!
		//#define _FASTSSPORT
	#elif defined(ESP8266)	
		const static uint32_t MAXSPISPEED	= 8000000UL;  //don't go higher than 22000000!;
		//#define _FASTSSPORT
	#elif defined(SPARK)
		const static uint32_t MAXSPISPEED	= 8000000UL;  //don't go higher than 22000000!;
	// TODO: add more CPU here!
	#else												 //rest of the world (UNO, etc)
		const static uint32_t MAXSPISPEED	= 10000000UL;  //be careful, higher values result in extremely slow rendering!
		#define _FASTSSPORT
	#endif
#else
// legacy SPI library-------------------------------------------------------------
	#if defined(ENERGIA) 
	// LaunchPad, FraunchPad and StellarPad specific
		#if defined(__TM4C129XNCZAD__) || defined(__TM4C1294NCPDT__)//tiva???
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV4
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV8
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV8
		#elif defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM__)//stellaris first version
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV4
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV8
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV8
		#elif defined(__MSP430MCU__)//MSP430???
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV4
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV4
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV8
		#elif defined(TMS320F28069)//C2000???
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV4
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV4
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV8
		#elif defined(__CC3200R1M1RGC__)//CC3200???
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV4
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV4
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV8
		#endif
	#else
	// DUE
		#if defined(___DUESTUFF)
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV4	//84 divided by 4 = 21Mhz
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV8
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV6	//10.5Mhz
			//#define _FASTSSPORT
		#elif defined(ESP8266)//legacy
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV4	//8mhz
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV8
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV4	//8
		#elif defined(SPARK)//Really early development
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV4	//8mhz
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV8
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV4	//8
		#else
		// TODO: Add more CPU here!
	//rest of the world included UNO, etc.
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV2	//UNO = 4Mhz
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV4
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV2
			#define _FASTSSPORT
		#endif
	#endif
#endif

//Touch Screen stuff (do not touch)
#if defined(_AVOID_TOUCHSCREEN)
	#undef USE_RA8875_TOUCH
	#undef USE_FT5206_TOUCH
#else
	#if defined(USE_RA8875_TOUCH) && defined(USE_FT5206_TOUCH)
		#error you have to choose between USE_RA8875_TOUCH & USE_FT5206_TOUCH!
	#elif !defined(USE_RA8875_TOUCH) && !defined(USE_FT5206_TOUCH)
		#define _AVOID_TOUCHSCREEN
	#elif defined(USE_FT5206_TOUCH) && !defined(USE_RA8875_TOUCH)
		//#include "Wire.h"//include the support for FT5206
		static const uint8_t _FT5206REgisters[9] = {
			0x16,0x3C,0xE9,0x01,0x01,0xA0,0x0A,0x06,0x28
		};
	#elif !defined(USE_FT5206_TOUCH) && defined(USE_RA8875_TOUCH)
		#define TOUCSRCAL_XLOW	70//62
		#define TOUCSRCAL_YLOW	111//153
		#define TOUCSRCAL_XHIGH	895//924
		#define TOUCSRCAL_YHIGH	880//917
		//#include "_settings/RA8875Calibration.h"
	#endif
#endif
// end RA8875UserSettings.h

#if defined(_FORCE_PROGMEM__) && !defined(ESP8266)
template <typename T> T PROGMEM_read (const T * sce)
  {
  static T temp;
  memcpy_P (&temp, sce, sizeof (T));
  return temp;
  }
#endif



#if defined(__MKL26Z64__)
	static bool _altSPI;
#endif
#ifdef SPI_HAS_TRANSACTION
	static volatile uint32_t _SPImaxSpeed;//holder for SPI speed
#endif

#if defined(ESP8266) && defined(_FASTSSPORT)
	#include <eagle_soc.h>
#endif


class spark_ra8875 : public Print {
 public:
	// void 		debugData(uint16_t data,uint8_t len=8);
	// void 		showLineBuffer(uint8_t data[],int len);
//------------- INSTANCE -------------------------------------------------------------------
	#if defined(__MK20DX128__) || defined(__MK20DX256__)//Teensy 3.0, Teensy 3.1
		spark_ra8875(const uint8_t CSp,const uint8_t RSTp=255,const uint8_t mosi_pin=11,const uint8_t sclk_pin=13,const uint8_t miso_pin=12);
	#elif defined(__MKL26Z64__)//TeensyLC with FT5206_TOUCH
		spark_ra8875(const uint8_t CSp,const uint8_t RSTp=255,const uint8_t mosi_pin=11,const uint8_t sclk_pin=13,const uint8_t miso_pin=12);
	#elif defined(___DUESTUFF)//DUE
		spark_ra8875(const uint8_t CSp, const uint8_t RSTp=255);
	#elif defined(SPARK)//SPARK
		spark_ra8875(const uint8_t CSp, const uint8_t RSTp=255);
	#elif defined(NEEDS_SET_MODULE)//ENERGIA
		spark_ra8875(const uint8_t module, const uint8_t RSTp=255);
	#else//8 BIT ARDUINO's
		spark_ra8875(const uint8_t CSp, const uint8_t RSTp=255);
	#endif
//------------- SETUP -----------------------------------------------------------------------
	void 		begin(const enum RA8875sizes s,uint8_t colors=16);
	//(RA8875_480x272, RA8875_800x480, Adafruit_480x272, Adafruit_800x480) , (8/16 bit)
//------------- HARDWARE ------------------------------------------------------------
	void 		backlight(boolean on);
	void    	displayOn(boolean on);//turn diplay on/off
	void    	sleep(boolean sleep);//put display in sleep or not
	void 		brightness(uint8_t val);//ok
	uint8_t 	readStatus(void);//used to verify when an operation has concluded
	void 		clearMemory(bool stop=false);
	void 		clearWidthColor(bool bte);
	//void		clearMemory(boolean full);//clear the RA8875 internal buffer (fully or current layer)
	uint8_t		errorCode(void);//0: no error,
	/*
	format: 1byte 0b00000000;, bit described below:
	nan,nan,nan,nan,nan,CS(out-of-range),MOSI/MISO/SCLK(out-of-range),display(not-recognized)
	*/
//------------ Low Level functions ---------------------------------------------------------
	void    	writeCommand(const uint8_t d);
	void  		writeData16(uint16_t data);
//-------------- AREA ----------------------------------------------------------------------
	void		setActiveWindow(int16_t XL,int16_t XR,int16_t YT,int16_t YB);//The working area where to draw on
	void		setActiveWindow(void);
	void 		getActiveWindow(int16_t &XL,int16_t &XR ,int16_t &YT ,int16_t &YB);
	void		clearActiveWindow(bool full=false);//it clears the active window
	uint16_t 	width(bool absolute=false) const;//the phisical display width
	uint16_t 	height(bool absolute=false) const;//the phisical display height
	void		setRotation(uint8_t rotation); //rotate text and graphics
	uint8_t		getRotation(); //return the current rotation 0-3
	boolean		isPortrait(void);
//-------------- COLOR ----------------------------------------------------------------------
	void		setForegroundColor(uint16_t color);//color of objects in 16bit
	void		setForegroundColor(uint8_t R,uint8_t G,uint8_t B);//color of objects in 8+8+8bit
	void		setBackgroundColor(uint16_t color);//color of objects background in 16bit
	void		setBackgroundColor(uint8_t R,uint8_t G,uint8_t B);//color of objects background in 8+8+8bit
	void 		setTransparentColor(uint16_t color);//the current transparent color in 16bit
	void 		setTransparentColor(uint8_t R,uint8_t G,uint8_t B);//the current transparent color in 8+8+8bit
	void 		setColor(uint16_t fcolor,uint16_t bcolor,bool bcolorTraspFlag=false);
	void 		setColorBpp(uint8_t colors);//set the display color space 8 or 16!
	uint8_t 	getColorBpp(void);//get the current display color space (return 8 or 16)
	uint16_t	grandient(uint8_t val);
	uint16_t 	colorInterpolation(uint16_t color1,uint16_t color2,uint16_t pos,uint16_t div=100);
	uint16_t 	colorInterpolation(uint8_t r1,uint8_t g1,uint8_t b1,uint8_t r2,uint8_t g2,uint8_t b2,uint16_t pos,uint16_t div=100);
//-------------- COLOR CONVERSION -----------------------------------------------------------
	inline uint16_t Color565(uint8_t r,uint8_t g,uint8_t b) { return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3); }
	inline uint16_t Color24To565(int32_t color_) { return ((((color_ >> 16) & 0xFF) / 8) << 11) | ((((color_ >> 8) & 0xFF) / 4) << 5) | (((color_) &  0xFF) / 8);}
	inline uint16_t htmlTo565(int32_t color_) { return (uint16_t)(((color_ & 0xF80000) >> 8) | ((color_ & 0x00FC00) >> 5) | ((color_ & 0x0000F8) >> 3));}
	inline void 	Color565ToRGB(uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b){r = (((color & 0xF800) >> 11) * 527 + 23) >> 6; g = (((color & 0x07E0) >> 5) * 259 + 33) >> 6; b = ((color & 0x001F) * 527 + 23) >> 6;}
//-------------- CURSOR ----------------------------------------------------------------------
	void 		cursorIncrement(bool on);
	void 		setCursorBlinkRate(uint8_t rate);//set blink rate of the cursor 0...255 0:faster
	void 		showCursor(enum RA8875tcursor c,bool blink);//show cursor(NOCURSOR,IBEAM,UNDER,BLOCK), default blinking
	void    	setCursor(int16_t x, int16_t y,bool autocenter=false);//set cursor position to write text(pixels or CENTER)
	void 		getCursor(int16_t &x, int16_t &y);//from the RA8875 registers
	void 		getCursorFast(int16_t &x, int16_t &y);//from library (faster)
	int16_t		getCursorX(void);
	int16_t		getCursorY(void);
	void 		setGraphicCursor(uint8_t cur);//0...7 Select a custom graphic cursor (you should upload first)
	void 		showGraphicCursor(boolean cur);//show graphic cursor
//-------------- TEXT -----------------------------------------------------------------------
	void 		uploadUserChar(const uint8_t symbol[],uint8_t address);//upload user defined char as array at the address 0..255
	void		showUserChar(uint8_t symbolAddrs,uint8_t wide=0);//show user uploaded char at the adrs 0...255
	void    	setTextColor(uint16_t fcolor, uint16_t bcolor);//set text color + text background color
	void 		setTextColor(uint16_t fcolor);//set text color (backgroud will be transparent)
	void 		setTextGrandient(uint16_t fcolor1,uint16_t fcolor2);
	void    	setFontScale(uint8_t scale);//global font scale (w+h)
	void    	setFontScale(uint8_t xscale,uint8_t yscale);//font scale separated by w and h
	void    	setFontSize(enum RA8875tsize ts);//X16,X24,X32
	void 		setFontSpacing(uint8_t spc);//0:disabled ... 63:pix max
	void 		setFontInterline(uint8_t pix);//0...63 pix
	void 		setFontFullAlign(boolean align);//mmmm... doesn't do nothing! Have to investigate
	uint8_t 	getFontWidth(boolean inColums=false);
	uint8_t 	getFontHeight(boolean inRows=false);
	//----------FONT -------------------------------------------------------------------------
	void		setExternalFontRom(enum RA8875extRomType ert, enum RA8875extRomCoding erc,enum RA8875extRomFamily erf=STANDARD);
	void 		setFont(enum RA8875fontSource s);//INT,EXT (if you have a chip installed)
	//void 		setFont(const struct FONT_DEF *	fnt);
	void		setFont(const tFont *font);
	void 		setIntFontCoding(enum RA8875fontCoding f);
	void		setExtFontFamily(enum RA8875extRomFamily erf,boolean setReg=true);
//-------------- GRAPHIC POSITION --------------------------------------------------------------
	void    	setXY(int16_t x, int16_t y);//graphic set location
	void 		setX(int16_t x);
	void 		setY(int16_t y) ;
	//--------------- DRAW ---------------------------------------------------------------------
	void    	drawPixel(int16_t x, int16_t y, uint16_t color);
	void 		drawPixels(uint16_t p[], uint16_t count, int16_t x, int16_t y);
	uint16_t 	getPixel(int16_t x, int16_t y);
	//void 		getPixels(uint16_t * p, uint32_t count, int16_t x, int16_t y);
	void    	drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
	void    	drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
	void    	fillWindow(uint16_t color=_RA8875_DEFAULTBACKLIGHT);//fill the ActiveWindow with a color(default black)
	void		clearScreen(uint16_t color=_RA8875_DEFAULTBACKLIGHT);//fill the entire screen (regardless ActiveWindow) with a color(default black)
	void    	drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
	void 		drawLineAngle(int16_t x, int16_t y, int16_t angle, uint16_t length, uint16_t color,int offset = -90);
	void 		drawLineAngle(int16_t x, int16_t y, int16_t angle, uint16_t start, uint16_t length, uint16_t color,int offset = -90);
	void    	drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	void    	fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	void    	drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void    	fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void    	drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void    	fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void    	drawEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color);
	void    	fillEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color);
	void    	drawCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color);
	void    	fillCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color);
	void 		drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);//ok
	void 		fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
	void 		drawQuad(int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2,int16_t x3, int16_t y3, uint16_t color);
	void 		fillQuad(int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color, bool triangled=true);
	void 		drawPolygon(int16_t cx, int16_t cy, uint8_t sides, int16_t diameter, float rot, uint16_t color);
	void 		drawMesh(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t spacing,uint16_t color);
	void 		setArcParams(float arcAngleMax, int arcAngleOffset);
	void 		setAngleOffset(int16_t angleOffset);
	inline __attribute__((always_inline)) void drawArc(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, float start, float end, uint16_t color) {
		if (start == 0 && end == _arcAngle_max) {
			_drawArc_helper(cx, cy, radius, thickness, 0, _arcAngle_max, color);
		} else {
			_drawArc_helper(cx, cy, radius, thickness, start + (_arcAngle_offset / (float)360)*_arcAngle_max, end + (_arcAngle_offset / (float)360)*_arcAngle_max, color);
		}	
	}
//-------------- GAUGES ---------------------------------------------------------------------------
	void 		ringMeter(int val, int minV, int maxV, int16_t x, int16_t y, uint16_t r, const char* units="none", uint16_t colorScheme=4,uint16_t backSegColor=RA8875_BLACK,int16_t angle=150,uint8_t inc=10);
	void		roundGaugeTicker(uint16_t x, uint16_t y, uint16_t r, int from, int to, float dev,uint16_t color);
//-------------- LAYERS --------------------------------------------------------------------------
	void 		useLayers(boolean on);//mainly used to turn of layers!
	void		writeTo(enum RA8875writes d);//L1, L2, CGRAM, PATTERN, CURSOR
	void 		layerEffect(enum RA8875boolean efx);//LAYER1, LAYER2, TRANSPARENT, LIGHTEN, OR, AND, FLOATING
	void 		layerTransparency(uint8_t layer1,uint8_t layer2);
	uint8_t		getCurrentLayer(void); //return the current drawing layer. If layers are OFF, return 255
//--------------- SCROLL --------------------------------------------------------------------------
	void        setScrollMode(enum RA8875scrollMode mode); // The experimentalist
	void 		setScrollWindow(int16_t XL,int16_t XR ,int16_t YT ,int16_t YB);
	void 		scroll(int16_t x,int16_t y);
//-------------- DMA ------------------------------------------------------------------------------
	void 		DMA_blockModeSize(int16_t BWR,int16_t BHR,int16_t SPWR);
	void 		DMA_startAddress(unsigned long adrs);
	void 		DMA_enable(void);
	void 		drawFlashImage(int16_t x,int16_t y,int16_t w,int16_t h,uint8_t picnum);
//-------------- BTE -------------------------------------------------------------------------------
	void 		BTE_size(int16_t w, int16_t h);
	void	 	BTE_moveFrom(int16_t SX,int16_t SY);
	void	 	BTE_moveTo(int16_t DX,int16_t DY);
	//void 		BTE_source_destination(uint16_t SX,uint16_t DX ,uint16_t SY ,uint16_t DY);
	void		BTE_ropcode(unsigned char setx);//
	void 		BTE_enable(bool on);//
	void 		BTE_dataMode(enum RA8875btedatam m);//CONT,RECT
	void 		BTE_layer(enum RA8875btelayer sd,uint8_t l);//SOURCE,DEST - 1 or 2
	void		BTE_move(int16_t SourceX, int16_t SourceY, int16_t Width, int16_t Height, int16_t DestX, int16_t DestY, uint8_t SourceLayer=0, uint8_t DestLayer=0, bool Transparent = false, uint8_t ROP=RA8875_BTEROP_SOURCE, bool Monochrome=false, bool ReverseDir = false);
	//---------- PATTERN --------------------------------------------------------------------------
	void 		setPattern(uint8_t num, enum RA8875pattern p=P8X8);
	void 		writePattern(int16_t x,int16_t y,const uint8_t *data,uint8_t size,bool setAW=true);
//-------------- GPIO & PWM -------------------------
	void    	GPIOX(boolean on);
	void    	PWMout(uint8_t pw,uint8_t p);//1:backlight, 2:free
//-------------- ISR ------------------------------------------------------------------------------
	void		useINT(const uint8_t INTpin=2,const uint8_t INTnum=0);
	void 		enableISR(bool force = false); 
	void 		setInternalInt(enum RA8875intlist b);//   BTE,TOUCH,DMA,KEY
	void 		clearInternalInt(enum RA8875intlist b);// BTE,TOUCH,DMA,KEY
//-------------- TOUCH SCREEN ---------------------------------------------------------------------
#if !defined(_AVOID_TOUCHSCREEN)
	bool 		touched(bool safe=false);
	void 		setTouchLimit(uint8_t limit);//5 for FT5206, 1 for  RA8875
	uint8_t 	getTouchLimit(void);
#	if defined(USE_RA8875_TOUCH)
		//void		useINT(const uint8_t INTpin=2,const uint8_t INTnum=0);
		//void 		enableISR(bool force = false); 
		void 		touchBegin(void);//prepare Touch Screen driver
		void    	touchEnable(boolean enabled);//enable/disable Touch Polling (disable INT)
		void 		touchReadAdc(uint16_t *x, uint16_t *y);//returns 10bit ADC data (0...1024)
		void 		touchReadPixel(uint16_t *x, uint16_t *y);//return pixels (0...width, 0...height)
		boolean		touchCalibrated(void);//true if screen calibration it's present
	#elif defined (USE_FT5206_TOUCH)
		void		useCapINT(const uint8_t INTpin=2,const uint8_t INTnum=0);
		void 		enableCapISR(bool force = false); 
		void	 	updateTS(void);
		uint8_t 	getGesture(void);
		uint8_t 	getTouches(void);
		uint8_t 	getTouchState(void);
		uint8_t 	getTScoordinates(uint16_t (*touch_coordinates)[2]);
	#endif
#endif

//--------------Text Write -------------------------
virtual size_t write(uint8_t b) {
	if (_FNTgrandient) _FNTgrandient = false;//cannot use this with write
	_textWrite((const char *)&b, 1);
	return 1;
}

virtual size_t write(const uint8_t *buffer, size_t size) {
	_textWrite((const char *)buffer, size);
	return size;
}

using Print::write;

 protected:
	volatile bool 				  _textMode;
	volatile uint8_t 			  _MWCR0_Reg; //keep track of the register 		  [0x40]
	int16_t 		 			   RA8875_WIDTH, 	   RA8875_HEIGHT;//absolute
	int16_t 		 			  _width, 			  _height;
	int16_t						  _cursorX, 		  _cursorY;
	uint8_t						  _scaleX,			  _scaleY;
	bool						  _scaling;
	uint8_t						  _FNTwidth, 		  _FNTheight;
	uint8_t						  _FNTbaselineLow, 	  _FNTbaselineTop;
	volatile uint8_t			  _TXTparameters;
	/* It contains several parameters in one byte
	bit			 parameter
	0	->		_extFontRom 		i's using an ext rom font
	1	->		_autoAdvance		after a char the pointer move ahead
	2	->		_textWrap
	3	->		_fontFullAlig		
	4	->		_fontRotation       (actually not used)
	5	->		_alignXToCenter;
	6	->		_alignYToCenter;
	7	->		_renderFont active;
	*/
	bool						  _FNTgrandient;
	uint16_t					  _FNTgrandientColor1;
	uint16_t					  _FNTgrandientColor2;
	bool						  _FNTcompression;
	int							  _spaceCharWidth;
	volatile bool				  _needISRrearm;
	volatile uint8_t			  _enabledInterrups;
	static void 		 		  _isr(void);
	
	#if !defined(_AVOID_TOUCHSCREEN)
		volatile bool			  _touchEnabled;
		volatile bool			  _clearTInt;
	#endif

	#if defined(USE_FT5206_TOUCH)
		volatile bool			  _needCTS_ISRrearm;
		static void 		 	  cts_isr(void);
	#elif defined(USE_RA8875_TOUCH)
		//volatile bool			  _touchEnabled;
		//volatile bool			  _clearTInt;
	#endif

	
	#if defined(___TEENSYES)//all of them (32 bit only)
		uint8_t 				  _cs;
		uint8_t 				  _miso, _mosi, _sclk;
	#elif defined(ENERGIA)
		uint8_t _cs;
	#else
		#if defined(___DUESTUFF)
			#if defined(_FASTSSPORT)
				uint32_t 		  _cs, cspinmask;
			#else
				uint8_t 		  _cs;
			#endif
		#elif defined(ESP8266)	
			#if defined(_FASTSSPORT)
				uint32_t 		  _cs;
			#else
				uint8_t 		  _cs;
			#endif
		#elif defined(SPARK)
			#if defined(_FASTSSPORT)
				uint32_t 		  _cs;
			#else
				uint8_t 		  _cs;
			#endif
		#else
			#if defined(_FASTSSPORT)
				uint8_t 		  _cs, cspinmask;
			#else
				uint8_t 		  _cs;
			#endif
		#endif
	#endif
 private:
	uint8_t							_rst;
	uint8_t							_intPin;
	uint8_t						 	_intNum;
	bool							_useISR;
	const tFont 			  	 *  _currentFont;
	bool 						 	_checkInterrupt(uint8_t _bit,bool _clear=true);
	void 						    _disableISR(void);
	// Touch Screen vars ---------------------
	#if !defined(_AVOID_TOUCHSCREEN)
	uint8_t						    _maxTouch;//5 on FT5206, 1 on resistive
		#if defined(USE_FT5206_TOUCH)// FT5206 specifics
			uint8_t					_intCTSNum;
			uint8_t					_intCTSPin;
			uint8_t 				_cptRegisters[28];
			uint8_t					_gesture;
			uint8_t					_currentTouches;//0...5
			uint8_t					_currentTouchState;//0,1,2
			void 					_initializeFT5206(void);
			void 					_sendRegFT5206(uint8_t reg,const uint8_t val);
			void 					_disableCapISR(void);
		#elif defined(USE_RA8875_TOUCH)//RA8875 internal resistive specifics
			uint16_t 				_tsAdcMinX,_tsAdcMinY,_tsAdcMaxX,_tsAdcMaxY;
			void					readTouchADC(uint16_t *x, uint16_t *y);
			bool					_calibrated;
			boolean					_isCalibrated(void);//true if screen calibration it's present
		#endif
	#endif
	#if defined(USE_RA8875_KEYMATRIX)
		bool						_keyMatrixEnabled;
	#endif
	//system vars -------------------------------------------
	bool							_inited;//true when init has been ended
	bool							_sleep;
	enum RA8875sizes 				_displaySize;//Adafruit_480x272, etc
	bool							_portrait;
	uint8_t							_rotation;
	uint8_t							_initIndex;
	int16_t							_activeWindowXL,_activeWindowXR,_activeWindowYT,_activeWindowYB;
	uint8_t							_errorCode;
	//color vars ----------------------------------------------
	uint16_t						_foreColor;
	uint16_t						_backColor;
	bool							_backTransparent;
	uint8_t							_colorIndex;
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		uint16_t					_TXTForeColor;
		uint16_t					_TXTBackColor;
		bool						_TXTrecoverColor;
	#endif
	//text vars-------------------------------------------------		
	uint8_t							_FNTspacing;
	uint8_t							_FNTinterline;
	enum RA8875extRomFamily 		_EXTFNTfamily;
	enum RA8875extRomType 			_EXTFNTrom;
	enum RA8875extRomCoding 		_EXTFNTcoding;
	enum RA8875tsize				_EXTFNTsize;//X16,X24,X32
	enum RA8875fontSource 			_FNTsource;
	enum RA8875tcursor				_FNTcursorType;
	//centering -------------------------------
	bool							_relativeCenter;
	bool							_absoluteCenter;
	//layer vars -----------------------------
	uint8_t							_maxLayers;
	bool							_useMultiLayers;
	uint8_t							_currentLayer;
	bool 							_hasLayerLimits;//helper
	//scroll vars ----------------------------
	int16_t							_scrollXL,_scrollXR,_scrollYT,_scrollYB;
	//color space-----------------------------
	uint8_t							_color_bpp;//8=256, 16=64K colors
	uint8_t							_brightness;
	//various
	float 							_arcAngle_max;
	int 							_arcAngle_offset;
	int								_angle_offset;
	// Register containers -----------------------------------------
	// this needed to  prevent readRegister from chip that it's slow.
	
	uint8_t			 _DPCR_Reg;  ////Display Configuration		  	  [0x20]
	uint8_t			 _FNCR0_Reg; //Font Control Register 0 		  	  [0x21]
	uint8_t			 _FNCR1_Reg; //Font Control Register1 			  [0x22]
	uint8_t			 _FWTSET_Reg; //Font Write Type Setting Register   [0x2E]
	uint8_t			 _SFRSET_Reg; //Serial Font ROM Setting 		  	  [0x2F]
	uint8_t			 _INTC1_Reg; //Interrupt Control Register1		  [0xF0]
	//		functions --------------------------------------------------
	void 		_initialize();
	void 		_setSysClock(uint8_t pll1,uint8_t pll2,uint8_t pixclk);
	//      TEXT writing stuff-------------------------------------------
	void    	_textWrite(const char* buffer, uint16_t len=0);//thanks to Paul Stoffregen for the initial version of this one
	void 		_charWrite(const char c,uint8_t offset);
	void 		_charWriteR(const char c,uint8_t offset,uint16_t fcolor,uint16_t bcolor);
	int 		_getCharCode(uint8_t ch);
	#if defined(_RA8875_TXTRNDOPTIMIZER)
		void 		_drawChar_unc(int16_t x,int16_t y,int charW,int index,uint16_t fcolor);
	#else
		void 		_drawChar_unc(int16_t x,int16_t y,int16_t w,const uint8_t *data,uint16_t fcolor,uint16_t bcolor);
	#endif
	
	//void 		_drawChar_com(int16_t x,int16_t y,int16_t w,const uint8_t *data);
	void 		_textPosition(int16_t x, int16_t y,bool update);
	void 		_setFNTdimensions(uint8_t index);
	int16_t 	_STRlen_helper(const char* buffer,uint16_t len=0);
	void 		fontRomSpeed(uint8_t sp);
	//--------------------------------------------------------------------
	void 		PWMsetup(uint8_t pw,boolean on, uint8_t clock);
	void 		_updateActiveWindow(bool full);
	void 		_setTextMode(bool m);
	void 		_scanDirection(boolean invertH,boolean invertV);
	// 		helpers-----------------------------
	
	void 		_circle_helper(int16_t x0, int16_t y0, int16_t r, uint16_t color, bool filled);
	void 		_rect_helper(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool filled);
	void 		_roundRect_helper(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color, bool filled);
	void 		_triangle_helper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled);
	void 		_ellipseCurve_helper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis,uint8_t curvePart, uint16_t color, bool filled);
	void 		_drawArc_helper(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, float startAngle, float endAngle, uint16_t color);
	void 		_line_addressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
	void 		_curve_addressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
	float 		_cosDeg_helper(float angle);
	float 		_sinDeg_helper(float angle);
	#if defined(_RA8875_TXTRNDOPTIMIZER)
	void 		_charLineRender(bool lineBuffer[],int charW,int16_t x,int16_t y,int16_t currentYposition,uint16_t fcolor);
	#endif
	
	//convert a 16bit color(565) into 8bit color(332) as requested by RA8875 datasheet
	//inline __attribute__((always_inline))
	uint8_t _color16To8bpp(uint16_t color) 
		__attribute__((always_inline)) {
		return ((color & 0x3800) >> 6 | (color & 0x00E0) >> 3 | (color & 0x0003));
	}
		
	//inline __attribute__((always_inline)) 
	void _checkLimits_helper(int16_t &x,int16_t &y)
		__attribute__((always_inline)) {
			if (x < 0) x = 0;
			if (y < 0) y = 0;
			if (x >= RA8875_WIDTH) x = RA8875_WIDTH - 1;
			if (y >= RA8875_HEIGHT) y = RA8875_HEIGHT -1;
			x = x;
			y = y;
	}
		
	//inline __attribute__((always_inline)) 	
	void _center_helper(int16_t &x, int16_t &y)
		__attribute__((always_inline)) {
			if (x == CENTER) x = _width/2;
			if (y == CENTER) y = _height/2;
	}
	#if defined(___TEENSYES)//all of them (32 bit only)
		// nothing, already done
	#elif defined(ENERGIA)
		// TODO
	#else
		#if defined(___DUESTUFF) // DUE
			#if defined(_FASTSSPORT)
				volatile uint32_t *csport;
			#endif
		#elif defined(ESP8266) // ESP8266
				uint32_t _pinRegister(uint8_t pin)
				__attribute__((always_inline)) {
					return _BV(pin);
				}
		#elif defined(SPARK)
			// Mmmm, dunno... put nothing for now
		#else// AVR,ARM (not DUE),STM,CHIPKIT
	    //TODO:must check if all processor are compatible
			#if defined(_FASTSSPORT)
				volatile uint8_t *csport;
			#endif
		#endif
	#endif
    // Low level access  commands ----------------------
	
	//inline __attribute__((always_inline)) 
	void _startSend()
		__attribute__((always_inline)) {
		#if defined(SPI_HAS_TRANSACTION)
			#if defined(__MKL26Z64__)	
				_altSPI == true ? SPI1.beginTransaction(SPISettings(_SPImaxSpeed, MSBFIRST, SPI_MODE3)) : SPI.beginTransaction(SPISettings(_SPImaxSpeed, MSBFIRST, SPI_MODE3));
			#elif defined(ESP8266)	
				SPI.beginTransaction(SPISettings(_SPImaxSpeed, MSBFIRST, SPI_MODE0));//it works, anyway ESP doesn't work in MODE3!
			#elif defined(SPARK)	
				SPI.beginTransaction(SPISettings(_SPImaxSpeed, MSBFIRST, SPI_MODE0));//TODO !
			#else
				SPI.beginTransaction(SPISettings(_SPImaxSpeed, MSBFIRST, SPI_MODE3));
			#endif
		#elif !defined(ENERGIA) && !defined(SPI_HAS_TRANSACTION) && !defined(___STM32STUFF) && !defined(ESP8266) && !defined(SPARK)
			cli();//protect from interrupts
		#endif//end has transaction
		
		#if defined(___TEENSYES)//all of them (32 bit only)
			digitalWriteFast(_cs, LOW);
		#elif !defined(ENERGIA)//UNO,DUE,ETC.
			#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)//DUE extended SPI
				//nothing
			#else//DUE (normal),UNO,ETC.
				#if defined(ESP8266)	
					#if defined(_FASTSSPORT)
						GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, _pinRegister(_cs));//L
					#else
						digitalWrite(_cs, LOW);// for now
					#endif
				#elif defined(SPARK)
					pinResetFast(_cs, LOW);
				#elif !defined(ESP8266) && defined(_FASTSSPORT)
					*csport &= ~cspinmask;
				#else
					digitalWrite(_cs, LOW);
				#endif
			#endif
		#else//ENERGIA
			digitalWrite(_cs, LOW);
		#endif
	}
	
	//inline __attribute__((always_inline)) 
	void _endSend()
		__attribute__((always_inline)) {
	#if defined(___TEENSYES)//all of them (32 bit only)
		digitalWriteFast(_cs, HIGH);
	#elif !defined(ENERGIA)
		#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)//DUE extended SPI
			//nothing
		#else//DUE (normal),UNO,ETC.
			#if defined(ESP8266)	
				#if defined(_FASTSSPORT)
					GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, _pinRegister(_cs));//H
				#else
					digitalWrite(_cs, HIGH);
				#endif
			#elif defined(SPARK)
				pinSetFast(_cs, HIGH);
			#elif !defined(ESP8266) && defined(_FASTSSPORT)
				*csport |= cspinmask;
			#else
				digitalWrite(_cs, HIGH);
			#endif
		#endif
	#else//ENERGIA
		digitalWrite(_cs, HIGH);
	#endif
	
	#if defined(SPI_HAS_TRANSACTION)
		#if defined(__MKL26Z64__)	
			_altSPI == true ? SPI1.endTransaction() : SPI.endTransaction();
		#else
			SPI.endTransaction();
		#endif
	#elif !defined(ENERGIA) && !defined(SPI_HAS_TRANSACTION) && !defined(___STM32STUFF) && !defined(ESP8266) && !defined(SPARK)
		sei();//enable interrupts
	#endif
} 
	void    	_writeRegister(const uint8_t reg, uint8_t val);
	uint8_t 	_readRegister(const uint8_t reg);
	void    	_writeData(uint8_t data);
	uint8_t 	_readData(bool stat=false);
	boolean 	_waitPoll(uint8_t r, uint8_t f);//from adafruit
	void 		_waitBusy(uint8_t res=0x80);//0x80, 0x40(BTE busy), 0x01(DMA busy)

	#if defined(NEEDS_SET_MODULE)//for Energia
		void 		selectCS(uint8_t module);
	#endif

#if defined(_FASTCPU)
//inline __attribute__((always_inline)) 
void _slowDownSPI(bool slow,uint32_t slowSpeed=10000000UL)
	__attribute__((always_inline)) {
	#if defined(SPI_HAS_TRANSACTION)
		if (slow){
			_SPImaxSpeed = slowSpeed;
		} else {
			#if defined(__MKL26Z64__)	
				if (_altSPI){
					_SPImaxSpeed = MAXSPISPEED2;//TeensyLC max SPI speed on alternate SPI
				} else {
					_SPImaxSpeed = MAXSPISPEED;
				}
			#else
				_SPImaxSpeed = MAXSPISPEED;
			#endif
		}
	#else
		if (slow){
			#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)
				SPI.setClockDivider(_cs,SPI_SPEED_SAFE);
			#else
				SPI.setClockDivider(SPI_SPEED_SAFE);
			#endif
		} else {
			#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)
				SPI.setClockDivider(_cs,SPI_SPEED_WRITE);
			#else
				SPI.setClockDivider(SPI_SPEED_WRITE);
			#endif
		}
	#endif
}
#endif

#if defined(__AVR__)
	//inline __attribute__((always_inline))
		void _spiwrite16(uint16_t d) 
			__attribute__((always_inline)) {
			SPDR = highByte(d);
			while (!(SPSR & _BV(SPIF)));
			SPDR = lowByte(d);
			while (!(SPSR & _BV(SPIF)));
		}

	//inline __attribute__((always_inline))
		void _spiwrite(uint8_t c) 
			__attribute__((always_inline)) {
			SPDR = c;
			//asm volatile("nop");
			while (!(SPSR & _BV(SPIF)));
		}
		
	//inline __attribute__((always_inline))
		uint8_t _spiread(void) 
			__attribute__((always_inline)) {
			uint8_t r = 0;
			SPDR = 0x00;
			//asm volatile("nop");
			while(!(SPSR & _BV(SPIF)));
			r = SPDR;
			return r;
		}
#elif defined(SPARK)
		void _spiwrite16(uint16_t d) 
			__attribute__((always_inline)) {
			SPI.transfer(d >> 8);
			SPI.transfer(d & 0xFF);
		}

		void _spiwrite(uint8_t c) 
			__attribute__((always_inline)) {
			SPI.transfer(c);
		}
		
		uint8_t _spiread(void) 
			__attribute__((always_inline)) {
			uint8_t r = SPI.transfer(0x00);
			return r;
		}
		
#endif
};
#endif