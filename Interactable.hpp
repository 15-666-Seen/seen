#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <map>
#include <utility>

#include "Scene.hpp"

static constexpr float ITEM_INTERACT_DISTANCE = 5.0f;
static constexpr float FURNITURE_INTERACT_DISTANCE = 5.0f;
static constexpr float INTERACT_ANGLE = 2.2f;

/* interactable object that can be picked up */
// =========================  mesh name must match =========================
enum ItemType { BEDROOM_KEY };
static std::map<std::string, ItemType> MeshNameToItemType = {
    {"BedroomKey", BEDROOM_KEY}};
static std::map<ItemType, std::string> ItemTypeToInteractText = {
    {BEDROOM_KEY, "grab key"}};

/* Type of interactable object that cannot be picked up */
enum FurnitureType { NONE, BED, CLOSET, BEDROOM_DOOR, FRONT_DOOR, DESK };
static std::map<FurnitureType, std::pair<std::string, std::string>>
    FurnitureTypeToInteractText = {{BED, {"sleep", "exit hiding"}},
                                   {BEDROOM_DOOR, {"open door", "close door"}}};
static std::map<std::string, FurnitureType> MeshNameToFurnitureType = {
    {"Bed", BED}, {"BedroomDoor", BEDROOM_DOOR}};

/* A single furniture */
struct Furniture {

  Furniture();

  FurnitureType type;
  Scene::Transform *transform;

  // if the furniture is allowed to be interact in this phase
  bool phase_allow_interact = false;
  // is the player currently interacting with it?
  bool interactStatus = false;
  // TODO: TBD
  bool can_interact = false;

  // when calling interact, pre-condition must be valid
  virtual bool interact(float elapsed);

  // see if this furniture is close enough to be interacted with
  virtual bool interactable(Scene::Transform *player_transform,
                            Scene::Camera *camera);

  virtual glm::vec3 getCenterPos() { return transform->position; }

  virtual std::string interactText() {
    return interactStatus ? FurnitureTypeToInteractText[type].second
                          : FurnitureTypeToInteractText[type].first;
  }
};

struct Door : Furniture {
  virtual glm::vec3 getCenterPos() override {
    glm::vec4 center_local_offset = glm::vec4(0.5f, 0.0f, 0.0f, 1.0f);
    return transform->make_local_to_world() * center_local_offset;
  }

  virtual bool interact(float elapsed) override;
  float animation_time = 0.0f;
};

/* A single item */
struct Item {

  Item();

  ItemType type;
  Scene::Transform *transform;

  bool visible;
  bool phase_allow_interact;
  bool can_interact = false;

  virtual bool interact(float elapsed);

  // see if this item is good to be interacted with
  virtual bool interactable(Scene::Transform *player_transform,
                            Scene::Camera *camera);

  virtual std::string interactText() { return ItemTypeToInteractText[type]; }
};
