#include "Overlay.h"

namespace {
    //watermark
    float g_window_alpha_w = 0.f;
    float g_window_height_w = 35.f;
    std::chrono::steady_clock::time_point g_last_update_w;
    float g_window_width_w = 180.f;
    ImVec2 g_window_pos_w = ImVec2(50, 50);
    bool g_window_pos_initialized_w = false;

    //kb list
    float g_window_alpha = 0.f;
    float g_window_height = 30.f;
    float g_bind_alpha[5] = { 0.f, 0.f, 0.f, 0.f, 0.f };
    std::chrono::steady_clock::time_point g_last_update;
    float g_window_width = 165.f;
    ImVec2 g_window_pos = ImVec2(50, 50);
    bool g_window_pos_initialized = false;

    //spec list
    float g_spec_alpha = 0.f;
    float g_spec_height = 30.f;
    std::chrono::steady_clock::time_point g_spec_last_update;
    float g_spec_width = 165.f;
    ImVec2 g_spec_pos = ImVec2(50, 50);
    bool g_spec_pos_initialized = false;

    //indicators
    float g_indicator_alpha[10] = { 0.f };
    float g_indicator_heights[10] = { 0.f };
    std::chrono::steady_clock::time_point g_indicator_last_update;
    ImVec2 g_indicator_pos = ImVec2(50, 50);
    bool g_indicator_pos_initialized = false;
}

