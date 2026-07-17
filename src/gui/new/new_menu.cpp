#include "new_menu.h"
#include "../../ext/imgui/imgui_settings.h"

namespace font {
    ImFont* inter_bold = nullptr;
    ImFont* inter_default = nullptr;
    ImFont* icon = nullptr;
}

namespace pic
{
    ID3D11ShaderResourceView* background = nullptr;

    ID3D11ShaderResourceView* aimbot = nullptr;
    ID3D11ShaderResourceView* visuals = nullptr;
    ID3D11ShaderResourceView* skins = nullptr;
    ID3D11ShaderResourceView* settings = nullptr;

    ID3D11ShaderResourceView* combo_widget = nullptr;
    ID3D11ShaderResourceView* input_widget = nullptr;

    ID3D11ShaderResourceView* menu_settings_icon = nullptr;

    ID3D11ShaderResourceView* circle_success = nullptr;
    ID3D11ShaderResourceView* circle_error = nullptr;
}

DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

const char* cheat_name = "SYNCHOOK BETA";

static int currentWeaponType = 0;

std::vector<MenuRageHitbox_t>& GetHitboxesForCurrentWeapon()
{
    switch (currentWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_LIGHT_PISTOL_vecHitBoxes);
    case DEAGLE:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_DEAGLE_vecHitBoxes);
    case REVOLVER:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_REVOLVER_vecHitBoxes);
    case SMG:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_SMG_vecHitBoxes);
    case LMG:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_LMG_vecHitBoxes);
    case AR:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_AR_vecHitBoxes);
    case SHOTGUN:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_SHOTGUN_vecHitBoxes);
    case SCOUT:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_SCOUT_vecHitBoxes);
    case AUTOSNIPER:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_AUTOSNIPER_vecHitBoxes);
    case AWP:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_AWP_vecHitBoxes);
    default:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_LIGHT_PISTOL_vecHitBoxes);
    }
}

int& GetPointscaleForCurrentWeapon()
{
    switch (currentWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMaxPointscale);
    case DEAGLE:
        return Config::i(g_Variables.m_Ragebot.m_DEAGLE_iMaxPointscale);
    case REVOLVER:
        return Config::i(g_Variables.m_Ragebot.m_REVOLVER_iMaxPointscale);
    case SMG:
        return Config::i(g_Variables.m_Ragebot.m_SMG_iMaxPointscale);
    case LMG:
        return Config::i(g_Variables.m_Ragebot.m_LMG_iMaxPointscale);
    case AR:
        return Config::i(g_Variables.m_Ragebot.m_AR_iMaxPointscale);
    case SHOTGUN:
        return Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iMaxPointscale);
    case SCOUT:
        return Config::i(g_Variables.m_Ragebot.m_SCOUT_iMaxPointscale);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iMaxPointscale);
    case AWP:
        return Config::i(g_Variables.m_Ragebot.m_AWP_iMaxPointscale);
    default:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMaxPointscale);
    }
}

int& GetTargetTypeForCurrentWeapon()
{
    switch (currentWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iTargetPriority);
    case DEAGLE:
        return Config::i(g_Variables.m_Ragebot.m_DEAGLE_iTargetPriority);
    case REVOLVER:
        return Config::i(g_Variables.m_Ragebot.m_REVOLVER_iTargetPriority);
    case SMG:
        return Config::i(g_Variables.m_Ragebot.m_SMG_iTargetPriority);
    case LMG:
        return Config::i(g_Variables.m_Ragebot.m_LMG_iTargetPriority);
    case AR:
        return Config::i(g_Variables.m_Ragebot.m_AR_iTargetPriority);
    case SHOTGUN:
        return Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iTargetPriority);
    case SCOUT:
        return Config::i(g_Variables.m_Ragebot.m_SCOUT_iTargetPriority);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iTargetPriority);
    case AWP:
        return Config::i(g_Variables.m_Ragebot.m_AWP_iTargetPriority);
    default:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iTargetPriority);
    }
}

int& GetHitchanceForCurrentWeapon()
{
    switch (currentWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iHitchance);
    case DEAGLE:
        return Config::i(g_Variables.m_Ragebot.m_DEAGLE_iHitchance);
    case REVOLVER:
        return Config::i(g_Variables.m_Ragebot.m_REVOLVER_iHitchance);
    case SMG:
        return Config::i(g_Variables.m_Ragebot.m_SMG_iHitchance);
    case LMG:
        return Config::i(g_Variables.m_Ragebot.m_LMG_iHitchance);
    case AR:
        return Config::i(g_Variables.m_Ragebot.m_AR_iHitchance);
    case SHOTGUN:
        return Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iHitchance);
    case SCOUT:
        return Config::i(g_Variables.m_Ragebot.m_SCOUT_iHitchance);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iHitchance);
    case AWP:
        return Config::i(g_Variables.m_Ragebot.m_AWP_iHitchance);
    default:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iHitchance);
    }
}

int& GetMinDamageForCurrentWeapon()
{
    switch (currentWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMinDamage);
    case DEAGLE:
        return Config::i(g_Variables.m_Ragebot.m_DEAGLE_iMinDamage);
    case REVOLVER:
        return Config::i(g_Variables.m_Ragebot.m_REVOLVER_iMinDamage);
    case SMG:
        return Config::i(g_Variables.m_Ragebot.m_SMG_iMinDamage);
    case LMG:
        return Config::i(g_Variables.m_Ragebot.m_LMG_iMinDamage);
    case AR:
        return Config::i(g_Variables.m_Ragebot.m_AR_iMinDamage);
    case SHOTGUN:
        return Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iMinDamage);
    case SCOUT:
        return Config::i(g_Variables.m_Ragebot.m_SCOUT_iMinDamage);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iMinDamage);
    case AWP:
        return Config::i(g_Variables.m_Ragebot.m_AWP_iMinDamage);
    default:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMinDamage);
    }
}

int& GetMinDamageOverrideForCurrentWeapon()
{
    switch (currentWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMinDamageOverride);
    case DEAGLE:
        return Config::i(g_Variables.m_Ragebot.m_DEAGLE_iMinDamageOverride);
    case REVOLVER:
        return Config::i(g_Variables.m_Ragebot.m_REVOLVER_iMinDamageOverride);
    case SMG:
        return Config::i(g_Variables.m_Ragebot.m_SMG_iMinDamageOverride);
    case LMG:
        return Config::i(g_Variables.m_Ragebot.m_LMG_iMinDamageOverride);
    case AR:
        return Config::i(g_Variables.m_Ragebot.m_AR_iMinDamageOverride);
    case SHOTGUN:
        return Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iMinDamageOverride);
    case SCOUT:
        return Config::i(g_Variables.m_Ragebot.m_SCOUT_iMinDamageOverride);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iMinDamageOverride);
    case AWP:
        return Config::i(g_Variables.m_Ragebot.m_AWP_iMinDamageOverride);
    default:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMinDamageOverride);
    }
}

