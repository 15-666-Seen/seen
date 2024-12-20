
#include "Text.hpp"

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

// construct a character from a char request and typeface (glyph) collection
Character Character::Load(hb_codepoint_t request, FT_Face typeface) {
  // taken almost verbatim from
  // https://learnopengl.com/In-Practice/Text-Rendering
  if (FT_Load_Glyph(typeface, static_cast<FT_UInt>(request), FT_LOAD_RENDER))
    throw std::runtime_error("Failed to load glyph: " +
                             std::to_string(request));
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, typeface->glyph->bitmap.width,
               typeface->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
               typeface->glyph->bitmap.buffer);
  // set tex options
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR); // not using mipmap
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                  GL_LINEAR); // not using mipmap
  glBindTexture(GL_TEXTURE_2D, 0);
  GL_ERRORS();
  // now store character for later use
  return {
      tex,
      glm::ivec2(typeface->glyph->bitmap.width, typeface->glyph->bitmap.rows),
      glm::ivec2(typeface->glyph->bitmap_left, typeface->glyph->bitmap_top),
      (unsigned int)(typeface->glyph->advance.x)};
}

// Text::Text();

void Text::init(std::string ttf_file) {
  if (init_flag) {
    return;
  }
  if (ttf_file != "") {
    text_file = ttf_file;
  }
  // (try to) load freetype library and typeface
  {
    FT_Library ftlibrary;
    if (FT_Init_FreeType(&ftlibrary))
      throw std::runtime_error("Failed to initialize FreeType library");
    if (FT_New_Face(ftlibrary, const_cast<char *>(data_path(text_file).c_str()),
                    0, &typeface))
      throw std::runtime_error("Failed to load font from typeface \"" +
                               text_file + "\"");
  }

  // create shaders for rendering
  {
    // https://learnopengl.com/code_viewer_gh.php?code=src/7.in_practice/2.text_rendering/text.vs
    const auto vertex_shader =
        "#version 330 core\n"
        "in vec4 vertex;\n" // layout (location = 0) in vec4 vertex;\n
        "out vec2 TexCoords;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = projection * vec4(vertex.xyz, 1.0);\n"
        "    TexCoords = vertex.zw;\n"
        "}\n";

    // https://learnopengl.com/code_viewer_gh.php?code=src/7.in_practice/2.text_rendering/text.fs
    const auto fragment_shader = "#version 330 core\n"
                                 "in vec2 TexCoords;\n"
                                 "out vec4 color;\n"
                                 "uniform sampler2D text;\n"
                                 "uniform vec3 textColor;\n"
                                 "void main()\n"
                                 "{\n"
                                 "	vec4 sampled = vec4(1.0, 1.0, 1.0, "
                                 "texture(text, TexCoords).r);\n"
                                 "   color = vec4(textColor, 1.0) * sampled;\n"
                                 "}\n";
    draw_text_program = gl_compile_program(vertex_shader, fragment_shader);

    // std::cout << "Text program" << (int)draw_text_program << std::endl;
  }

  // set initial harfbuzz buffer params/data
  {
    std::cout << "setting font size to " << font_size << " and scale to "
              << font_scale << std::endl;
    /* FT_F26Dot6 zero = 0;*/
    FT_Set_Char_Size(typeface, 0, (FT_F26Dot6)font_size * font_scale, 0,
                     0); // 64 units per pixel
    if (FT_Load_Char(typeface, 'X', FT_LOAD_RENDER))
      throw std::runtime_error("Failed to load char \"X\" from typeface!");
    // reset these characters to regenerate them with the new font size
    chars.clear();
    hb_typeface = hb_ft_font_create(typeface, nullptr);
    // set_text("Initial text"); /// TODO: remove
  }

  // initialize openGL for rendering
  {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //glDisable(GL_BLEND);
    GL_ERRORS();
  }

  init_flag = true;
}

void Text::update_buffer(const std::string &new_text) {
  /// TODO add more public setters
  if (hb_buffer != nullptr) {
    hb_buffer_destroy(hb_buffer);
  }
  hb_buffer = hb_buffer_create();
  hb_buffer_add_utf8(hb_buffer, new_text.c_str(), -1, 0, -1);
  hb_buffer_set_direction(hb_buffer, HB_DIRECTION_LTR);
  hb_buffer_set_script(hb_buffer, HB_SCRIPT_LATIN);
  hb_buffer_set_language(hb_buffer, hb_language_from_string("en", -1));
  if (hb_typeface == nullptr) {
    throw std::runtime_error("Harfbuzz typeface is null!");
  }
  hb_shape(hb_typeface, hb_buffer, NULL, 0);
}

void Text::set_text(const std::string &new_text) {
  if (!init_flag) {
    init();
  }
  text_content = new_text;
  update_buffer(text_content);
  anim_time = text_content.size() / 40.f; // reset animation time

   for (int i = 0; i < 64; i++) {
  	color2_bit_array[i] = false;
  }
}

void Text::set_bound(float new_bound) { right_bound = new_bound; }

void Text::set_color(const glm::vec3 new_color) { color = new_color; }

void Text::set_color2(const glm::vec3 new_color) { color2 = new_color; }

void Text::set_color2_index(uint8_t index) {
  assert(index < 64);
  color2_bit_array[index] = true;
}

void Text::reset_time() { time = 0.f; }
void Text::show_full() { time = 100.f; }

void Text::highlight() {
  // show some effect for highlighting
  update_buffer("**" + text_content + "**");
}