void COverlay::Watermark() {
    if (!Config::vb(g_Variables.m_Gui.m_vbOverlay).at(OVERLAY_WATERMARK))
        return;

    static float last_update_time = 0.f;
    static int fps = 0, ping = 0, frame_count = 0;
    float current_time = ImGui::GetTime();

    if (current_time - last_update_time >= 1.f) {
        fps = frame_count;
        frame_count = 0;
        ping = Interfaces::m_pEngine->IsInGame() && Globals::m_pLocalPlayerController ? Globals::m_pLocalPlayerController->m_iPing() : 0;
        last_update_time = current_time;
    }

    frame_count++;
    auto now = std::chrono::steady_clock::now();
    float delta_time = std::min(std::chrono::duration<float>(now - g_last_update_w).count(), 0.1f);
    g_last_update_w = now;

    g_window_alpha_w = ImLerp(g_window_alpha_w, 1.f, delta_time * 12.f);
    if (g_window_alpha_w <= 0.01f)
        return;

    ImFont* font = Fonts::Verdana;
    std::string watermark_text = "sensical   " + std::to_string(fps) + "fps   " + std::to_string(ping) + "ms";

    ImVec2 text_size = font->CalcTextSizeA(18, FLT_MAX, 0.f, watermark_text.c_str());

    float total_width = text_size.x + 40.f;
    float line_height = text_size.y + 10.f;

    if (!g_window_pos_initialized_w) {
        ImVec2 screen_size = ImGui::GetIO().DisplaySize;
        g_window_pos_w = ImVec2((screen_size.x - total_width) * 0.5f, 10.f);
        g_window_pos_initialized_w = true;
    }

    const bool is_menu_open = Gui::m_bOpen;

    if (is_menu_open) {
        ImGui::SetNextWindowBgAlpha(0.f);
        ImGui::SetNextWindowSize(ImVec2(total_width, line_height));
        ImGui::SetNextWindowPos(g_window_pos_w, ImGuiCond_Once);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground;

        ImGui::Begin("WatermarkDrag", nullptr, flags);
        g_window_pos_w = ImGui::GetWindowPos();
        ImGui::End();
    }

    float start_x = g_window_pos_w.x;
    float gradient_size = total_width * 0.15f;
    float solid_start_x = start_x + gradient_size;
    float solid_end_x = start_x + total_width - gradient_size;

    float text_x = start_x + (total_width - text_size.x) * 0.5f;
    float text_y = g_window_pos_w.y;
    float line_top = text_y - 5.f;
    float line_bottom = line_top + line_height;

    auto draw_list = ImGui::GetBackgroundDrawList();

    if (g_window_alpha_w > 0.01f && font) {
        draw_list->AddRectFilledMultiColor(
            ImVec2(start_x, line_top),
            ImVec2(solid_start_x, line_bottom),
            IM_COL32(0, 0, 0, 0),
            IM_COL32(0, 0, 0, static_cast<int>(150 * g_window_alpha_w)),
            IM_COL32(0, 0, 0, static_cast<int>(150 * g_window_alpha_w)),
            IM_COL32(0, 0, 0, 0)
        );

        draw_list->AddRectFilled(
            ImVec2(solid_start_x, line_top),
            ImVec2(solid_end_x, line_bottom),
            IM_COL32(0, 0, 0, static_cast<int>(150 * g_window_alpha_w))
        );

        draw_list->AddRectFilledMultiColor(
            ImVec2(solid_end_x, line_top),
            ImVec2(start_x + total_width, line_bottom),
            IM_COL32(0, 0, 0, static_cast<int>(150 * g_window_alpha_w)),
            IM_COL32(0, 0, 0, 0),
            IM_COL32(0, 0, 0, 0),
            IM_COL32(0, 0, 0, static_cast<int>(150 * g_window_alpha_w))
        );

        draw_list->AddText(
            font,
            18,
            ImVec2(text_x, text_y),
            IM_COL32(255, 255, 255, static_cast<int>(255 * g_window_alpha_w)),
            watermark_text.c_str()
        );
    }
}
//void COverlay::KeybindList() {
//    //if (!Config::vb(g_Variables.m_Gui.m_vbOverlay).at(OVERLAY_KEYBIND))
//    //    return;
//
//    const bool is_menu_open = Gui::m_bOpen;
//    const auto now = std::chrono::steady_clock::now();
//    const float delta_time = std::min(std::chrono::duration<float>(now - g_last_update).count(), 0.1f);
//    g_last_update = now;
//
//    auto get_bind_type = [](int mode) -> const char* {
//        switch (mode) {
//        case HOLD: return "[hold]";
//        case TOGGLE: return "[toggle]";
//        case ALWAYS_ON: return "[always]";
//        default: return "[none]";
//        }
//        };
//
//    struct KeybindInfo {
//        const char* name;
//        bool* enabled;
//        KeyBind_t* bind;
//        float* value;
//    };
//
//    static bool edgejump = true;
//
//    std::vector<KeybindInfo> keybinds = {
//        {"Damage override", &Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), &Config::kb(g_Variables.m_Ragebot.m_iMinDamageOverrideKey), nullptr},
//        {"Hitchance override", &Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), &Config::kb(g_Variables.m_Ragebot.m_iHitchanceOverrideKey), nullptr},
//        {"Force bodyaim", &Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), &Config::kb(g_Variables.m_Ragebot.m_iForceBodyaimKey), nullptr},
//        {"Force headshot", &Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), &Config::kb(g_Variables.m_Ragebot.m_iForceHeadshotKey), nullptr},
//        {"Force shoot", &Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), &Config::kb(g_Variables.m_Ragebot.m_kbForceShoot), nullptr},
//        {"Manual right", &Config::b(g_Variables.m_Ragebot.m_bAntiaim), &Config::kb(g_Variables.m_Ragebot.m_iRightAntiaimKeybind), nullptr},
//        {"Manual left", &Config::b(g_Variables.m_Ragebot.m_bAntiaim), &Config::kb(g_Variables.m_Ragebot.m_iLeftAntiaimKeybind), nullptr},
//        {"Auto retreat", &Config::b(g_Variables.m_Ragebot.m_bAutoPeek), &Config::kb(g_Variables.m_Ragebot.m_kbAutoPeek), nullptr},
//        {"Fake pitch", &Config::b(g_Variables.m_Ragebot.m_bAntiaim), &Config::kb(g_Variables.m_Ragebot.m_iFakePitch), nullptr},
//        {"Mouse override", &Config::b(g_Variables.m_Ragebot.m_bAntiaim), &Config::kb(g_Variables.m_Ragebot.m_bMouseOverride), nullptr},
//        {"Edge jump", &edgejump, &Config::kb(g_Variables.m_Movement.m_kEdgeJump), nullptr},
//    };
//
//    std::vector<KeybindInfo> active_keybinds;
//    for (const auto& kb : keybinds) {
//        if (*kb.enabled && Input::HandleInput(*kb.bind)) {
//            active_keybinds.push_back(kb);
//        }
//    }
//
//    bool should_show_window = is_menu_open || !active_keybinds.empty();
//    float target_alpha = should_show_window ? 1.f : 0.f;
//    g_window_alpha = ImLerp(g_window_alpha, target_alpha, delta_time * 12.f);
//
//    if (g_window_alpha <= 0.01f && !should_show_window)
//        return;
//
//    const float item_height = 22;
//    float target_height = 36 + (active_keybinds.empty() ? 0 : active_keybinds.size() * item_height);
//
//    g_window_height = ImLerp(g_window_height, target_height, delta_time * 10.f);
//
//    float max_text_width = 220;
//    ImFont* font = Fonts::overlay3;
//
//    for (const auto& kb : active_keybinds) {
//        float total_width = font->CalcTextSizeA(16, FLT_MAX, 0.0f, (std::string(kb.name) + (kb.value ? std::to_string((int)*kb.value) : get_bind_type(kb.bind->m_iMode))).c_str()).x + 24;
//        if (total_width > max_text_width)
//            max_text_width = total_width;
//    }
//
//    g_window_width = ImLerp(g_window_width, max_text_width, delta_time * 10.f);
//
//    if (!g_window_pos_initialized) {
//        g_window_pos = ImVec2(50, 50);
//        g_window_pos_initialized = true;
//    }
//
//    ImGui::SetNextWindowBgAlpha(160 * g_window_alpha / 255.f);
//    ImGui::SetNextWindowSize(ImVec2(g_window_width, g_window_height));
//    ImGui::SetNextWindowPos(g_window_pos, ImGuiCond_Once);
//
//    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize;
//    if (!is_menu_open) flags |= ImGuiWindowFlags_NoInputs;
//
//    ImGui::Begin("Keybinds", nullptr, flags);
//
//    if (g_window_alpha > 0.01f) {
//        g_window_pos = ImGui::GetWindowPos();
//        auto draw_list = ImGui::GetWindowDrawList();
//        ImVec2 window_pos = ImGui::GetWindowPos();
//
//        draw_list->AddRectFilled(
//            window_pos,
//            ImVec2(window_pos.x + g_window_width, window_pos.y + g_window_height),
//            IM_COL32(0, 0, 0, static_cast<int>(160 * g_window_alpha)),
//            6
//        );
//
//        draw_list->AddRectFilledMultiColor(
//            window_pos,
//            ImVec2(window_pos.x + g_window_width, window_pos.y + 3),
//            IM_COL32(143, 154, 255, static_cast<int>(255 * g_window_alpha)),
//            IM_COL32(0, 0, 0, 0),
//            IM_COL32(0, 0, 0, 0),
//            IM_COL32(143, 154, 255, static_cast<int>(255 * g_window_alpha))
//        );
//
//        ImVec2 text_size = font->CalcTextSizeA(16, FLT_MAX, 0.0f, "Binds");
//        draw_list->AddText(
//            font, 16,
//            ImVec2(window_pos.x + (g_window_width - text_size.x) * 0.5f, window_pos.y + 10),
//            IM_COL32(255, 255, 255, static_cast<int>(255 * g_window_alpha)),
//            "Binds"
//        );
//
//        ImVec2 binds_pos(window_pos.x + 6, window_pos.y + 34);
//        for (const auto& kb : active_keybinds) {
//            draw_list->AddText(
//                font, 16,
//                ImVec2(binds_pos.x, binds_pos.y),
//                IM_COL32(255, 255, 255, static_cast<int>(255 * g_window_alpha)),
//                kb.name
//            );
//
//            const char* type_str = kb.value ? std::to_string((int)*kb.value).c_str() : get_bind_type(kb.bind->m_iMode);
//            ImVec2 type_size = font->CalcTextSizeA(16, FLT_MAX, 0.0f, type_str);
//            draw_list->AddText(
//                font, 16,
//                ImVec2(window_pos.x + g_window_width - type_size.x - 6, binds_pos.y),
//                IM_COL32(200, 200, 200, static_cast<int>(160 * g_window_alpha)),
//                type_str
//            );
//
//            binds_pos.y += item_height;
//        }
//    }
//
//    ImGui::End();
//}

