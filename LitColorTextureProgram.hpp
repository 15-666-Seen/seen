#pragma once

#include "GL.hpp"
#include "Load.hpp"
#include "Scene.hpp"
#include "img_loader.hpp"

#include "data_path.hpp"

#include <filesystem>

#include <unordered_map>

//Shader program that draws transformed, lit, textured vertices tinted with vertex colors:
struct LitColorTextureProgram {
	LitColorTextureProgram();
	~LitColorTextureProgram();

	GLuint program = 0;

	//Attribute (per-vertex variable) locations:
	GLuint Position_vec4 = -1U;
	GLuint Normal_vec3 = -1U;
	GLuint Tangent_vec3 = -1U;
	GLuint Bitangent_vec3 = -1U;
	GLuint Color_vec4 = -1U;
	GLuint TexCoord_vec2 = -1U;

	//Uniform (per-invocation variable) locations:
	GLuint OBJECT_TO_CLIP_mat4 = -1U;
	GLuint OBJECT_TO_LIGHT_mat4x3 = -1U;
	GLuint NORMAL_TO_LIGHT_mat3 = -1U;

	//lighting:
	GLuint LIGHT_TYPE_int = -1U;
	GLuint LIGHT_LOCATION_vec3 = -1U;
	GLuint LIGHT_DIRECTION_vec3 = -1U;
	GLuint LIGHT_ENERGY_vec3 = -1U;
	GLuint LIGHT_CUTOFF_float = -1U;

	//--------
	GLuint LoadTexture(const std::string filepath, const std::string filename);
	
	//Match filenames to texture indices:
	std::unordered_map<std::string, GLint> tex_file_to_glint;
};

extern Load< LitColorTextureProgram > lit_color_texture_program;

//For convenient scene-graph setup, copy this object:
// NOTE: by default, has texture bound to 1-pixel white texture -- so it's okay to use with vertex-color-only meshes.
extern Scene::Drawable::Pipeline lit_color_texture_program_pipeline;



