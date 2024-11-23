#include "StartMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "Load.hpp"
#include "Mesh.hpp"
#include "Scene.hpp"
#include "sound_prep.hpp"
#include "util.hpp"

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

//Load<Scene> phonebank_scene(LoadTagDefault, []() -> Scene const * {
//  return new Scene(data_path("house.scene"), [&](Scene &scene,
//                                                 Scene::Transform *transform,
//                                                 std::string const &mesh_name) {
//    Mesh const &mesh = phonebank_meshes->lookup(mesh_name);
//
//    scene.drawables.emplace_back(transform, mesh_name);
//    Scene::Drawable &drawable = scene.drawables.back();
//
//    drawable.pipeline = lit_color_texture_program_pipeline;
//
//    drawable.pipeline.vao = phonebank_meshes_for_lit_color_texture_program;
//    drawable.pipeline.type = mesh.type;
//    drawable.pipeline.start = mesh.start;
//    drawable.pipeline.count = mesh.count;
//
//
//    drawable.tex = drawable.pipeline.tex_name_to_glint[mesh.tex];
//    drawable.tex_normal = drawable.pipeline.tex_name_to_glint[getNormalMapName(mesh.tex)];
//
//    /* some asserts to ensure the shader is loaded correctly */
//    auto s = drawable.pipeline.tex_name_to_glint.find("0");
//    assert(s != drawable.pipeline.tex_name_to_glint.end());
//    s = drawable.pipeline.tex_name_to_glint.find("0_n");
//    assert(s != drawable.pipeline.tex_name_to_glint.end());
//
//    // if mesh textures are missing
//    if (drawable.tex == 0) {
//      printf("Texture missing in scene loading for mesh %s named %s\n",
//             mesh_name.c_str(), mesh.tex.c_str());
//      drawable.tex = drawable.pipeline.tex_name_to_glint["0"];
//    }
//
//    //if (drawable.tex_normal == 0) {
//    //    printf("No normal map for mesh %s named %s\n", mesh_name.c_str(), getNormalMapName(mesh.tex).c_str());
//    //    drawable.tex_normal = drawable.pipeline.tex_name_to_glint["0_n"];
//    //}
//
//  });
//});
Load< Scene > bg_scene(LoadTagDefault, []() -> Scene const* {
    return new Scene(data_path("empty.scene"), [&](Scene& scene, Scene::Transform* transform, std::string const& mesh_name) {
        });
    });

StartMode::StartMode() : scene(*bg_scene) {

    Mesh mesh1 = bg_meshes->lookup("Plane");

    auto newTrans1 = new Scene::Transform();
    scene.drawables.emplace_back(newTrans1);
    background = &scene.drawables.back();

    background->pipeline = lit_color_texture_program_pipeline;
    background->pipeline.vao = bg_meshes_for_lit_color_texture_program;
    background->pipeline.type = mesh1.type;
    background->pipeline.start = mesh1.start;
    background->pipeline.count = mesh1.count;

	if (lit_color_texture_program->tex_file_to_glint.find("IMG_3075.png") == lit_color_texture_program->tex_file_to_glint.end()) {
		throw std::runtime_error("Texture not found");
	}
	background->tex = lit_color_texture_program->tex_file_to_glint.find("fabric.png")->second;
	std::cout << "Texture: " << background->tex << std::endl;
    background->tex_normal = background->pipeline.tex_name_to_glint["0_n"];
	background->transform->rotation = glm::angleAxis(glm::radians(-90.f), glm::vec3(0.0f, 1.0f, 0.f));

	background->transform->scale = glm::vec3(0.5f, 0.5f, 0.5f);

    if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
    camera = &scene.cameras.front();
    camera->transform->position = glm::vec3(10.0f, 0.f, 0.0f);
    /*camera->transform->rotation *= glm::angleAxis(
        glm::radians(-15.0f),
        glm::vec3(1.0f, 0.0f, 0.0f));*/

  // UI
  gameplayUI = new GameplayUI();

}

StartMode::~StartMode() {}

