#include "UIShaderProgram.hpp"

#include "gl_errors.hpp"
#include "gl_compile_program.hpp"

#include <glm/gtc/type_ptr.hpp>

//===== Shader Program ====

Load< SpriteProgram > sprite_program(LoadTagEarly);

SpriteProgram::SpriteProgram() {
	//Compile vertex and fragment shaders using the convenient 'gl_compile_program' helper function:
	program = gl_compile_program(
		//vertex shader:
		"#version 330\n"
		"uniform mat4 OBJECT_TO_CLIP;\n"
		"in vec4 Position;\n"
		"in vec2 TexCoord;\n"
		"out vec2 texCoord;\n"
		"void main() {\n"
		"	gl_Position = OBJECT_TO_CLIP * Position;\n"
		"	texCoord = TexCoord;\n"
		"}\n"
		,
		//fragment shader:
		"#version 330\n"
		"uniform sampler2D TEX;\n"
		"in vec2 texCoord;\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"	fragColor = texture(TEX, texCoord);\n"
		"}\n"
	);
	//As you can see above, adjacent strings in C/C++ are concatenated.
	// this is very useful for writing long shader programs inline.

	//look up the locations of vertex attributes:
	Position_vec4 = glGetAttribLocation(program, "Position");
	Color_vec4 = glGetAttribLocation(program, "Color");

	//look up the locations of uniforms:
	OBJECT_TO_CLIP_mat4 = glGetUniformLocation(program, "OBJECT_TO_CLIP");
}

SpriteProgram::~SpriteProgram() {
	glDeleteProgram(program);
	program = 0;
}

// ==== UI Shader ====

static GLuint vertex_buffer = 0;
static GLuint vertex_buffer_for_sprite_program = 0;

static Load<void> setup_buffers(LoadTagDefault, []()
	{

		{ // set up vertex buffer:
			glGenBuffers(1, &vertex_buffer);
			// for now, buffer will be un-filled.
		}

		{ // vertex array mapping buffer for sprite_program:
			// ask OpenGL to fill vertex_buffer_for_sprite_program with the name of an unused vertex array object:
			glGenVertexArrays(1, &vertex_buffer_for_sprite_program);

			// set vertex_buffer_for_sprite_program as the current vertex array object:
			glBindVertexArray(vertex_buffer_for_sprite_program);

			// set vertex_buffer as the source of glVertexAttribPointer() commands:
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

			// set up the vertex array object to describe arrays of PongMode::Vertex:
			glVertexAttribPointer(
				sprite_program->Position_vec4,						// attribute
				3,													// size
				GL_FLOAT,											// type
				GL_FALSE,											// normalized
				sizeof(UIShader::Vertex),							// stride
				(GLbyte*)0 + offsetof(UIShader::Vertex, Position) // offset
			);
			glEnableVertexAttribArray(sprite_program->Position_vec4);
			//[Note that it is okay to bind a vec3 input to a vec4 attribute -- the w component will be filled with 1.0 automatically]

			glVertexAttribPointer(
				sprite_program->Color_vec4,						 // attribute
				4,												 // size
				GL_UNSIGNED_BYTE,								 // type
				GL_TRUE,										 // normalized
				sizeof(UIShader::Vertex),						 // stride
				(GLbyte*)0 + offsetof(UIShader::Vertex, Color) // offset
			);
			glEnableVertexAttribArray(sprite_program->Color_vec4);

			// done referring to vertex_buffer, so unbind it:
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// done setting up vertex array object, so unbind it:
			glBindVertexArray(0);
		}

		GL_ERRORS(); // PARANOIA: make sure nothing strange happened during setup
	});

UIShader::UIShader(glm::mat4 const& world_to_clip_) : world_to_clip(world_to_clip_)
{
}

void UIShader::draw(glm::vec3 const& a, glm::vec3 const& b, glm::u8vec4 const& color)
{
	attribs.emplace_back(a, color);
	attribs.emplace_back(b, color);
}

void UIShader::draw_box(glm::mat4x3 const& mat, glm::u8vec4 const& color)
{
	// draw cube as three edge sets:

	draw(mat * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), mat * glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), color);
	draw(mat * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), mat * glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), color);
	draw(mat * glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), mat * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), color);
	draw(mat * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), mat * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), color);

	draw(mat * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), mat * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), color);
	draw(mat * glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), mat * glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), color);
	draw(mat * glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), mat * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), color);
	draw(mat * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), mat * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), color);

	draw(mat * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), mat * glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), color);
	draw(mat * glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), mat * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), color);
	draw(mat * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), mat * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), color);
	draw(mat * glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), mat * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), color);
}

//void UIShader::draw_sprite(glm::mat4x3 const& mat, Sprite const& sp) {
//}

UIShader::~UIShader()
{
	if (attribs.empty())
		return;

	// upload vertices to vertex_buffer:
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);														// set vertex_buffer as current
	glBufferData(GL_ARRAY_BUFFER, attribs.size() * sizeof(attribs[0]), attribs.data(), GL_STREAM_DRAW); // upload attribs array
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// set sprite_program as current program:
	glUseProgram(sprite_program->program);

	// upload OBJECT_TO_CLIP to the proper uniform location:
	glUniformMatrix4fv(sprite_program->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(world_to_clip));

	// use the mapping vertex_buffer_for_sprite_program to fetch vertex data:
	glBindVertexArray(vertex_buffer_for_sprite_program);

	// run the OpenGL pipeline:
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(attribs.size()));

	// reset vertex array to none:
	glBindVertexArray(0);

	// reset current program to none:
	glUseProgram(0);
}

