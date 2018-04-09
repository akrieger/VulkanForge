#pragma once

extern "C" {

unsigned char* read_png(const char* filename, unsigned int* width, unsigned int* height, int* bit_depth);

}