

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <vector>

/* interactable object that can be picked up */
enum ItemType
{
	BEDROOM_KEY
};

/* Type of interactable object that cannot be picked up */
enum FurnitureType
{
	BED,
	CLOSET,
	BEDROOM_DOOR,
	FRONT_DOOR,
	DESK
};

/* Manages the inventory, all items, all furniture*/
struct InteractableManager
{

	Inventory inventory = Inventory();
	std::vector<Item *> items; // better name? This collides with inventory->items
	std::vector<Furniture *> furnitures;
};

/* Player inventory */
struct Inventory
{

	Inventory();

	std::vector<ItemType> items; // all items
	int size = 5;				 // can initially hold 5 items
	int current_item = 0;		 // the item the player currently holds. 0 = nothing

	virtual bool hasItem() override;
	virtual bool addItem() override;
	virtual bool removeItem() override;
};

/* A single item */
struct Item
{

	Item();

	ItemType type;
	Scene::Transform *transform;

	bool visible;
	bool canInteract;
	bool interactStatus = falses; // is the player currently interacting with it?

	virtual bool interact() override;
};

/* A single furniture */
struct Furniture
{

	Furniture();

	FurnitureType type;
	Scene::Transform *transform;

	bool visible;
	bool canInteract;
	bool interactStatus = false; // is the player currently interacting with it?

	virtual bool interact() override;
};