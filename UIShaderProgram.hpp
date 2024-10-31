#pragma once

#include "GL.hpp"
#include "Load.hpp"

// shader code based oon https://learnopengl.com/In-Practice/2D-Game/Rendering-Sprites
//Shader program that draws transformed, vertices tinted with vertex colors:
struct UIShaderProgram {
	UIShaderProgram();
	~UIShaderProgram();

	GLuint program = 0;
	//Attribute (per-vertex variable) locations:
	GLuint Position_vec4 = -1U;
	//Uniform (per-invocation variable) locations:
	GLuint OBJECT_TO_CLIP_mat4 = -1U;
	//Textures:
	//TEXTURE0 - texture that is accessed by TexCoord
};

extern Load< UIShaderProgram > ui_shader_program;
