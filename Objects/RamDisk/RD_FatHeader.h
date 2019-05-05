#pragma once

// Put these data blocks at these locations to get a basic FAT16 volume

#define FH_0_COUNT     192
#define FH_510_COUNT   2
#define FH_2048_COUNT  4
#define FH_12288_COUNT 4

const uint8_t rd_fat_header_0[] = {
    0xEB,0x3C,0x90,0x72,0x61,0x6D,0x64,0x69,0x73,0x6B,0x2E,0x00,0x02,0x04,0x04,0x00,
    0x02,0x00,0x02,0x00,0x50,0xF8,0x14,0x00,0x20,0x00,0x40,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x80,0x00,0x29,0xEB,0x63,0xEB,0xAD,0x4E,0x4F,0x20,0x4E,0x41,
    0x4D,0x45,0x20,0x20,0x20,0x20,0x46,0x41,0x54,0x31,0x36,0x20,0x20,0x20,0x0E,0x1F,
    0xBE,0x5B,0x7C,0xAC,0x22,0xC0,0x74,0x0B,0x56,0xB4,0x0E,0xBB,0x07,0x00,0xCD,0x10,
    0x5E,0xEB,0xF0,0x32,0xE4,0xCD,0x16,0xCD,0x19,0xEB,0xFE,0x54,0x68,0x69,0x73,0x20,
    0x69,0x73,0x20,0x6E,0x6F,0x74,0x20,0x61,0x20,0x62,0x6F,0x6F,0x74,0x61,0x62,0x6C,
    0x65,0x20,0x64,0x69,0x73,0x6B,0x2E,0x20,0x20,0x50,0x6C,0x65,0x61,0x73,0x65,0x20,
    0x69,0x6E,0x73,0x65,0x72,0x74,0x20,0x61,0x20,0x62,0x6F,0x6F,0x74,0x61,0x62,0x6C,
    0x65,0x20,0x66,0x6C,0x6F,0x70,0x70,0x79,0x20,0x61,0x6E,0x64,0x0D,0x0A,0x70,0x72,
    0x65,0x73,0x73,0x20,0x61,0x6E,0x79,0x20,0x6B,0x65,0x79,0x20,0x74,0x6F,0x20,0x74,
    0x72,0x79,0x20,0x61,0x67,0x61,0x69,0x6E,0x20,0x2E,0x2E,0x2E,0x20,0x0D,0x0A,0x00
};
const uint8_t rd_fat_header_510[] = {0x55, 0xAA};
const uint8_t rd_fat_header_2048[] = {0xF8,0xFF,0xFF,0xFF };
const uint8_t rd_fat_header_12288[] = {0xF8,0xFF,0xFF,0xFF };