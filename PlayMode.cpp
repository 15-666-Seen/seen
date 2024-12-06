#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "Ghost.hpp"
#include "Load.hpp"
#include "Mesh.hpp"
#include "Scene.hpp"
#include "glm/ext/vector_float3.hpp"
#include "sound_prep.hpp"
#include "util.hpp"

#include "data_path.hpp"
#include "gl_errors.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

GLuint phonebank_meshes_for_lit_color_texture_program = 0;
Load<MeshBuffer> phonebank_meshes(LoadTagDefault, []() -> MeshBuffer const * {
  MeshBuffer const *ret = new MeshBuffer(data_path("house.pnct"));
  phonebank_meshes_for_lit_color_texture_program =
      ret->make_vao_for_program(lit_color_texture_program->program);
  return ret;
});

// used to get normal map texture name
// following the stupid code idealogy of defining this close to where its used
// :)
std::string getNormalMapName(const std::string &filename) {
  std::string new_filename = filename;
  size_t pos = new_filename.rfind('.');
  if (pos != std::string::npos) {
    new_filename.insert(pos, "_n");
  }
  return new_filename;
}

Load<Scene> house_scene(LoadTagDefault, []() -> Scene const * {
  return new Scene(data_path("house.scene"), [&](Scene &scene,
                                                 Scene::Transform *transform,
                                                 std::string const &mesh_name) {
    Mesh const &mesh = phonebank_meshes->lookup(mesh_name);

    scene.drawables.emplace_back(transform, mesh_name);
    Scene::Drawable &drawable = scene.drawables.back();

    drawable.pipeline = lit_color_texture_program_pipeline;

    drawable.pipeline.vao = phonebank_meshes_for_lit_color_texture_program;
    drawable.pipeline.type = mesh.type;
    drawable.pipeline.start = mesh.start;
    drawable.pipeline.count = mesh.count;

    drawable.tex = drawable.pipeline.tex_name_to_glint[mesh.tex];
    drawable.tex_normal =
        drawable.pipeline.tex_name_to_glint[getNormalMapName(mesh.tex)];

    /* some asserts to ensure the shader is loaded correctly */
    auto s = drawable.pipeline.tex_name_to_glint.find("0");
    assert(s != drawable.pipeline.tex_name_to_glint.end());
    s = drawable.pipeline.tex_name_to_glint.find("0_n");
    assert(s != drawable.pipeline.tex_name_to_glint.end());

    // if mesh textures are missing
    if (drawable.tex == 0) {
      printf("Texture missing in scene loading for mesh %s named %s\n",
             mesh_name.c_str(), mesh.tex.c_str());
      drawable.tex = drawable.pipeline.tex_name_to_glint["0"];
    }

    if (drawable.tex_normal == 0) {
      printf("No normal map for mesh %s named %s\n", mesh_name.c_str(),
             getNormalMapName(mesh.tex).c_str());
      drawable.tex_normal = drawable.pipeline.tex_name_to_glint["0_n"];
    }
  });
});

const WalkMesh *walkmesh = nullptr;
Load<WalkMeshes>
phonebank_walkmeshes(LoadTagDefault, []() -> WalkMeshes const * {
  WalkMeshes *ret = new WalkMeshes(data_path("house.w"));
  walkmesh = &ret->lookup("phase0");
  return ret;
});

PlayMode::PlayMode() : scene(*house_scene) {
  // create a player transform:
  scene.transforms.emplace_back();
  player.transform = &scene.transforms.back();

  // create a player camera attached to a child of the player transform:
  scene.transforms.emplace_back();
  scene.cameras.emplace_back(&scene.transforms.back());

  player.camera = &scene.cameras.back();
  player.camera->fovy = glm::radians(60.0f);
  player.camera->near = 0.01f;
  player.camera->transform->parent = nullptr;

  player.transform->position = glm::vec3(0.0f, -11.0f, 0.0f);

  // rotate camera facing direction (-z) to player facing direction (+y):
  player.camera->transform->rotation =
      glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  // start player walking at nearest walk point:
  player.at = walkmesh->nearest_walk_point(player.transform->position);

  // player's eyes are 1.8 units above the ground:
  player.camera->transform->position =
      glm::vec3(player.transform->position.x, player.transform->position.y,
                player.transform->position.z + PLAYER_HEIGHT);

  // UI
  gameplayUI = new GameplayUI();

  // items & furniture loading
  interactableManager.load(scene, gameplayUI);

  // story manager
  storyManager = new StoryManager();
  // set up game. TODO: Move this to after the title screen when applicable.
  storyManager->SetUpManager(gameplayUI, &interactableManager);

  // ghosts setup
  setupGhosts();
}

PlayMode::~PlayMode() {}

