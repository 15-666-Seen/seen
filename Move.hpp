#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Base class for movable actions
struct Movable {
    float total_time;              // Total time to complete the movement
    float current_time = 0.0f;     // Elapsed time since the start
    bool* finished = nullptr;      // Pointer to track if the movement is finished
    bool stopped = false;          // Flag to stop the movement

    Movable(float total_time, bool* finished = nullptr)
        : total_time(total_time), finished(finished) {
    }

    virtual ~Movable() = default;  // Virtual destructor for polymorphism

    virtual void move(float elapsed) = 0; // Pure virtual function
};

// Linear movement class
struct linear_move : public Movable {
    glm::vec3* transform;          // Pointer to the current transform
    glm::vec3 target;              // Target position

    linear_move(glm::vec3* transform, glm::vec3 target, float total_time, bool* finished = nullptr)
        : Movable(total_time, finished), transform(transform), target(target) {
    }

    void move(float elapsed) override;
};

// Rotational movement class
struct rotate_move : public Movable {
    glm::quat* transform;          // Pointer to the current rotation
    glm::quat target;              // Target rotation

    rotate_move(glm::quat* transform, glm::quat target, float total_time, bool* finished = nullptr)
        : Movable(total_time, finished), transform(transform), target(target) {
    }

    void move(float elapsed) override;
};

// Movement manager
class Move {
public:
    void update(float elapsed); // Update all active movements

    void AddTransition(glm::vec3& transform, const glm::vec3& target, float total_time, bool* finished = nullptr);
    void AddRotation(glm::quat& transform, const glm::quat& target, float total_time, bool* finished = nullptr);

private:
    std::vector<std::unique_ptr<Movable>> movables; // Store active movements
};
