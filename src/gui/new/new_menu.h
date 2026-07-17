#pragma once
#include "../../Precompiled.h"
#include "new_fonts.h"

namespace pic
{
    extern ID3D11ShaderResourceView* background;

    extern ID3D11ShaderResourceView* aimbot;
    extern ID3D11ShaderResourceView* visuals;
    extern ID3D11ShaderResourceView* skins;
    extern ID3D11ShaderResourceView* settings;

    extern ID3D11ShaderResourceView* combo_widget;
    extern ID3D11ShaderResourceView* input_widget;

    extern ID3D11ShaderResourceView* menu_settings_icon;

    extern ID3D11ShaderResourceView* circle_success;
    extern ID3D11ShaderResourceView* circle_error;
}

namespace font
{
    extern ImFont* inter_bold;
    extern ImFont* inter_default;
    extern ImFont* icon;
}

class cmenu {
public:
    void render();
};

const auto menu = std::make_unique<cmenu>();