bool PlayMode::handle_event(SDL_Event const &evt,
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
      if (gStop) {
        return true;
      }

      glm::vec2 motion = glm::vec2(evt.motion.xrel / float(window_size.y),
                                   evt.motion.yrel / float(window_size.y));

      auto &camera = player.camera;

      camera->yaw += motion.x * camera->fovy;
      camera->pitch += motion.y * camera->fovy;
      // camera pitch should be limited to (-pi/2, pi/2)
      camera->pitch =
          glm::clamp(camera->pitch, -glm::pi<float>() / 2.0f + 0.0001f,
                     glm::pi<float>() / 2.0f - 0.0001f);

      // use it to get the new rotation, first by having lookAt
      glm::vec3 lookAt = camera->getLookAt();

      glm::vec3 lookRight =
          glm::normalize(glm::vec3(-lookAt.y, lookAt.x, 0.0f));
      glm::vec3 lookUp = glm::cross(lookAt, lookRight);

      glm::mat3 rotationMatrix(lookRight, lookUp, lookAt);
      camera->transform->rotation = glm::quat_cast(rotationMatrix);
      return true;
    }
  }

  return false;
}

bool ending = false; // if the game is ending, move to EndMode
void PlayMode::update(float elapsed) {

   
  if (ending || interactableManager.interactStatusCheck(FRONT_DOOR) >= 2) {
      waitBeforeEnd -= elapsed;
      if (waitBeforeEnd <= 0.f) {
          next = "EndMode";
          finished = true;
      }
    return;
  }

  if (gStop || gamePause) {
    return;
  }

  // player should not walk while hiding
  if (!interactableManager.isHiding) {
    // player walking:
    {
      // combine inputs into a move:
      constexpr float PlayerSpeed = 4.0f;
      glm::vec2 move = glm::vec2(0.0f);
      if (left.pressed && !right.pressed)
        move.x = -1.0f;
      if (!left.pressed && right.pressed)
        move.x = 1.0f;
      if (down.pressed && !up.pressed)
        move.y = -1.0f;
      if (!down.pressed && up.pressed)
        move.y = 1.0f;

      // make it so that moving diagonally doesn't go faster:
      glm::vec3 camera_shift = glm::vec3(0.0f);
      isPlayerWalking = false;
      if (move != glm::vec2(0.0f)) {
        isPlayerWalking = true;
        move = glm::normalize(move) * PlayerSpeed * elapsed;
        camera_shift = cameraShake(elapsed);
      } else {
        player.theta = 0.0f;
      }

      if (isPlayerWalking) {
        glm::vec3 base_player_pos = player.transform->make_local_to_world() *
                                    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        if (!walking_sound) {
          walking_sound =
              Sound::loop_3D(*footstep_sample, 0.8f, base_player_pos, 1.0f);
        } else {
          walking_sound->set_position(base_player_pos, 1.0f);
        }
      } else if (!isPlayerWalking && walking_sound) {
        walking_sound->stop(1.0f / 60.0f);
        walking_sound.reset();
      }

      // get move in world coordinate system:
      glm::vec3 remain = player.transform->make_local_to_world() *
                         glm::vec4(move.x, move.y, 0.0f, 0.0f);
      const auto &camera = player.camera;
      glm::vec3 lookAt = camera->getLookAt();
      glm::vec3 lookRight =
          glm::normalize(glm::vec3(-lookAt.y, lookAt.x, 0.0f));
      remain = lookRight * remain.x -
               lookAt * remain.y; // TODO: idk why it should be negative for y

      // using a for() instead of a while() here so that if walkpoint gets stuck
      // in
      // some awkward case, code will not infinite loop:
      for (uint32_t iter = 0; iter < 10; ++iter) {
        if (remain == glm::vec3(0.0f))
          break;
        WalkPoint end;
        float time;
        walkmesh->walk_in_triangle(player.at, remain, &end, &time);
        player.at = end;
        if (time == 1.0f) {
          // finished within triangle:
          remain = glm::vec3(0.0f);
          break;
        }
        // some step remains:
        remain *= (1.0f - time);
        // try to step over edge:
        glm::quat rotation;
        if (walkmesh->cross_edge(player.at, &end, &rotation)) {
          // stepped to a new triangle:
          player.at = end;
          // rotate step to follow surface:
          remain = rotation * remain;
        } else {
          // ran into a wall, bounce / slide along it:
          glm::vec3 const &a = walkmesh->vertices[player.at.indices.x];
          glm::vec3 const &b = walkmesh->vertices[player.at.indices.y];
          glm::vec3 const &c = walkmesh->vertices[player.at.indices.z];
          glm::vec3 along = glm::normalize(b - a);
          glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
          glm::vec3 in = glm::cross(normal, along);

          // check how much 'remain' is pointing out of the triangle:
          float d = glm::dot(remain, in);
          if (d < 0.0f) {
            // bounce off of the wall:
            remain += (-1.25f * d) * in;
          } else {
            // if it's just pointing along the edge, bend slightly away from
            // wall:
            remain += 0.01f * d * in;
          }
        }
      }

      if (remain != glm::vec3(0.0f)) {
        std::cout << "NOTE: code used full iteration budget for walking."
                  << std::endl;
      }

      // update player's position to respect walking:
      player.transform->position = walkmesh->to_world_point(player.at);
      camera->transform->position =
          glm::vec3(player.transform->position.x, player.transform->position.y,
                    player.transform->position.z + PLAYER_HEIGHT) +
          camera_shift;

      { // update player's rotation to respect local (smooth) up-vector:

        glm::quat adjust = glm::rotation(
            player.transform->rotation *
                glm::vec3(0.0f, 0.0f, PLAYER_HEIGHT), // current up vector
            walkmesh->to_world_smooth_normal(
                player.at) // smoothed up vector at walk location
        );
        player.transform->rotation =
            glm::normalize(adjust * player.transform->rotation);
      }

      /*
      glm::mat4x3 frame = camera->transform->make_local_to_parent();
      glm::vec3 right = frame[0];
      //glm::vec3 up = frame[1];
      glm::vec3 forward = -frame[2];

      camera->transform->position += move.x * right + move.y * forward;
      */
    }
  }

  // reset button press counters:
  left.downs = 0;
  right.downs = 0;
  up.downs = 0;
  down.downs = 0;

  {
    // interactable items and furniture UPDATES
    // avoid multiple interactions in one frame
    bool interact_pressed = F.pressed;
    F.pressed = false;
    interactableManager.update(player.transform, player.camera,
                               interact_pressed, elapsed);
  }

  // Story manager update
  if (gameplayUI->dialogueText.size() > 0) {
    gStop = true;
  }
  bool advanced = storyManager->advanceStory();

  for (auto &[name, ghost] : storyManager->GhostMap) {
    if (!ghost->active)
      continue;
    ghost->path_points.push_back(player.transform->position);
    ghost->update(elapsed);
    if (ghost->got_player) {
      Sound::stop_all_samples();
      next = "GOverMode";
      finished = true;
      return;
    }
  }

  // TODO: check phase updates -> update walkmesh?
  if (advanced) {
    checkPhaseUpdates();
  }
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
  if (gamePause)
    return;
  // update camera aspect ratio for drawable:
  player.camera->aspect = float(drawable_size.x) / float(drawable_size.y);

  // set up light type and position for lit_color_texture_program:
  // TODO: consider using the Light(s) in the scene to do this
  glUseProgram(lit_color_texture_program->program);
  glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 4); // 4
  glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1,
               glm::value_ptr(glm::vec3(0.0f, 0.0f, -1.0f)));
  glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1,
               glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));

  glUseProgram(0);

  glClearColor(0.065f, 0.01f, 0.01f, 0.4f);
  glClearDepth(1.0f); // 1.0 is actually the default value to clear the depth
                      // buffer to, but FYI you can change it.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); // this is the default depth comparison function, but
                        // FYI you can change it.

  scene.draw(*player.camera);

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

