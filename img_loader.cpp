#pragma once

#include "GL.hpp"
#include "img_loader.hpp"
#include "Load.hpp"
#include "Scene.hpp"

#include "data_path.hpp"

#include <filesystem>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned char* loadImg(const std::string filepath, const std::string filename, int& width, int& height, int& channels ) {

	std::string f = filepath + "/" + filename;

	unsigned char* data = stbi_load(f.c_str(), &width, &height, &channels, 4); // 4 = RGBA format
	if (!data) {
		throw std::runtime_error("Failed to load texture: " + f);
	}
	printf("%s size %d %d \n", f.c_str(), width, height);

	return data;
}

void freeImg(unsigned char* data) {
	stbi_image_free(data);

}