void COverlay::SpectatorsList() {
    if (!Config::vb(g_Variables.m_Gui.m_vbOverlay).at(OVERLAY_SPECTATORS))
        return;

    const bool is_menu_open = Gui::m_bOpen;
    const auto now = std::chrono::steady_clock::now();
    const float delta_time = std::min(std::chrono::duration<float>(now - g_spec_last_update).count(), 0.1f);
    g_spec_last_update = now;

    std::vector<const char*> spectators;
    bool in_game = Interfaces::m_pEngine->IsInGame() && Interfaces::m_pEngine->IsConnected() && Globals::m_pLocalPlayerPawn;

    if (in_game) {
        for (EntityObject_t& object : g_Entities->m_vecEntities) {
            if (object.m_eType != EEntityType::ENTITY_PLAYER)
                continue;

            CCSPlayerController* pPlayerController = reinterpret_cast<CCSPlayerController*>(object.m_pEntity);
            if (!pPlayerController || pPlayerController->m_bPawnIsAlive())
                continue;

            C_CSObserverPawn* pPawn = pPlayerController->m_hObserverPawn().Get();
            if (!pPawn)
                continue;

            CPlayer_ObserverServices* pObserverServices = pPawn->m_pObserverServices();
            if (!pObserverServices)
                continue;

            C_BaseEntity* pObserverTarget = pObserverServices->m_hObserverTarget().Get();
            if (!pObserverTarget)
                continue;

            if (pObserverTarget->GetRefEHandle().GetEntryIndex() != Globals::m_pLocalPlayerPawn->GetRefEHandle().GetEntryIndex())
                continue;

            if (pPlayerController->m_sSanitizedPlayerName()) {
                spectators.push_back(pPlayerController->m_sSanitizedPlayerName());
            }
        }
    }

    bool should_show_window = is_menu_open || (!spectators.empty() && in_game);
    float target_alpha = should_show_window ? 1.f : 0.f;
    g_spec_alpha = ImLerp(g_spec_alpha, target_alpha, delta_time * 12.f);

    if (g_spec_alpha <= 0.01f && !should_show_window)
        return;

    ImFont* font = Fonts::Verdana;
    const char* header_text = "spectators";

    ImVec2 header_text_size = font->CalcTextSizeA(18, FLT_MAX, 0.f, header_text);

    float max_name_width = header_text_size.x;
    if (in_game && !spectators.empty()) {
        for (const auto& name : spectators) {
            ImVec2 name_size = font->CalcTextSizeA(18, FLT_MAX, 0.f, name);
            if (name_size.x > max_name_width)
                max_name_width = name_size.x;
        }
    }

    float total_width = max_name_width + 40.f;
    float header_height = header_text_size.y + 10.f;
    float name_height = header_text_size.y + 5.f;
    float total_height = header_height + (spectators.empty() ? 0 : spectators.size() * name_height);

    if (!g_spec_pos_initialized) {
        ImVec2 screen_size = ImGui::GetIO().DisplaySize;
        g_spec_pos = ImVec2(screen_size.x - total_width - 10.f, 50.f);
        g_spec_pos_initialized = true;
    }

    if (is_menu_open) {
        ImGui::SetNextWindowBgAlpha(0.f);
        ImGui::SetNextWindowSize(ImVec2(total_width, total_height));
        ImGui::SetNextWindowPos(g_spec_pos, ImGuiCond_Once);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground;

        ImGui::Begin("SpectatorsDrag", nullptr, flags);
        g_spec_pos = ImGui::GetWindowPos();
        ImGui::End();
    }

    float start_x = g_spec_pos.x;
    float gradient_size = total_width * 0.15f;
    float solid_start_x = start_x + gradient_size;
    float solid_end_x = start_x + total_width - gradient_size;

    float header_text_x = start_x + (total_width - header_text_size.x) * 0.5f;
    float header_text_y = g_spec_pos.y;
    float header_top = header_text_y - 5.f;
    float header_bottom = header_top + header_height;

    auto draw_list = ImGui::GetBackgroundDrawList();

    if (g_spec_alpha > 0.01f && font) {
        draw_list->AddRectFilledMultiColor(
            ImVec2(start_x, header_top),
            ImVec2(solid_start_x, header_bottom),
            IM_COL32(0, 0, 0, 0),
            IM_COL32(0, 0, 0, static_cast<int>(150 * g_spec_alpha)),
            IM_COL32(0, 0, 0, static_cast<int>(150 * g_spec_alpha)),
            IM_COL32(0, 0, 0, 0)
        );

        draw_list->AddRectFilled(
            ImVec2(solid_start_x, header_top),
            ImVec2(solid_end_x, header_bottom),
            IM_COL32(0, 0, 0, static_cast<int>(150 * g_spec_alpha))
        );

        draw_list->AddRectFilledMultiColor(
            ImVec2(solid_end_x, header_top),
            ImVec2(start_x + total_width, header_bottom),
            IM_COL32(0, 0, 0, static_cast<int>(150 * g_spec_alpha)),
            IM_COL32(0, 0, 0, 0),
            IM_COL32(0, 0, 0, 0),
            IM_COL32(0, 0, 0, static_cast<int>(150 * g_spec_alpha))
        );

        draw_list->AddText(
            font,
            18,
            ImVec2(header_text_x, header_text_y),
            IM_COL32(255, 255, 255, static_cast<int>(255 * g_spec_alpha)),
            header_text
        );

        if (in_game && !spectators.empty()) {
            float names_start_y = header_bottom;

            for (const auto& name : spectators) {
                ImVec2 name_size = font->CalcTextSizeA(18, FLT_MAX, 0.f, name);
                float name_x = start_x + (total_width - name_size.x) * 0.5f;
                float name_y = names_start_y;

                float name_top = name_y - 2.f;
                float name_bottom = name_top + name_height;

                draw_list->AddRectFilledMultiColor(
                    ImVec2(start_x, name_top),
                    ImVec2(solid_start_x, name_bottom),
                    IM_COL32(0, 0, 0, 0),
                    IM_COL32(0, 0, 0, static_cast<int>(100 * g_spec_alpha)),
                    IM_COL32(0, 0, 0, static_cast<int>(100 * g_spec_alpha)),
                    IM_COL32(0, 0, 0, 0)
                );

                draw_list->AddRectFilled(
                    ImVec2(solid_start_x, name_top),
                    ImVec2(solid_end_x, name_bottom),
                    IM_COL32(0, 0, 0, static_cast<int>(100 * g_spec_alpha))
                );

                draw_list->AddRectFilledMultiColor(
                    ImVec2(solid_end_x, name_top),
                    ImVec2(start_x + total_width, name_bottom),
                    IM_COL32(0, 0, 0, static_cast<int>(100 * g_spec_alpha)),
                    IM_COL32(0, 0, 0, 0),
                    IM_COL32(0, 0, 0, 0),
                    IM_COL32(0, 0, 0, static_cast<int>(100 * g_spec_alpha))
                );

                draw_list->AddText(
                    font,
                    18,
                    ImVec2(name_x, name_y),
                    IM_COL32(255, 255, 255, static_cast<int>(255 * g_spec_alpha)),
                    name
                );

                names_start_y += name_height;
            }
        }
    }
}