bool StartMode::handle_event(SDL_Event const &evt,
                            glm::uvec2 const &window_size) {

  if (evt.type == SDL_KEYDOWN) {
    if (evt.key.keysym.sym == SDLK_ESCAPE) {
      SDL_SetRelativeMouseMode(SDL_FALSE);
      gamePause = !gamePause;
      return true;
    } else if (evt.key.keysym.sym == SDLK_a) {
      left.downs += 1;
      left.pressed = true;
      return true;
    } else if (evt.key.keysym.sym == SDLK_d) {
      right.downs += 1;
      right.pressed = true;
      return true;
    } else if (evt.key.keysym.sym == SDLK_w) {
      up.downs += 1;
      up.pressed = true;
      return true;
    } else if (evt.key.keysym.sym == SDLK_s) {
      down.downs += 1;
      down.pressed = true;
      return true;
    } else if (evt.key.keysym.sym == SDLK_f) {
      F.downs += 1;
      F.pressed = true;
      return true;
    }
  } else if (evt.type == SDL_KEYUP) {
    if (evt.key.keysym.sym == SDLK_a) {
      left.pressed = false;
      return true;
    } else if (evt.key.keysym.sym == SDLK_d) {
      right.pressed = false;
      return true;
    } else if (evt.key.keysym.sym == SDLK_w) {
      up.pressed = false;
      return true;
    } else if (evt.key.keysym.sym == SDLK_s) {
      down.pressed = false;
      return true;
    } else {
      F.pressed = false;
      return true;
    }
  } else if (evt.type == SDL_MOUSEBUTTONDOWN) {
      int x, y;
      SDL_GetMouseState(&x, &y);
      gameplayUI->InteractOnClick(x, y);
      if (gameplayUI->dialogueText.size() == 0) {
          gStop = false;
      }

      if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
          SDL_SetRelativeMouseMode(SDL_TRUE);
          return true;
      }
  } else if (evt.type == SDL_MOUSEMOTION) {
      if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
          glm::vec2 motion = glm::vec2(
              evt.motion.xrel / float(window_size.y),
              -evt.motion.yrel / float(window_size.y)
          );
          camera->transform->rotation = glm::normalize(
              camera->transform->rotation
              * glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 1.0f, 0.0f))
              * glm::angleAxis(motion.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f))
          );
          //std::cout << "\r" << camera->transform->rotation[0] << ", " << camera->transform->rotation[1] << ", " << camera->transform->rotation[2] << std::endl;
          return true;
      }
  }

  
  //} else if (evt.type == SDL_MOUSEMOTION) {
  //  if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
  //    if (gStop) {
  //      return true;
  //    }

  //    glm::vec2 motion = glm::vec2(evt.motion.xrel / float(window_size.y),
  //                                 evt.motion.yrel / float(window_size.y));

  //    auto &camera = player.camera;

  //    camera->yaw += motion.x * camera->fovy;
  //    camera->pitch += motion.y * camera->fovy;
  //    // camera pitch should be limited to (-pi/2, pi/2)
  //    camera->pitch =
  //        glm::clamp(camera->pitch, -glm::pi<float>() / 2.0f + 0.0001f,
  //                   glm::pi<float>() / 2.0f - 0.0001f);

  //    // use it to get the new rotation, first by having lookAt
  //    glm::vec3 lookAt = camera->getLookAt();

  //    glm::vec3 lookRight =
  //        glm::normalize(glm::vec3(-lookAt.y, lookAt.x, 0.0f));
  //    glm::vec3 lookUp = glm::cross(lookAt, lookRight);

  //    glm::mat3 rotationMatrix(lookRight, lookUp, lookAt);
  //    camera->transform->rotation = glm::quat_cast(rotationMatrix);
  //    return true;
  //  }
  //}

  return false;
}

