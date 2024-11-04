#pragma once

/*
 * This was taken and changed from DrawLines.hpp
 *
 */

#include <glm/glm.hpp>

#include "GL.hpp"
#include "Load.hpp"
#include <string>
#include <vector>

// === Shader Program ===
struct SpriteProgram {
	SpriteProgram();
	~SpriteProgram();

	GLuint program = 0;
	//Attribute (per-vertex variable) locations:
	GLuint Position_vec4 = -1U;
	GLuint Color_vec4 = -1U;
	//Uniform (per-invocation variable) locations:
	GLuint OBJECT_TO_CLIP_mat4 = -1U;
	//Textures:
	// none
};

//extern Load< SpriteProgram > sprite_program(LoadTagEarly);

// === UI Shader ===
struct UIShader
{
	// Start drawing; will remember world_to_clip matrix:
	UIShader(glm::mat4 const& world_to_clip);

	// draw a single line from a to b (in world space):
	void draw(glm::vec3 const& a, glm::vec3 const& b, glm::u8vec4 const& color = glm::u8vec4(0xff));

	// draw a wireframe box corresponding to the [-1,1]^3 cube transformed by mat:
	void draw_box(glm::mat4x3 const& mat, glm::u8vec4 const& color = glm::u8vec4(0xff));

	//void draw_sprite(glm::mat4x3 const& mat, Sprite const& sp);

	// Finish drawing (push attribs to GPU):
	~UIShader();

	glm::mat4 world_to_clip;
	struct Vertex
	{
		Vertex(glm::vec3 const& Position_, glm::u8vec4 const& Color_) : Position(Position_), Color(Color_) {}
		glm::vec3 Position;
		glm::u8vec4 Color;
	};
	std::vector<Vertex> attribs;
};

//struct Sprite
//{
//	Sprite(glm::vec3 const& Position_, glm::u8vec4 const& Color_) : Position(Position_), Color(Color_) {}
//	glm::vec3 Position;
//	glm::u8vec4 Color;
//};


