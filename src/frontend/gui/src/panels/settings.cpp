#include "editor/panels/settings.hpp"

using namespace ed;

void SettingsPanel::draw_content()
{
    ImGui::Text("Display");
}

SettingsPanel::SettingsPanel()
    : IEditorPanel("Settings")
{ }