void Text::set_font_size(FT_F26Dot6 new_font_size, FT_F26Dot6 new_font_scale,
                         bool override) {
  if (!init_flag) {
    init();
  }
  if ((font_size != new_font_size || font_scale != new_font_scale) ||
      override) {
    font_size = new_font_size;
    font_scale = new_font_scale;
    std::cout << "setting font size to " << font_size << " and scale to "
              << font_scale << std::endl;
    /* FT_F26Dot6 zero = 0;*/
    FT_Set_Char_Size(typeface, 0, (FT_F26Dot6)font_size * font_scale, 0,
                     0); // 64 units per pixel
    if (FT_Load_Char(typeface, 'X', FT_LOAD_RENDER))
      throw std::runtime_error("Failed to load char \"X\" from typeface!");
    // reset these characters to regenerate them with the new font size
    chars.clear();
  }
}

//void Text::draw(float dt, const glm::vec2& drawable_size, float width,
//    const glm::vec2& pos, float ss_scale, bool animate) {
//    return;
//}


Character Text::get_character(unsigned int i, hb_glyph_info_t* glyph_info) {
    hb_codepoint_t char_req = glyph_info[i].codepoint;
    if (chars.find(char_req) == chars.end()) {
        Character ch = Character::Load(char_req, typeface);
        chars.insert(std::pair<hb_codepoint_t, Character>(char_req, ch));
    }

    const Character& ch = chars[char_req];
	return ch;
}

void Text::draw(float dt, const glm::vec2 &drawable_size, const glm::vec2 &pos, float ss_scale, bool animate) {
  // drawable_size - window size
  // width - how wide the displayed string gets to be
  // pos - position in screenspace where the text gets rendered
  // ss_scale - screenspace scale (lower quality but cheap)
  // primary_color - RGB color for the primary color
  if (!init_flag) {
    init();
  }

  if (text_content.empty() || text_content == "") {
    return;
  }
  if (!animate)
    dt = 100.f; // if not animating, just draw the text

  FT_F26Dot6 new_font_scale =
      (FT_F26Dot6)((float)drawable_size.y * (32.0f / 720.0f) *
                   32.f); // scale font size off window height
  set_font_size(font_size, new_font_scale);

  projection = glm::ortho(0.0f, drawable_size.x, 0.0f, drawable_size.y);

  glUseProgram(draw_text_program);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // Use member variable color2 for the secondary color
  glUniformMatrix4fv(glGetUniformLocation(draw_text_program, "projection"), 1,
                     GL_FALSE, glm::value_ptr(projection) );    //&projection[0][0]
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);

  unsigned int num_chars = 0;
  hb_glyph_info_t *glyph_info =
      hb_buffer_get_glyph_infos(hb_buffer, &num_chars);
  if (glyph_info == nullptr) {
    throw std::runtime_error("Unable to get glyph info from buffer!");
  }

  float char_x = pos.x;
  float char_y = pos.y;

  time += dt;
  float amnt = std::min(time / anim_time, 1.f);

  unsigned int word_index = 0; // Index to track which word we're on
  bool use_secondary = color2_bit_array[word_index];

  // Set initial color
  glUniform3f(glGetUniformLocation(draw_text_program, "textColor"),
              use_secondary ? color2.x : color.x,
              use_secondary ? color2.y : color.y,
              use_secondary ? color2.z : color.z);

  for (unsigned int i = 1; i <= static_cast<unsigned int>(amnt * num_chars);
       i++) {
    
	  Character ch = get_character(i-1, glyph_info);

    float xpos = char_x + ch.Bearing.x * ss_scale;
    float ypos = char_y - (ch.Size.y - ch.Bearing.y) * ss_scale;
    float w = ch.Size.x * ss_scale;
    float h = ch.Size.y * ss_scale;

    static float y_size = -1.f;
    if (y_size < ch.Size.y) {
      y_size = ch.Size.y * 1.2f;
    }

    float vertices[6][4] = {
        {xpos, ypos + h, 0.0f, 0.0f},    {xpos, ypos, 0.0f, 1.0f},
        {xpos + w, ypos, 1.0f, 1.0f},

        {xpos, ypos + h, 0.0f, 0.0f},    {xpos + w, ypos, 1.0f, 1.0f},
        {xpos + w, ypos + h, 1.0f, 0.0f}};

    // glDepthMask(GL_FALSE);

    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    if (text_content[i] == ' ') { 

      unsigned int j = i + 1;
	  float next_word_width = (ch.Advance >> 6) * ss_scale;

	  for (; text_content[j] != ' ' && j < text_content.size(); j++) {
		  Character next_ch = get_character(j, glyph_info);
		  next_word_width += (next_ch.Advance >> 6) * ss_scale;
      }

      if ((char_x + next_word_width) >= right_bound) {
        char_x = pos.x;
        char_y -= (y_size + 5.0f);
	  }
	  else
		  char_x += (ch.Advance >> 6) * ss_scale;
    }
    else char_x += (ch.Advance >> 6) * ss_scale;

    if (text_content[i] == ' ') {
      // Move to the next word
      word_index++;
      use_secondary = word_index < 64 ? color2_bit_array[word_index] : false;

      // Update the text color
      glUniform3f(glGetUniformLocation(draw_text_program, "textColor"),
                  use_secondary ? color2.x : color.x,
                  use_secondary ? color2.y : color.y,
                  use_secondary ? color2.z : color.z);
    }
  }

  glDisable(GL_BLEND);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
  GL_ERRORS();
}