void COverlay::Indicators() {
    if (!Config::vb(g_Variables.m_Gui.m_vbOverlay).at(OVERLAY_INDICATORS))
        return;

    const bool is_menu_open = Gui::m_bOpen;
    const auto now = std::chrono::steady_clock::now();
    const float delta_time = std::min(std::chrono::duration<float>(now - g_indicator_last_update).count(), 0.1f);
    g_indicator_last_update = now;

    struct IndicatorInfo {
        const char* name;
        bool* enabled;
        KeyBind_t* bind;
        bool condition;
        int index;
        bool show_value;  // Whether to show the value (used for Minimum Damage)
    };

    // Move Minimum Damage to the beginning of the indicators list and set a unique index
    std::vector<IndicatorInfo> indicators = {
        {"", &Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), &Config::kb(g_Variables.m_Ragebot.m_iMinDamageOverrideKey), false, 0, true},  // No name, show value at the top
        {"Force Shoot", &Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), &Config::kb(g_Variables.m_Ragebot.m_kbForceShoot), false, 1, false},
        {"Left Antiam", &Config::b(g_Variables.m_Ragebot.m_bAntiaim), &Config::kb(g_Variables.m_Ragebot.m_iLeftAntiaimKeybind), false, 2, false},
        {"Right Antiam", &Config::b(g_Variables.m_Ragebot.m_bAntiaim), &Config::kb(g_Variables.m_Ragebot.m_iRightAntiaimKeybind), false, 3, false},
        {"Body Aim", &Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), &Config::kb(g_Variables.m_Ragebot.m_iForceBodyaimKey), false, 4, false},
        {"Head Only", &Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), &Config::kb(g_Variables.m_Ragebot.m_iForceHeadshotKey), false, 5, false},
        {"Hitchance", &Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), &Config::kb(g_Variables.m_Ragebot.m_iHitchanceOverrideKey), false, 6, false},
        {"Auto Peek", &Config::b(g_Variables.m_Ragebot.m_bAutoPeek), &Config::kb(g_Variables.m_Ragebot.m_kbAutoPeek), false, 7, false},
    };

    static float force_color_alpha = 0.f;
    static float slow_color_alpha = 0.f;

    bool slow_condition = false;
    float velocity_modifier = 1.0f;

    if (Interfaces::m_pEngine->IsInGame() && Globals::m_pLocalPlayerPawn) {
        velocity_modifier = Globals::m_pLocalPlayerPawn->m_flVelocityModifier();
        slow_condition = velocity_modifier < 1.0f;
    }

    // Loop through each indicator and manage its animations
    for (auto& indicator : indicators) {
        bool current_condition = false;

        if (indicator.index == 1) {
            current_condition = slow_condition;
        }
        else {
            current_condition = *indicator.enabled && Input::HandleInput(*indicator.bind);
        }

        if (indicator.condition != current_condition) {
            indicator.condition = current_condition;
        }

        // Smooth fade-in/fade-out for the indicator
        float target_alpha = (indicator.condition || is_menu_open) ? 1.f : 0.f;
        g_indicator_alpha[indicator.index] = ImLerp(g_indicator_alpha[indicator.index], target_alpha, delta_time * 10.f);

        // Height smooth transition for the indicators (sliding in/out)
        float target_height = (indicator.condition || is_menu_open) ? 45.f : 0.f;
        g_indicator_heights[indicator.index] = ImLerp(g_indicator_heights[indicator.index], target_height, delta_time * 12.f);
    }

    // Handling force color animation (smooth transition)
    bool has_max_accuracy = NRagebot::g_Accuracy->HasMaximumAccuracy();
    float target_force_color = has_max_accuracy ? 0.f : 1.f;
    force_color_alpha = ImLerp(force_color_alpha, target_force_color, delta_time * 10.f);

    // Handling slow condition animation (smooth transition)
    float target_slow_color = slow_condition ? (1.0f - velocity_modifier) : 0.f;
    slow_color_alpha = ImLerp(slow_color_alpha, target_slow_color, delta_time * 10.f);

    auto draw_list = ImGui::GetBackgroundDrawList();
    ImFont* font = Fonts::SegoeUIBold25;

    if (font) {
        ImVec2 screen_size = ImGui::GetIO().DisplaySize;
        const float spacing = 12.f; // Better spacing for a modern clean look

        float total_active_height = 0.f;
        for (int i = 0; i < indicators.size(); i++) {
            if (g_indicator_alpha[i] > 0.01f) {
                total_active_height += g_indicator_heights[i] + (i > 0 ? spacing : 0);
            }
        }

        float start_y = screen_size.y * 0.5f - total_active_height * 0.5f;
        float current_y = start_y;

        // Draw the indicators with smooth animations
        for (int i = 0; i < indicators.size(); i++) {
            const auto& indicator = indicators[i];

            if (g_indicator_alpha[i] <= 0.01f || g_indicator_heights[i] <= 1.f)
                continue;

            const char* text = indicator.name;
            ImVec2 text_size = font->CalcTextSizeA(30, FLT_MAX, 0.f, text);

            float text_x = 10.f;
            float text_y = current_y + (g_indicator_heights[i] - text_size.y) * 0.5f - 3.f;
            float line_start_x = 0.f;
            float line_end_x = text_x + text_size.x + 25.f;  // Wider end for smoother look
            float line_height = g_indicator_heights[i];
            float line_top = current_y;
            float line_bottom = line_top + line_height;
            float gradient_start_x = line_start_x + (line_end_x - line_start_x) * 0.75f;

            // Smooth background with gradient transition for sleek visuals
            draw_list->AddRectFilled(
                ImVec2(line_start_x, line_top),
                ImVec2(gradient_start_x, line_bottom),
                IM_COL32(0, 0, 0, static_cast<int>(180 * g_indicator_alpha[i]))  // Transparent dark background
            );

            draw_list->AddRectFilledMultiColor(
                ImVec2(gradient_start_x, line_top),
                ImVec2(line_end_x, line_bottom),
                IM_COL32(0, 0, 0, static_cast<int>(180 * g_indicator_alpha[i])),
                IM_COL32(0, 0, 0, 0),
                IM_COL32(0, 0, 0, 0),
                IM_COL32(0, 0, 0, static_cast<int>(180 * g_indicator_alpha[i]))
            );

            // Smooth color transitions for each indicator type
            ImU32 text_color;
            if (i == 0) {
                int r = 255;
                int g = static_cast<int>(255 * (1.f - force_color_alpha));
                int b = static_cast<int>(255 * (1.f - force_color_alpha));
                text_color = IM_COL32(r, g, b, static_cast<int>(255 * g_indicator_alpha[i]));
            }
            else if (i == 1) {
                int r = 255;
                int g = static_cast<int>(75 + (255 - 75) * (1.f - slow_color_alpha));
                int b = static_cast<int>(75 + (255 - 75) * (1.f - slow_color_alpha));
                text_color = IM_COL32(r, g, b, static_cast<int>(255 * g_indicator_alpha[i]));
            }
            else {
                text_color = IM_COL32(255, 255, 255, static_cast<int>(255 * g_indicator_alpha[i]));
            }

            // **Display minimum damage value dynamically** (replace fixed value with the actual value)
            if (indicator.index == 0) {  // For the Minimum Damage indicator
                // Get the minimum damage value from the active Ragebot configuration
                int min_damage_value = g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamage;

                char value_text[64];
                _snprintf_s(value_text, sizeof(value_text), "%d", min_damage_value);  // Formatting value

                draw_list->AddText(
                    font,
                    30,
                    ImVec2(text_x + text_size.x + 10.f, text_y),  // Move text to the right of the indicator
                    text_color,
                    value_text
                );
            }
            else {
                // Add the name text for other indicators
                draw_list->AddText(
                    font,
                    30,
                    ImVec2(text_x, text_y),
                    text_color,
                    text
                );
            }

            current_y += g_indicator_heights[i] + spacing;
        }
    }
}
