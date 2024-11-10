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
	UV_vec2 = glGetAttribLocation(program, "TexCoord");

	//look up the locations of uniforms:
	OBJECT_TO_CLIP_mat4 = glGetUniformLocation(program, "OBJECT_TO_CLIP");

	//std::cout << "UI program" << (int)program << std::endl;

	//set TEX to always refer to texture binding zero:
	glUseProgram(program); //bind program -- glUniform* calls refer to this program now

	// load dialogue box texture
	GLuint tex;
	glGenTextures(1, &tex);

	int width, height, channels;
	unsigned char* data = loadImg(data_path("textures/UI"), "dialogue.png", width, height, channels);

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	dialogue_texture = tex;

	glBindTexture(GL_TEXTURE_2D, 0);
	freeImg(data);

	glUseProgram(0); 
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

			// set up the vertex array object 
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
				sprite_program->UV_vec2,						 // attribute
				2,												 // size
				GL_FLOAT,										 // type
				GL_TRUE,										 // normalized
				sizeof(UIShader::Vertex),						 // stride
				(GLbyte*)0 + offsetof(UIShader::Vertex, UV) // offset
			);
			glEnableVertexAttribArray(sprite_program->UV_vec2);

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

void UIShader::draw(glm::vec3 const& a, glm::vec2 const& uv)
{
	attribs.emplace_back(a, uv);
	
}

void UIShader::draw_dialogue_box(glm::mat4x3 const& mat)
{
	// draw cube as three edge sets:
	//draw(mat * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec2(1.f, 0.f)); // top
	draw(mat * glm::vec4(-1.0f, 1.0f, 0.f, 1.0f), glm::vec2(0.f, 1.f)); // top left
	draw(mat * glm::vec4(1.0f, -1.0f, 0.f, 1.0f), glm::vec2(1.f,0.f)); // btm right
	draw(mat * glm::vec4(1.0f, 1.0f, 0.f, 1.0f), glm::vec2(1.f,1.f)); // top right

	//draw(mat * glm::vec4(-1.0f, -1.0f, 0.5f, 1.0f), glm::vec2(0.f, 0.f)); // bt, left
	//draw(mat * glm::vec4(1.0f, -1.0f, 0.5f, 1.0f), glm::vec2(1.f, 0.f)); // btm right
	//draw(mat * glm::vec4(-1.0f, 1.0f, 0.5f, 1.0f), glm::vec2(0.f, 1.f)); // top left
}


UIShader::~UIShader()
{
	if (attribs.empty())
		return;

	//glDisable(GL_DEPTH_TEST);
	// upload vertices to vertex_buffer:
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);														// set vertex_buffer as current
	glBufferData(GL_ARRAY_BUFFER, attribs.size() * sizeof(attribs[0]), attribs.data(), GL_STREAM_DRAW); // upload attribs array
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// set sprite_program as current program:
	glUseProgram(sprite_program->program);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set 2D texture to sample from :
	GLuint TEX_sampler2D = glGetUniformLocation(sprite_program->program, "TEX");
	glUniform1i(TEX_sampler2D, sprite_program->dialogue_texture - 1);

	glActiveTexture(GL_TEXTURE0 + sprite_program->dialogue_texture - 1);
	glBindTexture(GL_TEXTURE_2D, sprite_program->dialogue_texture);

	// upload OBJECT_TO_CLIP to the proper uniform location:
	glUniformMatrix4fv(sprite_program->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(world_to_clip));

	// use the mapping vertex_buffer_for_sprite_program to fetch vertex data:
	glBindVertexArray(vertex_buffer_for_sprite_program);

	// run the OpenGL pipeline:
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(attribs.size()));

	// reset vertex array to none:
	glDisable(GL_BLEND);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	// reset current program to none:
	glUseProgram(0);
	//glEnable(GL_DEPTH_TEST);
	GL_ERRORS(); // PARANOIA: make sure nothing strange happened during draw
}


