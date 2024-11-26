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
enum ItemType {
  BEDROOM_KEY,
  DOOR1_KEY,
  FILE1,
  FILE2,
  DEN_KEY,
  REDROOM_KEY,
  CLIP_L,
  CLIP_R,
  CLIP_M,
  EYEBALL,
};
static std::map<std::string, ItemType> MeshNameToItemType = {
    {"BedroomKey", BEDROOM_KEY}, {"Folder", FILE1},
    {"Folder_Police", FILE2},    {"DenKey", DEN_KEY},
    {"RedroomKey", REDROOM_KEY}, {"ClipL", CLIP_L},
    {"ClipR", CLIP_R},           {"ClipM", CLIP_M},
    {"Eyeball", EYEBALL},
};
static std::map<ItemType, std::string> ItemTypeToInteractText = {
    {BEDROOM_KEY, "grab key"},
    {FILE1, "read confidential file"},
    {FILE2, "read \"Countermeasures to Police Inspection\""},
    {DEN_KEY, "grab key"},
    {REDROOM_KEY, "grab key"},
    {CLIP_L, "grab chain cutter (left)"},
    {CLIP_R, "grab chain cutter (right)"},
    {CLIP_M, "grab chain cutter (center)"},
    {EYEBALL, "grab eyeball"},
};

/* Type of interactable object that cannot be picked up */
enum FurnitureType {
  NONE,
  BED,
  BEDROOM_DOOR,
  DOOR1,
  FRONT_DOOR,
  DESK,
  REDROOM_DOOR,
  CHAIN,
  CHAIN_CUT1,
  CHAIN_CUT2,
  CORPSE,
  DOORBLOCK,
  TINY_SCULPTURE,
  SCULPTURE_EYE_R,
  TINY_SCULPTURE_TENTACLES,
  BOOKSHELF,
  SOFA,
  LADDER,
  FRIDGE1,
  FRIDGE2,
  FRIDGE3,
  FRIDGE4,
  FRIDGE5,
  CLOSET0,
  CLOSET1,
  CLOSET2
};
static std::map<FurnitureType, std::vector<std::string>>
    FurnitureTypeToInteractText = {
        {BED, {"hide under bed", "exit hiding"}},
        {BEDROOM_DOOR, {"open door"}},
        {DOOR1, {"open door"}},
        {TINY_SCULPTURE, {"interact"}},
        {BOOKSHELF, {"move shelf"}},
        {SOFA, {"push sofa away"}},
        {LADDER, {"use ladder", "use ladder", "use ladder"}},
        {FRIDGE1, {"open fridge"}},
        {FRIDGE2, {"open fridge"}},
        {FRIDGE3, {"open fridge"}},
        {FRIDGE4, {"open fridge"}},
        {FRIDGE5, {"open fridge"}},
        {CLOSET0, {"hide", "exit hiding"}},
        {CLOSET1, {"hide", "exit hiding"}},
        {CLOSET2, {"hide", "exit hiding"}},
};

static std::map<std::string, FurnitureType> MeshNameToFurnitureType = {
    {"Bed", BED},
    {"BedroomDoor", BEDROOM_DOOR},
    {"DenDoor", DOOR1},
    {"front_door1", FRONT_DOOR},
    {"RedroomDoor", REDROOM_DOOR},
    {"Chain", CHAIN},
    {"Chaincut1", CHAIN_CUT1},
    {"Chaincut2", CHAIN_CUT2},
    {"CorpseDen", CORPSE},
    {"DoorBlock", DOORBLOCK},
    {"DenSculpture", TINY_SCULPTURE},
    {"DenEyeballR", SCULPTURE_EYE_R},
    {"DenSculpture2", TINY_SCULPTURE_TENTACLES},
    {"DenBookshelf", BOOKSHELF},
    {"Sofa", SOFA},
    {"ladder", LADDER},
    {"FridgeDoor1", FRIDGE1},
    {"FridgeDoor2", FRIDGE2},
    {"FridgeDoor3", FRIDGE3},
    {"FridgeDoor4", FRIDGE4},
    {"FridgeDoor5", FRIDGE5},
    {"Closet0", CLOSET0},
    {"Closet1", CLOSET1},
    {"Closet2", CLOSET2},
};

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

  std::shared_ptr<Sound::PlayingSample> interact_sound = nullptr;
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

  std::shared_ptr<Sound::PlayingSample> interact_sound;
};
