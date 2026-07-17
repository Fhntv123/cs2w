#include "../Precompiled.h"

#define IMGUI_DEFINE_MATH_OPERATORS

using namespace ImGui;

struct CheckBoxObj_t
{
    // State: General
    float               flFrameRounding = 2.f;
    float                        flSize = 16.f;
    float          flAnimationTimeLabel = 20.f;

    ImColor           imBackgroundColor = ImColor(40, 40, 40, 255);

    bool                     bDrawDebug = false;

    // State: Unchecked
    bool bRenderCheckMarkWhen_Unchecked = true;

    ImColor       imCheckMark_Unchecked = ImColor(0, 0, 0, 0);

    // State: Hovered
    ImColor         imCheckMark_Hovered = ImColor(200, 200, 200, 120);

    bool   bRenderCheckMarkWhen_Hovered = true;
    float       flAnimationTime_Hovered = 20.f; // 0 = slow


    // State: Checked
    ImColor         imCheckMark_Checked = ImColor(160, 125, 255, 255);

    bool   bRenderCheckMarkWhen_Checked = true;
    float       flAnimationTime_Checked = 20.f; // 0 = slow
};

void ApraGui::SameLine(float flOffsetFromStartX, float flSpacingW)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* pWindow = g.CurrentWindow;

    if (pWindow->SkipItems)
        return;

    if (flOffsetFromStartX != 0.0f)
    {
        if (flSpacingW < 0.0f)
            flSpacingW = 0.0f;

        pWindow->DC.CursorPos.x = pWindow->Pos.x - pWindow->Scroll.x + flOffsetFromStartX + flSpacingW + pWindow->DC.GroupOffset.x + pWindow->DC.ColumnsOffset.x;
        pWindow->DC.CursorPos.y = pWindow->DC.CursorPosPrevLine.y;
    }
    else
    {
        if (flSpacingW < 0.0f)
            flSpacingW = g.Style.ItemSpacing.x;

        pWindow->DC.CursorPos.x = pWindow->DC.CursorPosPrevLine.x + flSpacingW;
        pWindow->DC.CursorPos.y = pWindow->DC.CursorPosPrevLine.y;
    }

    pWindow->DC.CurrLineSize = pWindow->DC.PrevLineSize;
    pWindow->DC.CurrLineTextBaseOffset = pWindow->DC.PrevLineTextBaseOffset;
    pWindow->DC.IsSameLine = true;
}

