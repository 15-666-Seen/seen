

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <map>
#include <unordered_set>
#include <vector>

#include "Scene.hpp"

const float ITEM_INTERACT_DISTANCE = 1.0f;
const float FURNITURE_INTERACT_DISTANCE = 1.0f;

/* interactable object that can be picked up */
enum ItemType { BEDROOM_KEY };

/* Type of interactable object that cannot be picked up */
enum FurnitureType { BED, CLOSET, BEDROOM_DOOR, FRONT_DOOR, DESK };
static std::map<FurnitureType, std::string> FurnitureTypeToString = {
    {BED, "Bed"},
    {CLOSET, "Closet"},
    {BEDROOM_DOOR, "Bedroom Door"},
    {FRONT_DOOR, "Front Door"},
    {DESK, "Desk"}};

/* A single furniture */
struct Furniture {

  Furniture();

  FurnitureType type;
  Scene::Transform *transform;

  // if the furniture is allowed to be interact in this phase
  bool phase_allow_interact;

  bool can_interact = false;

  virtual bool interact();

  // see if this furniture is close enough to be interacted with
  virtual bool interactable(Scene::Transform *player_transform);
};

/* A single item */
struct Item {

  Item();

  ItemType type;
  Scene::Transform *transform;

  bool visible;
  bool phase_allow_interact;

  bool interactStatus = false; // is the player currently interacting with it?
  bool can_interact = false;

  virtual bool interact();

  // see if this item is good to be interacted with
  virtual bool interactable(Scene::Transform *player_transform);
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
};
