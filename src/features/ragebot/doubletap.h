#pragma once

namespace Doubletap
{
    extern int g_iShotCount;

    int GetPlayerTick();
    void SetHistoryTick(int nTick);
    void Process();
    void SetupInputHistory();
}
