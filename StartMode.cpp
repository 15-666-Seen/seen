#include "StartMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "Load.hpp"
#include "Mesh.hpp"
#include "Scene.hpp"
#include "sound_prep.hpp"
#include "util.hpp"
#include "UIShaderProgram.hpp"
#include "data_path.hpp"
#include "gl_errors.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

GLuint bg_meshes_for_lit_color_texture_program = 0;
Load<MeshBuffer> bg_meshes(LoadTagDefault, []() -> MeshBuffer const * {
  MeshBuffer const *ret = new MeshBuffer(data_path("bg.pnct"));
  bg_meshes_for_lit_color_texture_program =
      ret->make_vao_for_program(lit_color_texture_program->program);
  return ret;
});


Load< Scene > bg_scene(LoadTagDefault, []() -> Scene const* {
    return new Scene(data_path("empty.scene"), [&](Scene& scene, Scene::Transform* transform, std::string const& mesh_name) {
        });
    });

StartMode::StartMode() : scene(*bg_scene) {
	next = "PlayMode";

    Mesh mesh1 = bg_meshes->lookup("Plane");
    {   // add image-containing plane to scene
        auto newTrans1 = new Scene::Transform();
        scene.drawables.emplace_back(newTrans1);
        background = &scene.drawables.back();

        background->pipeline = lit_color_texture_program_pipeline;
        background->pipeline.vao = bg_meshes_for_lit_color_texture_program;
        background->pipeline.type = mesh1.type;
        background->pipeline.start = mesh1.start;
        background->pipeline.count = mesh1.count;
    }
    // adjust positions
    background->tex_normal = background->pipeline.tex_name_to_glint["0_n"];
	background->transform->rotation = glm::angleAxis(glm::radians(-90.f), glm::vec3(0.0f, 1.0f, 0.f));
	background->transform->rotation *= glm::angleAxis(glm::radians(-90.f), glm::vec3(0.0f, 0.f, 1.f));
	background->transform->position = glm::vec3(0.0f, 0.0f, 0.8f);
	background->transform->scale = glm::vec3(0.7f, 0.4f, 0.5f);

    if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
    camera = &scene.cameras.front();
    camera->transform->position = glm::vec3(10.0f, 0.f, 0.0f);

    // initialize text 
    text.init();
    text.set_color(glm::vec3(0.8f, 0.7f, 0.7f));
	text.set_color2(glm::vec3(0.55f, 0.1f, 0.1f));
    text.set_text(texts[current_section].text);

	// set the background image
    if (lit_color_texture_program->tex_file_to_glint.find(texts[0].text_file) == lit_color_texture_program->tex_file_to_glint.end()) {
        throw std::runtime_error("Texture not found");
    }
    background->tex = lit_color_texture_program->tex_file_to_glint.find(texts[0].text_file)->second;
}

StartMode::~StartMode() {}

bool StartMode::handle_event(SDL_Event const &evt,
                            glm::uvec2 const &window_size) {

  if (evt.type == SDL_KEYDOWN) {
    if (evt.key.keysym.sym == SDLK_ESCAPE) {
      SDL_SetRelativeMouseMode(SDL_FALSE);
      gamePause = !gamePause;
      return true;
    } 
  } else if (evt.type == SDL_KEYUP) {
    if (evt.key.keysym.sym == SDLK_SPACE) {
      space.pressed = true;
      return true;
    } 
  } else if (evt.type == SDL_MOUSEBUTTONUP) {
	click.pressed = true;
	return true;
  }

  return false;
}

float text_elapsed = 0.0f;  // time for text animation
float time_elapsed = 0.0f;  // time count for the last ... dots
void StartMode::update(float elapsed) {
    if (gStop || gamePause) {
        return;
    }

    if ((space.pressed && current_section != 16) || (click.pressed && current_section != 16)
        || (current_section >= 12 && current_section < 16 && time_elapsed > 0.5f) ||
        (space.pressed && time_elapsed > 1.8f) || // force player to read tutorial
        (click.pressed && time_elapsed > 1.8f)) {

        if (!showfull) {
            showfull = true;
            text.show_full();
        } else {

            current_section++;
            if (current_section == 16) background->transform->position = glm::vec3(0.0f, 0.0f, 0.4f);
            if (current_section >= 17) {
                finished = true;
                return;
            }

            if (current_section < 12) showfull = false;

            text.set_text(texts[current_section].text);
            text.reset_time();
            if (lit_color_texture_program->tex_file_to_glint.find(texts[current_section].text_file) == lit_color_texture_program->tex_file_to_glint.end()) {
                throw std::runtime_error("Texture not found");
            }
            background->tex = lit_color_texture_program->tex_file_to_glint.find(texts[current_section].text_file)->second;

            time_elapsed = 0.0f;

            if (current_section == 2) {
                text.set_color2_index(14);
            }
        }
  }
  if (current_section >= 12 && time_elapsed < 2.0f) {
	  time_elapsed += elapsed;
  }

    // reset button press counters:
  space.downs = 0;
  space.pressed = false;
  click.downs = 0;
  click.pressed = false;

  //bool advanced = storyManager->advanceStory();
  text_elapsed = elapsed;
}

void StartMode::draw(glm::uvec2 const &drawable_size) {
  if (gamePause)
    return;
  // update camera aspect ratio for drawable:
  camera->aspect = float(drawable_size.x) / float(drawable_size.y);

  // set up light type and position for lit_color_texture_program:
  // TODO: consider using the Light(s) in the scene to do this
  glUseProgram(lit_color_texture_program->program);
  glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 5);
  glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1,
               glm::value_ptr(glm::vec3(0.0f, 0.0f, -1.0f)));
  glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1,
               glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));

  glUseProgram(0);

  glClearColor(0.f, 0.f, 0.f, 1.0f);
  glClearDepth(1.0f); // 1.0 is actually the default value to clear the depth
                      // buffer to, but FYI you can change it.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); // this is the default depth comparison function, but
                        // FYI you can change it.

  scene.draw(*camera);

    glDisable(GL_DEPTH_TEST);
    // render UI and text
  glm::mat4 world_to_clip =
      glm::mat4(1.0f / camera->aspect, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
          0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f); // world to clip

  // dont render this part on tutorial text
  if (current_section < 16) {
      UIShader sprites(world_to_clip);

      sprites.draw_dialogue_box(glm::mat4(0.9f * camera->aspect, 0.0f, 0.0f, 0.0f, 0.0f,
          0.3f, 0.0f, 0.f, 0.0f, 0.0f, 1.0f, 0.0f,
          0.0f, -0.65f, 0.0f, 1.0f));
  }
  //sprites.~UIShader(); // explicitly draws any sprite

  float x = drawable_size.x * 0.1f;
  float y = drawable_size.y * 0.22f;
  //float width = drawable_size.x * 0.8f;
  text.set_bound(drawable_size.x * 0.9f);
  text.draw(text_elapsed, drawable_size, glm::vec2(x, y), 1.1f, current_section < 13);

  GL_ERRORS();
}


