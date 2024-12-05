#include "EndMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "Load.hpp"
#include "Mesh.hpp"
#include "Scene.hpp"
#include "UIShaderProgram.hpp"
#include "data_path.hpp"
#include "gl_errors.hpp"
#include "sound_prep.hpp"
#include "util.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

//GLuint bg_meshes_for_lit_color_texture_program = 0;
//Load<MeshBuffer> bg_meshes(LoadTagDefault, []() -> MeshBuffer const * {
//  MeshBuffer const *ret = new MeshBuffer(data_path("bg.pnct"));
//  bg_meshes_for_lit_color_texture_program =
//      ret->make_vao_for_program(lit_color_texture_program->program);
//  return ret;
//});


Load< Scene > ebg_scene(LoadTagDefault, []() -> Scene const* {
    return new Scene(data_path("empty.scene"), [&](Scene& scene, Scene::Transform* transform, std::string const& mesh_name) {
        });
    });

EndMode::EndMode() : scene(*ebg_scene) {

    if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
    camera = &scene.cameras.front();
    //camera->transform->position = glm::vec3(10.0f, 0.f, 0.0f);

	text.init();
    text.set_color(glm::vec3(0.8f, 0.8f, 0.8f));
    text.set_text(texts[current_section].text);
	instructions.init();
    instructions.set_text("Press [Q] to Exit    Press [E] to Restart");
}

EndMode::~EndMode() {}

bool EndMode::handle_event(SDL_Event const &evt,
                            glm::uvec2 const &window_size) {
    if (evt.type == SDL_KEYDOWN) {
        if (evt.key.keysym.sym == SDLK_ESCAPE) {
            SDL_SetRelativeMouseMode(SDL_FALSE);
            gamePause = !gamePause;
            return true;
        }
        else if (evt.key.keysym.sym == SDLK_q) {
            q.pressed = true;
            return true;
        }
        else if (evt.key.keysym.sym == SDLK_e) {
            e.pressed = true;
            return true;
        }
        else if (evt.key.keysym.sym == SDLK_SPACE) {
            space.pressed = true;
            return true;
        }
    } else if (evt.type == SDL_MOUSEBUTTONDOWN) {
            click.pressed = true;
            return true;
    }

  return false;
}


void EndMode::update(float elapsed) {
  if (gStop || gamePause) {
    return;
  }

  if (q.pressed) {
      finished = true;
      next = "";
      //std::cout << "Q pressed" << std::endl;
      return;
  }
  if (e.pressed) {
      finished = true;
      next = "PlayMode";
      //std::cout << "E pressed" << std::endl;
      return;
  }

  if (space.pressed || click.pressed) {
	  current_section++;
	  if (current_section >= 4) {
          current_section = 3;
		  /*finished = true;
		  return;*/
	  }
	  text.set_text(texts[current_section].text);
	  text.reset_time();
      /*if (lit_color_texture_program->tex_file_to_glint.find(texts[current_section].text_file) == lit_color_texture_program->tex_file_to_glint.end()) {
          throw std::runtime_error("Texture not found");
      }*/
      //background->tex = lit_color_texture_program->tex_file_to_glint.find(texts[current_section].text_file)->second;

  }
    // reset button press counters:
  space.downs = 0;
  space.pressed = false;
  click.downs = 0;
  click.pressed = false;

  //bool advanced = storyManager->advanceStory();
  EndMode::text_elapsed = elapsed;
}

void EndMode::draw(glm::uvec2 const &drawable_size) {
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

  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glClearDepth(1.0f); // 1.0 is actually the default value to clear the depth
                      // buffer to, but FYI you can change it.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); // this is the default depth comparison function, but
                        // FYI you can change it.
  scene.draw(*camera);

  glDisable(GL_DEPTH_TEST);
  // render UI and text
  glm::mat4 world_to_clip = glm::mat4(
      1.0f / camera->aspect, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f); // world to clip
  {
      UIShader sprites(world_to_clip);

      sprites.draw_dialogue_box(glm::mat4(0.9f * camera->aspect, 0.0f, 0.0f, 0.0f,
          0.0f, 0.3f, 0.0f, 0.f, 0.0f, 0.0f, 1.0f,
          0.0f, 0.0f, -0.65f, 0.0f, 1.0f));
  }
  //sprites.~UIShader(); // explicitly draws any sprite

  float x = drawable_size.x * 0.1f;
  float y = drawable_size.y * 0.22f;
  //float width = drawable_size.x * 0.8f;
  text.set_bound(drawable_size.x * 0.9f);
  text.draw(EndMode::text_elapsed, drawable_size, glm::vec2(x, y), 1.1f, true);

  x = drawable_size.x * 0.7f;
  y = drawable_size.y * 0.5f;
  instructions.set_bound(drawable_size.x * 0.98f);
  instructions.draw(EndMode::text_elapsed, drawable_size, glm::vec2(x, y), 1.f, false);

  GL_ERRORS();
}
