#include "LitColorTextureProgram.hpp"

#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

#include <filesystem>

Scene::Drawable::Pipeline lit_color_texture_program_pipeline;

Load<LitColorTextureProgram>
lit_color_texture_program(LoadTagEarly, []() -> LitColorTextureProgram const * {
  LitColorTextureProgram *ret = new LitColorTextureProgram();

  //----- build the pipeline template -----
  lit_color_texture_program_pipeline.program = ret->program;

  lit_color_texture_program_pipeline.OBJECT_TO_CLIP_mat4 =
      ret->OBJECT_TO_CLIP_mat4;
  lit_color_texture_program_pipeline.OBJECT_TO_LIGHT_mat4x3 =
      ret->OBJECT_TO_LIGHT_mat4x3;
  lit_color_texture_program_pipeline.NORMAL_TO_LIGHT_mat3 =
      ret->NORMAL_TO_LIGHT_mat3;

  /* This will be used later if/when we build a light loop into the Scene:
  lit_color_texture_program_pipeline.LIGHT_TYPE_int = ret->LIGHT_TYPE_int;
  lit_color_texture_program_pipeline.LIGHT_LOCATION_vec3 =
  ret->LIGHT_LOCATION_vec3;
  lit_color_texture_program_pipeline.LIGHT_DIRECTION_vec3 =
  ret->LIGHT_DIRECTION_vec3;
  lit_color_texture_program_pipeline.LIGHT_ENERGY_vec3 = ret->LIGHT_ENERGY_vec3;
  lit_color_texture_program_pipeline.LIGHT_CUTOFF_float =
  ret->LIGHT_CUTOFF_float;
  */

  // make a 1-pixel white texture to bind by default:
  GLuint tex;
  glGenTextures(1, &tex);

  glBindTexture(GL_TEXTURE_2D, tex);
  std::vector<glm::u8vec4> tex_data(1, glm::u8vec4(0xff));
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               tex_data.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);

  lit_color_texture_program_pipeline.textures[0].texture = tex;
  lit_color_texture_program_pipeline.textures[0].target = GL_TEXTURE_2D;
  lit_color_texture_program_pipeline.tex_name_to_glint["0"] = tex;

  // make 1-pixel normal map
  GLuint texn;
  glGenTextures(1, &texn);

  glBindTexture(GL_TEXTURE_2D, texn);
  std::vector<glm::u8vec4> tex_data_n(1, glm::u8vec4(0x7f, 0x7f, 0xff,0xff));
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
      tex_data_n.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);

  lit_color_texture_program_pipeline.textures.push_back({ texn, GL_TEXTURE_2D });
  lit_color_texture_program_pipeline.tex_name_to_glint["0_n"] = texn;

  // load all in folder
  std::string filepath = data_path("textures/lit_textures");
  std::vector<std::string> filenames;

  try {
    for (const auto &entry : std::filesystem::directory_iterator(filepath)) {
      if (entry.is_regular_file()) {
        std::string extension = entry.path().extension().string();
        if (extension == ".png" ||
            extension == ".jpg") { // only want png or jpg files
          filenames.push_back(entry.path().filename().string());
        }
      }
    }
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Filepath doesn't exist: " << e.what() << std::endl;
  }

  for (const auto &f : filenames) {
    GLuint gli = ret->LoadTexture(filepath, f);
    lit_color_texture_program_pipeline.textures.push_back({gli, GL_TEXTURE_2D});
    lit_color_texture_program_pipeline.tex_name_to_glint[f] = gli;
	ret->tex_file_to_glint.emplace(f, gli);
  }

  return ret;
});

