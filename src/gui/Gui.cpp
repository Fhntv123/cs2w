#include "../Precompiled.h"
#include "fonts.h"
#include "new/new_fonts.h"
#include "new/new_menu.h"

constexpr ImVec2 MENU_SIZE = { 734, 489 };

const char* arrChamMaterials[] = {
    "Latex",
    "Bloom",
    "Glow",
    "Flat",
    "Ghost" // yep ts tuff
};

const char* arrCustomWeatherEffects[] = {
    "None",
    "Ashes",
    "Snow",
    "Stars",
};

const char* arrSkyboxNames[] = {
    "Sunset",
    "Overcast",
    "Night",
    "Aztec",
    "Jungle",
    "Cloudy",
    "Overpass",
    "Nuke",
    "Office",
    "Anubis",
    "Train",
    "Vertigo"
};

const char* szHitsounds[] = {
    "Metallic",
    "Light",
    "Coin",
    "Click",
    "Whack"
};

const char* szWatermark[] =
{
    "FPS",
    "Ping",
    "Speed",
};

const char* zxAutoBuy1[] =
{
    "None",
    "Auto-snipers",
    "AWP",
    "SSG-08",
};

const char* zxAutoBuy2[] =
{
    "None",
    "Elite",
    "Five-7/Tec-9",
    "Deagle",
    "Revolver",
};

const char* zxAutoBuy3[] =
{
    "Armor",
    "Taser",
    "Grenades",
    "Defuser",
};

const int iHitsoundCount = sizeof(szHitsounds) / sizeof(szHitsounds[0]);

enum EMenuHitsounds : int {
    COINDROP = 0,
    SHOPMENUCLICK,
    WHACK
};
enum ETabs : int
{
    TABS_RAGEBOT = 0,
    TABS_LEGITBOT,
    TABS_VISUALS,
    TABS_INVENTORY,
    TABS_CONFIG,
    TABS_MISC,
    TABS_COUNT
};

enum ESubTabs_Ragebot : int
{
    SUBTAB_AIMBOT = 0,
    SUBTAB_ANTIAIM
};

enum ESubTabs_Visuals : int
{
    SUBTAB_LOCAL = 0,
    SUBTAB_ENEMY,
    SUBTAB_WORLD,
    SUBTAB_SKYBOXES
};