int& GetHitchanceOverrideForCurrentWeapon()
{
    switch (currentWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Ragebot.m_AR_iHitchanceOverride);
    case DEAGLE:
        return Config::i(g_Variables.m_Ragebot.m_DEAGLE_iHitchanceOverride);
    case REVOLVER:
        return Config::i(g_Variables.m_Ragebot.m_REVOLVER_iHitchanceOverride);
    case SMG:
        return Config::i(g_Variables.m_Ragebot.m_SMG_iHitchanceOverride);
    case LMG:
        return Config::i(g_Variables.m_Ragebot.m_LMG_iHitchanceOverride);
    case AR:
        return Config::i(g_Variables.m_Ragebot.m_AR_iHitchanceOverride);
    case SHOTGUN:
        return Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iHitchanceOverride);
    case SCOUT:
        return Config::i(g_Variables.m_Ragebot.m_SCOUT_iHitchanceOverride);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iHitchanceOverride);
    case AWP:
        return Config::i(g_Variables.m_Ragebot.m_AWP_iHitchanceOverride);
    default:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iHitchanceOverride);
    }
}

const char* FormatDamageValue(int value) {
    static char buffer[32];

    if (value < 0) {
        strcpy_s(buffer, sizeof(buffer), "0HP");
        return buffer;
    }

    if (value > 100) {
        _snprintf_s(buffer, sizeof(buffer), _TRUNCATE, "HP+%d", value - 100);
    }
    else {
        _snprintf_s(buffer, sizeof(buffer), _TRUNCATE, "%dHP", value);
    }

    return buffer;
}

const char* FormatPointscale(int value) {
    static char buffer[32];

    if (value <= 0) {
        strcpy_s(buffer, sizeof(buffer), "Auto");
        return buffer;
    }

    if (value >= 1) {
        _snprintf_s(buffer, sizeof(buffer), _TRUNCATE, "%d", value);
    }
    else {
        _snprintf_s(buffer, sizeof(buffer), _TRUNCATE, "Auto", value);
    }

    return buffer;
}