void PlayMode::checkPhaseUpdates() {
  std::cout << "Walkmesh Updates" << std::endl;
  if (storyManager->getCurrentPhase() == 1) {
    walkmesh = &phonebank_walkmeshes->lookup("phase1");
    player.at = walkmesh->nearest_walk_point(player.transform->position);
  }

  else if (storyManager->getCurrentPhase() == 2) {
    walkmesh = &phonebank_walkmeshes->lookup("phase2");
    player.at = walkmesh->nearest_walk_point(player.transform->position);
  }

  else if (storyManager->getCurrentPhase() == 3) {
    walkmesh = &phonebank_walkmeshes->lookup("phase3");
    player.at = walkmesh->nearest_walk_point(player.transform->position);
  }

  else if (storyManager->getCurrentPhase() == 4) {
    walkmesh = &phonebank_walkmeshes->lookup("phase4");
    player.at = walkmesh->nearest_walk_point(player.transform->position);
  }

  else if (storyManager->getCurrentPhase() == 6) {
    walkmesh = &phonebank_walkmeshes->lookup("phase5");
    player.at = walkmesh->nearest_walk_point(player.transform->position);
  }

  else if (storyManager->getCurrentPhase() == 11) { // basement
    walkmesh = &phonebank_walkmeshes->lookup("phase6");
    player.at = walkmesh->nearest_walk_point(player.transform->position);
  }

  else if (storyManager->getCurrentPhase() == 12) {
    walkmesh = &phonebank_walkmeshes->lookup("phase7");
    player.at = walkmesh->nearest_walk_point(player.transform->position);
  }
}

glm::vec3 PlayMode::cameraShake(float elapsed) {
  static float R = 0.2f;
  static float theta_max = 3.1415926f / 3.0f;
  static float angle_speed = theta_max / 0.3f;

  player.theta += angle_speed * elapsed;
  if (player.theta > theta_max) {
    player.theta = theta_max;
    angle_speed = -angle_speed;
  } else if (player.theta < -theta_max) {
    player.theta = -theta_max;
    angle_speed = -angle_speed;
  }

  float dright = R * glm::sin(player.theta);
  float dheight = R * (1 - glm::cos(player.theta));

  return glm::vec3(dright, 0.0f, dheight);
}

void PlayMode::setupGhosts() {
  for (auto &drawable : scene.drawables) {
    if (drawable.mesh_name.find("ghost") != std::string::npos) {
      Ghost *ghost = new Ghost(drawable.mesh_name, &drawable, player.transform,
                               player.camera->transform);
      storyManager->GhostMap[drawable.mesh_name] = ghost;
      drawable.visible = false; // initially invisible
    }
  }
}