void Gui::Initialize(unsigned int uFontFlags)
{
    ImGui::StyleColorsDark();

    //ApraGui::SetStyles(MENU_SIZE);

    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 1.0f;
    io.FontAllowUserScaling = false;

    ImFontConfig cfg;
    cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_LoadColor;

    font::inter_default = io.Fonts->AddFontFromMemoryTTF(inter_medium, sizeof(inter_medium), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::inter_bold = io.Fonts->AddFontFromMemoryTTF(inter_bold, sizeof(inter_bold), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::icon = io.Fonts->AddFontFromMemoryTTF(icon_font, sizeof(icon_font), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    ImFontConfig imVerdanaConfig = { };
    imVerdanaConfig.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_ForceAutoHint;

    Fonts::Default = io.Fonts->AddFontFromFileTTF(X("C:\\Windows\\Fonts\\calibri.ttf"), 14, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic()); //Fonts::Default = io.Fonts->AddFontFromFileTTF(X("C:\\Windows\\Fonts\\Verdana.ttf"), 16, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::GunIcons = io.Fonts->AddFontFromMemoryTTF(GunIcons, sizeof(GunIcons), 12, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::ESP = io.Fonts->AddFontFromMemoryTTF(SmallestPixel, sizeof(SmallestPixel), 10, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::Verdana = io.Fonts->AddFontFromFileTTF(X("C:\\Windows\\Fonts\\Verdana.ttf"), 18, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::VerdanaBold = io.Fonts->AddFontFromFileTTF(X("C:\\Windows\\Fonts\\Verdana.ttf"), 14, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::VerdanaEsp = io.Fonts->AddFontFromFileTTF(X("C:\\Windows\\Fonts\\Verdana.ttf"), 10, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::SegoeUIBold = io.Fonts->AddFontFromFileTTF(X("C:\\Windows\\Fonts\\segoeui.ttf"), 12, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::SegoeUIBold18 = io.Fonts->AddFontFromFileTTF(X("C:\\Windows\\Fonts\\segoeuib.ttf"), 18, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::SegoeUIBold25 = io.Fonts->AddFontFromFileTTF(X("C:\\Windows\\Fonts\\segoeuib.ttf"), 30, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::SegoeUIBold18_ITALIC = io.Fonts->AddFontFromFileTTF(X("C:\\Windows\\Fonts\\seguisbi.ttf"), 18, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::overlay2 = io.Fonts->AddFontFromFileTTF(X("C:\\Windows\\Fonts\\framd.ttf"), 18, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());

    Fonts::icons[0] = io.Fonts->AddFontFromMemoryTTF(icons_data, sizeof icons_data, 24, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::logo = io.Fonts->AddFontFromMemoryTTF(icons_data, sizeof icons_data, 40, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::icons[1] = io.Fonts->AddFontFromMemoryTTF(icons_data, sizeof icons_data, 15, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::icons[2] = io.Fonts->AddFontFromMemoryTTF(icons_data, sizeof icons_data, 10, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    Fonts::inter[0] = io.Fonts->AddFontFromMemoryTTF(inter_semibold_data, sizeof inter_semibold_data, 15, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::inter[1] = io.Fonts->AddFontFromMemoryTTF(inter_medium_data, sizeof inter_medium_data, 13, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::inter[2] = io.Fonts->AddFontFromMemoryTTF(inter_semibold_data, sizeof inter_semibold_data, 13, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    Fonts::overlay = io.Fonts->AddFontFromMemoryTTF(overlay, sizeof overlay, 13.5f, &cfg, io.Fonts->GetGlyphRangesDefault());

    ImFontConfig cfg2;
    cfg2.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LightHinting;

    Fonts::overlay3 = io.Fonts->AddFontFromMemoryTTF(overlay, sizeof overlay, 16, &cfg2, io.Fonts->GetGlyphRangesCyrillic());

    static const ImWchar arrIconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    ImFontConfig imFAFontConfig;
    imFAFontConfig.MergeMode = true;
    imFAFontConfig.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LoadColor | ImGuiFreeTypeBuilderFlags_Bitmap;
    Fonts::imFA = io.Fonts->AddFontFromMemoryCompressedTTF(FA_FONT, FA_compressed_size, 15.f, &imFAFontConfig, arrIconRanges);

    m_bInitialized = ImGuiFreeType::BuildFontAtlas(io.Fonts, uFontFlags);
}

void Gui::Update(ImGuiIO& io)
{
    Gui::m_vecScreenSize = io.DisplaySize;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0.f;
    style.Alpha = 1.f;
}

Color GetRarityColor(int nRarity, int nID)
{
    if (nID == 0)
        return Color(176, 195, 217, 255);
    else
    {
        switch (nRarity)
        {
        case 0:
            return Color(0, 0, 0, 0);
        case 1:
            return Color(176, 195, 217, 255);
        case 2:
            return Color(94, 152, 217, 255);
        case 3:
            return Color(75, 105, 255, 255);
        case 4:
            return Color(136, 71, 255, 255);
        case 5:
            return Color(211, 44, 230, 255);
        case 6:
            return Color(235, 75, 75, 255);
        case 7:
            return Color(228, 174, 57, 255);
        default:
            return Color(176, 195, 217, 255);
        }
    }
}

static float s_flWatermarkWidth = 0.f;
void Gui::RenderWatermark()
{
    //if (!Config::b(g_Variables.m_Misc.m_bWatermark))
    //    return;

    struct StatComponent
    {
        int m_iValue;
        const char* szLabel;
        char m_szValueBuffer[16];
        ImVec2 m_vValueSize;
        ImVec2 m_vLabelSize;
    };

    std::vector<StatComponent> vecComponents;

    if (Config::vb(g_Variables.m_Misc.m_vbWatermark).at(EWatermark::WATERMARK_FPS))
    {
        vecComponents.push_back({ Gui::m_iAverageFPS, "fps" });
    }

    if (Config::vb(g_Variables.m_Misc.m_vbWatermark).at(EWatermark::WATERMARK_PING))
    {
        auto pEngine = Interfaces::m_pEngine;
        if (pEngine)
        {
            auto pNetChannelInfo = pEngine->GetNetChannelInfo();
            if (pNetChannelInfo)
            {
                int iLatency = (int)(pNetChannelInfo->GetNetLatency() * 1000.f);
                vecComponents.push_back({ iLatency, "ms" });
            }
        }
    }

    if (Config::vb(g_Variables.m_Misc.m_vbWatermark).at(EWatermark::WATERMARK_SPEED))
    {
        int iSpeed = 0;

        if (C_CSPlayerPawn* pPawn = Globals::m_pLocalPlayerPawn)
        {
            iSpeed = pPawn->m_vecAbsVelocity().Length2D();
        }

        vecComponents.push_back({ iSpeed, "speed" });
    }

    constexpr float PADDING = 8.0f;
    constexpr float ROUNDING = 4.0f;
    constexpr float BLUR_INTENSITY = 4.0f;
    constexpr float ANIMATION_SPEED = 0.3f;

    blurEffect.BeginBlur();

    ImDrawList* pDrawList = ImGui::GetBackgroundDrawList();
    ImVec2 vDisplaySize = ImGui::GetIO().DisplaySize;

    const char* szMainLabel = "SENSICAL V2";
    const char* szBuildLabel = " ALPHA";
    float flMainFontSize = Fonts::SegoeUIBold18->FontSize;
    ImVec2 vMainLabelSize = Fonts::SegoeUIBold18->CalcTextSizeA(flMainFontSize, FLT_MAX, 0, szMainLabel);
    ImVec2 vBuildLabelSize = Fonts::SegoeUIBold18->CalcTextSizeA(flMainFontSize, FLT_MAX, 0, szBuildLabel);

    float flSmallFontSize = Fonts::SegoeUIBold->FontSize;
    float flVerticalOffset = (flMainFontSize - flSmallFontSize) / 2;
    float flSpacing = flMainFontSize * 0.5f;

    float flTotalContentWidth = vMainLabelSize.x + vBuildLabelSize.x + (vecComponents.empty() ? 0.f : flSpacing);

    for (int i = 0; i < vecComponents.size(); ++i)
    {
        auto& component = vecComponents[i];
        _snprintf_s(component.m_szValueBuffer, sizeof(component.m_szValueBuffer), "%d", component.m_iValue);
        component.m_vValueSize = Fonts::SegoeUIBold18->CalcTextSizeA(flMainFontSize, FLT_MAX, 0, component.m_szValueBuffer);
        component.m_vLabelSize = Fonts::SegoeUIBold->CalcTextSizeA(flSmallFontSize, FLT_MAX, 0, component.szLabel);

        if (i < vecComponents.size() - 1)
            flTotalContentWidth += flSpacing + component.m_vValueSize.x + (flSpacing * 0.4f) + component.m_vLabelSize.x;

        else
            flTotalContentWidth += component.m_vValueSize.x + (flSpacing * 0.4f) + component.m_vLabelSize.x;
    }

    static float s_flWatermarkWidth = flTotalContentWidth + PADDING * 2;
    s_flWatermarkWidth = ImLerp(s_flWatermarkWidth, flTotalContentWidth + PADDING * 2, ANIMATION_SPEED);

    ImVec2 vPosition =
    {
        vDisplaySize.x - s_flWatermarkWidth - PADDING,
        PADDING
    };

    ImVec2 vSize = { s_flWatermarkWidth, flMainFontSize + PADDING };
    ImVec2 vEndPos = { vPosition.x + vSize.x, vPosition.y + vSize.y };

    blurEffect.ApplyBlur(pDrawList, vPosition, vSize, BLUR_INTENSITY, ROUNDING);

    pDrawList->PushClipRect(vPosition, vEndPos);

    ImVec2 vCurrentPos =
    {
        vPosition.x + PADDING,
        vPosition.y + (vSize.y - flMainFontSize) / 2 - 1.5f
    };

    pDrawList->AddText(Fonts::SegoeUIBold18, flMainFontSize, vCurrentPos, ImColor(160, 125, 255, 255), szMainLabel);
    vCurrentPos.x += vMainLabelSize.x;

    pDrawList->AddText(Fonts::SegoeUIBold18, flMainFontSize, vCurrentPos, ImColor(255, 255, 255, 255), szBuildLabel);
    vCurrentPos.x += vBuildLabelSize.x + flSpacing;


    for (int i = 0; i < vecComponents.size(); ++i)
    {
        StatComponent& Component = vecComponents[i];

        pDrawList->AddText(Fonts::SegoeUIBold18, flMainFontSize, vCurrentPos, ImColor(255, 255, 255, 255), Component.m_szValueBuffer);
        vCurrentPos.x += Component.m_vValueSize.x + (flSpacing * 0.4f);

        ImVec2 vLabelPos = { vCurrentPos.x, vCurrentPos.y + flVerticalOffset };
        pDrawList->AddText(Fonts::SegoeUIBold, flSmallFontSize, vLabelPos, ImColor(200, 200, 200, 220), Component.szLabel);

        if (i < vecComponents.size() - 1)
            vCurrentPos.x += Component.m_vLabelSize.x + flSpacing;
        else
            vCurrentPos.x += Component.m_vLabelSize.x;
    }

    pDrawList->PopClipRect();

    blurEffect.EndBlur();
}

const char* arrFlagsNames[] =
{
    "Money flag",
    "Armor flag",
    "Kit flag",
    "Defusing flag",
    "Zoom flag"
};

const char* arrScopeNames[] =
{
    "Default",
    "Remove overlay",
    "Static gradient",
    "Remove all",
};

const char* arrRecoilNames[] =
{
    "Default",
    "Remove punch",
    "Remove all",
};

void Gui::RenderSpectatorList()
{
    //if (!Config::b(g_Variables.m_Misc.m_bSpectatorList))
    //    return;

    if (!Interfaces::m_pEngine->IsInGame() || !Interfaces::m_pEngine->IsConnected() || !Globals::m_pLocalPlayerPawn)
        return;

    ImGui::SetNextWindowSize(ImVec2(160, 0));

    std::vector<const char*> vecNames;
    vecNames.clear();

    for (EntityObject_t& object : g_Entities->m_vecEntities)
    {
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

        if (pPlayerController->m_sSanitizedPlayerName())
        {
            vecNames.push_back(pPlayerController->m_sSanitizedPlayerName());
        }
    }

    bool bShouldRender = Gui::m_bOpen || !vecNames.empty();;

    if (!bShouldRender)
        return;


    if (ImGui::Begin(X("Spectators"), nullptr, ImGuiWindowFlags_NoDecoration))
    {
        ImGui::PushFont(Fonts::Verdana);

        for (auto szName : vecNames)
        {
            ApraGui::Label(szName);
        }

        ImGui::PopFont();
    }
    ImGui::End();
}

const char* InputTypeToString(KeyBind_t kb) {
    static const std::unordered_map<int, const char*> modeToString = {
        {HOLD, "(hold)"},
        {TOGGLE, "(toggle)"},
        {ALWAYS_ON, "(always on)"}
    };
    auto it = modeToString.find(kb.m_iMode);
    return (it != modeToString.end()) ? it->second : "(unknown)";
}

bool IsKeybindActive(bool featureEnabled, KeyBind_t keybind) {
    return featureEnabled && Input::HandleInput(keybind);
}

void RenderKeybinds() {
    //if (!Config::b(g_Variables.m_Misc.m_bKeybindList))
    //    return;

    struct KeybindFeature {
        const char* label;
        bool featureEnabled;
        KeyBind_t keybind;
    };

    const KeybindFeature keybindFeatures[] = {
        {"Damage override ", Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), Config::kb(g_Variables.m_Ragebot.m_iMinDamageOverrideKey)},
        {"Duck Peek Assist ", Config::b(g_Variables.m_Ragebot.m_bDuckPeekAssist), Config::kb(g_Variables.m_Ragebot.m_kbDuckPeekAssist)},
        {"Manual right ", Config::b(g_Variables.m_Ragebot.m_bAntiaim), Config::kb(g_Variables.m_Ragebot.m_iRightAntiaimKeybind)},
        {"Manual left ", Config::b(g_Variables.m_Ragebot.m_bAntiaim), Config::kb(g_Variables.m_Ragebot.m_iLeftAntiaimKeybind)},
        {"No Spread", Config::b(g_Variables.m_Ragebot.m_bNoSpread), Config::kb(g_Variables.m_Ragebot.m_iNoSpreadKey)},
        {"Force headshot ", Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), Config::kb(g_Variables.m_Ragebot.m_iForceHeadshotKey)},
        {"Force bodyaim ", Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), Config::kb(g_Variables.m_Ragebot.m_iForceBodyaimKey)},
        {"Auto retreat ", Config::b(g_Variables.m_Ragebot.m_bAutoPeek), Config::kb(g_Variables.m_Ragebot.m_kbAutoPeek)}
    };

    int maxTextWidth = 0;

    struct Feature_t
    {
        int iTextWidth;
        std::string strKeybind;
    };

    std::vector<Feature_t> activeKeybinds;

    for (const auto& feature : keybindFeatures) {
        if (IsKeybindActive(feature.featureEnabled, feature.keybind)) {
            std::string displayString = std::string(feature.label) + InputTypeToString(feature.keybind);

            int textWidth = ImGui::CalcTextSize(displayString.c_str()).x + 10;

            activeKeybinds.push_back(Feature_t(textWidth, displayString));

            if (textWidth > maxTextWidth) {
                maxTextWidth = textWidth;
            }
        }
    }

    if (activeKeybinds.empty())
        return;


    ImGui::SetNextWindowSize(ImVec2(maxTextWidth, 0));
    ImGui::SetNextWindowBgAlpha(0.2f);
    ImGui::Begin("Keybinds", nullptr, ImGuiWindowFlags_NoDecoration);
    {
        for (const auto& feature : activeKeybinds) {
            blurEffect.ApplyBlur(ImGui::GetWindowDrawList(), ImGui::GetCursorPos(), ImVec2(feature.iTextWidth, ImGui::CalcTextSize(feature.strKeybind.c_str()).y - 5), 5, 5);
            ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCursorPos(), ImVec2(feature.iTextWidth, ImGui::CalcTextSize(feature.strKeybind.c_str()).y - 5), ImColor(55, 55, 55, 120));
            ApraGui::Label(feature.strKeybind.c_str());
        }
    }
    ImGui::End();
}

static int iButtonCount = 7;
static int iButtonWidth = 20;

static int iSelectedTab = 0;
static int iSelectedSubTabRagebot = 0;
static int iSelectedSubTabVisuals = 0;

void RenderTabButtons()
{
    ImGui::PushFont(Fonts::SegoeUIBold18);

    constexpr float flAnimationSpeed = 10.0f;
    const ImVec4 defaultColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    static std::array<float, ETabs::TABS_COUNT> tabAlphas = {};

    struct TabConfig
    {
        const char* label;
        ETabs tab;
    };

    constexpr std::array<TabConfig, ETabs::TABS_COUNT> tabs = {
        {
            {"RAGE", ETabs::TABS_RAGEBOT},
            {"LEGIT", ETabs::TABS_LEGITBOT},
            {"VISUALS", ETabs::TABS_VISUALS},
            {"SKINS", ETabs::TABS_INVENTORY},
            {"CONFIG", ETabs::TABS_CONFIG},
            {"MISC", ETabs::TABS_MISC}
        }
    };

    auto AnimateTab = [](float& currentAlpha, bool isSelected, float speed)
        {
            const float targetAlpha = isSelected ? 1.0f : 0.5f;
            currentAlpha = ImLerp(currentAlpha, targetAlpha, ImGui::GetIO().DeltaTime * speed);
        };

    float menuWidth = ImGui::GetWindowWidth();
    float midpoint = menuWidth / 2.0f;
    float startX = midpoint + 20.0f;

    float maxWidth = 0.0f;
    for (const auto& tab : tabs) {
        float textWidth = ImGui::CalcTextSize(tab.label).x + 10.0f;
        maxWidth = std::max(maxWidth, textWidth);
    }
    static int iButtonWidth = static_cast<int>(maxWidth);

    float totalWidth = (iButtonWidth + 4.0f) * tabs.size();
    if (totalWidth > (menuWidth - startX)) {
        iButtonWidth = static_cast<int>((menuWidth - startX - (tabs.size() - 1) * 4.0f) / tabs.size());
    }

    ImGui::SetCursorPosX(startX);

    for (int i = 0; i < tabs.size(); ++i)
    {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.f);
        const auto& tab = tabs[i];
        float& alpha = tabAlphas[tab.tab];

        AnimateTab(alpha, iSelectedTab == tab.tab, flAnimationSpeed);

        ImVec4 color = defaultColor;
        color.w = alpha;
        ImGui::PushStyleColor(ImGuiCol_Text, color);

        if (ApraGui::TabButton(tab.label, tab.tab, iSelectedTab, 0, ImVec2(iButtonWidth, 0)))
        {
            iSelectedTab = tab.tab;
        }

        ImGui::PopStyleColor();

        if (i < tabs.size() - 1)
        {
            ImGui::SameLine(startX + (i + 1) * (iButtonWidth + 4.0f));
        }
    }

    ImGui::PopFont();
}

void RenderRagebotTabButtons()
{
    static float aimbotAlpha = 0.0f;
    static float antiaimAlpha = 0.0f;

    auto AnimateAlpha = [](float& alpha, bool active, float speed) {
        alpha = ImLerp(alpha, active ? 1.0f : 0.2f, ImGui::GetIO().DeltaTime * speed);
        };

    const float animationSpeed = 10.0f;
    const ImVec4 baseColor(1.0f, 1.0f, 1.0f, 1.0f);


    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    struct Tab {
        const char* label;
        ESubTabs_Ragebot id;
        float& alpha;
    } tabs[] = {
        {"Aimbot", ESubTabs_Ragebot::SUBTAB_AIMBOT, aimbotAlpha},
        {"Antiaim", ESubTabs_Ragebot::SUBTAB_ANTIAIM, antiaimAlpha}
    };

    const ImVec2 buttonSize(85, ImGui::CalcTextSize("Hi").y + 3);

    for (const auto& tab : tabs) {
        AnimateAlpha(tab.alpha, iSelectedSubTabRagebot == tab.id, animationSpeed);
        ImVec4 textColor = baseColor;
        textColor.w = tab.alpha;

        ImColor bgColor(50, 50, 50, 200);
        bgColor.Value.w = ImLerp(0.6f, 1.0f, 1.0f - tab.alpha);


        ImVec2 buttonPos = ImGui::GetCursorScreenPos();
        ImVec2 buttonEnd = buttonPos + buttonSize;

        blurEffect.ApplyBlur(drawList, buttonPos, buttonSize, 5.0f, 4.0f);

        drawList->AddRectFilled(buttonPos, buttonEnd, bgColor, 3.2f);

        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        if (ApraGui::TabButton(tab.label, tab.id, iSelectedSubTabRagebot, -1, buttonSize)) {
            iSelectedSubTabRagebot = tab.id;
        }
        ImGui::PopStyleColor();
    }
}

void RenderVisualsTabButtons()
{
    static float flLabelAlphaLocal = 0.0f;
    static float flLabelAlphaEnemy = 0.0f;
    static float flLabelAlphaWorld = 0.0f;
    static float flLabelAlphaSkyboxes = 0.0f;

    auto AnimateTab = [](float& currentAlpha, bool isSelected, float animationSpeed) {
        float targetAlpha = isSelected ? 1.0f : 0.2f;
        currentAlpha = ImLerp(currentAlpha, targetAlpha, ImGui::GetIO().DeltaTime * animationSpeed);
        };

    const float flAnimationSpeed = 10.0f;
    const ImVec4 baseColor(1.0f, 1.0f, 1.0f, 1.0f);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    struct Tab {
        const char* label;
        ESubTabs_Visuals id;
        float& alpha;
    } tabs[] = {
        {"Local", ESubTabs_Visuals::SUBTAB_LOCAL, flLabelAlphaLocal},
        {"Enemy", ESubTabs_Visuals::SUBTAB_ENEMY, flLabelAlphaEnemy},
        {"World", ESubTabs_Visuals::SUBTAB_WORLD, flLabelAlphaWorld},
        {"Skyboxes", ESubTabs_Visuals::SUBTAB_SKYBOXES, flLabelAlphaSkyboxes}
    };

    const ImVec2 buttonSize(85, ImGui::CalcTextSize("Hi").y + 3);

    for (const auto& tab : tabs) {
        AnimateTab(tab.alpha, iSelectedSubTabVisuals == tab.id, flAnimationSpeed);
        ImVec4 textColor = baseColor;
        textColor.w = tab.alpha;

        ImColor bgColor(50, 50, 50, 200);
        bgColor.Value.w = ImLerp(0.6f, 1.0f, 1.0f - tab.alpha);

        ImVec2 buttonPos = ImGui::GetCursorScreenPos();
        ImVec2 buttonEnd = buttonPos + buttonSize;

        blurEffect.ApplyBlur(drawList, buttonPos, buttonSize, 5.0f, 4.0f);

        drawList->AddRectFilled(buttonPos, buttonEnd, bgColor, 3.2f);

        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        if (ApraGui::TabButton(tab.label, tab.id, iSelectedSubTabVisuals, -1, buttonSize)) {
            iSelectedSubTabVisuals = tab.id;
        }
        ImGui::PopStyleColor();
    }
}

namespace fs = std::filesystem;

void Gui::DrawHitLogs()
{
    if (!Interfaces::m_pGlobalVariables || !Interfaces::m_pEngine->IsInGame() || !Interfaces::m_pEngine->IsConnected() || !Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerController->m_bPawnIsAlive())
    {
        if (!hitLogs.empty())
            hitLogs.clear();
        return;
    }

    ImDrawList* pDrawList = ImGui::GetBackgroundDrawList();
    float currentTime = Interfaces::m_pGlobalVariables->m_flCurrentTime;
    float messageHeight = 20.f;
    float startY = 5.0f;
    float startX = 5.0f;

    for (size_t i = 0; i < hitLogs.size(); ++i)
    {
        const HitLogEntry& entry = hitLogs[i];

        if (currentTime - entry.startTime > (entry.fadeStartTime - entry.startTime + entry.fadeDuration))
        {
            hitLogs.erase(hitLogs.begin() + i);
            continue;
        }

        float alpha = 1.0f;

        if (currentTime >= entry.fadeStartTime) {
            alpha = 1.0f - ((currentTime - entry.fadeStartTime) / entry.fadeDuration);
            alpha = std::max(0.0f, std::min(1.0f, alpha));
        }

        ImColor color(255, 255, 255, static_cast<int>(255 * alpha));
        ImColor color2(0, 0, 0, static_cast<int>(100 * alpha));

        pDrawList->AddText(Fonts::Verdana, 18.f, ImVec2(startX + 1, startY + i * messageHeight + 1), color2, entry.message.c_str());
        pDrawList->AddText(Fonts::Verdana, 18.f, ImVec2(startX, startY + i * messageHeight), color, entry.message.c_str());
    }
}

bool CustomButton_NoOutlineAndBg(const char* szLabel, int iTextAlign, ImVec2 imSize, int iCustomID)
{
    ImGuiWindow* imWindow = ImGui::GetCurrentWindow();
    if (imWindow->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID imID = imWindow->GetID(szLabel) + iCustomID;
    const ImVec2 imLabelSize = ImGui::CalcTextSize(szLabel, NULL, true);

    ImVec2 imPos = imWindow->DC.CursorPos;

    ImVec2 size = ImGui::CalcItemSize(imSize, imLabelSize.x + style.FramePadding.x * 2.0f, imLabelSize.y + style.FramePadding.y * 2.0f);

    const ImRect imTotalBB(imPos, imPos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(imTotalBB, imID))
        return false;

    bool bHovered, bHeld;
    bool bPressed = ImGui::ButtonBehavior(imTotalBB, imID, &bHovered, &bHeld, 0);

    float* flAnimatedAlpha = ImGui::GetStateStorage()->GetFloatRef(imID, 0.0f);
    float flTargetAlpha = bHovered ? 0.8f : 0.5f;
    *flAnimatedAlpha = ImLerp(*flAnimatedAlpha, flTargetAlpha, ImGui::GetIO().DeltaTime * 5);

    ImColor imButtonColor = ImColor(255, 255, 255, 100);
    if (bHovered && !bPressed)
    {
        imButtonColor = ImColor(255, 255, 255, 180);
    }
    if (bHovered && bPressed)
    {
        imButtonColor = ImColor(255, 255, 255, 255);
    }

    ImVec4 imLabelColor = imButtonColor.Value;
    imLabelColor.w = ImClamp(*flAnimatedAlpha, 0.0f, 1.0f);

    float alignFactor = (iTextAlign == -1) ? 0.0f : (iTextAlign == 1) ? 1.0f : 0.5f;
    ImVec2 imLabelTextPos = ImVec2(
        ImLerp(imTotalBB.Min.x + style.FramePadding.x, imTotalBB.Max.x - style.FramePadding.x - imLabelSize.x, alignFactor),
        ImLerp(imTotalBB.Min.y + style.FramePadding.y, imTotalBB.Max.y - style.FramePadding.y - imLabelSize.y, 0.2f)
    );

    imWindow->DrawList->AddText(imLabelTextPos, ImColor(imLabelColor), szLabel);

    return bPressed;
}

void Gui::Render()
{
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* pForeGroundDrawList = ImGui::GetForegroundDrawList();

    Gui::Update(io);

    if (!m_bInitialized)
        return;

    blurEffect.BeginBlur();

    //RenderKeybinds();
    DrawHitLogs();

    //Gui::RenderSpectatorList();
    //Gui::RenderWatermark();

    if (!Gui::m_bOpen)
        return;

    ImGui::SetNextWindowSize(MENU_SIZE);
    ApraGui::Begin("Menu");
    {
        blurEffect.ApplyBlur(ImGui::GetCurrentWindow()->DrawList, ImGui::GetWindowPos(), MENU_SIZE, 5, 5);

        ImGui::GetCurrentWindow()->DrawList->AddRectFilled(ImGui::GetWindowPos(), ImGui::GetWindowPos() + MENU_SIZE, ImColor(25, 25, 25, 220), ImGui::GetStyle().WindowRounding);

        ImRect imBarBackground = ImRect(
            ImGui::GetWindowPos(),
            ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + 32.0f)
        );

        ImVec2 blurPos = imBarBackground.Min;
        ImVec2 blurSize = ImVec2(imBarBackground.Max.x - imBarBackground.Min.x, imBarBackground.Max.y - imBarBackground.Min.y);

        ImGui::GetCurrentWindow()->DrawList->AddRectFilled(imBarBackground.Min, imBarBackground.Max, ImColor(45, 45, 45, 120), ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersTop);

        float flOriginalCursorPosY = ImGui::GetCursorPosY();
        ImGui::SetCursorPosY(flOriginalCursorPosY - 3.f);
        ImGui::PushFont(Fonts::SegoeUIBold18);
        ApraGui::Label("SENSICAL");
        ImGui::PopFont();
        ApraGui::SameLine(0.f, 0.f);
        ImGui::PushFont(Fonts::SegoeUIBold18);
        ApraGui::Label(" ALPHA");
        ImGui::PopFont();
        ImGui::SetCursorPosY(flOriginalCursorPosY);

        ApraGui::SameLine();

        static int iButtonSpacing = 4;

        int iRawWidth = iButtonCount * iButtonWidth;
        int iRawSpacing = iButtonCount * iButtonSpacing;

        int iTotalWidth = iRawWidth + iRawSpacing;
        int iButtonPositionX = ImGui::GetWindowWidth() - iTotalWidth;

        ImGui::SetCursorPosX(iButtonPositionX);

        RenderTabButtons();

        ImGui::Dummy(ImVec2(ImGui::GetCursorPosX(), 5));

        ImVec2 imAllChildSize = ImVec2((ImGui::GetContentRegionAvail().x - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().FramePadding.x - 2) / 3, ImGui::GetContentRegionAvail().y/* - 15for cs text*/);

        switch (iSelectedTab)
        {
        case ETabs::TABS_RAGEBOT:
        {
            ImVec2 imRagebotChildSize = ImVec2((ImGui::GetContentRegionAvail().x - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().FramePadding.x - 2) / 3.5, ImGui::GetContentRegionAvail().y/* - 15for cs text*/);

            ImGui::BeginChild(ImGui::GetCurrentWindow()->GetID("RagebotChild"), ImVec2(88.8, ImGui::GetContentRegionAvail().y));
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
                RenderRagebotTabButtons();
            }
            ImGui::EndChild();

            switch (iSelectedSubTabRagebot)
            {
            case ESubTabs_Ragebot::SUBTAB_AIMBOT:
            {
                ApraGui::SameLine();
                ApraGui::Child("General", &imRagebotChildSize);
                {
                    ApraGui::CheckBox("Enable", &Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), 0);

                    ApraGui::CheckBox("Penetrate walls", &Config::b(g_Variables.m_Ragebot.m_bAutoWall), 0);

                    ApraGui::CheckBox("Automatic fire", &Config::b(g_Variables.m_Ragebot.m_bAutoShoot), 0);

                    ApraGui::CheckBox("Automatic scope", &Config::b(g_Variables.m_Ragebot.m_bAutoscope), 0);

                    ApraGui::CheckBox("Silent aimbot", &Config::b(g_Variables.m_Ragebot.m_bSilentAim), 0);

                    //ApraGui::CheckBox("Delay shot", &Config::b(g_Variables.m_Ragebot.m_bDelayShot), 0);

                    ApraGui::Label("Force headshot");
                    ApraGui::SameLine();
                    ApraGui::Keybind("Force headshot key", &Config::kb(g_Variables.m_Ragebot.m_iForceHeadshotKey));

                    ApraGui::Label("Force bodyaim");
                    ApraGui::SameLine();
                    ApraGui::Keybind("Force bodyaim key", &Config::kb(g_Variables.m_Ragebot.m_iForceBodyaimKey));

                    ApraGui::ChildEnd();

                    ApraGui::SameLine();
                }
                ApraGui::Child("Accuracy", &imRagebotChildSize);
                {

                    std::vector<bool> hitboxSelections(HB_MAX, false);
                    std::vector<bool> multipointSelections(HB_MAX, false);

                    /*                   std::vector<MenuRageHitbox_t>& currentHitboxes = GetHitboxesForCurrentWeapon();

                                       for (const auto& hitbox : currentHitboxes) {
                                           hitboxSelections[hitbox.m_eDefinition] = true;
                                           multipointSelections[hitbox.m_eDefinition] = hitbox.m_bMultipoint;
                                       }

                                       if (ApraGui::MultiComboBox("Hitboxes", Tabs::HitboxNames, hitboxSelections, IM_ARRAYSIZE(Tabs::HitboxNames))) {
                                           currentHitboxes.erase(
                                               std::remove_if(currentHitboxes.begin(), currentHitboxes.end(),
                                                   [&](const MenuRageHitbox_t& hitbox) {
                                                       return !hitboxSelections[hitbox.m_eDefinition];
                                                   }),
                                               currentHitboxes.end()
                                           );

                                           for (int i = 0; i < HB_MAX; i++) {
                                               if (hitboxSelections[i] &&
                                                   std::none_of(currentHitboxes.begin(), currentHitboxes.end(),
                                                       [&](const MenuRageHitbox_t& h) { return h.m_eDefinition == i; })) {
                                                   currentHitboxes.push_back(MenuRageHitbox_t{ false, static_cast<EConfigHitBoxes>(i) });
                                               }
                                           }
                                       }

                                       std::vector<const char*> selectedHitboxNames;
                                       std::vector<bool> selectedMultipointValues;
                                       std::vector<int> hitboxToIndexMap;

                                       for (int i = 0; i < HB_MAX; i++) {
                                           if (hitboxSelections[i]) {
                                               selectedHitboxNames.push_back(Tabs::HitboxNames[i]);
                                               selectedMultipointValues.push_back(multipointSelections[i]);
                                               hitboxToIndexMap.push_back(i);
                                           }
                                       }

                                       if (ApraGui::MultiComboBox("Multipoints", selectedHitboxNames.data(), selectedMultipointValues, selectedHitboxNames.size())) {
                                           for (size_t i = 0; i < selectedMultipointValues.size(); i++) {
                                               int originalIndex = hitboxToIndexMap[i];

                                               for (auto& hitbox : currentHitboxes) {
                                                   if (hitbox.m_eDefinition == originalIndex) {
                                                       hitbox.m_bMultipoint = selectedMultipointValues[i];
                                                       break;
                                                   }
                                               }
                                           }
                                       }*/

                    ApraGui::Label("Override damage key");
                    ApraGui::Keybind("Override damage key", &Config::kb(g_Variables.m_Ragebot.m_iMinDamageOverrideKey));

                    ApraGui::CheckBox("Auto Stop", &Config::b(g_Variables.m_Ragebot.m_bAutostop), 0);
                    ApraGui::SameLine();
                    ApraGui::MultiComboBox("Autostop Mode", Tabs::szAutostopModes, Config::vb(g_Variables.m_Ragebot.m_vbAutostop), IM_ARRAYSIZE(Tabs::szAutostopModes), false);

                }
                ApraGui::ChildEnd();

                ApraGui::SameLine();

                ApraGui::Child("Extra", &imRagebotChildSize);
                {
                    ApraGui::MultiComboBox("History Aim", Tabs::szHistoryAimModes, Config::vb(g_Variables.m_Ragebot.m_vbRageHistoryAim), IM_ARRAYSIZE(Tabs::szHistoryAimModes), false);

                    ApraGui::CheckBox("Auto retreat", &Config::b(g_Variables.m_Ragebot.m_bAutoPeek), 0);
                    ApraGui::CheckBox("Force retreat on release", &Config::b(g_Variables.m_Ragebot.m_bRetreatOnRelease), 0);

                    ApraGui::Label("Auto retreat color");
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Auto retreat clr", &Config::c(g_Variables.m_Ragebot.m_AutoPeekColor));

                    ApraGui::Label("Auto retreat key");
                    ApraGui::SameLine();
                    ApraGui::Keybind("Auto retreat Key", &Config::kb(g_Variables.m_Ragebot.m_kbAutoPeek));

                    ApraGui::CheckBox("Performance mode", &Config::b(g_Variables.m_Misc.m_bPerformanceMode));
                }
                ApraGui::ChildEnd();
                break;
            }

            case ESubTabs_Ragebot::SUBTAB_ANTIAIM:
            {
                ApraGui::SameLine();

                ApraGui::Child("Antiaim", &imRagebotChildSize);
                {
                    ApraGui::CheckBox("Enable", &Config::b(g_Variables.m_Ragebot.m_bAntiaim), 0);
                    ApraGui::CheckBox("At targets", &Config::b(g_Variables.m_Ragebot.m_bAtTarget), 0);

                    ApraGui::SliderInt("Pitch value", &Config::i(g_Variables.m_Ragebot.m_iPitch), -89, 89);
                    ApraGui::SliderInt("Yaw value", &Config::i(g_Variables.m_Ragebot.m_iYaw), -179, 179);

                    ApraGui::Label("Freestanding");
                    ApraGui::SameLine();
                    ApraGui::Keybind(X("Freestanding"), &Config::kb(g_Variables.m_Ragebot.m_bFreestanding));

                    if (ApraGui::CheckBox(X("Yaw jitter"), &Config::b(g_Variables.m_Ragebot.m_bYawJitter), 1))
                    {
                        ApraGui::SliderInt("Yaw jitter", &Config::i(g_Variables.m_Ragebot.m_iYawJitter), -179, 179);
                    }

                    if (ApraGui::CheckBox(X("Pitch jitter"), &Config::b(g_Variables.m_Ragebot.m_bPitchJitter), 1))
                    {
                        ApraGui::SliderInt("Pitch jitter", &Config::i(g_Variables.m_Ragebot.m_iPitchJitter), -179, 179);
                    }

                    ApraGui::Label("Manual left");
                    ApraGui::SameLine();
                    ApraGui::Keybind("Left Key", &Config::kb(g_Variables.m_Ragebot.m_iLeftAntiaimKeybind));

                    ApraGui::Label("Manual right");
                    ApraGui::SameLine();
                    ApraGui::Keybind("Right Key", &Config::kb(g_Variables.m_Ragebot.m_iRightAntiaimKeybind));

                    if (ApraGui::CheckBox("Spinbot", &Config::b(g_Variables.m_Ragebot.m_bSpinAA), 0))
                    {
                        ApraGui::SliderInt("Spinbot angle", &Config::i(g_Variables.m_Ragebot.m_iSpinAASpeed), 0, 180);
                    }

                    ApraGui::Label("Accurate walk");
                    ApraGui::SameLine();
                    ApraGui::Keybind("Accurate walk Key", &Config::kb(g_Variables.m_Ragebot.m_bAccurateWalk));
                }
                ApraGui::ChildEnd();

                break;
            }
            }

            break;
        }
        case ETabs::TABS_LEGITBOT:
        {
            ImVec2 imLegitbotChildSize = ImVec2((ImGui::GetContentRegionAvail().x - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().FramePadding.x - 2) / 3.5, ImGui::GetContentRegionAvail().y);

            ApraGui::Child("General", &imLegitbotChildSize);
            {
                ApraGui::CheckBox(X("Enable"), &Config::b(g_Variables.m_Legitbot.m_bEnabled));

                ApraGui::MultiComboBox("Conditions", Tabs::LegitbotConditions, Config::vb(g_Variables.m_Legitbot.m_vbConditions), IM_ARRAYSIZE(Tabs::LegitbotConditions));
                ApraGui::MultiComboBox("Hitboxes", Tabs::HitboxNames, Config::vb(g_Variables.m_Legitbot.m_vbHitboxes), IM_ARRAYSIZE(Tabs::HitboxNames));

                ApraGui::SliderInt(X("FOV"), &Config::i(g_Variables.m_Legitbot.m_iFov), 1, 100);
                ApraGui::SliderInt(X("Smoothing"), &Config::i(g_Variables.m_Legitbot.m_iSmoothness), 1, 100);
            }
            ApraGui::ChildEnd();

            break;
        }
        case ETabs::TABS_VISUALS:
        {
            ImVec2 imVisualsChildSize = ImVec2((ImGui::GetContentRegionAvail().x - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().FramePadding.x - 2) / 3.5, ImGui::GetContentRegionAvail().y);

            ImGui::BeginChild(ImGui::GetCurrentWindow()->GetID("RagebotChild"), ImVec2(88.8, ImGui::GetContentRegionAvail().y));
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
                RenderVisualsTabButtons();
            }
            ImGui::EndChild();

            switch (iSelectedSubTabVisuals)
            {
            case ESubTabs_Visuals::SUBTAB_LOCAL:
            {
                ApraGui::SameLine();
                ApraGui::Child("Options", &imVisualsChildSize);
                {
                    ApraGui::CheckBox("Enable thirdperson", &Config::b(g_Variables.m_Visuals.m_bEnableThirdPerson), 1);
                    ApraGui::Keybind("Thirdperson key", &Config::kb(g_Variables.m_Visuals.m_iThirdPersonKeybind));
                    if (Config::b(g_Variables.m_Visuals.m_bEnableThirdPerson))
                        ApraGui::SliderInt("Thirdperson distance", &Config::i(g_Variables.m_Visuals.m_iThirdPersonDistance), 25, 200, 1);

                    ApraGui::CheckBox("Enable transparency in scope", &Config::b(g_Variables.m_Visuals.m_bTransparencyInScope));

                    if (ApraGui::CheckBox("Viewmodel changer", &Config::b(g_Variables.m_Visuals.m_bViewmodelChanger)))
                    {
                        ApraGui::SliderInt("Viewmodel FOV", &Config::i(g_Variables.m_Visuals.iViewmodelFov), 40, 120);
                    }
                }
                ApraGui::ChildEnd();

                ApraGui::SameLine();

                ApraGui::Child("Local chams", &imVisualsChildSize);
                {
                    ApraGui::CheckboxComboColor("Model",
                        &Config::b(g_Variables.m_Visuals.m_bEnableSelfChams),
                        &Config::i(g_Variables.m_Visuals.m_iChamMaterialLocal),
                        arrChamMaterials,
                        IM_ARRAYSIZE(arrChamMaterials),
                        &Config::c(g_Variables.m_Visuals.m_colSelfChams)
                    );

                    ApraGui::CheckboxComboColor("Arms",
                        &Config::b(g_Variables.m_Visuals.m_bEnableSelfArmsChams),
                        &Config::i(g_Variables.m_Visuals.m_iChamMaterialLocalArms),
                        arrChamMaterials,
                        IM_ARRAYSIZE(arrChamMaterials),
                        &Config::c(g_Variables.m_Visuals.m_colSelfArmsChams)
                    );

                    ApraGui::CheckboxComboColor("Weapon",
                        &Config::b(g_Variables.m_Visuals.m_bEnableSelfWeaponChams),
                        &Config::i(g_Variables.m_Visuals.m_iChamMaterialLocalWeapon),
                        arrChamMaterials,
                        IM_ARRAYSIZE(arrChamMaterials),
                        &Config::c(g_Variables.m_Visuals.m_colSelfWeaponChams)
                    );

                    ApraGui::CheckboxComboColor("Attachments",
                        &Config::b(g_Variables.m_Visuals.m_bEnableSelfAttachmentChams),
                        &Config::i(g_Variables.m_Visuals.m_iChamMaterialLocalAttachments),
                        arrChamMaterials,
                        IM_ARRAYSIZE(arrChamMaterials),
                        &Config::c(g_Variables.m_Visuals.m_colAttachmentChams)
                    );
                }
                ApraGui::ChildEnd();

                ApraGui::SameLine();

                ApraGui::Child("Other", &imVisualsChildSize);
                {
                    ApraGui::Combo("Remove scope", &Config::i(g_Variables.m_Visuals.m_iRemoveScope), arrScopeNames, IM_ARRAYSIZE(arrScopeNames));
                    if (Config::i(g_Variables.m_Visuals.m_iRemoveScope) == SCOPE_GRADIENT) {
                        ApraGui::Label("Color");
                        ApraGui::SameLine();
                        ApraGui::SimpleColorPicker("Color-in", &Config::c(g_Variables.m_Visuals.m_colScope), 2);
                        ApraGui::SameLine();
                        ApraGui::SimpleColorPicker("Color-out", &Config::c(g_Variables.m_Visuals.m_colScopeOutSide));

                        ApraGui::SliderInt("Scope length", &Config::i(g_Variables.m_Visuals.m_iScopeLength), 1, 500);
                        ApraGui::SliderInt("Scope thickness", &Config::i(g_Variables.m_Visuals.m_iScopeThickness), 1, 50);
                        ApraGui::SliderInt("Scope gap", &Config::i(g_Variables.m_Visuals.m_iScopeGap), 0, 100);
                    }

                    int& recoilVar = Config::i(g_Variables.m_Visuals.m_iRemoveRecoil);
                    if (recoilVar < 0 || recoilVar >= IM_ARRAYSIZE(arrRecoilNames))
                        recoilVar = RECOIL_NONE;

                    ApraGui::Combo("Remove recoil", &recoilVar, arrRecoilNames, IM_ARRAYSIZE(arrRecoilNames));
                }
                ApraGui::ChildEnd();

                break;
            }

            case ESubTabs_Visuals::SUBTAB_ENEMY:
            {
                ApraGui::SameLine();

                ApraGui::Child("World ESP", &imVisualsChildSize);
                {
                    ApraGui::CheckBox("Enable", &Config::b(g_Variables.m_Visuals.m_bEnableESP), 0);

                    ApraGui::CheckBox("Bounding box", &Config::b(g_Variables.m_Visuals.m_bEnableBoxESP), 0);
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Bounding box color", &Config::c(g_Variables.m_Visuals.m_colBoxESP));

                    ApraGui::CheckBox("Name", &Config::b(g_Variables.m_Visuals.m_bEnableNameESP), 0);
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Name color", &Config::c(g_Variables.m_Visuals.m_colNameEsp));

                    ApraGui::CheckBox("Health bar", &Config::b(g_Variables.m_Visuals.m_bEnableHealthESP), 0);
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Health bar color", &Config::c(g_Variables.m_Visuals.m_colHealthBarEsp), 2);
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Health bar gradient color", &Config::c(g_Variables.m_Visuals.m_colHealthBarGradient));

                    ApraGui::CheckBox("Weapon", &Config::b(g_Variables.m_Visuals.m_bEnableWeaponESP), 0);
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Weapon color", &Config::c(g_Variables.m_Visuals.m_colWeaponEsp));

                    ApraGui::CheckBox("Weapon icon", &Config::b(g_Variables.m_Visuals.m_bEnableWeaponIconESP), 0);
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Weapon icon color", &Config::c(g_Variables.m_Visuals.m_colWeaponIcon));

                    ApraGui::CheckBox("Armor bar", &Config::b(g_Variables.m_Visuals.m_bEnableArmorESP), 0);
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Armor bar Color", &Config::c(g_Variables.m_Visuals.m_colArmorBar));

                    ApraGui::CheckBox("Skeleton", &Config::b(g_Variables.m_Visuals.m_bEnableSkeletonESP), 0);
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Skeleton color", &Config::c(g_Variables.m_Visuals.m_colSkeletonEsp));

                    //ApraGui::CheckBox("On Shot Skeleton", &Config::b(g_Variables.m_Visuals.m_bEnableOnShotSkeletonESP), 0);
                    //ApraGui::SameLine();
                    //ApraGui::SimpleColorPicker("On Shot Skeleton Color", &Config::c(g_Variables.m_Visuals.m_colOnShotSkeletonBones));

                    ApraGui::CheckBox("History skeleton", &Config::b(g_Variables.m_Visuals.m_bEnableBacktrackSkeletonESP), 0);
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("History skeleton color", &Config::c(g_Variables.m_Visuals.m_colBacktrackSkeletonEsp));

                    ApraGui::CheckBox("Hit shock effect", &Config::b(g_Variables.m_Visuals.m_bShockEffect), 0);
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("##hitshockeffectclr", &Config::c(g_Variables.m_Visuals.m_colShockEffect));

                    ApraGui::CheckBox("OOF Arrows", &Config::b(g_Variables.m_Visuals.m_bEnableOOFArrows));
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("OOF Arrows color", &Config::c(g_Variables.m_Visuals.m_colOOFArrows));

                    //if (Config::b(g_Variables.m_Visuals.m_bEnableOOFArrows))
                    //{
                    //    bool bValuesTheSame = Config::i(g_Variables.m_Visuals.m_bOOFArrowsWidth) == Config::i(g_Variables.m_Visuals.m_bOOFArrowsHeight);
                    //    std::string strCircleOvalText = bValuesTheSame ? "Circle" : "Oval";
                    //    ApraGui::SliderInt(std::string("OOF " + strCircleOvalText + " Width").c_str(), &Config::i(g_Variables.m_Visuals.m_bOOFArrowsWidth), 0, 200, ImGui::GetCurrentWindow()->GetID("OOF Height"), true);
                    //    ApraGui::SliderInt(std::string("OOF " + strCircleOvalText + " Height").c_str(), &Config::i(g_Variables.m_Visuals.m_bOOFArrowsHeight), 0, 200, ImGui::GetCurrentWindow()->GetID("OOF Width"), true);
                    //}

                    ApraGui::MultiComboBox("Flags", arrFlagsNames, Config::vb(g_Variables.m_Visuals.m_vecFlags), IM_ARRAYSIZE(arrFlagsNames));
                }
                ApraGui::ChildEnd();

                ApraGui::SameLine();

                ApraGui::Child("Enemy", &imVisualsChildSize);
                {
                    ApraGui::CheckboxComboColor("Visible",
                        &Config::b(g_Variables.m_Visuals.m_bEnableEnemyChams),
                        &Config::i(g_Variables.m_Visuals.m_iChamMaterialEnemy),
                        arrChamMaterials,
                        IM_ARRAYSIZE(arrChamMaterials),
                        &Config::c(g_Variables.m_Visuals.m_colEnemyChams)
                    );

                    ApraGui::CheckboxComboColor("Occluded",
                        &Config::b(g_Variables.m_Visuals.m_bEnableEnemyChamsIgnoreZ),
                        &Config::i(g_Variables.m_Visuals.m_iChamMaterialEnemyIgnoreZ),
                        arrChamMaterials,
                        IM_ARRAYSIZE(arrChamMaterials),
                        &Config::c(g_Variables.m_Visuals.m_colEnemyChamsIgnoreZ)
                    );

                    ApraGui::CheckboxComboColor("History visible",
                        &Config::b(g_Variables.m_Visuals.m_bBacktrackChams),
                        &Config::i(g_Variables.m_Visuals.m_iBacktrackChamMaterialEnemy),
                        arrChamMaterials,
                        IM_ARRAYSIZE(arrChamMaterials),
                        &Config::c(g_Variables.m_Visuals.m_colBacktrackChams)
                    );

                    ApraGui::CheckboxComboColor("History occluded",
                        &Config::b(g_Variables.m_Visuals.m_bBacktrackChamsIgnoreZ),
                        &Config::i(g_Variables.m_Visuals.m_iBacktrackChamMaterialEnemy),
                        arrChamMaterials,
                        IM_ARRAYSIZE(arrChamMaterials),
                        &Config::c(g_Variables.m_Visuals.m_colBacktrackChamsIgnoreZ)
                    );

                    ApraGui::CheckBox("Ragdoll", &Config::b(g_Variables.m_Visuals.m_bRagdollChamsEnemy), 0);

                    ApraGui::CheckBox("Glow", &Config::b(g_Variables.m_Visuals.m_bGlow), 0);
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Glow color", &Config::c(g_Variables.m_Visuals.m_colGlow));
                }
                ApraGui::ChildEnd();

                break;
            }

            case ESubTabs_Visuals::SUBTAB_WORLD:
            {
                ApraGui::SameLine();

                ApraGui::Child("Overlay", &imVisualsChildSize);
                {
                    ApraGui::CheckBox("Dropped weapons", &Config::b(g_Variables.m_WorldVisuals.m_bWeaponNames), 0);
                    ApraGui::CheckBox("Grenade names", &Config::b(g_Variables.m_WorldVisuals.m_bDrawGrenadeNames));

                    ApraGui::CheckBox("Grenade prediction", &Config::b(g_Variables.m_WorldVisuals.m_bGrenadePrediction));
                    ImGui::SameLine();
                    ApraGui::SimpleColorPicker("##GrenadePredColor", &Config::c(g_Variables.m_WorldVisuals.m_cGrenadePrediction));
                    ApraGui::CheckBox("Grenade prediction Effect", &Config::b(g_Variables.m_WorldVisuals.m_bGrenadePredictionEffect));

                    ApraGui::CheckBox("Molotov area", &Config::b(g_Variables.m_WorldVisuals.m_bDrawInferno));
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Molotov area color", &Config::c(g_Variables.m_WorldVisuals.m_colInferno));
                }
                ApraGui::ChildEnd();

                ApraGui::SameLine();

                ApraGui::Child("World", &imVisualsChildSize);
                {
                    if (ApraGui::CheckBox("Enable", &Config::b(g_Variables.m_WorldVisuals.m_bWorldModulation)))
                    {
                        ApraGui::Label("Wall");
                        ApraGui::SameLine();
                        ApraGui::SimpleColorPicker("Wall color", &Config::c(g_Variables.m_WorldVisuals.m_colWorldColor));

                        ApraGui::Label("Lighting");
                        ApraGui::SameLine();
                        ApraGui::SimpleColorPicker("Lighting color", &Config::c(g_Variables.m_WorldVisuals.m_colLighting));

                        ApraGui::Label("Sun");
                        ApraGui::SameLine();
                        ApraGui::SimpleColorPicker("Sun color", &Config::c(g_Variables.m_WorldVisuals.m_colSunColor));
                    }

                    ApraGui::CheckBox("Molotov", &Config::b(g_Variables.m_WorldVisuals.m_bParticleModulation));
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Molotov color", &Config::c(g_Variables.m_WorldVisuals.m_colParticleColor));

                    ApraGui::CheckBox("Grenade", &Config::b(g_Variables.m_WorldVisuals.m_bGrenadeModulation));
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Grenade color", &Config::c(g_Variables.m_WorldVisuals.m_colGrenade));

                    ApraGui::CheckBox("Taser stim", &Config::b(g_Variables.m_WorldVisuals.m_bTaserStimModulation));
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Taser stim Color", &Config::c(g_Variables.m_WorldVisuals.m_colTaserStim));

                    ApraGui::CheckBox("Taser shock", &Config::b(g_Variables.m_Visuals.m_bShockEffect));
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Taser shock color", &Config::c(g_Variables.m_Visuals.m_colShockEffect));

                    ApraGui::NoneSlider("Field of view", &Config::i(g_Variables.m_Visuals.m_iWorldFov), 50, 140);

                    ApraGui::NoneSlider(X("Aspect ratio value"), &Config::i(g_Variables.m_WorldEffects.m_iAspectRatio), 0, 50);

                    ApraGui::Combo(X("Weather"), &Config::i(g_Variables.m_WorldEffects.m_iSelectedCustomWorldParticles), arrCustomWeatherEffects, IM_ARRAYSIZE(arrCustomWeatherEffects));
                    ApraGui::SliderInt("Weather density", &Config::i(g_Variables.m_WorldEffects.m_iSelectedCustomWorldParticleDensity), 5, 200);

                    ApraGui::SliderInt(X("Exposure"), &Config::i(g_Variables.m_WorldVisuals.m_nWorldExposure), 0, 150);

                }
                ApraGui::ChildEnd();

                ApraGui::SameLine();

                ApraGui::Child("Other", &imVisualsChildSize);
                {
                    ApraGui::CheckBox("Local tracers", &Config::b(g_Variables.m_WorldEffects.m_bLocalBulletTracers));
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("localclr", &Config::c(g_Variables.m_WorldEffects.m_colLocalBulletTracers));

                    ApraGui::CheckBox("Friendly tracers", &Config::b(g_Variables.m_WorldEffects.m_bTeamBulletTracers));
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("teamclr", &Config::c(g_Variables.m_WorldEffects.m_colTeamBulletTracers));

                    ApraGui::CheckBox("Enemy tracers", &Config::b(g_Variables.m_WorldEffects.m_bEnemyBulletTracers));
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("enemyclr", &Config::c(g_Variables.m_WorldEffects.m_colEnemyBulletTracers));

                    ApraGui::CheckBox("World hitmarker", &Config::b(g_Variables.m_Visuals.m_bHitmarker), 0);
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("World Hitmarker Color", &Config::c(g_Variables.m_Visuals.m_colHitmarker));

                    ApraGui::CheckBox("Local impacts", &Config::b(g_Variables.m_WorldEffects.m_bDrawImpacts));
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Local Impact Box Color", &Config::c(g_Variables.m_WorldEffects.m_colDrawImpactsColor));

                    ApraGui::CheckBox("Hit marker", &Config::b(g_Variables.m_Misc.m_bHitMarker));

                    ApraGui::SliderInt("Duration", &Config::i(g_Variables.m_Visuals.m_nHitEffectsDurration), 1, 12, 1);

                    ApraGui::MultiComboBox("Removals", Tabs::szRemovals, Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals), IM_ARRAYSIZE(Tabs::szRemovals));
                    if (Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals).at(REMOVE_ZOOM))
                        ApraGui::CheckBox("Force second zoom", &Config::b(g_Variables.m_Visuals.m_bForceZoom));

                    ApraGui::ChildEnd();
                }
                break;
            }
            case ESubTabs_Visuals::SUBTAB_SKYBOXES:
            {
                ApraGui::SameLine();

                ApraGui::Child("Materials", &imVisualsChildSize);
                {
                    ApraGui::CheckBox("Skybox Textures", &Config::b(g_Variables.m_WorldEffects.m_bCustomSkybox));
                    ApraGui::SameLine();
                    ApraGui::SimpleColorPicker("Skybox color", &Config::c(g_Variables.m_WorldEffects.m_colSkybox));

                    for (size_t i = 0U; i < g_WorldModulation->vecSkyboxes.size(); i++)
                    {
                        Skybox_t pSkybox = g_WorldModulation->vecSkyboxes[i];

                        if (ApraGui::Selectable(pSkybox.strSkyboxName.c_str(), (g_WorldModulation->nSelectedSkybox == i), ImVec2(0, 0)))
                            g_WorldModulation->nSelectedSkybox = i;
                    }

                    if (ApraGui::Button(X("Refresh list"), ImVec2(-1, 30)))
                    {
                        g_WorldModulation->UpdateSkyboxes();
                        g_WorldModulation->LoadDefaultSkyboxes();
                    }
                }
                ApraGui::ChildEnd();

                break;
            }
            }
            break;
        }
        case ETabs::TABS_INVENTORY:
        {
            ApraGui::Child("Select type", &imAllChildSize);
            {
                ApraGui::Label("Search");
                ApraGui::SameLine();

                static ImGuiTextFilter itemFilter = { };
                itemFilter.Draw(X("##Item-Search"), ImGui::GetContentRegionAvail().x);
                if (ApraGui::ListBox(X("##Items-List"), ImGui::GetContentRegionAvail(), InventoryChanger::m_vecDumpedItems.size()))
                {
                    for (DumpedItem_t& item : InventoryChanger::m_vecDumpedItems)
                    {
                        if (!itemFilter.PassFilter(item.m_strName.c_str()))
                            continue;

                        if (ApraGui::Selectable(item.m_strName.c_str(), InventoryChanger::m_pSelectedItem == &item, ImVec2(0, 0), GetRarityColor(item.m_nRarity, 1)))
                        {
                            InventoryChanger::m_pSelectedItem = &item;
                            InventoryChanger::m_pSelectedAgent = nullptr;
                        }
                    }

                    for (DumpedAgent_t& agent : InventoryChanger::m_vecDumpedAgents)
                    {
                        if (!itemFilter.PassFilter(agent.m_strName.c_str()))
                            continue;

                        if (ApraGui::Selectable(agent.m_strName.c_str(), InventoryChanger::m_pSelectedAgent == &agent, ImVec2(0, 0), GetRarityColor(agent.m_nRarity, 1)))
                        {
                            InventoryChanger::m_pSelectedItem = nullptr;
                            InventoryChanger::m_pSelectedAgent = &agent;
                        }
                    }

                    ApraGui::ListBoxEnd();
                }

                ApraGui::ChildEnd();
            }

            ApraGui::SameLine();

            ApraGui::Child("Select Item", &imAllChildSize);
            {
                ApraGui::Label("Search");
                ApraGui::SameLine();
                static ImGuiTextFilter skinFilter = { };
                skinFilter.Draw(X("##Skin-search"), ImGui::GetContentRegionAvail().x);

                ApraGui::ListBox(X("##SkinSelection"), ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 27), InventoryChanger::m_pSelectedItem ? InventoryChanger::m_pSelectedItem->m_vecDumpedSkins.size() : 0);
                {
                    if (InventoryChanger::m_pSelectedItem)
                    {
                        if (InventoryChanger::m_pSelectedItem->m_vecDumpedSkins.empty())
                        {
                            ImGui::PushFont(Fonts::Default);

                            ImVec2 vecTextSize = ImGui::CalcTextSize(X("Failed to dump skins"));
                            ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x * 0.5f) - (vecTextSize.x * 0.5f), (ImGui::GetWindowSize().y * 0.5f) - (vecTextSize.y * 0.5f)));
                            ApraGui::Label(X("Failed to dump skins"), ImColor(255, 0, 0));

                            ImGui::PopFont();
                        }
                        else
                        {
                            for (size_t uSize = 0U; uSize < InventoryChanger::m_pSelectedItem->m_vecDumpedSkins.size(); uSize++)
                            {
                                DumpedSkin_t& skin = InventoryChanger::m_pSelectedItem->m_vecDumpedSkins[uSize];
                                if (!&skin)
                                    continue;

                                std::string strLabel = X("");
                                if (skin.m_strName._Starts_with("Doppler"))
                                {
                                    switch (skin.m_nID)
                                    {
                                    case 415:
                                        strLabel = "Doppler (Ruby)##" + std::to_string(skin.m_nID);
                                        break;
                                    case 416:
                                        strLabel = "Doppler (Sapphire)##" + std::to_string(skin.m_nID);
                                        break;
                                    case 417:
                                        strLabel = "Doppler (Black pearl)##" + std::to_string(skin.m_nID);
                                        break;
                                    case 418:
                                        strLabel = "Doppler (Phase 1)##" + std::to_string(skin.m_nID);
                                        break;
                                    case 419:
                                        strLabel = "Doppler (Phase 2)##" + std::to_string(skin.m_nID);
                                        break;
                                    case 420:
                                        strLabel = "Doppler (Phase 3)##" + std::to_string(skin.m_nID);
                                        break;
                                    case 421:
                                        strLabel = "Doppler (Phase 4)##" + std::to_string(skin.m_nID);
                                        break;
                                    default:
                                        strLabel = "Doppler (Other)##" + std::to_string(skin.m_nID);
                                        break;
                                    }
                                }
                                else
                                    strLabel = skin.m_strName + "##" + std::to_string(skin.m_nID);

                                if (!skinFilter.PassFilter(strLabel.c_str()))
                                    continue;

                                if (ApraGui::Selectable(strLabel.c_str(), InventoryChanger::m_pSelectedItem->m_pSelectedSkin == &skin, ImVec2(0, 0), GetRarityColor(skin.m_nRarity, 1)))
                                    InventoryChanger::m_pSelectedItem->m_pSelectedSkin = &skin;
                            }
                        }
                    }
                    else
                    {
                        ImGui::PushFont(Fonts::Default);

                        ImVec2 vecTextSize = ImGui::CalcTextSize(X("Select an item first"));
                        ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x * 0.5f) - (vecTextSize.x * 0.5f), (ImGui::GetWindowSize().y * 0.5f) - (vecTextSize.y * 0.5f)));
                        ApraGui::Label(X("Select an item first"));

                        ImGui::PopFont();
                    }

                }
                ApraGui::ListBoxEnd();

                static std::string strWeaponName;
                static float flWear = 0.0001f;
                static int nSeed = 0;
                static int nStatTrack = 0;

                //if (ApraGui::Button("Add item", ImGui::GetContentRegionAvail()))
                //{
                //    if (InventoryChanger::m_pSelectedItem && InventoryChanger::m_pSelectedItem->m_pSelectedSkin)
                //    {
                //        CEconItem* pItem = CEconItem::CreateInstance();
                //        if (pItem)
                //        {
                //            CCSPlayerInventory* pInventory = CCSPlayerInventory::GetPlayerInventory();
                //            auto HighestIDs = pInventory->GetHighestIDs();
                //            pItem->m_ulID = HighestIDs.first + 1;
                //            pItem->m_unInventory = HighestIDs.second + 1;
                //            pItem->m_unAccountID = static_cast<std::uint32_t>(pInventory->GetOwner().m_uID);
                //            pItem->m_unDefIndex = InventoryChanger::m_pSelectedItem->m_uDefinitionIndex;
                //            if (InventoryChanger::m_pSelectedItem->m_bUnusualItem)
                //                pItem->m_nRarity = IQ_UNUSUAL;

                //            pItem->m_nRarity = std::clamp(InventoryChanger::m_pSelectedItem->m_nRarity + InventoryChanger::m_pSelectedItem->m_pSelectedSkin->m_nRarity - 1, 0, (InventoryChanger::m_pSelectedItem->m_pSelectedSkin->m_nRarity == 7) ? 7 : 6);
                //            pItem->SetPaintKit(static_cast<float>(InventoryChanger::m_pSelectedItem->m_pSelectedSkin->m_nID));
                //            pItem->SetPaintSeed(static_cast<float>(nSeed));
                //            pItem->SetPaintWear(flWear);

                //            if (nStatTrack > 0)
                //            {
                //                pItem->SetStatTrak(nStatTrack);
                //                pItem->SetStatTrakType(0);

                //                if (pItem->m_nQuality != IQ_UNUSUAL)
                //                    pItem->m_nQuality = IQ_STRANGE;
                //            }

                //            if (pInventory->AddEconItem(pItem))
                //                InventoryChanger::AddEconItemToList
                //                (
                //                    InventoryItem_t(
                //                        InventoryChanger::m_pSelectedItem->m_strName,
                //                        InventoryChanger::m_pSelectedItem->m_pSelectedSkin->m_strName,
                //                        InventoryChanger::m_pSelectedItem->m_pSelectedSkin->m_nID,
                //                        pItem->m_unDefIndex,
                //                        pItem->m_nRarity,
                //                        nStatTrack,
                //                        nSeed,
                //                        flWear,
                //                        strWeaponName,
                //                        pItem->m_ulID,
                //                        0,
                //                        false,
                //                        false
                //                    )

                //                );

                //            nStatTrack = 0;
                //            nSeed = 0;
                //            flWear = 0.0001f;
                //            strWeaponName.clear();

                //            ImGui::CloseCurrentPopup();
                //        }
                //    }

                //    else if (InventoryChanger::m_pSelectedAgent)
                //    {
                //        CEconItem* pItem = CEconItem::CreateInstance();
                //        if (pItem)
                //        {
                //            CCSPlayerInventory* pInventory = CCSPlayerInventory::GetPlayerInventory();
                //            auto HighestIDs = pInventory->GetHighestIDs();
                //            pItem->m_ulID = HighestIDs.first + 1;
                //            pItem->m_unInventory = HighestIDs.second + 1;
                //            pItem->m_unAccountID = static_cast<std::uint32_t>(pInventory->GetOwner().m_uID);
                //            pItem->m_unDefIndex = InventoryChanger::m_pSelectedAgent->m_nDefinitionIndex;

                //            pItem->m_nRarity = std::clamp(InventoryChanger::m_pSelectedAgent->m_nRarity + InventoryChanger::m_pSelectedAgent->m_nRarity - 1, 0, (InventoryChanger::m_pSelectedAgent->m_nRarity == 7) ? 7 : 6);

                //            if (pInventory->AddEconItem(pItem))
                //                InventoryChanger::AddEconItemToList
                //                (
                //                    InventoryItem_t(
                //                        InventoryChanger::m_pSelectedAgent->m_strName,
                //                        InventoryChanger::m_pSelectedAgent->m_strModel,
                //                        -1,
                //                        pItem->m_unDefIndex,
                //                        pItem->m_nRarity,
                //                        0,
                //                        0,
                //                        0,
                //                        X("Agent"),
                //                        pItem->m_ulID,
                //                        0,
                //                        false,
                //                        false
                //                    )

                //                );
                //        }
                //    }

                //    InventoryChanger::m_bWantsHUDUpdate = true;
                //}

                ApraGui::ChildEnd();
            }

            ApraGui::SameLine();

            ApraGui::Child("Added items", &imAllChildSize);
            {
                if (InventoryChanger::m_vecAddedItems.empty())
                {
                    ImGui::PushFont(Fonts::Default);
                    ImVec2 vecTextSize = ImGui::CalcTextSize(X("No items added to inventory"));
                    ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x * 0.5f) - (vecTextSize.x * 0.5f), (ImGui::GetWindowSize().y * 0.5f) - (vecTextSize.y * 0.5f)));
                    ApraGui::Label(X("No items added to inventory"));
                    ImGui::PopFont();
                }
                else
                {
                    static InventoryItem_t* pSelectedItem = nullptr;
                    static size_t uEntry = 0U;
                    if (ApraGui::ListBox(X("##InventoryList"), ImVec2(-1, ImGui::GetContentRegionAvail().y - 27.0f), InventoryChanger::m_vecAddedItems.size()))
                    {
                        for (size_t uSize = 0U; uSize < InventoryChanger::m_vecAddedItems.size(); uSize++)
                        {
                            InventoryItem_t& pItem = InventoryChanger::m_vecAddedItems[uSize];
                            if (!&pItem)
                                continue;

                            std::string strLabel = X("");
                            if (pItem.m_strCustomName == X("Agent"))
                            {
                                strLabel = pItem.m_strItemName + X(" (") + pItem.m_strCustomName + X(")") + X("##") + std::to_string(uSize);
                            }
                            else if (pItem.m_strCustomName.empty())
                            {
                                strLabel = pItem.m_strItemName + X(" | ") + pItem.m_strSkinName + X("##") + std::to_string(uSize);
                            }
                            else
                            {
                                // has a name
                                strLabel = pItem.m_strItemName + X(" (") + pItem.m_strCustomName + X(")") + X(" | ") + pItem.m_strSkinName + X("##") + std::to_string(uSize);
                            }

                            if (ApraGui::Selectable(strLabel.c_str(), pSelectedItem == &pItem, ImVec2(0, 0)))
                            {
                                pSelectedItem = &pItem; // only for selected visualization
                                uEntry = uSize;
                            }
                        }
                        ApraGui::ListBoxEnd();
                    }

                    if (ApraGui::Button(X("Remove item from inventory"), ImVec2(ImGui::GetContentRegionAvail().x, 25.f)))
                    {
                        CCSPlayerInventory* pInventory = CCSPlayerInventory::GetPlayerInventory();
                        pInventory->RemoveEconItem(pInventory->GetSOCDataForItem(InventoryChanger::m_vecAddedItems[uEntry].m_uItemID));

                        if (!InventoryChanger::m_vecAddedItems.empty())
                            InventoryChanger::m_vecAddedItems.erase(InventoryChanger::m_vecAddedItems.begin() + uEntry);
                    }
                }
            }
            ApraGui::ChildEnd();

            break;
        }
        case ETabs::TABS_CONFIG:
        {
            static int nSelected = -1;
            ApraGui::Child("Config", &imAllChildSize);
            {
                if (ApraGui::ListBox(X("##Config-List"), ImVec2(ImGui::GetContentRegionAvail().x + 5, ImGui::GetContentRegionAvail().y), Config::vecFileNames.size()))
                {
                    for (size_t uIndex = 0U; uIndex < Config::vecFileNames.size(); uIndex++)
                    {
                        if (ApraGui::Selectable(Config::vecFileNames.at(uIndex).c_str(), uIndex == nSelected, ImVec2(0, 0)))
                            nSelected = uIndex;
                    }

                    ApraGui::ListBoxEnd();
                }
            }
            ApraGui::ChildEnd();

            ApraGui::SameLine();

            static std::string strConfigFile;

            ApraGui::Child("Options", &imAllChildSize);
            {
                ApraGui::Label("Config Name");
                ImGui::InputText(X("##config.file"), &strConfigFile);
                if (ApraGui::Button("Create", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
                {
                    Config::Save(strConfigFile, true);
                    strConfigFile.clear();
                    Config::Refresh();
                }

                if (ApraGui::Button("Save", ImVec2(-1, 0)))
                {
                    if (nSelected != -1)
                    {
                        Config::Save(Config::vecFileNames.at(nSelected), true);
                        InventoryChanger::m_bWantsHUDUpdate = true;
                        InventoryChanger::OnConfigSave(Config::vecFileNames.at(nSelected));
                    }
                }

                if (ApraGui::Button("Load", ImVec2(-1, 0)))
                {
                    if (nSelected != -1)
                    {
                        Config::Load(Config::vecFileNames.at(nSelected), true);
                        InventoryChanger::m_bWantsHUDUpdate = true;
                        InventoryChanger::OnConfigLoad(Config::vecFileNames.at(nSelected));
                    }
                }

                if (ApraGui::Button("Refresh", ImVec2(-1, 0)))
                {
                    Config::Refresh();
                }
            }
            ApraGui::ChildEnd();

            break;
        }
        case ETabs::TABS_MISC:
        {
            ApraGui::Child("Misc", &imAllChildSize);
            {
                ApraGui::MultiComboBox("Overlay", Tabs::szOverlay, Config::vb(g_Variables.m_Gui.m_vbOverlay), IM_ARRAYSIZE(Tabs::szOverlay));

                ApraGui::CheckBox("Hit logs", &Config::b(g_Variables.m_Misc.m_bHitlogs));
                ApraGui::CheckBox("Distant scout", &Config::b(g_Variables.m_Misc.m_bDistantScoutSound));
                ApraGui::CheckBox("Auto pistol", &Config::b(g_Variables.m_Misc.m_bAutoPistol));

                ApraGui::CheckBox("Hit sound", &Config::b(g_Variables.m_Misc.m_bHitSound));
                ApraGui::SameLine();
                ApraGui::Combo("Hit sound type", &Config::i(g_Variables.m_Misc.m_iHitSoundIndex), szHitsounds, IM_ARRAYSIZE(szHitsounds), 0, false);

                ApraGui::CheckBox("Auto purchase", &Config::b(g_Variables.m_Misc.m_bAutoPurchase));
                ApraGui::Combo("Primary", &Config::i(g_Variables.m_Misc.m_iAutoPurchase1), zxAutoBuy1, IM_ARRAYSIZE(zxAutoBuy1));
                ApraGui::Combo("Secondary", &Config::i(g_Variables.m_Misc.m_iAutoPurchase2), zxAutoBuy2, IM_ARRAYSIZE(zxAutoBuy2));
                ApraGui::MultiComboBox("Additional", zxAutoBuy3, Config::vb(g_Variables.m_Misc.m_uAutoPurchase3), IM_ARRAYSIZE(zxAutoBuy3), true);
#ifdef _DEBUG
                ApraGui::CheckBox("Debug", &Config::b(g_Variables.m_Misc.m_bDebug));
                if (ApraGui::Button("Debug Mode", ImVec2(-1, 0)))
                {
                    Interfaces::m_pEngine->ExecuteClientCmdUnrestricted(std::string(
                        std::string("sv_cheats 1; ") +
                        std::string("bot_kick; ") +
                        std::string("bot_stop 1; ") +
                        std::string("bot_add; ") +
                        std::string("sv_quantize_movement_input 0; ") +
                        std::string("sv_airaccelerate 100; ") +
                        std::string("sv_steamauth_enforce 0; ") +
                        std::string("mp_respawn_immunitytime -1; ")
                    ).c_str());
                }
#endif 
            }
            ApraGui::ChildEnd();

            ApraGui::SameLine();

            ApraGui::Child("Movement", &imAllChildSize);
            {
                ApraGui::CheckBox("Bunnyhop", &Config::b(g_Variables.m_Movement.m_bEnableBunnyHop));
                ApraGui::CheckBox("Jump Bug", &Config::b(g_Variables.m_Movement.m_bEnableJumpBug));
                ApraGui::CheckBox("Quick stop", &Config::b(g_Variables.m_Movement.m_bEnableQuickStop));
                ApraGui::CheckBox("Auto strafe", &Config::b(g_Variables.m_Movement.m_bEnableAutoStrafe));
                ApraGui::CheckBox("Subtick strafe", &Config::b(g_Variables.m_Movement.m_bEnableAutoStrafeExploit));
                //ApraGui::CheckBox("Straigth throw", &Config::b(g_Variables.m_Movement.m_bStraigthThrow));
                ApraGui::Label("Edge jump");
                ApraGui::SameLine();
                ApraGui::Keybind("Edge jump", &Config::kb(g_Variables.m_Movement.m_kEdgeJump));
            }
            ApraGui::ChildEnd();
            break;
        }
        }

    }
    ApraGui::End();

    blurEffect.EndBlur();
}