bool ApraGui::CheckBox(const char* szLabel, bool* bBool, int iCustomID)
{
    static CheckBoxObj_t obj;

    ImGuiWindow* imWindow = GetCurrentWindow();
    if (imWindow->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID imID = imWindow->GetID(szLabel) + iCustomID * 8662;

    const ImVec2 imLabelSize = CalcTextSize(szLabel, NULL, true);
    const float flSquare = obj.flSize;

    const ImVec2 imPos = imWindow->DC.CursorPos;
    const ImRect imTotalBB(imPos, imPos + ImVec2(flSquare + (imLabelSize.x > 0.0f ? style.ItemInnerSpacing.x + imLabelSize.x : 0.0f), flSquare));

    const bool bIsVisible = ItemAdd(imTotalBB, imID);
    ItemSize(imTotalBB);

    bool bChecked = *bBool;

    bool bHovered, bHeld;
    bool bPressed = ButtonBehavior(imTotalBB, imID, &bHovered, &bHeld);

    if (bPressed)
        bChecked = !bChecked;

    if (*bBool != bChecked)
    {
        *bBool = bChecked;
        bPressed = true;
        MarkItemEdited(imID);
    }

    const ImRect imCheckBb(imPos, imPos + ImVec2(flSquare, flSquare));

    if (bIsVisible)
    {
        RenderFrame(imCheckBb.Min, imCheckBb.Max, obj.imBackgroundColor, true, obj.flFrameRounding);

        float* flAnimatedAlpha_Hovered = ImGui::GetStateStorage()->GetFloatRef(imID, 0.0f);
        float* flAnimatedAlpha_Checked = ImGui::GetStateStorage()->GetFloatRef(imID + 1, 0.0f);
        float* flInterpolatedLabelAlpha = ImGui::GetStateStorage()->GetFloatRef(imID + 2, 0.0f);

        float flTargetAlpha_Hovered = bHovered ? obj.imCheckMark_Hovered.Value.w : 0.0f;
        *flAnimatedAlpha_Hovered = ImLerp(*flAnimatedAlpha_Hovered, flTargetAlpha_Hovered, ImGui::GetIO().DeltaTime * obj.flAnimationTime_Hovered);

        float flTargetAlpha_Checked = *bBool ? obj.imCheckMark_Checked.Value.w : obj.imCheckMark_Hovered.Value.w;
        *flAnimatedAlpha_Checked = ImLerp(*flAnimatedAlpha_Checked, flTargetAlpha_Checked, ImGui::GetIO().DeltaTime * obj.flAnimationTime_Checked);


        ImColor imLabelColor = ImColor(255, 255, 255);

        // Render Unchecked Checkmark
        if (!bHovered && !*bBool)
        {
            if (obj.bRenderCheckMarkWhen_Unchecked)
            {
                const float flPad = std::max(1.0f, flSquare / 6.0f);

                RenderCheckMark(imWindow->DrawList, imCheckBb.Min + ImVec2(flPad, flPad), obj.imCheckMark_Unchecked, flSquare - flPad * 2.0f);
            }
        }

        // Render Hovered Checkmark
        if (bHovered && !*bBool)
        {
            if (obj.bRenderCheckMarkWhen_Hovered)
            {
                ImColor imHoverColor = obj.imCheckMark_Hovered.Value;
                imHoverColor.Value.w = *flAnimatedAlpha_Hovered;

                const float flPad = std::max(1.0f, flSquare / 6.0f);

                RenderCheckMark(imWindow->DrawList, imCheckBb.Min + ImVec2(flPad, flPad), imHoverColor, flSquare - flPad * 2.0f);
            }
        }

        // Render Checked Checkmark
        if (*bBool)
        {
            ImColor imCheckedColor = obj.imCheckMark_Checked.Value;
            imCheckedColor.Value.w = ImClamp(*flAnimatedAlpha_Checked, 0.0f, 1.0f);

            const float flPad = std::max(1.0f, flSquare / 6.0f);

            RenderCheckMark(imWindow->DrawList, imCheckBb.Min + ImVec2(flPad, flPad), imCheckedColor, flSquare - flPad * 2.0f);
        }

        float flTargetLabelAlpha = imLabelColor.Value.w;
        *flInterpolatedLabelAlpha = ImLerp(*flInterpolatedLabelAlpha, flTargetLabelAlpha, ImGui::GetIO().DeltaTime * obj.flAnimationTimeLabel);

        imLabelColor.Value.w = ImClamp(*flInterpolatedLabelAlpha, 0.0f, 1.0f);

        const float flCheckboxCenterY = (imCheckBb.Min.y + imCheckBb.Max.y) / 2;
        const float flLabelY = flCheckboxCenterY - imLabelSize.y / 2;
        const ImVec2 imLabelPos = ImVec2(imCheckBb.Max.x + 4, flLabelY);


        imWindow->DrawList->AddText(imLabelPos, imLabelColor, szLabel);
    }

    if (obj.bDrawDebug)
    {
        imWindow->DrawList->AddRect(imTotalBB.Min, imTotalBB.Max, ImColor(255, 0, 0, 70));
    }

    return bChecked;
}

bool ApraGui::SliderInt(const char* szLabel, int* iInt, int iMin, int iMax, int iCustomID, bool bForceCustomID, const char* szFormat, ImGuiSliderFlags imSliderFlags)
{
    return SliderScalar(szLabel, ImGuiDataType_S32, iInt, &iMin, &iMax, szFormat, imSliderFlags, iCustomID, bForceCustomID);
}

struct SliderObj_t
{
    // State: General
    float               flFrameRounding = 2.f;
    float                    flWidthAdd = 0.f;
    float                    flWidthSub = 0.f/*300.f*/;

    ImColor           imBackgroundColor = ImColor(40, 40, 40, 255);
    ImColor               imSliderColor = ImColor(160, 125, 255, 255);

    bool                     bDrawDebug = false;

    float         flAnimationTime_Value = 10.f; // 0 = slow

    // State: Hover
    float        flAnimationTime_Hover = 10.f; // 0 = slow

    // State: Active
    ImColor        imSliderColor_Active = ImColor(160, 125, 255, 255);
    float        flAnimationTime_Active = 25.f; // 0 = slow
};

bool ApraGui::NoneSlider(const char* szLabel, int* iInt, int iMin, int iMax, int iCustomID, const char* szFormat, ImGuiSliderFlags imSliderFlags)
{
    char displayBuf[64];
    if (*iInt <= iMin)
        _snprintf_s(displayBuf, sizeof(displayBuf), "None");
    else
        _snprintf_s(displayBuf, sizeof(displayBuf), "%d", *iInt);

    return SliderScalar(szLabel, ImGuiDataType_S32, iInt, &iMin, &iMax, displayBuf, imSliderFlags, iCustomID, false);
}

bool ApraGui::MinDamageSlider(const char* szLabel, int* iInt, int iMin, int iMax, int iCustomID, const char* szFormat, ImGuiSliderFlags imSliderFlags)
{
    char displayBuf[64];
    if (*iInt > 100)
        _snprintf_s(displayBuf, sizeof(displayBuf), "HP + %d", *iInt - 100);
    else
        _snprintf_s(displayBuf, sizeof(displayBuf), "%d", *iInt);

    return SliderScalar(szLabel, ImGuiDataType_S32, iInt, &iMin, &iMax, displayBuf, imSliderFlags, iCustomID, false);
}

bool ApraGui::SliderFloat(const char* szLabel, float* flFloat, float flMin, float flMax, int iCustomID, bool bForceCustomID, const char* szFormat, ImGuiSliderFlags imSliderFlags)
{
    return SliderScalar(szLabel, ImGuiDataType_Float, flFloat, &flMin, &flMax, szFormat, imSliderFlags, iCustomID, bForceCustomID);
}


bool ApraGui::SliderScalar(const char* szLabel, ImGuiDataType imDataType, void* pData, const void* pMin, const void* pMax, const char* szFormat, ImGuiSliderFlags imSliderFlags, int iCustomID, bool bForceCustomID)
{
    static SliderObj_t obj;

    ImGuiWindow* imWindow = GetCurrentWindow();
    if (imWindow->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID imID = bForceCustomID ? iCustomID : imWindow->GetID(szLabel) + iCustomID;
    float flFullWidth = GetContentRegionAvail().x;

    const ImVec2 imLabelSize = CalcTextSize(szLabel, NULL, true);
    const float flPadding = 6.0f;
    const float flSliderHeight = imLabelSize.y/* + 10.0f*/;

    // Position for label
    ImVec2 imLabelPos = imWindow->DC.CursorPos;
    //ImGui::RenderText( imLabelPos, szLabel );

    // Slider starts to the right of the label
    float flSliderStartX = imLabelPos.x + imLabelSize.x + style.ItemSpacing.x;
    float flSliderWidth = flFullWidth - imLabelSize.x - style.ItemSpacing.x;

    ImVec2 sliderPosMin(flSliderStartX, imLabelPos.y);
    ImVec2 sliderPosMax(flSliderStartX + flSliderWidth, imLabelPos.y + flSliderHeight);

    ImRect imFrameBB(sliderPosMin, sliderPosMax);
    const ImRect imTotalBB(imLabelPos, imFrameBB.Max);

    const bool bTempInputAllowed = (imSliderFlags & ImGuiSliderFlags_NoInput) == 0;
    ItemSize(imTotalBB, style.FramePadding.y);
    if (!ItemAdd(imTotalBB, imID, &imFrameBB, bTempInputAllowed ? ImGuiItemFlags_Inputable : 0))
        return false;

    if (szFormat == NULL)
        szFormat = DataTypeGetInfo(imDataType)->PrintFmt;

    const bool bHovered = false;
    const bool bClicked = bHovered && IsMouseClicked(0, ImGuiInputFlags_None, imID);
    const bool bItemActive = IsItemActive();

    bool bTemmpInputActive = bTempInputAllowed && TempInputIsActive(imID);
    if (!bTemmpInputActive)
    {
        const bool bMakeActive = (bClicked || g.NavActivateId == imID);
        if (bMakeActive && bClicked)
            SetKeyOwner(ImGuiKey_MouseLeft, imID);
        if (bMakeActive)
        {
            SetActiveID(imID, imWindow);
            SetFocusID(imID, imWindow);
            FocusWindow(imWindow);
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
        }
    }

    ImRect imGrabBB;
    const bool bValueChanged = SliderBehavior(imFrameBB, imID, imDataType, pData, pMin, pMax, szFormat, imSliderFlags, &imGrabBB);
    if (bValueChanged)
        MarkItemEdited(imID);

    float* flAnimatedValue = ImGui::GetStateStorage()->GetFloatRef(imID + 1, *(float*)pData); // Unique key
    float targetValue = (float)(*(int*)pData);
    *flAnimatedValue = ImLerp(*flAnimatedValue, targetValue, ImGui::GetIO().DeltaTime * obj.flAnimationTime_Active);

    float* labelAlpha = ImGui::GetStateStorage()->GetFloatRef(imID + 2000, 0.3f);
    float labelAnimSpeed = ImGui::GetIO().DeltaTime * obj.flAnimationTime_Hover;
    float* valueAlpha = ImGui::GetStateStorage()->GetFloatRef(imID + 3000, 0.0f);
    float valueSpeed = ImGui::GetIO().DeltaTime * obj.flAnimationTime_Value;
    *valueAlpha = ImClamp(*valueAlpha + ((bHovered || bItemActive) ? valueSpeed : -valueSpeed), 0.0f, 1.0f);

    int min = *(int*)pMin;
    int max = *(int*)pMax;
    float fraction = ImClamp((*flAnimatedValue - min) / (float)(max - min), 0.0f, 1.0f);
    float animatedX = ImLerp(imFrameBB.Min.x, imFrameBB.Max.x, fraction);
    ImVec2 imEndSliderPos = ImVec2(animatedX, imFrameBB.Max.y);

    // Background
    imWindow->DrawList->AddRectFilled(
        imFrameBB.Min + ImVec2(0, 3),
        imFrameBB.Max - ImVec2(0, 3),
        obj.imBackgroundColor,
        obj.flFrameRounding
    );

    // Fill
    imWindow->DrawList->AddRectFilled(
        ImVec2(imFrameBB.Min.x, imFrameBB.Min.y + 3),
        ImVec2(animatedX, imFrameBB.Max.y - 3),
        g.ActiveId == imID ? obj.imSliderColor_Active : obj.imSliderColor,
        obj.flFrameRounding
    );

    if (*valueAlpha > 0.0f)
    {
        char szValueBuf[64];
        const char* szValueBufEnd = szValueBuf + DataTypeFormatString(szValueBuf, IM_ARRAYSIZE(szValueBuf), imDataType, pData, szFormat);
        ImVec2 imValueTextSize = CalcTextSize(szValueBuf, szValueBufEnd);

        ImVec2 padding(4.0f, 2.0f);
        ImVec2 boxSize = imValueTextSize + padding * 2.0f;

        // Compute initial centered position
        ImVec2 imValueTextPos = ImVec2(animatedX - boxSize.x / 2.0f + padding.x, imFrameBB.Min.y - imValueTextSize.y - 6);

        // Compute background box bounds
        ImVec2 bgMin = imValueTextPos - padding;
        ImVec2 bgMax = imValueTextPos + imValueTextSize + padding;

        // Clamp horizontal position of the box to the slider bounds
        float boxLeft = bgMin.x;
        float boxRight = bgMax.x;
        float sliderLeft = imFrameBB.Min.x;
        float sliderRight = imFrameBB.Max.x;

        float shift = 0.0f;
        if (boxLeft < sliderLeft)
            shift = sliderLeft - boxLeft;
        else if (boxRight > sliderRight)
            shift = sliderRight - boxRight;

        // Apply shift
        imValueTextPos.x += shift;
        bgMin.x += shift;
        bgMax.x += shift;

        imWindow->DrawList->AddRectFilled(bgMin, bgMax, ImColor(35, 35, 35, static_cast<int>(*valueAlpha * 255.f)), 4.0f);
    }

    if (obj.bDrawDebug)
        imWindow->DrawList->AddRect(imTotalBB.Min, imTotalBB.Max, IM_COL32(255, 0, 0, 80));

    return bValueChanged;
}


struct BeginComboObj_t
{
    // State: General
    float               flFrameRounding = 2.f;
    float                    flWidthAdd = 0.f;
    float                    flWidthSub = 300.f;
    float          flAnimationTimeArrow = 8.f;
    float              flPaddingForText = 2.f;
    float          flAnimationTimeLabel = 8.f;
    float          flAnimationTimePopup = 12.f;

    ImColor           imBackgroundColor = ImColor(40, 40, 40, 255);
    ImColor                imArrowColor = ImColor(160, 125, 255, 255);

    bool                     bDrawDebug = false;

    // State: Unused

    // State: Using
};


bool ApraGui::BeginCombo(const char* szLabel, const char* szCurrentValue, bool bDrawLabel, bool bColorPickerInline)
{
    static BeginComboObj_t obj;

    ImGuiContext& g = *GImGui;
    ImGuiWindow* imWindow = GetCurrentWindow();

    g.NextWindowData.ClearFlags();
    if (imWindow->SkipItems)
        return false;

    const ImGuiStyle& imStyle = g.Style;
    const ImGuiID imID = imWindow->GetID(szLabel);

    ImVec2 label_size(0.0f, 0.0f);
    const char* render_label_text_ptr = szLabel;

    if (bDrawLabel) {
        label_size = CalcTextSize(szLabel, NULL, true);
        if (label_size.x == 0.0f) {
            bDrawLabel = false;
            render_label_text_ptr = NULL;
        }
    }
    else {
        render_label_text_ptr = NULL;
    }

    const float flFrameHeight = GetFrameHeight() - 2.f;

    float flWidthForComboAndPicker = GetContentRegionAvail().x;
    if (bDrawLabel) {
        flWidthForComboAndPicker -= (label_size.x + imStyle.ItemInnerSpacing.x);
        flWidthForComboAndPicker = ImMax(1.0f, flWidthForComboAndPicker);
    }

    float flComboActualWidth = flWidthForComboAndPicker;
    if (bColorPickerInline) {
        flComboActualWidth -= (14.0f /*color picker width*/ + 10.0f /*just padding*/);
        flComboActualWidth = ImMax(1.0f, flComboActualWidth);
    }

    ImVec2 combo_frame_pos = imWindow->DC.CursorPos;
    if (bDrawLabel)
    {
        combo_frame_pos.x += label_size.x + imStyle.ItemInnerSpacing.x;
    }
    else
    {
        combo_frame_pos.y -= 1.5f;
    }

    const ImRect imFrameBB(
        combo_frame_pos,
        combo_frame_pos + ImVec2(flComboActualWidth, flFrameHeight)
    );

    const ImRect imTotalBB(
        imWindow->DC.CursorPos,
        imWindow->DC.CursorPos + ImVec2(
            (bDrawLabel ? label_size.x + imStyle.ItemInnerSpacing.x : 0.0f) + flWidthForComboAndPicker,
            flFrameHeight
        )
    );

    ItemSize(imTotalBB, imStyle.FramePadding.y);
    if (!ItemAdd(imTotalBB, imID, &imFrameBB))
        return false;

    bool bHovered, bHeld;
    bool bPressed = ButtonBehavior(imFrameBB, imID, &bHovered, &bHeld);
    const ImGuiID imPopupID = ImHashStr("##ComboPopup", 0, imID);
    bool bPopupOpen = IsPopupOpen(imPopupID, ImGuiPopupFlags_None);

    if (bPressed && !bPopupOpen) {
        OpenPopupEx(imPopupID, ImGuiPopupFlags_None);
        bPopupOpen = true;
    }

    /*float* flPopupAlpha = ImGui::GetStateStorage()->GetFloatRef(imPopupID, 0.0f);
    float flTargetAlpha = bPopupOpen ? 1.0f : 0.1f;
    *flPopupAlpha = ImLerp(*flPopupAlpha, flTargetAlpha, ImGui::GetIO().DeltaTime * obj.flAnimationTimePopup);*/

    imWindow->DrawList->AddRectFilled(imFrameBB.Min, imFrameBB.Max, obj.imBackgroundColor, obj.flFrameRounding);

    const float flArrowSize = GetFrameHeight();

    float* flArrowAngle = ImGui::GetStateStorage()->GetFloatRef(imID, 0.0f);

    float flTargetArrowAngle = bPopupOpen ? IM_PI * 0.5f : -IM_PI * 0.5f;
    *flArrowAngle = ImLerp(*flArrowAngle, flTargetArrowAngle, ImGui::GetIO().DeltaTime * obj.flAnimationTimeArrow);

    *flArrowAngle = ImClamp(*flArrowAngle, -90.f, 90.f);

    const float flValue2x = ImMax(imFrameBB.Min.x, imFrameBB.Max.x - flArrowSize);

    if (flValue2x + flArrowSize - imStyle.FramePadding.x <= imFrameBB.Max.x)
        RenderArrow(imWindow, ImVec2(flValue2x + imStyle.FramePadding.y, imFrameBB.Min.y + imStyle.FramePadding.y), obj.imArrowColor, *flArrowAngle, 0.7f);


    RenderFrameBorder(imFrameBB.Min, imFrameBB.Max, imStyle.FrameRounding);

    float* flInterpolatedLabelAlpha = ImGui::GetStateStorage()->GetFloatRef(imID + 1, 0.f);

    if (bDrawLabel && render_label_text_ptr) {
        ImVec4 base_color_rgb_values;

        ImVec4 final_label_color = ImVec4(base_color_rgb_values.x, base_color_rgb_values.y, base_color_rgb_values.z, *flInterpolatedLabelAlpha);

        ImVec2 label_render_pos = ImVec2(
            imWindow->DC.CursorPos.x,
            imFrameBB.Min.y + (flFrameHeight - label_size.y) * 0.5f
        );
        imWindow->DrawList->AddText(label_render_pos, ColorConvertFloat4ToU32(final_label_color), render_label_text_ptr);
    }

    ImVec2 imSelectedTextPos = ImVec2(
        imFrameBB.Min.x + imStyle.FramePadding.x,
        (imFrameBB.Min.y + imFrameBB.Max.y) * 0.5f - CalcTextSize(szCurrentValue).y * 0.5f
    );

    float selected_text_max_x = imFrameBB.Max.x - flArrowSize - imStyle.FramePadding.x;
    ImRect clip_rect_text(imFrameBB.Min.x + imStyle.FramePadding.x, imFrameBB.Min.y, selected_text_max_x, imFrameBB.Max.y);

    imWindow->DrawList->PushClipRect(clip_rect_text.Min, clip_rect_text.Max, true);
    imWindow->DrawList->PopClipRect();

    if (obj.bDrawDebug) {
        imWindow->DrawList->AddRect(imTotalBB.Min, imTotalBB.Max, IM_COL32(255, 0, 0, 120)); // Red for total
        imWindow->DrawList->AddRect(imFrameBB.Min, imFrameBB.Max, IM_COL32(0, 255, 0, 120)); // Green for frame
    }

    if (bPopupOpen)
        if (ApraGui::BeginComboPopup(imPopupID, imFrameBB))
            return true;

    return false;
}

bool ApraGui::BeginComboPopup(ImGuiID imPopupID, const ImRect& imFrameBB)
{
    static BeginComboObj_t obj;

    ImGuiContext& g = *GImGui;
    if (!IsPopupOpen(imPopupID, ImGuiPopupFlags_None))
    {
        g.NextWindowData.ClearFlags();
        return false;
    }

    // Set popup size
    float w = imFrameBB.GetWidth();
    if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint)
    {
        g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
    }
    else
    {
        /* if ((flags & ImGuiComboFlags_HeightMask_) == 0)
             flags |= ImGuiComboFlags_HeightRegular;*/
             //IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_)); // Only one
        int popup_max_height_in_items = 8;
        /*if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
        else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
        else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;*/
        ImVec2 constraint_min(0.0f, 0.0f), constraint_max(FLT_MAX, FLT_MAX);
        if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize) == 0 || g.NextWindowData.SizeVal.x <= 0.0f) // Don't apply constraints if user specified a size
            constraint_min.x = w;
        if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize) == 0 || g.NextWindowData.SizeVal.y <= 0.0f)
            constraint_max.y = CalcMaxPopupHeightFromItemCount(popup_max_height_in_items);
        SetNextWindowSizeConstraints(constraint_min, constraint_max);
    }

    // This is essentially a specialized version of BeginPopupEx()
    char name[16];
    ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

    // Set position given a custom constraint (peak into expected window size so we can position it)
    // FIXME: This might be easier to express with an hypothetical SetNextWindowPosConstraints() function?
    // FIXME: This might be moved to Begin() or at least around the same spot where Tooltips and other Popups are calling FindBestWindowPosForPopupEx()?
    if (ImGuiWindow* popup_window = FindWindowByName(name))
        if (popup_window->WasActive)
        {
            // Always override 'AutoPosLastDirection' to not leave a chance for a past value to affect us.
            ImVec2 size_expected = CalcWindowNextAutoFitSize(popup_window);
            popup_window->AutoPosLastDirection = ImGuiDir_Left; // Left = "Below, Toward Left", Down = "Below, Toward Right (default)"
            ImRect r_outer = GetPopupAllowedExtentRect(popup_window);
            ImVec2 pos = FindBestWindowPosForPopupEx(imFrameBB.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, imFrameBB, ImGuiPopupPositionPolicy_ComboBox);
            SetNextWindowPos(pos + ImVec2(0, 4));
        }

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));

    PushStyleVar(ImGuiStyleVar_PopupRounding, obj.flFrameRounding);

    PushStyleColor(ImGuiCol_PopupBg, obj.imBackgroundColor.Value);
    PushStyleColor(ImGuiCol_Border, ImColor(45, 45, 45, 150).Value);

    bool ret = ImGui::Begin(name, NULL, window_flags);

    PopStyleColor(2);
    PopStyleVar(2);

    if (!ret)
    {
        EndPopup();
        IM_ASSERT(0);
        return false;
    }
    return true;
}

