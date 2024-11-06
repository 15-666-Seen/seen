#pragma once

/*
 * This was taken and changed from DrawLines.hpp
 *
 */

#include <glm/glm.hpp>

#include "GL.hpp"
#include "Load.hpp"
#include "data_path.hpp"
#include "img_loader.hpp"

#include <string>
#include <vector>
#include <filesystem>



// === Shader Program ===
struct SpriteProgram {
	SpriteProgram();
	~SpriteProgram();

	GLuint program = 0;

	//Attribute (per-vertex variable) locations:
	GLuint Position_vec4 = -1U;
	GLuint UV_vec2 = -1U;
	//Uniform (per-invocation variable) locations:
	GLuint OBJECT_TO_CLIP_mat4 = -1U;

	/* All UI Textures */
	GLuint dialogue_texture = 0;
	//Textures:
	// none
};


// === UI Shader ===
struct UIShader
{
	// Start drawing; will remember world_to_clip matrix:
	UIShader(glm::mat4 const& world_to_clip);

	// draw a single line from a to b (in world space):
	void draw(glm::vec3 const& a, glm::vec2 const& uv);

	// draw a wireframe box corresponding to the [-1,1]^3 cube transformed by mat:
	void draw_dialogue_box(glm::mat4x3 const& mat);

	//void draw_sprite(glm::mat4x3 const& mat, Sprite const& sp);

	// Finish drawing (push attribs to GPU):
	~UIShader();

	glm::mat4 world_to_clip;
	struct Vertex
	{
		Vertex(glm::vec3 const& Position_, glm::vec2 const& UV_) : Position(Position_), UV(UV_) {}
		glm::vec3 Position;
		glm::vec2 UV;
	};
	std::vector<Vertex> attribs;
};



