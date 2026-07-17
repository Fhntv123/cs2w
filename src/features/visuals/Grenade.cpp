#include "../../Precompiled.h"

struct Grenade_t
{
    C_BaseEntity* entity;
    std::vector<Vector> positions;
    float addTime;
    Vector finalPosition;
    bool hasFinalPosition;
};

class CGrenade
{
public:
    bool CheckGrenades(C_BaseEntity* ent)
    {
        for (Grenade_t grenade : vecGrenades)
        {
            if (grenade.entity == ent)
                return false;
        }

        return true;
    }

    void AddGrenade(Grenade_t grenade)
    {
        vecGrenades.push_back(grenade);
    }

    void UpdatePosition(C_BaseEntity* ent, Vector position)
    {
        vecGrenades.at(FindGrenade(ent)).positions.push_back(position);
    }

    void SetFinalPosition(C_BaseEntity* ent, Vector finalPos)
    {
        int index = FindGrenade(ent);
        if (index >= 0 && index < vecGrenades.size())
        {
            vecGrenades.at(index).finalPosition = finalPos;
            vecGrenades.at(index).hasFinalPosition = true;
        }
    }

    Vector GetFinalPosition(C_BaseEntity* ent)
    {
        int index = FindGrenade(ent);
        if (index >= 0 && index < vecGrenades.size() && vecGrenades.at(index).hasFinalPosition)
        {
            return vecGrenades.at(index).finalPosition;
        }
        return Vector(0, 0, 0);
    }

    bool HasFinalPosition(C_BaseEntity* ent)
    {
        int index = FindGrenade(ent);
        return (index >= 0 && index < vecGrenades.size() && vecGrenades.at(index).hasFinalPosition);
    }

    void Draw();

private:
    std::vector<Grenade_t> vecGrenades;

    int FindGrenade(C_BaseEntity* ent)
    {
        for (size_t i = 0; i < vecGrenades.size(); i++)
        {
            if (vecGrenades.at(i).entity == ent)
                return i;
        }
        return -1;
    }
};

const char* NadeToFontLetter(const char* weaponName)
{
    if (strcmp(weaponName, "C_FlashbangProjectile") == 0)        return "i";
    if (strcmp(weaponName, "C_HEGrenadeProjectile") == 0)        return "j";
    if (strcmp(weaponName, "C_SmokeGrenadeProjectile") == 0)     return "k";
    if (strcmp(weaponName, "C_MolotovProjectile") == 0)   return "l";
    if (strcmp(weaponName, "C_DecoyProjectile") == 0)     return "m";
    if (strcmp(weaponName, "C_Inferno") == 0)return "n";
    return "?";
}

Vector GetGrenadeFinalPositionFromPrediction(C_BaseEntity* pEntity)
{
    if (!pEntity || s_vecPredictedGrenades.empty())
        return Vector(0, 0, 0);

    CGameSceneNode* pGameSceneNode = pEntity->m_pGameSceneNode();
    if (!pGameSceneNode)
        return Vector(0, 0, 0);

    Vector entityPos = pGameSceneNode->m_vecAbsOrigin();

    for (const auto& predictedGrenade : s_vecPredictedGrenades)
    {
        if (predictedGrenade.m_GrenadePathPoint.empty())
            continue;

        Vector predictionStartPos = predictedGrenade.m_GrenadePathPoint[0].m_vPos;

        if (predictionStartPos.DistTo(entityPos) < 100.0f)
        {
            return predictedGrenade.m_GrenadePathPoint.back().m_vPos;
        }

        for (const auto& point : predictedGrenade.m_GrenadePathPoint)
        {
            if (point.m_vPos.DistTo(entityPos) < 50.0f)
            {
                return predictedGrenade.m_GrenadePathPoint.back().m_vPos;
            }
        }
    }

    return Vector(0, 0, 0);
}

