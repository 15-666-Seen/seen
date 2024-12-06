#include "Move.hpp"
#include <algorithm>
//#include <glm/gtx/mix.hpp> // For glm::mix

// Linear movement logic
void linear_move::move(float elapsed) {
    if (finished && *finished || stopped) {
        return;
    }

    current_time += elapsed;
    if (current_time >= total_time) {
        *transform = target; // Assign target to the final position
        if (finished) *finished = true;
    }
    else {
        float t = current_time / total_time; // Normalize time
        *transform = glm::mix(*transform, target, t); // Interpolate
    }
}

// Rotational movement logic
void rotate_move::move(float elapsed) {
    if (finished && *finished || stopped) {
        return;
    }

    current_time += elapsed;
    if (current_time >= total_time) {
        *transform = target; // Assign target to the final orientation
        if (finished) *finished = true;
    }
    else {
        float t = current_time / total_time; // Normalize time
        *transform = glm::mix(*transform, target, t); // Spherical interpolation
    }
}

// Update all movements
void Move::update(float elapsed) {
    for (auto& m : movables) {
        m->move(elapsed);
    }

    // Remove finished movements
    movables.erase(
        std::remove_if(
            movables.begin(),
            movables.end(),
            [](const std::unique_ptr<Movable>& m) { return m->finished && *(m->finished) && !m->stopped; }
        ),
        movables.end()
    );
}

// Add a linear movement
void Move::AddTransition(glm::vec3& transform, const glm::vec3& target, float total_time, bool* finished) {
    movables.push_back(
        std::make_unique<linear_move>(&transform, target, total_time, finished)
    );
}

// Add a rotational movement
void Move::AddRotation(glm::quat& transform, const glm::quat& target, float total_time, bool* finished) {
    movables.push_back(
        std::make_unique<rotate_move>(&transform, target, total_time, finished)
    );
}