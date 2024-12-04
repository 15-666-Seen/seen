#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "data_path.hpp"
#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

#include <iostream>
#include <map>
#include <string>

// freetype
#include <ft2build.h>
#include FT_FREETYPE_H

// harfbuzz
#include <hb-ft.h>
#include <hb.h>

// Adapted from
// https://github.com/cmu15466-gsilvera/15-466-f22-base4/blob/main/Text.hpp

// referenced the following examples/documentation for inspiration
// https://learnopengl.com/In-Practice/Text-Rendering
// https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c

struct Character {
  unsigned int TextureID; // ID handle of the glyph texture
  glm::ivec2 Size;        // Size of glyph
  glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
  unsigned int Advance;   // Offset to advance to next glyph

  // construct a character from a char request and typeface (glyph) collection
  static Character Load(hb_codepoint_t request, FT_Face typeface);
};

struct Text {
  std::string text_file = "RubikDistressed-Regular.ttf";
  FT_F26Dot6 font_size = 2;
  FT_F26Dot6 font_scale = 2; // number of units per pixel

  float anim_time = 1.0f; // time (seconds) to complete animation
  float time = 0.f;       // current time for new text

  float right_bound = -1.f; // right bound of the text

  // freetype typeface
  FT_Face typeface;

  // harfbuzz for shaping the text
  hb_buffer_t *hb_buffer = nullptr;
  hb_font_t *hb_typeface = nullptr;

  // for drawing
  glm::mat4 projection;
  GLuint draw_text_program = 0;
  GLuint VAO = 0;
  GLuint VBO = 0;

  // for actual text content
  std::string text_content;

  // using hb_codepoint_t as the codepoint type from
  // hb_buffer_get_glyph_positions
  std::map<hb_codepoint_t, Character> chars;

  glm::vec3 color = glm::vec3(0.3f, 0.01f, 0.01f);
  glm::vec3 color2 = glm::vec3(0.3f, 0.f, 0.f);
  bool color2_bit_array[64] = {false};

  bool init_flag = false; // flag to check if the Text has been initialized

  // Text();

  void init(std::string ttf_file = "");

  void update_buffer(const std::string &new_text);

  void set_text(const std::string &new_text);

  void set_bound(float new_bound);

  void set_color(const glm::vec3 new_color);

  void set_color2(const glm::vec3 new_color);

  void set_color2_index(uint8_t index);

  void reset_time();

  void highlight();

  void set_font_size(FT_F26Dot6 new_font_size, FT_F26Dot6 new_font_scale,
                     bool override = false);

  Character get_character(unsigned int i, hb_glyph_info_t* glyph_info);

  void draw(float dt, const glm::vec2& drawable_size, const glm::vec2& pos, float ss_scale, bool animate = false);
};