LitColorTextureProgram::LitColorTextureProgram() {
  // Compile vertex and fragment shaders using the convenient
  // 'gl_compile_program' helper function:
  program = gl_compile_program(
      // vertex shader:
      "#version 330\n"
      "uniform mat4 OBJECT_TO_CLIP;\n"
      "uniform mat4x3 OBJECT_TO_LIGHT;\n"
      "uniform mat3 NORMAL_TO_LIGHT;\n"
      "in vec4 Position;\n"
      "in vec3 Normal;\n"
      "in vec3 Tangent;\n"
      "in vec3 Bitangent;\n"
      "in vec4 Color;\n"
      "in vec2 TexCoord;\n"
      "out vec3 position;\n"
      "out mat3 TBN;\n"
      "out vec4 color;\n"
      "out vec2 texCoord;\n"
      "void main() {\n"
      "	gl_Position = OBJECT_TO_CLIP * Position;\n"
      "	position = OBJECT_TO_LIGHT * Position;\n"
      "	TBN = mat3(NORMAL_TO_LIGHT * Tangent, NORMAL_TO_LIGHT * Bitangent, NORMAL_TO_LIGHT * Normal);\n"
      "	color = Color;\n"
      "	texCoord = TexCoord;\n"
      "}\n",

      // fragment shader:
      "#version 330\n"
      "uniform sampler2D TEX;\n"
      "uniform sampler2D TEX_NORMAL;\n"
      "uniform int LIGHT_TYPE;\n"
      "uniform vec3 LIGHT_LOCATION;\n"
      "uniform vec3 LIGHT_DIRECTION;\n"
      "uniform vec3 LIGHT_ENERGY;\n"
      "uniform float LIGHT_CUTOFF;\n"
      "in vec3 position;\n"
      "in mat3 TBN;\n"
      "in vec4 color;\n"
      "in vec2 texCoord;\n"
      "out vec4 fragColor;\n"
      "void main() {\n"
      " vec3 normal = texture(TEX_NORMAL, fract(-texCoord)).rgb; \n"
      " normal = normalize(normal * 2.0 - 1.0);\n"
      "	vec3 n = normalize(TBN * normal);\n"
      "	vec3 e;\n"
      "	if (LIGHT_TYPE == 0) { //point light \n"
      "		vec3 l = (LIGHT_LOCATION - position);\n"
      "		float dis2 = dot(l,l);\n"
      "		l = normalize(l);\n"
      "		float nl = max(0.0, dot(n, l)) / max(1.0, dis2);\n"
      "		e = nl * LIGHT_ENERGY;\n"
      "	} else if (LIGHT_TYPE == 1) { //hemi light \n"
      "		e = (dot(n,-LIGHT_DIRECTION) * 0.5 + 0.5) * LIGHT_ENERGY;\n"
      "		e.x = max(0.15, e.x);\n"
      "		e.y = max(0.15, e.y);\n"
      "		e.z = max(0.15, e.z);\n"
      "	} else if (LIGHT_TYPE == 2) { //spot light \n"
      "		vec3 l = (LIGHT_LOCATION - position);\n"
      "		float dis2 = dot(l,l);\n"
      "		l = normalize(l);\n"
      "		float nl = max(0.0, dot(n, l)) / max(1.0, dis2);\n"
      "		float c = dot(l,-LIGHT_DIRECTION);\n"
      "		nl *= smoothstep(LIGHT_CUTOFF,mix(LIGHT_CUTOFF,1.0,0.1), c);\n"
      "		e = nl * LIGHT_ENERGY;\n"
      "	} else if (LIGHT_TYPE == 3) { //(LIGHT_TYPE == 3) //directional light\n"
      "		e = max(0.0, dot(n,-LIGHT_DIRECTION)) * LIGHT_ENERGY;\n"
	  " } else if (LIGHT_TYPE == 5) { //no light\n"
	  "		e = vec3(1.0);\n"
      "	} else { // LIGHT_TYPE == 4 shadow light\n"
      "   float distance = pow(length(position), 2.0) + 0.01f;\n"
      "	  vec3 l = normalize(LIGHT_LOCATION - position);\n"
      "   e = min(1.0, 2.0/distance) * LIGHT_ENERGY * max(0.0, dot(n, l));\n"
      "	  e.x = max(0.02, e.x);\n"
      "	  e.y = max(0.02, e.y);\n"
      "	  e.z = max(0.02, e.z);\n"
      " }\n"
      "	vec4 albedo = texture(TEX, fract(-texCoord)) * color; \n"
      "	fragColor = vec4(e*albedo.rgb, albedo.a);\n"
      "}\n");
  // As you can see above, adjacent strings in C/C++ are concatenated.
  // this is very useful for writing long shader programs inline.

  // look up the locations of vertex attributes:
  Position_vec4 = glGetAttribLocation(program, "Position");
  Normal_vec3 = glGetAttribLocation(program, "Normal");
  Tangent_vec3 = glGetAttribLocation(program, "Tangent");
  
  Bitangent_vec3 = glGetAttribLocation(program, "Bitangent");
  Color_vec4 = glGetAttribLocation(program, "Color");
  TexCoord_vec2 = glGetAttribLocation(program, "TexCoord");

  // look up the locations of uniforms:
  OBJECT_TO_CLIP_mat4 = glGetUniformLocation(program, "OBJECT_TO_CLIP");
  OBJECT_TO_LIGHT_mat4x3 = glGetUniformLocation(program, "OBJECT_TO_LIGHT");
  NORMAL_TO_LIGHT_mat3 = glGetUniformLocation(program, "NORMAL_TO_LIGHT");

  LIGHT_TYPE_int = glGetUniformLocation(program, "LIGHT_TYPE");
  LIGHT_LOCATION_vec3 = glGetUniformLocation(program, "LIGHT_LOCATION");
  LIGHT_DIRECTION_vec3 = glGetUniformLocation(program, "LIGHT_DIRECTION");
  LIGHT_ENERGY_vec3 = glGetUniformLocation(program, "LIGHT_ENERGY");
  LIGHT_CUTOFF_float = glGetUniformLocation(program, "LIGHT_CUTOFF");

  GLuint TEX_sampler2D = glGetUniformLocation(program, "TEX");
  GLuint TEX_sampler2D_Normal = glGetUniformLocation(program, "TEX_NORMAL");

  // set TEX to always refer to texture binding zero:
  glUseProgram(program);
  // bind program -- glUniform* calls refer to this program now

  glUniform1i(TEX_sampler2D, 0); // set TEX to sample from GL_TEXTURE0
  glUniform1i(TEX_sampler2D_Normal, 0);

  glUseProgram(0); // unbind program -- glUniform* calls refer to ??? now
}

LitColorTextureProgram::~LitColorTextureProgram() {
  glDeleteProgram(program);
  program = 0;
}

//--------
GLuint LitColorTextureProgram::LoadTexture(const std::string filepath,
                                           const std::string filename) {

  int width, height, channels;
  unsigned char *data = loadImg(filepath, filename, width, height, channels);

  GLuint tex;
  glGenTextures(1, &tex);

  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);

  freeImg(data);

  return tex;
}