void ApraGui::CheckboxComboColor(const char* strCheckboxLabel, bool* pbBool, int* piSelectedItem, const char* szItems[], int iNumItems, Color* pcColor)
{
    CheckBox(strCheckboxLabel, pbBool);
    SameLine();
    Combo(
        std::string("##" + std::string(strCheckboxLabel) + "combo").c_str(),
        piSelectedItem,
        szItems,
        iNumItems,
        0, false, true);
    SameLine();
    SimpleColorPicker(
        std::string("##" + std::string(strCheckboxLabel) + "color").c_str(),
        pcColor);
    SetCursorPosY(GetCursorPosY() - 3);
}

struct ComboButtonObj_t
{
    // State: Default
    bool                     bDrawDebug = false;

    // State: Unhovered

    // State: Hovered

    // State: Pressed
    ImColor       imCircleColor_Pressed = ImColor(160, 125, 255, 255);

    bool             bDrawIndentedLabel = true;
};

bool ApraGui::ComboButton(const char* szLabel, ImColor imLabelColor, bool bIsSelectedItem, int iCustomID)
{
    static ComboButtonObj_t obj;

    ImGuiWindow* imWindow = GetCurrentWindow();
    if (imWindow->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID imID = imWindow->GetID(szLabel) + iCustomID;
    ImVec2 imLabelSize = CalcTextSize(szLabel, NULL, true);

    ImVec2 imPos = imWindow->DC.CursorPos;

    ImVec2 imSize = CalcItemSize(ImVec2(-1, 0), imLabelSize.x + style.FramePadding.x * 2.0f, imLabelSize.y + 1.f * 0.5f);

    const ImRect imTotalBB(imPos, imPos + imSize);
    ItemSize(imSize);
    if (!ItemAdd(imTotalBB, imID))
        return false;

    bool bHovered, bHeld;
    bool bPressed = ButtonBehavior(imTotalBB, imID, &bHovered, &bHeld, 0);

    if (obj.bDrawDebug)
    {
        imWindow->DrawList->AddRect(imTotalBB.Min, imTotalBB.Max, ImColor(255, 0, 0, 70));
    }

    ImVec2 imLabelTextPos = imTotalBB.Min;

    if (bIsSelectedItem && obj.bDrawIndentedLabel)
    {
        imLabelTextPos.x += 4;
    }

    imWindow->DrawList->AddText(imLabelTextPos, imLabelColor, szLabel);

    return bPressed;
}

struct TabButtonObj_t
{
    // State: Default
    ImColor imDefaultColor = ImColor(255, 255, 255, 100);
    float flAnimationTime = 15.f;
    // State: Hovered
    ImColor imHoveredColor = ImColor(255, 255, 255, 180);

    // State: Selected
    ImColor imSelectedColor = ImColor(255, 255, 255, 255);
};

bool ApraGui::TabButton(const char* szLabel, int iThisTab, int iCurrentTab, int iTextAlign, ImVec2 imSize, int iCustomID)
{
    static TabButtonObj_t obj;

    ImGuiWindow* imWindow = GetCurrentWindow();
    if (imWindow->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID imID = imWindow->GetID(szLabel) + iCustomID;
    const ImVec2 imLabelSize = CalcTextSize(szLabel, NULL, true);

    ImVec2 imPos = imWindow->DC.CursorPos;

    ImVec2 size = CalcItemSize(imSize, imLabelSize.x + style.FramePadding.x * 2.0f, imLabelSize.y + style.FramePadding.y * 2.0f);

    const ImRect imTotalBB(imPos, imPos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(imTotalBB, imID))
        return false;

    bool bHovered, bHeld;
    bool bPressed = ButtonBehavior(imTotalBB, imID, &bHovered, &bHeld, 0);

    float* flAnimatedAlpha = ImGui::GetStateStorage()->GetFloatRef(imID, 0.0f);
    float flTargetAlpha = (iCurrentTab == iThisTab) ? 1.0f : (bHovered ? 0.8f : 0.5f);
    *flAnimatedAlpha = ImLerp(*flAnimatedAlpha, flTargetAlpha, ImGui::GetIO().DeltaTime * obj.flAnimationTime);

    ImColor imButtonColor = obj.imDefaultColor;
    if (bHovered && !bPressed)
    {
        imButtonColor = obj.imHoveredColor;
    }
    if (iCurrentTab == iThisTab)
    {
        imButtonColor = obj.imSelectedColor;
    }

    //RenderNavCursor(imTotalBB, imID);

    ImVec4 imLabelColor = imButtonColor.Value;
    imLabelColor.w = ImClamp(*flAnimatedAlpha, 0.0f, 1.0f);

    // Calculate horizontal alignment
    float alignFactor = (iTextAlign == -1) ? 0.0f : (iTextAlign == 1) ? 1.0f : 0.5f;
    ImVec2 imLabelTextPos = ImVec2(
        ImLerp(imTotalBB.Min.x + style.FramePadding.x, imTotalBB.Max.x - style.FramePadding.x - imLabelSize.x, alignFactor),
        ImLerp(imTotalBB.Min.y + style.FramePadding.y, imTotalBB.Max.y - style.FramePadding.y - imLabelSize.y, 0.2f)
    );

    imWindow->DrawList->AddText(imLabelTextPos, ImColor(imLabelColor), szLabel);

    return bPressed;
}

struct ComboObj_t
{
    // State: Default

    // State: Unselecetd

    // State: Selected
    float  flAnimationTimeItem_Selected = 4.f;

    // State: Hovered

    // State: Pressed
};

bool ApraGui::MultiComboBox(const char* label, const char* items[], std::vector<bool>& v, int size, bool bDrawLabel)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* pWindow = g.CurrentWindow;
    if (pWindow->SkipItems)
        return false;

    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;

    std::string szBuffer;
    const ImVec2 vecLabelSize = ImGui::CalcTextSize(label);
    float flActiveWidth = ImGui::CalcItemWidth() - (style.ItemInnerSpacing.x + ImGui::GetFrameHeight()) - 10.f;

    for (int i = 0; i < size; i++)
    {
        if (v[i])
        {
            ImVec2 vecTextSize = ImGui::CalcTextSize(szBuffer.c_str());

            if (szBuffer.empty())
                szBuffer.assign(items[i]);
            else
                szBuffer.append(X(", ")).append(items[i]);

            if (vecTextSize.x > flActiveWidth)
                szBuffer.erase(szBuffer.find_last_of(X(","))).append(X("..."));
        }
    }

    if (szBuffer.empty())
        szBuffer.assign(X("None"));

    ImGuiID imID = ImGui::GetID(label);

    bool bValueChanged = false;
    if (ApraGui::BeginCombo(label, szBuffer.c_str(), bDrawLabel))
    {
        for (int i = 0; i < size; i++)
        {
            bool bIsSelectedItem = v[i];
            ImColor imLabelColor = bIsSelectedItem ? ImColor(255, 255, 255, 255) : ImColor(255, 255, 255, 100);

            ImGuiID imIDItem = imID + i;

            float* flInterpolatedLabelAlpha = ImGui::GetStateStorage()->GetFloatRef(imIDItem, 0.f);
            float flTargetAlpha_Selected = bIsSelectedItem ? 1.0f : 0.5f;

            *flInterpolatedLabelAlpha = ImLerp(*flInterpolatedLabelAlpha, flTargetAlpha_Selected, ImGui::GetIO().DeltaTime * 4.f);
            *flInterpolatedLabelAlpha = ImClamp(*flInterpolatedLabelAlpha, 0.0f, 1.0f);

            imLabelColor.Value.w = *flInterpolatedLabelAlpha;

            if (ApraGui::ComboButton(items[i], imLabelColor, v[i]))
            {
                v[i] = !v[i];
                bValueChanged = true;
            }
        }

        ApraGui::EndCombo();
    }

    /*
    if ( BeginCombo( szLabel, szItems[ *iSelectedItem ], bDrawLabel, bColorPickerInline ) )
    {
        for ( int i = 0; i < iItemsCount; i++ )
        {
            bool bIsSelectedItem = ( *iSelectedItem == i );
            ImColor imLabelColor = bIsSelectedItem ? ImColor( 255, 255, 255, 255 ) : ImColor( 255, 255, 255, 100 );

            ImGuiID imIDItem = imID + i;

            float* flInterpolatedLabelAlpha = ImGui::GetStateStorage( )->GetFloatRef( imIDItem, 0.f );
            float flTargetAlpha_Selected = bIsSelectedItem ? 1.0f : 0.5f;

            *flInterpolatedLabelAlpha = ImLerp( *flInterpolatedLabelAlpha, flTargetAlpha_Selected, ImGui::GetIO( ).DeltaTime * obj.flAnimationTimeItem_Selected );
            *flInterpolatedLabelAlpha = ImClamp( *flInterpolatedLabelAlpha, 0.0f, 1.0f );

            imLabelColor.Value.w = *flInterpolatedLabelAlpha;

            if ( ComboButton( szItems[ i ], imLabelColor, bIsSelectedItem ) )
            {
                *iSelectedItem = i;
            }
        }
        EndCombo( );
    }
    */

    return bValueChanged;
}

bool ApraGui::Combo(const char* szLabel, int* iSelectedItem, const char* szItems[], int iItemsCount, int iCustomID, bool bDrawLabel, bool bColorPickerInline)
{
    static ComboObj_t obj;

    ImGuiID imID = GetID(szLabel) + iCustomID;

    if (BeginCombo(szLabel, szItems[*iSelectedItem], bDrawLabel, bColorPickerInline))
    {
        for (int i = 0; i < iItemsCount; i++)
        {
            bool bIsSelectedItem = (*iSelectedItem == i);
            ImColor imLabelColor = bIsSelectedItem ? ImColor(255, 255, 255, 255) : ImColor(255, 255, 255, 100);

            ImGuiID imIDItem = imID + i;

            float* flInterpolatedLabelAlpha = ImGui::GetStateStorage()->GetFloatRef(imIDItem, 0.f);
            float flTargetAlpha_Selected = bIsSelectedItem ? 1.0f : 0.5f;

            *flInterpolatedLabelAlpha = ImLerp(*flInterpolatedLabelAlpha, flTargetAlpha_Selected, ImGui::GetIO().DeltaTime * obj.flAnimationTimeItem_Selected);
            *flInterpolatedLabelAlpha = ImClamp(*flInterpolatedLabelAlpha, 0.0f, 1.0f);

            imLabelColor.Value.w = *flInterpolatedLabelAlpha;

            if (ComboButton(szItems[i], imLabelColor, bIsSelectedItem))
            {
                *iSelectedItem = i;
            }
        }
        EndCombo();
    }

    return true;
}

bool ApraGui::EndCombo()
{
    ImGuiContext& g = *GImGui;
    EndPopup();

    return true;
}

void ApraGui::Label(const char* szFmt, ImColor cColor)
{
    ImGui::TextColored(cColor, szFmt);
}

void ApraGui::RenderArrow(ImGuiWindow* imWindow, ImVec2 imPos, ImColor imCol, float flAngle, float flScale)
{
    const float h = imWindow->DrawList->_Data->FontSize * 1.00f;
    const float r = h * 0.40f * flScale;
    const ImVec2 center = imPos + ImVec2(h * 0.50f, h * 0.50f * flScale);

    ImVec2 a = ImVec2(cosf(flAngle), sinf(flAngle)) * r;
    ImVec2 b = ImVec2(cosf(flAngle + IM_PI * 2.0f / 3.0f), sinf(flAngle + IM_PI * 2.0f / 3.0f)) * r;
    ImVec2 c = ImVec2(cosf(flAngle - IM_PI * 2.0f / 3.0f), sinf(flAngle - IM_PI * 2.0f / 3.0f)) * r;

    imWindow->DrawList->AddTriangleFilled(center + a, center + b, center + c, imCol);
}

struct ButtonObj_t
{
    // State: Unhovered
    ImColor imBackgroundColor_Unhovered = ImColor(255, 255, 255, 20);

    // State: Hovered
    ImColor   imBackgroundColor_Hovered = ImColor(255, 255, 255, 20);

    // State: Pressed
    ImColor   imBackgroundColor_Pressed = ImColor(255, 255, 255, 10);
};

bool ApraGui::Button(const char* szLabel, ImVec2 imSize, ImColor imLabelColor, int iCustomID)
{
    static ButtonObj_t obj;

    ImGuiWindow* imWindow = GetCurrentWindow();
    if (imWindow->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID imID = imWindow->GetID(szLabel) + iCustomID;
    const ImVec2 imLabelSize = CalcTextSize(szLabel, NULL, true);

    ImVec2 imPos = imWindow->DC.CursorPos;

    ImVec2 size = CalcItemSize(imSize, imLabelSize.x + style.FramePadding.x * 2.0f, imLabelSize.y + style.FramePadding.y * 2.0f);

    const ImRect imTotalBB(imPos, imPos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(imTotalBB, imID))
        return false;

    bool bHovered, bHeld;
    bool bPressed = ButtonBehavior(imTotalBB, imID, &bHovered, &bHeld, 0);

    // Render
    ImColor imButtonColor = obj.imBackgroundColor_Unhovered;

    if (bHovered && !bPressed && !bHeld)
    {
        imButtonColor = obj.imBackgroundColor_Hovered;
    }

    if (bPressed || bHeld)
    {
        imButtonColor = obj.imBackgroundColor_Pressed;
    }

    GetWindowDrawList()->AddRect(imTotalBB.Min, imTotalBB.Max, ImColor(255, 255, 255, 20), style.FrameRounding);

    RenderFrame(imTotalBB.Min, imTotalBB.Max, imButtonColor, true, style.FrameRounding);

    ImVec2 imLabelTextPos = ImLerp(imTotalBB.Min + style.FramePadding, imTotalBB.Max - style.FramePadding - imLabelSize, style.ButtonTextAlign);
    imWindow->DrawList->AddText(imLabelTextPos, imLabelColor, szLabel);

    return bPressed;
}

struct ChildObj_t
{
    int                       iRounding = 5.f;

    ImColor           imBackgroundColor = ImColor(0, 0, 0, 25);
    ImColor       imTextBackgroundColor = ImColor(0, 0, 0, 15);
    ImColor     imBackgroundBorderColor = ImColor(255, 255, 255, 15);
};

bool ApraGui::Child(const char* szLabel, ImVec2* imSize, bool bDrawLabel)
{
    static ChildObj_t obj;

    ImGuiWindow* imWindow = GetCurrentWindow();

    ImVec2 imCursorPos = imWindow->DC.CursorPos;

    ImVec2 imActualSize;


    if (!imSize)
        imActualSize = ImGui::GetContentRegionAvail();
    else
        imActualSize = *imSize;

    ImRect imRectBackground = ImRect(imCursorPos, imCursorPos + imActualSize);

    imWindow->DrawList->AddRectFilled(imRectBackground.Min, imRectBackground.Max, obj.imBackgroundColor, obj.iRounding);
    imWindow->DrawList->AddRect(imRectBackground.Min, imRectBackground.Max, obj.imBackgroundBorderColor, obj.iRounding, 0, 1.f);

    if (bDrawLabel)
    {
        ImRect imRectHeader(imCursorPos, ImVec2(imRectBackground.Max.x, imCursorPos.y + 28));

        imWindow->DrawList->AddRectFilled(imRectHeader.Min, imRectHeader.Max, obj.imTextBackgroundColor, obj.iRounding, ImDrawFlags_RoundCornersTop);

        // This is the only exception from not using: LABEL_SELECTED_COLOR, LABEL_HOVERED_COLOR or LABEL_UNHOVERED_COLOR
        ImGui::TextColored(ImColor(255, 255, 255, 200), szLabel);

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
    }

    return false;
}


bool ApraGui::ChildEnd()
{
    ImGui::EndChild();

    return true;
}

bool ApraGui::ListBox(const char* szLabel, ImVec2 imSize, int iItemsCount)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiID id = GetID(szLabel);

    //ImGuiWindow* child_window = GetCurrentWindow();
    //const float scrollMaxY = child_window->ScrollMax.y;

    //if (scrollMaxY > 0.0f)
    //{
    //    const float scrollbarWidth = 6.0f;
    //    const float scrollbarPadding = 2.0f;
    //    const float minScrollbarHeight = 20.0f;

    //    // Scrollbar geometry
    //    ImRect scrollbarRect(
    //        ImVec2(frame_bb.Max.x - scrollbarPadding - scrollbarWidth, frame_bb.Min.y),
    //        ImVec2(frame_bb.Max.x - scrollbarPadding, frame_bb.Max.y)
    //    );

    //    // Calculate thumb size and position
    //    float scrollbarHeight = scrollbarRect.GetHeight();
    //    float thumbHeight = ImMax(minScrollbarHeight,
    //        scrollbarHeight * (scrollbarHeight / (scrollbarHeight + scrollMaxY)));

    //    float thumbPos = ImClamp((child_window->Scroll.y / scrollMaxY) * (scrollbarHeight - thumbHeight),
    //        0.0f, scrollbarHeight - thumbHeight);

    //    ImRect thumbRect(
    //        ImVec2(scrollbarRect.Min.x, scrollbarRect.Min.y + thumbPos),
    //        ImVec2(scrollbarRect.Max.x, scrollbarRect.Min.y + thumbPos + thumbHeight)
    //        );

    //    // Register scrollbar for interaction
    //    ImGuiID scrollbar_id = window->GetID("##SCROLLBAR");
    //    ItemAdd(scrollbarRect, scrollbar_id);
    //    ItemAdd(thumbRect, scrollbar_id);

    //    // Handle interaction states
    //    bool hovered, held;
    //    bool pressed = ButtonBehavior(thumbRect, scrollbar_id, &hovered, &held);

    //    // Visual feedback
    //    ImU32 thumbColor = GetColorU32(held ? ImGuiCol_ScrollbarGrabActive :
    //        hovered ? ImGuiCol_ScrollbarGrabHovered :
    //        ImGuiCol_ScrollbarGrab);

    //    draw_list->AddRectFilled(thumbRect.Min, thumbRect.Max, thumbColor, 3.0f);

    //    // Handle dragging
    //    if (held && IsMouseDragging(ImGuiMouseButton_Left))
    //    {
    //        float scrollRatio = (g.IO.MousePos.y - thumbRect.GetHeight() * 0.5f - scrollbarRect.Min.y) /
    //            (scrollbarRect.GetHeight() - thumbRect.GetHeight());
    //        child_window->Scroll.y = ImClamp(scrollRatio * scrollMaxY, 0.0f, scrollMaxY);
    //    }
    //}

    return true;
}


bool ApraGui::ListBoxEnd()
{
    ApraGui::ChildEnd();

    return true;
}
bool ApraGui::Selectable(const char* szLabel, bool bSelected, ImVec2 imSize, Color colForegroundColorOverride)
{
    ImGuiWindow* imWindow = ImGui::GetCurrentWindow();
    if (imWindow->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& imStyle = g.Style;

    ImVec2 imLabelSize = CalcTextSize(szLabel, NULL, true);
    ImVec2 imAvailRegion = ImGui::GetContentRegionAvail();

    ImVec2 imItemSize(
        imSize.x > 0.0f ? imSize.x : imAvailRegion.x,
        imSize.y > 0.0f ? imSize.y : imLabelSize.y
    );

    ImVec2 imPos = imWindow->DC.CursorPos;
    ItemSize(imItemSize, 0.0f);

    ImRect imBB(imPos, imPos + imItemSize);
    if (!ItemAdd(imBB, imWindow->GetID(szLabel)))
        return false;

    bool bHovered, bHeld;
    bool bPressed = ButtonBehavior(imBB, imWindow->GetID(szLabel), &bHovered, &bHeld);

    ImDrawList* imDrawList = imWindow->DrawList;
    ImVec2 imTextPos = imBB.Min + imStyle.FramePadding;

    std::string strNewLabel(szLabel);
    size_t pos = strNewLabel.find("##");

    if (pos != std::string::npos) {
        strNewLabel = strNewLabel.substr(0, pos);
    }

    ImGuiID id = imWindow->GetID(szLabel);

    float* selectProgress = ImGui::GetStateStorage()->GetFloatRef(id, 0.0f);
    float* hoverProgress = ImGui::GetStateStorage()->GetFloatRef(id + 1, 0.0f);
    float* colorTransition = ImGui::GetStateStorage()->GetFloatRef(id + 2, 0.0f); // Smooth color transition

    // Animate selection and hover
    *selectProgress = ImSaturate(*selectProgress + g.IO.DeltaTime * (bSelected ? 15.0f : -15.0f));
    *hoverProgress = ImSaturate(*hoverProgress + g.IO.DeltaTime * (bHovered && !bSelected ? 15.0f : -15.0f));

    // Apply smooth movement
    float xOffset = ImLerp(0.0f, 3.3f, *selectProgress) + ImLerp(0.0f, 1.5f, *hoverProgress);
    imTextPos.x += xOffset;

    return bPressed;
}

bool ApraGui::ColorPicker(const char* szLabel, ImColor* imColor, int iColorPickersOnLine)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiID id = window->GetID(szLabel);

    ImGuiStorage* storage = window->DC.StateStorage;
    float* flColor = (float*)storage->GetVoidPtr(id);
    if (!flColor)
    {
        flColor = new float[4] { imColor->Value.x, imColor->Value.y, imColor->Value.z, imColor->Value.w };
        storage->SetVoidPtr(id, flColor);
    }

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float square_sz = GetFrameHeight();
    const char* label_display_end = FindRenderedTextEnd(szLabel);
    float w_full = CalcItemWidth();
    g.NextItemData.ClearFlags();

    BeginGroup();
    PushID(szLabel);
    const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
    if (set_current_color_edit_id)
        g.ColorEditCurrentID = window->IDStack.back();

    const ImGuiColorEditFlags flags_untouched = 0;

    ColorEditOptionsPopup(flColor, 0);

    const bool alpha = (0 & ImGuiColorEditFlags_NoAlpha) == 0;
    const bool hdr = (0 & ImGuiColorEditFlags_HDR) != 0;
    const int components = alpha ? 4 : 3;
    const float w_button = 14;
    const float w_inputs = ImMax(w_full - w_button, 1.0f);
    w_full = w_inputs + w_button;

    float f[4] = { flColor[0], flColor[1], flColor[2], alpha ? flColor[3] : 1.0f };

    bool value_changed = false;

    const ImVec2 pos = window->DC.CursorPos;
    const float inputs_offset_x = ImGui::GetContentRegionAvail().x - ((w_button - ImGui::GetStyle().FramePadding.x) * (iColorPickersOnLine * 2));
    window->DC.CursorPos.x = pos.x + inputs_offset_x;
    //window->DC.CursorPos.y += 2; // was offset a little

    const ImVec4 col_v4(flColor[0], flColor[1], flColor[2], flColor[3]);
    if (ApraGui::ColorButton("##ColorButton", col_v4, 0, ImVec2(w_button, w_button)))
    {
        g.ColorPickerRef = col_v4;
        OpenPopup("picker");
        SetNextWindowPos(g.LastItemData.Rect.GetBL() + ImVec2(0.0f, style.ItemSpacing.y));
    }

    ImGui::SetNextWindowBgAlpha(1.f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(55, 55, 55).Value);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3.f);

    if (BeginPopup("picker", ImGuiWindowFlags_NoMove))
    {
        if (g.CurrentWindow->BeginCount == 1)
        {
            ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
            ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
            SetNextItemWidth(square_sz * 12.0f);
            value_changed |= ApraGui::ColorPicker4("##picker", flColor, 0, &g.ColorPickerRef.x);
        }
        EndPopup();
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    if (value_changed)
    {
        imColor->Value = ImVec4(flColor[0], flColor[1], flColor[2], alpha ? flColor[3] : 1.0f);
    }

    if (set_current_color_edit_id)
        g.ColorEditCurrentID = 0;

    PopID();
    EndGroup();

    return value_changed;
}

bool ApraGui::ColorButton(const char* szDescId, const ImVec4& imCol, ImGuiColorEditFlags imColorEditFlags, const ImVec2& imSize)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(szDescId);
    const float default_size = GetFrameHeight();
    const ImVec2 size(imSize.x == 0.0f ? default_size : imSize.x, imSize.y == 0.0f ? default_size : imSize.y);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(bb, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    if (imColorEditFlags & ImGuiColorEditFlags_NoAlpha)
        imColorEditFlags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

    ImVec4 col_rgb = imCol;
    if (imColorEditFlags & ImGuiColorEditFlags_InputHSV)
        ColorConvertHSVtoRGB(col_rgb.x, col_rgb.y, col_rgb.z, col_rgb.x, col_rgb.y, col_rgb.z);

    float grid_step = ImMin(size.x, size.y) / 2.99f;
    float rounding = 2;
    ImRect bb_inner = bb;
    float off = 0.0f;
    if ((imColorEditFlags & ImGuiColorEditFlags_NoBorder) == 0)
    {
        off = -0.75f; // The border (using Col_FrameBg) tends to look off when color is near-opaque and rounding is enabled. This offset seemed like a good middle ground to reduce those artifacts.
        bb_inner.Expand(off);
    }

    RenderColorRectWithAlphaCheckerboard(window->DrawList, bb_inner.Min, bb_inner.Max, GetColorU32(imCol), grid_step, ImVec2(off, off), rounding);
    window->DrawList->AddRectFilled(bb_inner.Min, bb_inner.Max, GetColorU32(imCol), rounding);

    return pressed;
}

static void RenderArrowsForVerticalBar(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, float bar_w, float alpha)
{
    ImU32 alpha8 = IM_F32_TO_INT8_SAT(alpha);
    ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x + 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Right, IM_COL32(0, 0, 0, alpha8));
    ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x, pos.y), half_sz, ImGuiDir_Right, IM_COL32(255, 255, 255, alpha8));
    ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x - 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Left, IM_COL32(0, 0, 0, alpha8));
    ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x, pos.y), half_sz, ImGuiDir_Left, IM_COL32(255, 255, 255, alpha8));
}

