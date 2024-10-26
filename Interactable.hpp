#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Load.hpp"
#include "Scene.hpp"
#include "UI.hpp"

const float ITEM_INTERACT_DISTANCE = 1.0f;
const float FURNITURE_INTERACT_DISTANCE = 1.0f;

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
    FurnitureTypeToInteractText = {{BED, {"hide under bed", "exit hiding"}},
                                   {BEDROOM_DOOR, {"open door", "close door"}}};
static std::map<std::string, FurnitureType> MeshNameToFurnitureType = {
    {"Bed", BED}, {"BedroomDoor", BEDROOM_DOOR}};

struct InteractableInterface {
  virtual bool interact() = 0;
  virtual bool interactable(Scene::Transform *player_transform) = 0;
  virtual std::string interact_text() = 0;
};

/* A single furniture */
struct Furniture : InteractableInterface {

  Furniture();

  FurnitureType type;
  Scene::Transform *transform;

  // if the furniture is allowed to be interact in this phase
  bool phase_allow_interact;
  bool interactStatus = false; // is the player currently interacting with it?
  bool can_interact = false;

  virtual bool interact() override;

  // see if this furniture is close enough to be interacted with
  virtual bool interactable(Scene::Transform *player_transform) override;

  virtual std::string interact_text() override {
    return interactStatus ? FurnitureTypeToInteractText[type].second
                          : FurnitureTypeToInteractText[type].first;
  }
};

/* A single item */
struct Item : InteractableInterface {

  Item();

  ItemType type;
  Scene::Transform *transform;

  bool visible;
  bool phase_allow_interact;
  bool can_interact = false;

  virtual bool interact() override;

  // see if this item is good to be interacted with
  virtual bool interactable(Scene::Transform *player_transform) override;

  virtual std::string interact_text() override {
    return ItemTypeToInteractText[type];
  }
};

/* Player inventory */
struct Inventory {

  Inventory();

  std::unordered_set<ItemType> items; // all items
  int size = 5;                       // can initially hold 5 items
  int holding_item = 0; // the item the player currently hold s. 0 = nothing

  virtual bool hasItem(ItemType item_type);
  virtual bool addItem(ItemType item_type);
  virtual bool removeItem(ItemType item_type);
};

/* Manages the inventory, all items, all furniture*/
struct InteractableManager {

  Inventory inventory = Inventory();

  std::vector<Item *> items; // better name? This collides with inventory->items
  std::vector<Furniture *> furnitures;

  void load(Load<Scene> meshes);

  // in each frame, we check interactable objects
  void update(Scene::Transform *player_transform, GameplayUI *gameplayUI,
              bool interact_pressed);

  // furniture interaction in current phase, used to forward phase
  FurnitureType cur_furniture = NONE;
};
