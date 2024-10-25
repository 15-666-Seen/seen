#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Interactable.hpp"
#include "DrawLines.hpp"

#include <string>
#include <vector>

/* On screen UI during game play */
struct GameplayUI
{
    GameplayUI(glm::uvec2 const &drawable_size);

    float aspect = 1.f;
    constexpr float H = 0.09f; // font height

    // upper left corner: interaction (press [F] to interact)
    std::string interactionText = "";
    void setInteractionText(Furniture *f);
    // lower left corner: tutorial or mission (find the key without being killed)
    std::string missionText = "";
    void setMissionText(std::string s);
    // lower half of screen: dialogue ("The owner of this house is not human!!")
    std::vector<std::string> dialogueText;
    // TODO: add background image for the dialogue
    void setDialogueTexts(std::vector<std::string> v);

    /* Draw UI for each frame */
    void DrawUI();

    /* Each time the mouse is clicked, figure out if its interacting with some UI controlled by this struct */
    void InteractOnClick(glm::vec2 const &click_pos);
};

/* We'll get to this after the prototype

struct MainMenu
{
};

*/

struct Button
{
};