bool ApraGui::ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImDrawList* draw_list = window->DrawList;
    ImGuiStyle& style = g.Style;
    ImGuiIO& io = g.IO;

    const float width = CalcItemWidth();
    g.NextItemData.ClearFlags();

    PushID(label);
    const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
    if (set_current_color_edit_id)
        g.ColorEditCurrentID = window->IDStack.back();
    BeginGroup();

    if (!(flags & ImGuiColorEditFlags_NoSidePreview))
        flags |= ImGuiColorEditFlags_NoSmallPreview;

    // Context menu: display and store options.
    if (!(flags & ImGuiColorEditFlags_NoOptions))
        ColorPickerOptionsPopup(col, flags);

    // Read stored options
    if (!(flags & ImGuiColorEditFlags_PickerMask_))
        flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_PickerMask_;
    if (!(flags & ImGuiColorEditFlags_InputMask_))
        flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_InputMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_InputMask_;
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_PickerMask_)); // Check that only 1 is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));  // Check that only 1 is selected
    if (!(flags & ImGuiColorEditFlags_NoOptions))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);

    // Setup
    int components = (flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4;
    bool alpha_bar = true/*(flags & ImGuiColorEditFlags_AlphaBar) && !(flags & ImGuiColorEditFlags_NoAlpha)*/;
    ImVec2 picker_pos = window->DC.CursorPos;
    float square_sz = GetFrameHeight();
    float bars_width = square_sz; // Arbitrary smallish width of Hue/Alpha picking bars
    float sv_picker_size = ImMax(bars_width * 1, width - (alpha_bar ? 2 : 1) * (bars_width + style.ItemInnerSpacing.x)); // Saturation/Value picking box
    float bar0_pos_x = picker_pos.x + sv_picker_size + style.ItemInnerSpacing.x;
    float bar1_pos_x = bar0_pos_x + bars_width + style.ItemInnerSpacing.x;

    float backup_initial_col[4];
    memcpy(backup_initial_col, col, components * sizeof(float));

    float wheel_thickness = sv_picker_size * 0.08f;
    float wheel_r_outer = sv_picker_size * 0.50f;
    float wheel_r_inner = wheel_r_outer - wheel_thickness;
    ImVec2 wheel_center(picker_pos.x + (sv_picker_size + bars_width) * 0.5f, picker_pos.y + sv_picker_size * 0.5f);

    // Note: the triangle is displayed rotated with triangle_pa pointing to Hue, but most coordinates stays unrotated for logic.
    float triangle_r = wheel_r_inner - (int)(sv_picker_size * 0.027f);
    ImVec2 triangle_pa = ImVec2(triangle_r, 0.0f); // Hue point.
    ImVec2 triangle_pb = ImVec2(triangle_r * -0.5f, triangle_r * -0.866025f); // Black point.
    ImVec2 triangle_pc = ImVec2(triangle_r * -0.5f, triangle_r * +0.866025f); // White point.

    float H = col[0], S = col[1], V = col[2];
    float R = col[0], G = col[1], B = col[2];
    if (flags & ImGuiColorEditFlags_InputRGB)
    {
        // Hue is lost when converting from grayscale rgb (saturation=0). Restore it.
        ColorConvertRGBtoHSV(R, G, B, H, S, V);
        //ColorEditRestoreHS(col, &H, &S, &V);
    }
    else if (flags & ImGuiColorEditFlags_InputHSV)
    {
        ColorConvertHSVtoRGB(H, S, V, R, G, B);
    }

    bool value_changed = false, value_changed_h = false, value_changed_sv = false;

    PushItemFlag(ImGuiItemFlags_NoNav, true);
    if (flags & ImGuiColorEditFlags_PickerHueBar)
    {
        // SV rectangle logic
        InvisibleButton("sv", ImVec2(sv_picker_size, sv_picker_size));
        if (IsItemActive())
        {
            S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size - 1));
            V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            //ColorEditRestoreH(col, &H); // Greatly reduces hue jitter and reset to 0 when hue == 255 and color is rapidly modified using SV square.
            value_changed = value_changed_sv = true;
        }
        if (!(flags & ImGuiColorEditFlags_NoOptions))
            OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

        // Hue bar logic
        SetCursorScreenPos(ImVec2(bar0_pos_x, picker_pos.y));
        InvisibleButton("hue", ImVec2(bars_width, sv_picker_size));
        if (IsItemActive())
        {
            H = ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            value_changed = value_changed_h = true;
        }
    }

    // Alpha bar logic
    if (alpha_bar)
    {
        SetCursorScreenPos(ImVec2(bar1_pos_x, picker_pos.y));
        InvisibleButton("alpha", ImVec2(bars_width, sv_picker_size));
        if (IsItemActive())
        {
            col[3] = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            value_changed = true;
        }
    }
    PopItemFlag(); // ImGuiItemFlags_NoNav

    if (!(flags & ImGuiColorEditFlags_NoLabel))
    {
        const char* label_display_end = FindRenderedTextEnd(label);
        if (label != label_display_end)
        {
            if ((flags & ImGuiColorEditFlags_NoSidePreview))
                SameLine(0, style.ItemInnerSpacing.x);
            TextEx(label, label_display_end);
        }
    }

    // Convert back color to RGB
    if (value_changed_h || value_changed_sv)
    {
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            ColorConvertHSVtoRGB(H, S, V, col[0], col[1], col[2]);
            g.ColorEditSavedHue = H;
            g.ColorEditSavedSat = S;
            g.ColorEditSavedID = g.ColorEditCurrentID;
            g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0));
        }
        else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            col[0] = H;
            col[1] = S;
            col[2] = V;
        }
    }

    // R,G,B and H,S,V slider color editor
    bool value_changed_fix_hue_wrap = false;
    if ((flags & ImGuiColorEditFlags_NoInputs) == 0)
    {
        PushItemWidth((alpha_bar ? bar1_pos_x : bar0_pos_x) + bars_width - picker_pos.x);
        ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
        ImGuiColorEditFlags sub_flags = (flags & sub_flags_to_forward) | ImGuiColorEditFlags_NoPicker;
        //if (flags & ImGuiColorEditFlags_DisplayRGB || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
        //    if (ColorEdit4("##rgb", col, sub_flags | ImGuiColorEditFlags_DisplayRGB))
        //    {
        //        // FIXME: Hackily differentiating using the DragInt (ActiveId != 0 && !ActiveIdAllowOverlap) vs. using the InputText or DropTarget.
        //        // For the later we don't want to run the hue-wrap canceling code. If you are well versed in HSV picker please provide your input! (See #2050)
        //        value_changed_fix_hue_wrap = (g.ActiveId != 0 && !g.ActiveIdAllowOverlap);
        //        value_changed = true;
        //    }
        //if (flags & ImGuiColorEditFlags_DisplayHSV || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
        //    value_changed |= ColorEdit4("##hsv", col, sub_flags | ImGuiColorEditFlags_DisplayHSV);
        if (flags & ImGuiColorEditFlags_DisplayHex || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
            value_changed |= ColorEdit4("##hex", col, sub_flags | ImGuiColorEditFlags_DisplayHex);
        PopItemWidth();
    }

    // Try to cancel hue wrap (after ColorEdit4 call), if any
    if (value_changed_fix_hue_wrap && (flags & ImGuiColorEditFlags_InputRGB))
    {
        float new_H, new_S, new_V;
        ColorConvertRGBtoHSV(col[0], col[1], col[2], new_H, new_S, new_V);
        if (new_H <= 0 && H > 0)
        {
            if (new_V <= 0 && V != new_V)
                ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, col[0], col[1], col[2]);
            else if (new_S <= 0)
                ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, col[0], col[1], col[2]);
        }
    }

    if (value_changed)
    {
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            R = col[0];
            G = col[1];
            B = col[2];
            ColorConvertRGBtoHSV(R, G, B, H, S, V);
            //ColorEditRestoreHS(col, &H, &S, &V);   // Fix local Hue as display below will use it immediately.
        }
        else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            H = col[0];
            S = col[1];
            V = col[2];
            ColorConvertHSVtoRGB(H, S, V, R, G, B);
        }
    }

    const int style_alpha8 = IM_F32_TO_INT8_SAT(style.Alpha);
    const ImU32 col_black = IM_COL32(0, 0, 0, style_alpha8);
    const ImU32 col_white = IM_COL32(255, 255, 255, style_alpha8);
    const ImU32 col_midgrey = IM_COL32(128, 128, 128, style_alpha8);
    const ImU32 col_hues[6 + 1] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(0,255,255,style_alpha8), IM_COL32(0,0,255,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(255,0,0,style_alpha8) };

    ImVec4 hue_color_f(1, 1, 1, style.Alpha); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
    ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);
    ImU32 user_col32_striped_of_alpha = ColorConvertFloat4ToU32(ImVec4(R, G, B, style.Alpha)); // Important: this is still including the main rendering/style alpha!!

    ImVec2 sv_cursor_pos;

    if (flags & ImGuiColorEditFlags_PickerHueBar)
    {
        // Render SV Square
        draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), col_white, hue_color32, hue_color32, col_white);
        draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), 0, 0, col_black, col_black);
        RenderFrameBorder(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), 0.0f);
        sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * sv_picker_size), picker_pos.x + 2, picker_pos.x + sv_picker_size - 2); // Sneakily prevent the circle to stick out too much
        sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * sv_picker_size), picker_pos.y + 2, picker_pos.y + sv_picker_size - 2);

        // Render Hue Bar
        for (int i = 0; i < 6; ++i)
            draw_list->AddRectFilledMultiColor(ImVec2(bar0_pos_x, picker_pos.y + i * (sv_picker_size / 6)), ImVec2(bar0_pos_x + bars_width, picker_pos.y + (i + 1) * (sv_picker_size / 6)), col_hues[i], col_hues[i], col_hues[i + 1], col_hues[i + 1]);
        float bar0_line_y = IM_ROUND(picker_pos.y + H * sv_picker_size);
        RenderFrameBorder(ImVec2(bar0_pos_x, picker_pos.y), ImVec2(bar0_pos_x + bars_width, picker_pos.y + sv_picker_size), 0.0f);
    }

    // Render cursor/preview circle (clamp S/V within 0..1 range because floating points colors may lead HSV values to be out of range)
    float sv_cursor_rad = value_changed_sv ? wheel_thickness * 0.55f : wheel_thickness * 0.40f;
    int sv_cursor_segments = draw_list->_CalcCircleAutoSegmentCount(sv_cursor_rad); // Lock segment count so the +1 one matches others.
    draw_list->AddCircleFilled(sv_cursor_pos, sv_cursor_rad, user_col32_striped_of_alpha, sv_cursor_segments);
    draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad + 1, col_midgrey, sv_cursor_segments);
    draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad, col_white, sv_cursor_segments);

    // Render alpha bar
    if (alpha_bar)
    {
        float alpha = ImSaturate(col[3]);
        ImRect bar1_bb(bar1_pos_x, picker_pos.y, bar1_pos_x + bars_width, picker_pos.y + sv_picker_size);
        RenderColorRectWithAlphaCheckerboard(draw_list, bar1_bb.Min, bar1_bb.Max, 0, bar1_bb.GetWidth() / 2.0f, ImVec2(0.0f, 0.0f));
        draw_list->AddRectFilledMultiColor(bar1_bb.Min, bar1_bb.Max, user_col32_striped_of_alpha, user_col32_striped_of_alpha, user_col32_striped_of_alpha & ~IM_COL32_A_MASK, user_col32_striped_of_alpha & ~IM_COL32_A_MASK);
        float bar1_line_y = IM_ROUND(picker_pos.y + (1.0f - alpha) * sv_picker_size);
        RenderFrameBorder(bar1_bb.Min, bar1_bb.Max, 0.0f);
    }

    EndGroup();

    if (value_changed && memcmp(backup_initial_col, col, components * sizeof(float)) == 0)
        value_changed = false;
    if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
        MarkItemEdited(g.LastItemData.ID);

    if (set_current_color_edit_id)
        g.ColorEditCurrentID = 0;
    PopID();

    return value_changed;
}

