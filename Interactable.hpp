#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <map>
#include <memory>
#include <utility>

#include "Scene.hpp"
#include "Sound.hpp"

static constexpr float ITEM_INTERACT_DISTANCE = 2.0f;
static constexpr float FURNITURE_INTERACT_DISTANCE = 2.0f;
static constexpr float INTERACT_ANGLE = 2.2f;

/* interactable object that can be picked up */
// =========================  mesh name must match =========================
enum ItemType { BEDROOM_KEY, DOOR1_KEY, FILE1 };
static std::map<std::string, ItemType> MeshNameToItemType = {
    {"BedroomKey", BEDROOM_KEY}, {"Folder", FILE1}};
static std::map<ItemType, std::string> ItemTypeToInteractText = {
    {BEDROOM_KEY, "grab key"}, {FILE1, "read confidential file"}};

/* Type of interactable object that cannot be picked up */
enum FurnitureType { NONE, BED, CLOSET, BEDROOM_DOOR, DOOR1, FRONT_DOOR, DESK };
static std::map<FurnitureType, std::vector<std::string>>
    FurnitureTypeToInteractText = {{BED, {"sleep", "exit hiding"}},
                                   {BEDROOM_DOOR, {"open door"}},
                                   {DOOR1, {"open door"}}};
static std::map<std::string, FurnitureType> MeshNameToFurnitureType = {
    {"Bed", BED}, {"BedroomDoor", BEDROOM_DOOR}, {"Door1", DOOR1}};

/* A single furniture */
struct Furniture {

  Furniture();

  FurnitureType type;
  Scene::Drawable *drawable;

  // if the furniture is allowed to be interact in this phase
  bool phase_allow_interact = false;
  // is the player currently interacting with it?
  int interact_status = 0;
  // TODO: TBD
  bool can_interact = false;

  // when calling interact, pre-condition must be valid
  virtual bool interact(float elapsed);

  // see if this furniture is close enough to be interacted with
  virtual bool interactable(Scene::Transform *player_transform,
                            Scene::Camera *camera);

  virtual glm::vec3 getCenterPos() { return drawable->transform->position; }

  virtual std::string interactText() {
    if (interact_status >= FurnitureTypeToInteractText[type].size()) {
      return "No interaction available";
    }
    return FurnitureTypeToInteractText[type][interact_status];
  }

  virtual int getInteractStatus() { return interact_status; }
  virtual bool isInteracting() { return interact_status > 0; }

  std::shared_ptr<Sound::PlayingSample> interact_sound;
};

struct Door : Furniture {
  virtual glm::vec3 getCenterPos() override {
    glm::vec4 center_local_offset = glm::vec4(0.5f, 0.0f, 0.0f, 1.0f);
    return drawable->transform->make_local_to_world() * center_local_offset;
  }

  virtual bool interact(float elapsed) override;
  float animation_time = 0.0f;

  enum struct DoorState {
    CLOSED,
    OPENING,
    OPEN,
    CLOSING
  } state = DoorState::CLOSED;
  virtual int getInteractStatus() override { return static_cast<int>(state); }
  virtual bool isInteracting() override {
    return state == DoorState::OPENING || state == DoorState::CLOSING;
  }
};

/* A single item */
struct Item {

  Item();

  ItemType type;
  Scene::Drawable *drawable;

  bool visible;
  bool phase_allow_interact;
  bool can_interact = false;

  int interact_status = 0;

  virtual bool interact(float elapsed);

  // see if this item is good to be interacted with
  virtual bool interactable(Scene::Transform *player_transform,
                            Scene::Camera *camera);

  virtual std::string interactText() { return ItemTypeToInteractText[type]; }
};