float angle = 0.0f;
void StartMode::update(float elapsed) {
  if (gStop || gamePause) {
    return;
  }
  angle += 1.f;
  background->transform->rotation = glm::angleAxis(glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.f));
  if (angle > 360.0f) {
    angle = 0.0f;
  }
  std::cout << "\rAngle: " << angle << std::endl;
  //// player should not walk while hiding
  //if (!interactableManager.isHiding) {
  //  // player walking:
  //  {
  //    // combine inputs into a move:
  //    constexpr float PlayerSpeed = 4.0f;
  //    glm::vec2 move = glm::vec2(0.0f);
  //    if (left.pressed && !right.pressed)
  //      move.x = -1.0f;
  //    if (!left.pressed && right.pressed)
  //      move.x = 1.0f;
  //    if (down.pressed && !up.pressed)
  //      move.y = -1.0f;
  //    if (!down.pressed && up.pressed)
  //      move.y = 1.0f;



  //    // get move in world coordinate system:
  //    glm::vec3 remain = player.transform->make_local_to_world() *
  //                       glm::vec4(move.x, move.y, 0.0f, 0.0f);
  //    const auto &camera = player.camera;
  //    glm::vec3 lookAt = camera->getLookAt();
  //    glm::vec3 lookRight =
  //        glm::normalize(glm::vec3(-lookAt.y, lookAt.x, 0.0f));
  //    remain = lookRight * remain.x -
  //             lookAt * remain.y; // TODO: idk why it should be negative for y

  //    // using a for() instead of a while() here so that if walkpoint gets stuck
  //    // in
  //    // some awkward case, code will not infinite loop:
  //    for (uint32_t iter = 0; iter < 10; ++iter) {
  //      if (remain == glm::vec3(0.0f))
  //        break;
  //      WalkPoint end;
  //      float time;
  //      walkmesh->walk_in_triangle(player.at, remain, &end, &time);
  //      player.at = end;
  //      if (time == 1.0f) {
  //        // finished within triangle:
  //        remain = glm::vec3(0.0f);
  //        break;
  //      }
  //      // some step remains:
  //      remain *= (1.0f - time);
  //      // try to step over edge:
  //      glm::quat rotation;
  //      if (walkmesh->cross_edge(player.at, &end, &rotation)) {
  //        // stepped to a new triangle:
  //        player.at = end;
  //        // rotate step to follow surface:
  //        remain = rotation * remain;
  //      } else {
  //        // ran into a wall, bounce / slide along it:
  //        glm::vec3 const &a = walkmesh->vertices[player.at.indices.x];
  //        glm::vec3 const &b = walkmesh->vertices[player.at.indices.y];
  //        glm::vec3 const &c = walkmesh->vertices[player.at.indices.z];
  //        glm::vec3 along = glm::normalize(b - a);
  //        glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
  //        glm::vec3 in = glm::cross(normal, along);

  //        // check how much 'remain' is pointing out of the triangle:
  //        float d = glm::dot(remain, in);
  //        if (d < 0.0f) {
  //          // bounce off of the wall:
  //          remain += (-1.25f * d) * in;
  //        } else {
  //          // if it's just pointing along the edge, bend slightly away from
  //          // wall:
  //          remain += 0.01f * d * in;
  //        }
  //      }
  //    }

  //    if (remain != glm::vec3(0.0f)) {
  //      std::cout << "NOTE: code used full iteration budget for walking."
  //                << std::endl;
  //    }

  //    // update player's position to respect walking:
  //    player.transform->position = walkmesh->to_world_point(player.at);
  //    camera->transform->position =
  //        glm::vec3(player.transform->position.x, player.transform->position.y,
  //                  player.transform->position.z + PLAYER_HEIGHT) +
  //        camera_shift;

  //    { // update player's rotation to respect local (smooth) up-vector:

  //      glm::quat adjust = glm::rotation(
  //          player.transform->rotation *
  //              glm::vec3(0.0f, 0.0f, PLAYER_HEIGHT), // current up vector
  //          walkmesh->to_world_smooth_normal(
  //              player.at) // smoothed up vector at walk location
  //      );
  //      player.transform->rotation =
  //          glm::normalize(adjust * player.transform->rotation);
  //    }

  //    /*
  //    glm::mat4x3 frame = camera->transform->make_local_to_parent();
  //    glm::vec3 right = frame[0];
  //    //glm::vec3 up = frame[1];
  //    glm::vec3 forward = -frame[2];

  //    camera->transform->position += move.x * right + move.y * forward;
  //    */
  //  }
  //}


  // reset button press counters:
  left.downs = 0;
  right.downs = 0;
  up.downs = 0;
  down.downs = 0;



  // Story manager update
  if (gameplayUI->dialogueText.size() > 0) {
    gStop = true;
  }
  //bool advanced = storyManager->advanceStory();

}

void StartMode::draw(glm::uvec2 const &drawable_size) {
  if (gamePause)
    return;
  // update camera aspect ratio for drawable:
  camera->aspect = float(drawable_size.x) / float(drawable_size.y);

  // set up light type and position for lit_color_texture_program:
  // TODO: consider using the Light(s) in the scene to do this
  glUseProgram(lit_color_texture_program->program);
  glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 4);
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

  /* In case you are wondering if your walkmesh is lining up with your scene,
  try:
  {

          DrawLines lines(player.camera->make_projection() *
  glm::mat4(player.camera->transform->make_world_to_local())); for (auto const
  &tri : walkmesh->triangles) { lines.draw(walkmesh->vertices[tri.x],
  walkmesh->vertices[tri.y], glm::u8vec4(0x88, 0x00, 0xff, 0xff));
                  lines.draw(walkmesh->vertices[tri.y],
  walkmesh->vertices[tri.z], glm::u8vec4(0x88, 0x00, 0xff, 0xff));
                  lines.draw(walkmesh->vertices[tri.z],
  walkmesh->vertices[tri.x], glm::u8vec4(0x88, 0x00, 0xff, 0xff));
          }
  }
  */

  // render UI and text

  glDisable(GL_DEPTH_TEST);

  gameplayUI->DrawUI(drawable_size);

  GL_ERRORS();
}