bool ApraGui::SimpleColorPicker(const std::string strName, Color* colColor, int iColorPickersOnLine)
{
    ImColor col = { colColor->rBase(), colColor->gBase(), colColor->bBase(), colColor->aBase() };
    if (ApraGui::ColorPicker(strName.c_str(), &col, iColorPickersOnLine))
    {
        colColor->Set(col.Value.x, col.Value.y, col.Value.z, col.Value.w);
        return true;
    }
    return false;
}

bool ApraGui::IsKeyDown(int user_key_index)
{
    if (user_key_index < 0)
        return false;
    ImGuiContext& g = *GImGui;
    IM_ASSERT(user_key_index >= 0 && user_key_index < IM_ARRAYSIZE(g.IO.KeysDown));
    return g.IO.KeysDown[user_key_index];
}

bool ApraGui::IsKeyDown(ImGuiKey key)
{
    return ApraGui::IsKeyDown(key, ImGuiKeyOwner_Any);
}

bool ApraGui::IsKeyDown(ImGuiKey key, ImGuiID owner_id)
{
    const ImGuiKeyData* key_data = GetKeyData(key);
    if (!key_data->Down)
        return false;
    if (!TestKeyOwner(key, owner_id))
        return false;
    return true;
}

bool ApraGui::IsKeyPressed(ImGuiKey key, bool repeat)
{
    return IsKeyPressed(key, ImGuiKeyOwner_Any, repeat ? ImGuiInputFlags_Repeat : ImGuiInputFlags_None);
}

