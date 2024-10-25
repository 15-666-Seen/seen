#include "UI.hpp"

GameplayUI::GameplayUI(glm::uvec2 const &drawable_size)
{
    aspect = float(drawable_size.x) / float(drawable_size.y);
}

void GameplayUI::setInteractionText(Furniture *f)
{
    if (f != nullptr)
    {
        interactionText = "Press [F] to interact with the " + FurnitureTypeToString[f->type];
    }
    else
    {
        interactionText = "";
    }
}

void GameplayUI::setMissionText(std::string s)
{
    missionText = s;
}

void GameplayUI::setDialogueTexts(std::vector<std::string> v)
{
    dialogueText.size() = v;
}

void GameplayUI::DrawUI()
{
    // use DrawLines to overlay some text:
    glDisable(GL_DEPTH_TEST);

    DrawLines lines(glm::mat4(1.0f / aspect, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                              1.0f));

    lines.draw_text(interactionText.c_str(),
                    glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
                    glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
                    glm::u8vec4(0x00, 0x00, 0x00, 0x00));

    lines.draw_text(missionText.c_str(),
                    glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
                    glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
                    glm::u8vec4(0x00, 0x00, 0x00, 0x00));

    lines.draw_text(dialogueText[0].c_str(),
                    glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
                    glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
                    glm::u8vec4(0x00, 0x00, 0x00, 0x00));
}

void GameplayUI::InteractOnClick(glm::vec2 const &click_pos)
{
    // check if clicked on dialogue text
    // if so, advance dialogue
    // if no more dialogue, hide the dialogue box and clicking can no longer interact with this
}