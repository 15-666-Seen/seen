#pragma once
#include <vector>
#include "Scene.hpp"

#include <glm/glm.hpp>

struct Movable
{
	Scene::Transform* transform;
	glm::vec3 target;
	float speed;
	bool finished;
	bool stopped;
};

struct Move
{
	std::vector<Movable*> movables;

	void 
};