bool ApraGui::IsKeyPressed(ImGuiKey key, ImGuiID owner_id, ImGuiInputFlags flags)
{
    const ImGuiKeyData* key_data = GetKeyData(key);
    if (!key_data->Down) // In theory this should already be encoded as (DownDuration < 0.0f), but testing this facilitates eating mechanism (until we finish work on key ownership)
        return false;
    const float t = key_data->DownDuration;
    if (t < 0.0f)
        return false;
    IM_ASSERT((flags & ~ImGuiInputFlags_SupportedByIsKeyPressed) == 0); // Passing flags not supported by this function!

    bool pressed = (t == 0.0f);
    if (!pressed && ((flags & ImGuiInputFlags_Repeat) != 0))
    {
        float repeat_delay, repeat_rate;
        GetTypematicRepeatRate(flags, &repeat_delay, &repeat_rate);
        pressed = (t > repeat_delay) && GetKeyPressedAmount(key, repeat_delay, repeat_rate) > 0;
    }
    if (!pressed)
        return false;
    if (!TestKeyOwner(key, owner_id))
        return false;
    return true;
}

bool ApraGui::IsKeyReleased(ImGuiKey key)
{
    return IsKeyReleased(key, ImGuiKeyOwner_Any);
}

bool ApraGui::IsKeyReleased(ImGuiKey key, ImGuiID owner_id)
{
    const ImGuiKeyData* key_data = GetKeyData(key);
    if (key_data->DownDurationPrev < 0.0f || key_data->Down)
        return false;
    if (!TestKeyOwner(key, owner_id))
        return false;
    return true;
}

