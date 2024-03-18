#include "stdafx.hpp"

#include "editor/panels/panel.hpp"

using namespace ed;

IEditorPanel::IEditorPanel(std::string_view name, panel_info_t setup)
    : name(name)
    , visible(setup.open_on_start)
{ }

bool IEditorPanel::draw_window()
{
    if (!visible) return false;

    if (ImGui::Begin(get_title(), &visible))
    {
        draw_content();
    }
    ImGui::End();

    return visible;
}

void IEditorPanel::draw()
{
    ImGui::PushID(this);
    draw_content();
    ImGui::PopID();
}

bool IEditorPanel::menu_item(const char *shortcut)
{
    return ImGui::MenuItem(get_title(), shortcut, &visible, enabled);
}

void ed::draw_seperated(IEditorPanel& panel, const char *title)
{
    const char *name = title ? title : panel.get_title();
    ImGui::SeparatorText(name);
    panel.draw();
}

bool ed::draw_collapsing(IEditorPanel& panel, const char *title, ImGuiTreeNodeFlags flags)
{
    const char *name = title ? title : panel.get_title();
    bool result = ImGui::CollapsingHeader(name, flags);

    if (result)
    {
        panel.draw();
    }

    return result;
}