void cmenu::render() {
    if (!Gui::m_bOpen)
        return;

    ImGuiStyle* style = &ImGui::GetStyle();

    style->WindowPadding = ImVec2(0, 0);
    style->WindowBorderSize = 0;
    style->ItemSpacing = ImVec2(20, 20);
    style->ScrollbarSize = 8.f;

    ImGui::SetNextWindowSize({ c::bg::size });

    ImGui::Begin("IMGUI", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
    {
        const ImVec2& pos = ImGui::GetWindowPos();
        const ImVec2& spacing = style->ItemSpacing;
        const ImVec2& region = ImGui::GetContentRegionMax();

        ImGui::GetBackgroundDrawList()->AddRectFilled(pos, pos + region, ImGui::GetColorU32(c::bg::background), c::bg::rounding);

        ImGui::GetBackgroundDrawList()->AddRectFilled(pos + spacing, pos + ImVec2(180, 62 + +spacing.y), ImGui::GetColorU32(c::child::background), c::child::rounding);
        ImGui::GetBackgroundDrawList()->AddRect(pos + spacing, pos + ImVec2(180, 62 + spacing.y), ImGui::GetColorU32(c::child::outline), c::child::rounding);

        ImGui::GetBackgroundDrawList()->AddRectFilled(pos + ImVec2(0, 62 + spacing.y) + spacing, pos + ImVec2(180, region.y - spacing.y), ImGui::GetColorU32(c::child::background), c::child::rounding);
        ImGui::GetBackgroundDrawList()->AddRect(pos + ImVec2(0, 62 + spacing.y) + spacing, pos + ImVec2(180, region.y - spacing.y), ImGui::GetColorU32(c::child::outline), c::child::rounding);

        ImGui::GetBackgroundDrawList()->AddRectFilled(pos + ImVec2(180, 0) + spacing, pos + ImVec2(region.x - spacing.x, 62 + spacing.y), ImGui::GetColorU32(c::child::background), c::child::rounding);
        ImGui::GetBackgroundDrawList()->AddRect(pos + ImVec2(180, 0) + spacing, pos + ImVec2(region.x - spacing.x, 62 + spacing.y), ImGui::GetColorU32(c::child::outline), c::child::rounding);

        //ImGui::GetBackgroundDrawList()->AddRectFilled(pos + ImVec2(region.x - (spacing.x * 2) - 62, 0) + spacing, pos + ImVec2(region.x - spacing.x, 62 + spacing.y), ImGui::GetColorU32(c::child::background), c::child::rounding);
        //ImGui::GetBackgroundDrawList()->AddRect(pos + ImVec2(region.x - (spacing.x * 2) - 62, 0) + spacing, pos + ImVec2(region.x - spacing.x, 62 + spacing.y), ImGui::GetColorU32(c::child::outline), c::child::rounding);

        //ImGui::GetWindowDrawList()->AddImage(pic::menu_settings_icon, pos + ImVec2(region.x - (spacing.x * 2) - 62, 0) + spacing + ImVec2(16, 16), pos + ImVec2(region.x - spacing.x, 62 + spacing.y) - ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(c::accent));

        ImGui::GlowText(font::inter_bold, 17.f, cheat_name, pos + (ImVec2(180, 62 + spacing.y) + spacing - ImGui::CalcTextSize(cheat_name)) / 2, ImGui::GetColorU32(c::accent));

        static int tabs = 0;
        static int aimbot_subtabs = 0;
        static int visuals_subtabs = 0;
        static int settings_subtabs = 0;

        ImGui::SetCursorPos(ImVec2(spacing.x * 2, 62 + spacing.y * 3));
        ImGui::BeginGroup();
        {
            if (ImGui::Tabs(0 == tabs, pic::aimbot, "Aimbot", ImVec2(180 - spacing.x * 3, 35), NULL)) tabs = 0;
            if (ImGui::Tabs(1 == tabs, pic::visuals, "Visuals", ImVec2(180 - spacing.x * 3, 35), NULL)) tabs = 1;
            if (ImGui::Tabs(2 == tabs, pic::settings, "Misc", ImVec2(180 - spacing.x * 3, 35), NULL)) tabs = 2;
        }
        ImGui::EndGroup();

        ImGui::SetCursorPos(ImVec2(180 + spacing.x, spacing.y));
        ImGui::BeginGroup();
        {
            if (tabs == 0) {
                if (ImGui::SubTab(0 == aimbot_subtabs, "RAGE", ImVec2(80, 62))) aimbot_subtabs = 0;
                ImGui::SameLine(0, 5);
                if (ImGui::SubTab(1 == aimbot_subtabs, "ANTI-AIM", ImVec2(80, 62))) aimbot_subtabs = 1;
            }
            else if (tabs == 1) {
                if (ImGui::SubTab(0 == visuals_subtabs, "WORLD", ImVec2(80, 62))) visuals_subtabs = 0;
                ImGui::SameLine(0, 5);
                if (ImGui::SubTab(1 == visuals_subtabs, "ESP", ImVec2(80, 62))) visuals_subtabs = 1;
                ImGui::SameLine(0, 5);
                if (ImGui::SubTab(2 == visuals_subtabs, "LOCAL", ImVec2(80, 62))) visuals_subtabs = 2;
            }
            else if (tabs == 2) {
                if (ImGui::SubTab(0 == settings_subtabs, "MISC", ImVec2(80, 62))) settings_subtabs = 0;
                ImGui::SameLine(0, 5);
                if (ImGui::SubTab(1 == settings_subtabs, "CONFIGS", ImVec2(80, 62))) settings_subtabs = 1;
            }
        }
        ImGui::EndGroup();

        ImGui::SetCursorPos(ImVec2(180, 62 + spacing.y) + spacing);

        if (tabs == 0) {
            if (aimbot_subtabs == 0) {
                ImGui::BeginGroup();
                {
                    ImGui::CustomBeginChild("GENERAL", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, (region.y - (spacing.y * 4 + 62)) / 2.0f + 80));
                    {
                        ImGui::Checkbox("Enabled", &Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled));
                        ImGui::BeginDisabled(!Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled));
                        {
                            ImGui::Checkbox(X("Automatic Fire"), &Config::b(g_Variables.m_Ragebot.m_bAutoShoot));

                            ImGui::Checkbox(X("Double Tap"), &Config::b(g_Variables.m_Ragebot.m_bDoubleTap));

                            ImGui::Checkbox(X("Automatic Scope"), &Config::b(g_Variables.m_Ragebot.m_bAutoscope));

                            ImGui::Checkbox(X("No Spread [UNSAFE]"), &Config::b(g_Variables.m_Ragebot.m_bClientNoSpread));

                            ImGui::Checkbox(X("Silent Aimbot"), &Config::b(g_Variables.m_Ragebot.m_bSilentAim));

                            ImGui::Checkbox(X("Auto Revolver"), &Config::b(g_Variables.m_Ragebot.m_bAutoRevolver));

                            ImGui::SliderFloat(X("Aimbot FOV"), &Config::f(g_Variables.m_Ragebot.m_flFov), 1.f, 180.f);

                            ImGui::Keybind(X("Force Headshot"), (int*)&Config::kb(g_Variables.m_Ragebot.m_iForceHeadshotKey).m_iKey, (int*)&Config::kb(g_Variables.m_Ragebot.m_iForceHeadshotKey).m_iMode, true);
                            ImGui::Keybind(X("Force Bodyaim"), (int*)&Config::kb(g_Variables.m_Ragebot.m_iForceBodyaimKey).m_iKey, (int*)&Config::kb(g_Variables.m_Ragebot.m_iForceBodyaimKey).m_iMode, true);
                        }
                        ImGui::EndDisabled();
                    }
                    ImGui::CustomEndChild();

                    ImGui::CustomBeginChild("ACCURACY", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, (region.y - (spacing.y * 4 + 62)) / 2.0f - 80));
                    {
                        static const char* modes[] = { "Early", "In Air" };

                        ImGui::Checkbox(X("Auto Stop"), &Config::b(g_Variables.m_Ragebot.m_bAutostop));
                        ImGui::BeginDisabled(!Config::b(g_Variables.m_Ragebot.m_bAutostop));
                        {
                            ImGui::MultiCombo(X("Auto Stop Modifiers"), Config::vb(g_Variables.m_Ragebot.m_vbAutostop), modes, IM_ARRAYSIZE(modes));
                        }
                        ImGui::EndDisabled();
                    }
                    ImGui::CustomEndChild();
                }
                ImGui::EndGroup();

                ImGui::SameLine();

                ImGui::BeginGroup();
                {
                    ImGui::CustomBeginChild("SETTINGS", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, (region.y - (spacing.y * 4 + 62)) / 2.0f + 80));
                    {
                        static const char* target_select[] = { "Damage", "Hit-chance", "Health" };
                        static const char* weapon_names[] = { ("Pistols"), ("Deagle"), ("Revolver"), ("SMGs"), ("LMGs"), ("Rifles"), ("Shotguns"), ("SSG-08"), ("Auto-snipers"), ("AWP") };
                        static const char* hb[] = { "Head", "Chest", "Stomatch", "Pelvis", "Arms", "Legs", "Feet" };
                        static bool hitbox_selections[HB_MAX] = { false };
                        static bool multipoint_selections[HB_MAX] = { false };

                        ImGui::Combo(X("Weapon type"), &currentWeaponType, weapon_names, IM_ARRAYSIZE(weapon_names));

                        auto& currentHitboxes = GetHitboxesForCurrentWeapon();

                        memset(hitbox_selections, 0, sizeof(hitbox_selections));
                        memset(multipoint_selections, 0, sizeof(multipoint_selections));

                        for (const auto& hitbox : currentHitboxes) {
                            hitbox_selections[hitbox.m_eDefinition] = true;
                            multipoint_selections[hitbox.m_eDefinition] = hitbox.m_bMultipoint;
                        }

                        std::vector<bool> hitbox_vars;
                        for (int i = 0; i < HB_MAX; i++) {
                            hitbox_vars.push_back(hitbox_selections[i]);
                        }
                        ImGui::MultiCombo("Hitboxes", hitbox_vars, hb, HB_MAX);

                        bool any_hitbox_selected = std::any_of(hitbox_vars.begin(), hitbox_vars.end(), [](bool selected) { return selected; });

                        std::vector<bool> multipoint_vars;
                        for (int i = 0; i < HB_MAX; i++) {
                            multipoint_vars.push_back(multipoint_selections[i]);
                        }

                        ImGui::BeginDisabled(!any_hitbox_selected);
                        ImGui::MultiCombo("Multipoints", multipoint_vars, hb, HB_MAX);
                        ImGui::EndDisabled();

                        for (int i = 0; i < HB_MAX; i++) {
                            hitbox_selections[i] = hitbox_vars[i];
                            multipoint_selections[i] = multipoint_vars[i];
                        }

                        currentHitboxes.erase(
                            std::remove_if(currentHitboxes.begin(), currentHitboxes.end(),
                                [&](const MenuRageHitbox_t& h) {
                                    return !hitbox_selections[h.m_eDefinition];
                                }),
                            currentHitboxes.end()
                        );

                        for (int i = 0; i < HB_MAX; i++) {
                            if (hitbox_selections[i] &&
                                std::find_if(currentHitboxes.begin(), currentHitboxes.end(),
                                    [i](const MenuRageHitbox_t& h) { return h.m_eDefinition == i; }) == currentHitboxes.end()) {
                                currentHitboxes.push_back({ multipoint_selections[i], static_cast<EConfigHitBoxes>(i) });
                            }
                        }

                        for (auto& hitbox : currentHitboxes) {
                            hitbox.m_bMultipoint = multipoint_selections[hitbox.m_eDefinition];
                        }

                        ImGui::SliderInt(X("Point Scale"), &GetPointscaleForCurrentWeapon(), 0, 100, FormatPointscale(GetPointscaleForCurrentWeapon()));

                        ImGui::SliderInt(X("Hit Chance"), &GetHitchanceForCurrentWeapon(), 0, 100, "%d%%");
                        ImGui::SliderInt(X("Hit Chance Override"), &GetHitchanceOverrideForCurrentWeapon(), 0, 100, "%d%%");
                        ImGui::Keybind(X("Override Hitchance Key"), (int*)&Config::kb(g_Variables.m_Ragebot.m_iHitchanceOverrideKey).m_iKey, (int*)&Config::kb(g_Variables.m_Ragebot.m_iHitchanceOverrideKey).m_iMode, true);

                        ImGui::SliderInt(X("Minimum Damage"), &GetMinDamageForCurrentWeapon(), 1, 120, FormatDamageValue(GetMinDamageForCurrentWeapon()));

                        ImGui::SliderInt(X("Override Damage"), &GetMinDamageOverrideForCurrentWeapon(), 1, 120, FormatDamageValue(GetMinDamageOverrideForCurrentWeapon()));

                        ImGui::Keybind(X("Override Damage Key"), (int*)&Config::kb(g_Variables.m_Ragebot.m_iMinDamageOverrideKey).m_iKey, (int*)&Config::kb(g_Variables.m_Ragebot.m_iMinDamageOverrideKey).m_iMode, true);
                    }
                    ImGui::CustomEndChild();

                    ImGui::CustomBeginChild("EXTRA", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, (region.y - (spacing.y * 4 + 62)) / 2.0f - 80));
                    {
                        Color& peekColor = Config::c(g_Variables.m_Ragebot.m_AutoPeekColor);
                        float colorArray[4] = {
                            peekColor.rBase(),
                            peekColor.gBase(),
                            peekColor.bBase(),
                            peekColor.aBase()
                        };

                        ImGui::Pickerbox(X("Quick Peek Assist"), &Config::b(g_Variables.m_Ragebot.m_bAutoPeek), colorArray, picker_flags);

                        peekColor.Set(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);

                        ImGui::BeginDisabled(!Config::b(g_Variables.m_Ragebot.m_bAutoPeek));
                        {
                            ImGui::Checkbox(X("Retreat On Key Release"), &Config::b(g_Variables.m_Ragebot.m_bRetreatOnRelease));
                            ImGui::Keybind(X("Quick Peek Assist Key"), (int*)&Config::kb(g_Variables.m_Ragebot.m_kbAutoPeek).m_iKey, (int*)&Config::kb(g_Variables.m_Ragebot.m_kbAutoPeek).m_iMode, true);
                        }
                        ImGui::EndDisabled();
                    }
                    ImGui::CustomEndChild();
                }
                ImGui::EndGroup();
            }
            else if (aimbot_subtabs == 1) {
                ImGui::BeginGroup();
                {
                    ImGui::CustomBeginChild("GENERAL", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, region.y - 62 - spacing.y * 3));
                    {
                        const char* pitch[] = { "None", "Up", "Down", "Zero" };
                        const char* yaw[] = { "None", "Backward" };

                        ImGui::Checkbox(X("Enabled"), &Config::b(g_Variables.m_Ragebot.m_bAntiaim));
                        ImGui::BeginDisabled(!Config::b(g_Variables.m_Ragebot.m_bAntiaim));
                        {
                            ImGui::Combo("Override Pitch", &Config::i(g_Variables.m_Ragebot.m_iPitchValue), pitch, IM_ARRAYSIZE(pitch));
                            ImGui::Combo("Override Yaw", &Config::i(g_Variables.m_Ragebot.m_iYawValue), yaw, IM_ARRAYSIZE(yaw));
                            ImGui::Checkbox("At Targets", &Config::b(g_Variables.m_Ragebot.m_bAtTarget));

                            ImGui::Keybind("Manual Left", (int*)&Config::kb(g_Variables.m_Ragebot.m_iLeftAntiaimKeybind).m_iKey, (int*)&Config::kb(g_Variables.m_Ragebot.m_iLeftAntiaimKeybind).m_iMode, true);
                            ImGui::Keybind("Manual Right", (int*)&Config::kb(g_Variables.m_Ragebot.m_iRightAntiaimKeybind).m_iKey, (int*)&Config::kb(g_Variables.m_Ragebot.m_iRightAntiaimKeybind).m_iMode, true);
                        }
                        ImGui::EndDisabled();
                    }
                    ImGui::CustomEndChild();
                }
                ImGui::EndGroup();

                ImGui::SameLine();

                ImGui::BeginGroup();
                {
                    ImGui::CustomBeginChild("OTHER", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, region.y - 62 - spacing.y * 3));
                    {
                        ImGui::BeginDisabled(!Config::b(g_Variables.m_Ragebot.m_bAntiaim));
                        ImGui::Keybind("Fake Pitch", (int*)&Config::kb(g_Variables.m_Ragebot.m_iFakePitch).m_iKey, (int*)&Config::kb(g_Variables.m_Ragebot.m_iFakePitch).m_iMode, true);
                        ImGui::EndDisabled();
                        ImGui::Keybind("Slow Walk", (int*)&Config::kb(g_Variables.m_Ragebot.m_bAccurateWalk).m_iKey, (int*)&Config::kb(g_Variables.m_Ragebot.m_bAccurateWalk).m_iMode, true);
                    }
                    ImGui::CustomEndChild();
                }
                ImGui::EndGroup();
            }
        }
        else if (tabs == 1) {
            if (visuals_subtabs == 0) {
                ImGui::BeginGroup();
                {
                    ImGui::CustomBeginChild("AMBIENCE", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, (region.y - (spacing.y * 4 + 62)) / 2.0f + 40));
                    {
                        Color& modelColor = Config::c(g_Variables.m_WorldVisuals.m_colWorldColor);
                        float colorArray[4] = {
                            modelColor.rBase(),
                            modelColor.gBase(),
                            modelColor.bBase(),
                            modelColor.aBase()
                        };

                        if (ImGui::ColorEdit3("World Color", colorArray, picker_flags))
                            modelColor.Set(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);

                        Color& modelColor1 = Config::c(g_Variables.m_WorldVisuals.m_colLighting);
                        float colorArray1[4] = {
                            modelColor1.rBase(),
                            modelColor1.gBase(),
                            modelColor1.bBase(),
                            modelColor1.aBase()
                        };

                        if (ImGui::ColorEdit3(X("Lighting Color"), colorArray1, picker_flags))
                            modelColor1.Set(colorArray1[0], colorArray1[1], colorArray1[2], colorArray1[3]);

                        Color& modelColor12 = Config::c(g_Variables.m_WorldVisuals.m_colSunColor);
                        float colorArray12[4] = {
                            modelColor12.rBase(),
                            modelColor12.gBase(),
                            modelColor12.bBase(),
                            modelColor12.aBase()
                        };

                        if (ImGui::ColorEdit3(X("Sun Color"), colorArray12, picker_flags))
                            modelColor12.Set(colorArray12[0], colorArray12[1], colorArray12[2], colorArray12[3]);

                        Color& modelColor124 = Config::c(g_Variables.m_WorldVisuals.m_colCloudColor);
                        float colorArray124[4] = {
                            modelColor124.rBase(),
                            modelColor124.gBase(),
                            modelColor124.bBase(),
                            modelColor124.aBase()
                        };

                        if (ImGui::ColorEdit3(X("Clouds Color"), colorArray124, picker_flags))
                            modelColor124.Set(colorArray124[0], colorArray124[1], colorArray124[2], colorArray124[3]);

                        Color& modelColor123 = Config::c(g_Variables.m_WorldEffects.m_colSkybox);
                        float colorArray123[4] = {
                            modelColor123.rBase(),
                            modelColor123.gBase(),
                            modelColor123.bBase(),
                            modelColor123.aBase()
                        };

                        if (ImGui::ColorEdit3(X("Sky Color"), colorArray123, picker_flags))
                            modelColor123.Set(colorArray123[0], colorArray123[1], colorArray123[2], colorArray123[3]);

                        ImGui::SliderInt("Exposure", &Config::i(g_Variables.m_WorldVisuals.m_nWorldExposure), 0, 200);
                    }
                    ImGui::CustomEndChild();

                    ImGui::CustomBeginChild("EFFECTS", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, (region.y - (spacing.y * 4 + 62)) / 2.0f - 40));
                    {
                        const static char* weather[] = { "None", "Ash", "Snow", "Stars" };

                        ImGui::Combo("Weather Effects", &Config::i(g_Variables.m_WorldEffects.m_iSelectedCustomWorldParticles), weather, IM_ARRAYSIZE(weather));
                        ImGui::SliderInt("Weather Density", &Config::i(g_Variables.m_WorldEffects.m_iSelectedCustomWorldParticleDensity), 5, 200);

                        Color& modelColor = Config::c(g_Variables.m_WorldEffects.m_colLocalBulletTracers);
                        float colorArray[4] = {
                            modelColor.rBase(),
                            modelColor.gBase(),
                            modelColor.bBase(),
                            modelColor.aBase()
                        };

                        ImGui::Pickerbox("Local Bullet Tracers", &Config::b(g_Variables.m_WorldEffects.m_bLocalBulletTracers), colorArray, picker_flags);

                        modelColor.Set(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);
                    }
                    ImGui::CustomEndChild();
                }
                ImGui::EndGroup();

                ImGui::SameLine();

                ImGui::BeginGroup();
                {
                    ImGui::CustomBeginChild("GRENADES", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, (region.y - (spacing.y * 4 + 62)) / 2.0f + 40));
                    {
                        Color& modelColor = Config::c(g_Variables.m_WorldVisuals.m_cGrenadePrediction);
                        float colorArray[4] = {
                            modelColor.rBase(),
                            modelColor.gBase(),
                            modelColor.bBase(),
                            modelColor.aBase()
                        };

                        ImGui::Pickerbox("Enable Grenade Prediction", &Config::b(g_Variables.m_WorldVisuals.m_bGrenadePrediction), colorArray, picker_flags);

                        modelColor.Set(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);

                        Color& modelColor1 = Config::c(g_Variables.m_WorldVisuals.m_colInferno);
                        float colorArray1[4] = {
                            modelColor1.rBase(),
                            modelColor1.gBase(),
                            modelColor1.bBase(),
                            modelColor1.aBase()
                        };

                        ImGui::Pickerbox("Enable Molotov Area", &Config::b(g_Variables.m_WorldVisuals.m_bDrawInferno), colorArray1, picker_flags);

                        modelColor1.Set(colorArray1[0], colorArray1[1], colorArray1[2], colorArray1[3]);
                    }
                    ImGui::CustomEndChild();

                    ImGui::CustomBeginChild("OTHER", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, (region.y - (spacing.y * 4 + 62)) / 2.0f - 40));
                    {
                        Color& modelColor = Config::c(g_Variables.m_Visuals.m_colHitmarker);
                        float colorArray[4] = {
                            modelColor.rBase(),
                            modelColor.gBase(),
                            modelColor.bBase(),
                            modelColor.aBase()
                        };

                        ImGui::Pickerbox("World Hitmarker", &Config::b(g_Variables.m_Visuals.m_bHitmarker), colorArray, picker_flags);

                        modelColor.Set(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);

                        Color& modelColor2 = Config::c(g_Variables.m_Misc.m_colHitMarker);
                        float colorArray2[4] = {
                            modelColor2.rBase(),
                            modelColor2.gBase(),
                            modelColor2.bBase(),
                            modelColor2.aBase()
                        };

                        ImGui::Pickerbox("2D Hitmarker", &Config::b(g_Variables.m_Misc.m_bHitMarker), colorArray2, picker_flags);

                        modelColor2.Set(colorArray2[0], colorArray2[1], colorArray2[2], colorArray2[3]);
                    }
                    ImGui::CustomEndChild();
                }
                ImGui::EndGroup();
            }
            else if (visuals_subtabs == 1) {
                ImGui::BeginGroup();
                {
                    ImGui::CustomBeginChild("PLAYER ESP", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, region.y - 62 - spacing.y * 3));
                    {
                        Color& modelColor = Config::c(g_Variables.m_Visuals.m_colNameEsp);
                        float colorArray[4] = {
                            modelColor.rBase(),
                            modelColor.gBase(),
                            modelColor.bBase(),
                            modelColor.aBase()
                        };

                        ImGui::Pickerbox("Name", &Config::b(g_Variables.m_Visuals.m_bEnableNameESP), colorArray, picker_flags);

                        modelColor.Set(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);

                        Color& modelColor1 = Config::c(g_Variables.m_Visuals.m_colBoxESP);
                        float colorArray1[4] = {
                            modelColor1.rBase(),
                            modelColor1.gBase(),
                            modelColor1.bBase(),
                            modelColor1.aBase()
                        };

                        ImGui::Pickerbox("Bounding Box", &Config::b(g_Variables.m_Visuals.m_bEnableBoxESP), colorArray1, picker_flags);

                        modelColor1.Set(colorArray1[0], colorArray1[1], colorArray1[2], colorArray1[3]);

                        Color& modelColor2 = Config::c(g_Variables.m_Visuals.m_colHealthBarEsp);
                        float colorArray2[4] = {
                            modelColor2.rBase(),
                            modelColor2.gBase(),
                            modelColor2.bBase(),
                            modelColor2.aBase()
                        };

                        ImGui::Pickerbox("Health Bar", &Config::b(g_Variables.m_Visuals.m_bEnableHealthESP), colorArray2, picker_flags);

                        modelColor2.Set(colorArray2[0], colorArray2[1], colorArray2[2], colorArray2[3]);

                        Color& modelColor3 = Config::c(g_Variables.m_Visuals.m_colWeaponEsp);
                        float colorArray3[4] = {
                            modelColor3.rBase(),
                            modelColor3.gBase(),
                            modelColor3.bBase(),
                            modelColor3.aBase()
                        };

                        ImGui::Pickerbox("Weapon Name", &Config::b(g_Variables.m_Visuals.m_bEnableWeaponESP), colorArray3, picker_flags);

                        modelColor3.Set(colorArray3[0], colorArray3[1], colorArray3[2], colorArray3[3]);

                        Color& modelColor4 = Config::c(g_Variables.m_Visuals.m_colWeaponIcon);
                        float colorArray4[4] = {
                            modelColor4.rBase(),
                            modelColor4.gBase(),
                            modelColor4.bBase(),
                            modelColor4.aBase()
                        };

                        ImGui::Pickerbox("Weapon Icon", &Config::b(g_Variables.m_Visuals.m_bEnableWeaponIconESP), colorArray4, picker_flags);

                        modelColor4.Set(colorArray4[0], colorArray4[1], colorArray4[2], colorArray4[3]);

                        Color& modelColor5 = Config::c(g_Variables.m_Visuals.m_colSkeletonEsp);
                        float colorArray5[4] = {
                            modelColor5.rBase(),
                            modelColor5.gBase(),
                            modelColor5.bBase(),
                            modelColor5.aBase()
                        };

                        ImGui::Pickerbox("Skeleton", &Config::b(g_Variables.m_Visuals.m_bEnableSkeletonESP), colorArray5, picker_flags);

                        modelColor5.Set(colorArray5[0], colorArray5[1], colorArray5[2], colorArray5[3]);

                        Color& modelColor6 = Config::c(g_Variables.m_Visuals.m_colOOFArrows);
                        float colorArray6[4] = {
                            modelColor6.rBase(),
                            modelColor6.gBase(),
                            modelColor6.bBase(),
                            modelColor6.aBase()
                        };


                        ImGui::Pickerbox("OOF Arrows", &Config::b(g_Variables.m_Visuals.m_bEnableOOFArrows), colorArray6, picker_flags);

                        modelColor6.Set(colorArray6[0], colorArray6[1], colorArray6[2], colorArray6[3]);

                        Color& modelColor7 = Config::c(g_Variables.m_Visuals.m_colGlow);
                        float colorArray7[4] = {
                            modelColor7.rBase(),
                            modelColor7.gBase(),
                            modelColor7.bBase(),
                            modelColor7.aBase()
                        };

                        ImGui::Pickerbox("Glow", &Config::b(g_Variables.m_Visuals.m_bGlow), colorArray7, picker_flags);

                        modelColor7.Set(colorArray7[0], colorArray7[1], colorArray7[2], colorArray7[3]);

                        static const char* flags[] = { "Money", "Armor", "Kit", "Scoped", "Defusing", "Latency" };
                        ImGui::MultiCombo("Flags", Config::vb(g_Variables.m_Visuals.m_vecFlags), flags, IM_ARRAYSIZE(flags));
                    }
                    ImGui::CustomEndChild();
                }
                ImGui::EndGroup();

                ImGui::SameLine();

                ImGui::BeginGroup();
                {
                    ImGui::CustomBeginChild("COLORED MODELS", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, region.y - 62 - spacing.y * 3));
                    {
                        Color& modelColor = Config::c(g_Variables.m_Visuals.m_colEnemyChams);
                        float colorArray[4] = {
                            modelColor.rBase(),
                            modelColor.gBase(),
                            modelColor.bBase(),
                            modelColor.aBase()
                        };

                        ImGui::Pickerbox("Enable Visible Chams", &Config::b(g_Variables.m_Visuals.m_bEnableEnemyChams), colorArray, picker_flags);

                        modelColor.Set(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);

                        static const char* materials_visible[] = { "Latex", "Bloom", "Glow", "Solid", "Ghost" };

                        ImGui::Combo("Visible Chams Material", &Config::i(g_Variables.m_Visuals.m_iChamMaterialEnemy), materials_visible, IM_ARRAYSIZE(materials_visible));

                        Color& modelColor1 = Config::c(g_Variables.m_Visuals.m_colEnemyChamsIgnoreZ);
                        float colorArray1[4] = {
                            modelColor1.rBase(),
                            modelColor1.gBase(),
                            modelColor1.bBase(),
                            modelColor1.aBase()
                        };

                        ImGui::Pickerbox("Enable Invisible Chams", &Config::b(g_Variables.m_Visuals.m_bEnableEnemyChamsIgnoreZ), colorArray1, picker_flags);

                        modelColor1.Set(colorArray1[0], colorArray1[1], colorArray1[2], colorArray1[3]);

                        ImGui::Combo("Invisible Chams Material", &Config::i(g_Variables.m_Visuals.m_iChamMaterialEnemyIgnoreZ), materials_visible, IM_ARRAYSIZE(materials_visible));

                        Color& modelColor2 = Config::c(g_Variables.m_Visuals.m_colSelfChams);
                        float colorArray2[4] = {
                            modelColor2.rBase(),
                            modelColor2.gBase(),
                            modelColor2.bBase(),
                            modelColor2.aBase()
                        };

                        ImGui::Pickerbox("Enable Local Chams", &Config::b(g_Variables.m_Visuals.m_bEnableSelfChams), colorArray2, picker_flags);

                        modelColor2.Set(colorArray2[0], colorArray2[1], colorArray2[2], colorArray2[3]);

                        ImGui::Combo("Local Chams Material", &Config::i(g_Variables.m_Visuals.m_iChamMaterialLocal), materials_visible, IM_ARRAYSIZE(materials_visible));
                    }
                    ImGui::CustomEndChild();
                }
                ImGui::EndGroup();
            }
            else if (visuals_subtabs == 2) {
                ImGui::BeginGroup();
                {
                    ImGui::CustomBeginChild("GENERAL", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, region.y - 62 - spacing.y * 3));
                    {
                        static const char* scope[] = { "Default", "Remove overlay", "Gradient", "Remove all" };
                        static const char* recoil[] = { "Default", "Remove punch", "Remove all" };
                        static const char* removals[] = { "Smoke", "Flash", "Torso", "Crosshair", "Overhead local", "Overhead team", "Team intro", "Fog brightness", "Scope zoom" };

                        ImGui::SliderInt(X("Override FOV"), &Config::i(g_Variables.m_Visuals.m_iWorldFov), 50, 140);

                        ImGui::SliderInt(X("Override Aspect Ratio"), &Config::i(g_Variables.m_WorldEffects.m_iAspectRatio), 0, 50);

                        ImGui::MultiCombo("Removals", Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals), removals, IM_ARRAYSIZE(removals));

                        if (Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals)[REMOVE_ZOOM]) {
                            ImGui::Checkbox(X("Force Second Zoom"), &Config::b(g_Variables.m_Visuals.m_bForceZoom));
                        }

                        ImGui::Combo("Remove Recoil", &Config::i(g_Variables.m_Visuals.m_iRemoveRecoil), recoil, IM_ARRAYSIZE(recoil));

                        ImGui::Combo("Remove Scope", &Config::i(g_Variables.m_Visuals.m_iRemoveScope), scope, IM_ARRAYSIZE(scope));

                        if (Config::i(g_Variables.m_Visuals.m_iRemoveScope) == SCOPE_GRADIENT) {
                            Color& modelColor = Config::c(g_Variables.m_Visuals.m_colScope);
                            float colorArray[4] = {
                                modelColor.rBase(),
                                modelColor.gBase(),
                                modelColor.bBase(),
                                modelColor.aBase()
                            };

                            if (ImGui::ColorEdit4(X("Color-in"), colorArray, picker_flags))
                                modelColor.Set(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);

                            Color& modelColor1 = Config::c(g_Variables.m_Visuals.m_colScopeOutSide);
                            float colorArray1[4] = {
                                modelColor1.rBase(),
                                modelColor1.gBase(),
                                modelColor1.bBase(),
                                modelColor1.aBase()
                            };

                            if (ImGui::ColorEdit4(X("Color-out"), colorArray1, picker_flags))
                                modelColor1.Set(colorArray1[0], colorArray1[1], colorArray1[2], colorArray1[3]);

                            ImGui::SliderInt(X("Scope Length"), &Config::i(g_Variables.m_Visuals.m_iScopeLength), 1, 500);
                            //ImGui::SliderInt(X("Scope Thickness"), &Config::i(g_Variables.m_Visuals.m_iScopeThickness), 1, 50);
                            ImGui::SliderInt(X("Scope Gap"), &Config::i(g_Variables.m_Visuals.m_iScopeGap), 0, 100);
                        }
                    }
                    ImGui::CustomEndChild();
                }
                ImGui::EndGroup();

                ImGui::SameLine();

                ImGui::BeginGroup();
                {
                    ImGui::CustomBeginChild("OTHER", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, region.y - 62 - spacing.y * 3));
                    {
                        ImGui::Checkbox("Override Viewmodel", &Config::b(g_Variables.m_Visuals.m_bViewmodelChanger));
                        ImGui::BeginDisabled(!Config::b(g_Variables.m_Visuals.m_bViewmodelChanger));
                        {
                            ImGui::SliderInt(X("Viewmodel FOV"), &Config::i(g_Variables.m_Visuals.iViewmodelFov), 40, 120);

                            ImGui::SliderFloat(X("Viewmodel X"), &Config::f(g_Variables.m_Visuals.flXOffset), -90.f, 90.f, "%.1f");
                            ImGui::SliderFloat(X("Viewmodel Y"), &Config::f(g_Variables.m_Visuals.flYOffset), -90.f, 90.f, "%.1f");
                            ImGui::SliderFloat(X("Viewmodel Z"), &Config::f(g_Variables.m_Visuals.flZOffset), -90.f, 90.f, "%.1f");
                        }
                        ImGui::EndDisabled();

                        ImGui::Checkbox("Enable Thirdperson", &Config::b(g_Variables.m_Visuals.m_bEnableThirdPerson));
                        ImGui::BeginDisabled(!Config::b(g_Variables.m_Visuals.m_bEnableThirdPerson));
                        {
                            ImGui::Keybind("Thirdperson Key", (int*)&Config::kb(g_Variables.m_Visuals.m_iThirdPersonKeybind).m_iKey, (int*)&Config::kb(g_Variables.m_Visuals.m_iThirdPersonKeybind).m_iMode, true);
                            ImGui::SliderInt("Thirdperson Distance", &Config::i(g_Variables.m_Visuals.m_iThirdPersonDistance), 30, 250);
                        }
                        ImGui::EndDisabled();
                    }
                    ImGui::CustomEndChild();
                }
                ImGui::EndGroup();
            }
        }
        else if (tabs == 2) {
            if (settings_subtabs == 0) {
                ImGui::BeginGroup();
                {
                    ImGui::CustomBeginChild("MOVEMENT", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, (region.y - (spacing.y * 4 + 62)) / 2.0f - 40));
                    {
                        ImGui::Checkbox("Bunny Hop", &Config::b(g_Variables.m_Movement.m_bEnableBunnyHop));
                        ImGui::Checkbox("Air Strafe", &Config::b(g_Variables.m_Movement.m_bEnableAutoStrafe));
                        ImGui::Checkbox("Standalone Quick Stop", &Config::b(g_Variables.m_Movement.m_bEnableQuickStop));
                        ImGui::Keybind("Edge Jump", (int*)&Config::kb(g_Variables.m_Movement.m_kEdgeJump).m_iKey, (int*)&Config::kb(g_Variables.m_Movement.m_kEdgeJump).m_iMode, true);
                    }
                    ImGui::CustomEndChild();

                    ImGui::CustomBeginChild("OTHER", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, (region.y - (spacing.y * 4 + 62)) / 2.0f + 40));
                    {
#ifdef _DEBUG
                        if (ImGui::Button(X("Debug"), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 40))) {
                            Interfaces::m_pEngine->ExecuteClientCmdUnrestricted(std::string(
                                std::string("sv_cheats 1; ") +
                                std::string("bot_kick; ") +
                                std::string("bot_stop 1; ") +
                                std::string("bot_add; ") +
                                std::string("sv_quantize_movement_input 0; ") +
                                std::string("sv_airaccelerate inf; ") +
                                std::string("sv_steamauth_enforce 0; ") +
                                std::string("mp_respawn_immunitytime -1; ")
                            ).c_str());
                        }
#endif
                        ImGui::Checkbox("Auto Pistol", &Config::b(g_Variables.m_Misc.m_bAutoPistol));
                        ImGui::Checkbox("Distant Scout", &Config::b(g_Variables.m_Misc.m_bDistantScoutSound));

                        ImGui::Checkbox("Enable Hit Sound", &Config::b(g_Variables.m_Misc.m_bHitSound));

                        ImGui::BeginDisabled(!Config::b(g_Variables.m_Misc.m_bHitSound));
                        {
                            const char* hitsound[] = { "Metallic", "Light", "Coin", "Money", "Whack" };
                            ImGui::Combo("Hit Sound", &Config::i(g_Variables.m_Misc.m_iHitSoundIndex), hitsound, IM_ARRAYSIZE(hitsound));

                            ImGui::SliderFloat("Hit Sound Volume", &Config::f(g_Variables.m_Misc.m_flHitsoundVolume), 0.f, 1.f);
                        }
                        ImGui::EndDisabled();
                    }
                    ImGui::CustomEndChild();
                }
                ImGui::EndGroup();

                ImGui::SameLine();

                ImGui::BeginGroup();
                {
                    ImGui::CustomBeginChild("AUTOBUY", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, (region.y - (spacing.y * 4 + 62)) / 2.0f + 40));
                    {
                        ImGui::Checkbox("Enable Auto Purchase", &Config::b(g_Variables.m_Misc.m_bAutoPurchase));
                        ImGui::BeginDisabled(!Config::b(g_Variables.m_Misc.m_bAutoPurchase)); {
                            const char* buy1[] = { "None", "Auto-sniper", "AWP", "SSG-08" };
                            const char* buy2[] = { "None", "Elite", "Five-7/Tec-9", "Deagle", "R8 Revolver" };
                            const char* buy3[] = { "Armor", "Taser", "Grenades", "Defuser" };

                            ImGui::Combo("Primary", &Config::i(g_Variables.m_Misc.m_iAutoPurchase1), buy1, IM_ARRAYSIZE(buy1));
                            ImGui::Combo("Secondary", &Config::i(g_Variables.m_Misc.m_iAutoPurchase2), buy2, IM_ARRAYSIZE(buy2));
                            ImGui::MultiCombo("Additional", Config::vb(g_Variables.m_Misc.m_uAutoPurchase3), buy3, IM_ARRAYSIZE(buy3));
                        }
                        ImGui::EndDisabled();
                    }
                    ImGui::CustomEndChild();

                    ImGui::CustomBeginChild("OVERLAY", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, (region.y - (spacing.y * 4 + 62)) / 2.0f - 40));
                    {
                        const char* overlay[] = { "Indicators", "Spectators", "Watermark" };
                        ImGui::MultiCombo("Overlay", Config::vb(g_Variables.m_Gui.m_vbOverlay), overlay, IM_ARRAYSIZE(overlay));
                        ImGui::Checkbox("Hit Logs", &Config::b(g_Variables.m_Misc.m_bHitlogs));
                    }
                    ImGui::CustomEndChild();
                }
                ImGui::EndGroup();
            }
            if (settings_subtabs == 1) {
                static int selected_cfg = -1;

                ImGui::BeginGroup();
                {
                    ImGui::CustomBeginChild("CONFIGS LIST", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, region.y - 62 - spacing.y * 3));
                    {
                        if (ImGui::BeginListBox("##configs_list", ImVec2(-1, -ImGui::GetFrameHeightWithSpacing() - 10)))
                        {
                            for (int i = 0; i < Config::vecFileNames.size(); ++i) {
                                bool is_selected = (selected_cfg == i);
                                if (ImGui::Selectable(Config::vecFileNames[i].c_str(), is_selected)) {
                                    selected_cfg = i;
                                }
                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndListBox();
                        }

                        if (ImGui::Button("Refresh", ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 40))) {
                            Config::Refresh();
                            selected_cfg = -1;
                        }
                    }
                    ImGui::CustomEndChild();
                }
                ImGui::EndGroup();

                ImGui::SameLine();

                ImGui::BeginGroup();
                {
                    ImGui::CustomBeginChild("CONFIG ACTIONS", ImVec2((region.x - (spacing.x * 3 + 180)) / 2, region.y - 62 - spacing.y * 3));
                    {
                        static char new_cfg_name[64] = "";

                        if (ImGui::InputTextEx("##newcfg", "Enter config name", new_cfg_name, sizeof(new_cfg_name), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 40), NULL));

                        if (ImGui::Button("Create", ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 40))) {
                            if (strlen(new_cfg_name) > 0) {
                                Config::Save(new_cfg_name, false);
                                Config::Refresh();
                                new_cfg_name[0] = '\0';
                            }
                        }

                        if (selected_cfg != -1) {
                            if (ImGui::Button("Load", ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 40))) {
                                Config::Load(Config::vecFileNames[selected_cfg], true);
                                InventoryChanger::m_bWantsHUDUpdate = true;
                                InventoryChanger::OnConfigLoad(Config::vecFileNames.at(selected_cfg));
                            }

                            if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 40))) {
                                Config::Save(Config::vecFileNames[selected_cfg], true);
                                InventoryChanger::m_bWantsHUDUpdate = true;
                                InventoryChanger::OnConfigSave(Config::vecFileNames.at(selected_cfg));
                            }

                            if (ImGui::Button("Delete", ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 40))) {
                                Config::Remove(Config::vecFileNames[selected_cfg]);
                                Config::Refresh();
                                selected_cfg = -1;
                            }
                        }
                        else {
                            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                            ImGui::Button("Load", ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 40));
                            ImGui::Button("Save", ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 40));
                            ImGui::Button("Delete", ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 40));
                            ImGui::PopStyleVar();
                        }
                    }
                    ImGui::CustomEndChild();
                }
                ImGui::EndGroup();
            }
        }
    }
    ImGui::End();
}