#define VK_MOUSE_WHEEL_UP   0xE0
#define VK_MOUSE_WHEEL_DOWN 0xE1

ImGuiKey VirtualKeyToImGuiKey(int vk)
{
    switch (vk)
    {
    case VK_TAB: return ImGuiKey_Tab;
    case VK_LEFT: return ImGuiKey_LeftArrow;
    case VK_RIGHT: return ImGuiKey_RightArrow;
    case VK_UP: return ImGuiKey_UpArrow;
    case VK_DOWN: return ImGuiKey_DownArrow;
    case VK_PRIOR: return ImGuiKey_PageUp;
    case VK_NEXT: return ImGuiKey_PageDown;
    case VK_HOME: return ImGuiKey_Home;
    case VK_END: return ImGuiKey_End;
    case VK_INSERT: return ImGuiKey_Insert;
    case VK_DELETE: return ImGuiKey_Delete;
    case VK_BACK: return ImGuiKey_Backspace;
    case VK_SPACE: return ImGuiKey_Space;
    case VK_RETURN: return ImGuiKey_Enter;
    case VK_ESCAPE: return ImGuiKey_Escape;
    case VK_SHIFT: return ImGuiKey_LeftShift;  // Added for Shift key
    case VK_LSHIFT: return ImGuiKey_LeftShift;
    case VK_RSHIFT: return ImGuiKey_RightShift;
    case VK_CONTROL: return ImGuiKey_LeftCtrl;
    case VK_LCONTROL: return ImGuiKey_LeftCtrl;
    case VK_RCONTROL: return ImGuiKey_RightCtrl;
    case VK_MENU: return ImGuiKey_LeftAlt;     // Added for Alt key
    case VK_LMENU: return ImGuiKey_LeftAlt;
    case VK_RMENU: return ImGuiKey_RightAlt;
    case 'A': return ImGuiKey_A;
    case 'B': return ImGuiKey_B;
    case 'C': return ImGuiKey_C;
    case 'D': return ImGuiKey_D;
    case 'E': return ImGuiKey_E;
    case 'F': return ImGuiKey_F;
    case 'G': return ImGuiKey_G;
    case 'H': return ImGuiKey_H;
    case 'I': return ImGuiKey_I;
    case 'J': return ImGuiKey_J;
    case 'K': return ImGuiKey_K;
    case 'L': return ImGuiKey_L;
    case 'M': return ImGuiKey_M;
    case 'N': return ImGuiKey_N;
    case 'O': return ImGuiKey_O;
    case 'P': return ImGuiKey_P;
    case 'Q': return ImGuiKey_Q;
    case 'R': return ImGuiKey_R;
    case 'S': return ImGuiKey_S;
    case 'T': return ImGuiKey_T;
    case 'U': return ImGuiKey_U;
    case 'V': return ImGuiKey_V;
    case 'W': return ImGuiKey_W;
    case 'X': return ImGuiKey_X;
    case 'Y': return ImGuiKey_Y;
    case 'Z': return ImGuiKey_Z;
    case VK_LBUTTON: return ImGuiKey_MouseLeft;
    case VK_RBUTTON: return ImGuiKey_MouseRight;
    case VK_MBUTTON: return ImGuiKey_MouseMiddle;
    case VK_XBUTTON1: return ImGuiKey_MouseX1;
    case VK_XBUTTON2: return ImGuiKey_MouseX2;
    case VK_MOUSE_WHEEL_UP: return ImGuiKey_MouseWheelY;
    case VK_MOUSE_WHEEL_DOWN: return ImGuiKey_MouseWheelY;
    default: return ImGuiKey_NamedKey_BEGIN;
    }
}

