#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "DrawLines.hpp"
#include "Interactable.hpp"

#include <string>
#include <vector>

/* On screen UI during game play */
struct GameplayUI
{
    GameplayUI();

    // upper left corner: interaction (press [F] to interact)
    std::string interactionText = "Interaction Text Holder";
    void setInteractionText(Furniture *f);

    // lower left corner: tutorial or mission (find the key without being killed)
    std::string missionText = "Mission Text";
    void setMissionText(std::string s);

    // lower half of screen: dialogue ("The owner of this house is not human!!")
    std::vector<std::string> dialogueText;
    // TODO: add background image for the dialogue
    void setDialogueTexts(std::vector<std::string> v);

    /* Draw UI for each frame */
    void DrawUI(glm::uvec2 const &drawable_size);

    /* Each time the mouse is clicked, figure out if its interacting with some UI
     * controlled by this struct */
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