void GrenadeESP::Draw(EntityObject_t& object)
{
    static CGrenade GrenadeClass;

    C_BaseEntity* pEntity = object.m_pEntity;
    if (!pEntity)
        return;

    C_CSPlayerPawn* pLocal = Globals::m_pLocalPlayerPawn;
    if (!pLocal || !pLocal->m_pGameSceneNode())
        return;

    SchemaClassInfoData_t* grenadeSchema;
    pEntity->GetSchemaClassInfo(&grenadeSchema);
    const char* szClassName = grenadeSchema->szName;

    bool isExplosive = (strcmp(szClassName, "C_HEGrenadeProjectile") == 0);

    CGameSceneNode* pGameSceneNode = pEntity->m_pGameSceneNode();
    if (!pGameSceneNode || !pGameSceneNode->GetSkeletonInstance())
        return;

    Vector nadePos = pGameSceneNode->m_vecAbsOrigin();
    ImVec2 screenPos;

    Vector displayPos = nadePos;

    Vector finalPos = GetGrenadeFinalPositionFromPrediction(pEntity);

    if (finalPos != Vector(0, 0, 0))
    {
        displayPos = finalPos;
        GrenadeClass.SetFinalPosition(pEntity, finalPos);
    }
    else if (GrenadeClass.HasFinalPosition(pEntity))
    {
        displayPos = GrenadeClass.GetFinalPosition(pEntity);
    }
    else
    {
        GrenadeClass.SetFinalPosition(pEntity, nadePos);
    }

    if (!Draw::WorldToScreen(displayPos, screenPos))
        return;

    const char* NadeFontLetter = NadeToFontLetter(szClassName);

    C_BaseGrenade* pGrenade = static_cast<C_BaseGrenade*>(pEntity);
    float detonateTime = pGrenade->m_flDetonateTime();

    if (Config::b(g_Variables.m_WorldVisuals.m_bGrenadeProximityWarning) && NadeFontLetter[0] != '?')
    {
        ImVec2 iconSize = Fonts::GunIcons->CalcTextSizeA(16.0f, FLT_MAX, 0.0f, NadeFontLetter);

        Draw::AddCircleFilled(screenPos, 10.f, Color(0, 0, 0, 255));

        Draw::AddText(Fonts::GunIcons, 16.0f,
            ImVec2(screenPos.x - iconSize.x * 0.5f, screenPos.y - iconSize.y * 0.5f),
            NadeFontLetter, Color(255, 255, 255), DRAW_TEXT_DROPSHADOW, Color(0, 0, 0, 150));
    }

    if (!isExplosive)
        return;

    float baseDamage = pGrenade->m_flDamage();
    float damageRadius = pGrenade->m_DmgRadius();
    Vector localPos = pLocal->m_pGameSceneNode()->m_vecAbsOrigin();
    float distance = displayPos.DistTo(localPos);

    int predictedDamage = CalculateGrenadeDamage(pLocal, pGrenade, distance);

    if (predictedDamage > 0 && Config::b(g_Variables.m_WorldVisuals.m_bGrenadeProximityWarning))
    {
        int currentHP = pLocal->m_iHealth();
        bool isLethal = (predictedDamage >= currentHP);

        Color damageColor;
        if (isLethal)
            damageColor = Color(255, 0, 0);
        else if (predictedDamage >= 80)
            damageColor = Color(255, 50, 50);
        else if (predictedDamage >= 50)
            damageColor = Color(255, 165, 0);
        else if (predictedDamage >= 25)
            damageColor = Color(255, 255, 50);
        else
            damageColor = Color(100, 255, 100);

        char dmgText[32];
        if (isLethal)
            _snprintf_s(dmgText, _countof(dmgText), _TRUNCATE, "LETHAL");
        else
            _snprintf_s(dmgText, _countof(dmgText), _TRUNCATE, "%d", predictedDamage);

        ImVec2 textSize = Fonts::ESP->CalcTextSizeA(13.f, FLT_MAX, 0.0f, dmgText);

        float fontSize = isLethal ? 14.f : 13.f;

        Draw::AddText(Fonts::SegoeUIBold18, fontSize,
            ImVec2(screenPos.x - textSize.x * 0.5f, screenPos.y + 15.f),
            dmgText, damageColor, DRAW_TEXT_DROPSHADOW, Color(0, 0, 0, 180));
    }
}

int GrenadeESP::CalculateGrenadeDamage(C_CSPlayerPawn* pLocal, C_BaseGrenade* pGrenade, float distance)
{
    if (!pLocal || !pGrenade)
        return 0;

    float baseDamage = pGrenade->m_flDamage();
    float damageRadius = pGrenade->m_DmgRadius();

    if (distance > damageRadius)
        return 0;

    float damageRatio = 1.0f - (distance / damageRadius);
    damageRatio = std::clamp(damageRatio, 0.0f, 1.0f);

    float damageFalloff = damageRatio * damageRatio;
    float rawDamage = baseDamage * damageFalloff;
    int armorValue = pLocal->m_ArmorValue();

    if (armorValue > 0)
    {
        const float armorPenetration = 0.5f;
        float damageThroughArmor = rawDamage * armorPenetration;
        float damageToArmor = rawDamage * (1.0f - armorPenetration);

        if (damageToArmor > armorValue)
        {
            rawDamage = damageThroughArmor + (damageToArmor - armorValue);
        }
        else
        {
            rawDamage = damageThroughArmor;
        }
    }

    int finalDamage = static_cast<int>(std::round(rawDamage));
    finalDamage = std::max(finalDamage, 1);
    finalDamage = std::min(finalDamage, static_cast<int>(baseDamage));

    return finalDamage;
}