bool ApraGui::Keybind(const char* str_id, KeyBind_t* keybind)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGui::SameLine(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(KeyNames[keybind->m_iKey]).x - ImGui::GetStyle().FramePadding.x);

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(str_id);
    ImGuiIO* io = &ImGui::GetIO();

    ImVec2 label_size = ImGui::CalcTextSize(KeyNames[keybind->m_iKey]);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + label_size);
    const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(window->Pos.x + window->Size.x - window->DC.CursorPos.x, label_size.y));

    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
        return false;

    const bool hovered = ImGui::IsItemHovered();
    const bool edit_requested = hovered && io->MouseClicked[0];
    const bool style_requested = hovered && io->MouseClicked[1];

    if (edit_requested) {
        if (g.ActiveId != id) {
            memset(io->MouseDown, 0, sizeof(io->MouseDown));
            memset(io->KeysDown, 0, sizeof(io->KeysDown));
            keybind->m_iKey = 0;
        }
        ImGui::SetActiveID(id, window);
        ImGui::FocusWindow(window);
    }
    else if (!hovered && io->MouseClicked[0] && g.ActiveId == id) {
        ImGui::ClearActiveID();
    }

    bool value_changed = false;
    int key = keybind->m_iKey;

    if (g.ActiveId == id) {
        for (int i = 0; i < 5; i++) {
            if (ImGui::IsMouseClicked(i)) {
                switch (i) {
                case 0: key = VK_LBUTTON; break;
                case 1: key = VK_RBUTTON; break;
                case 2: key = VK_MBUTTON; break;
                case 3: key = VK_XBUTTON1; break;
                case 4: key = VK_XBUTTON2; break;
                }
                value_changed = true;
                ImGui::ClearActiveID();
            }
        }

        if (!value_changed && io->MouseWheel != 0.0f) {
            key = io->MouseWheel > 0.0f ? VK_MOUSE_WHEEL_UP : VK_MOUSE_WHEEL_DOWN;
            value_changed = true;
            ImGui::ClearActiveID();
        }

        if (!value_changed) {
            if (ImGui::IsKeyPressed(ImGuiKey_LeftShift)) {
                key = VK_LSHIFT;
                value_changed = true;
                ImGui::ClearActiveID();
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_RightShift)) {
                key = VK_RSHIFT;
                value_changed = true;
                ImGui::ClearActiveID();
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_LeftAlt)) {
                key = VK_LMENU;
                value_changed = true;
                ImGui::ClearActiveID();
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_RightAlt)) {
                key = VK_RMENU;
                value_changed = true;
                ImGui::ClearActiveID();
            }
            else {
                for (int i = VK_BACK; i <= VK_RMENU; i++) {
                    ImGuiKey key2 = VirtualKeyToImGuiKey(i);
                    if (!ImGui::IsNamedKey(key2))
                        continue;
                    if (ImGui::IsKeyPressed(key2)) {
                        key = i;
                        value_changed = true;
                        ImGui::ClearActiveID();
                    }
                }
            }
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            keybind->m_iKey = 0;
            ImGui::ClearActiveID();
        }
        else {
            keybind->m_iKey = key;
        }
    }

    const bool bLeftClicked = hovered && io->MouseClicked[1];
    if (bLeftClicked && g.ActiveId != id && keybind->m_iKey != 0) {
        ImGui::OpenPopup(str_id);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImColor(30, 30, 30, 255).Value);
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(255, 255, 255, 25).Value);

    if (ImGui::BeginPopup(str_id)) {
        if (ApraGui::Button("Hold", ImVec2(70, 15)))
            keybind->m_iMode = 0;
        if (ApraGui::Button("Toggle", ImVec2(70, 15)))
            keybind->m_iMode = 1;
        if (ApraGui::Button("Always on", ImVec2(70, 15)))
            keybind->m_iMode = 2;
        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);

    char buf_display[64] = "[-]";
    std::string active = std::vformat("[{}]", std::make_format_args(KeyNames[keybind->m_iKey]));
    if (keybind->m_iKey != 0 && g.ActiveId != id)
        strcpy_s(buf_display, active.c_str());
    else if (g.ActiveId == id)
        strcpy_s(buf_display, "[-]");

    window->DrawList->AddText(frame_bb.Min, g.ActiveId == id ? ImColor(160, 125, 255, 255) : ImColor(255, 255, 255, 50), buf_display);

    return value_changed;
}