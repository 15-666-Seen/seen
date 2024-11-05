#pragma once

#include <string>

unsigned char* loadImg(const std::string filepath, const std::string filename, int& width, int& height, int& channels);

void freeImg(unsigned char* data);