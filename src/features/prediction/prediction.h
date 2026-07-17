#pragma once

class CEnginePrediction
{
private:
    struct PredictionData_t {
        uint32_t m_nTickBase;
        float m_flFrameTime;
        float m_flFrameTime2;
        float m_flIntervalPerSubTick;
        float m_flCurrentTime;
        float m_flCurrentTime2;
        int32_t m_nTickCount;

        bool m_bInPrediction;
        bool m_bFirstPrediction;
        bool m_bHasBeenPredicted;
        bool m_bShouldPredict;

        std::int64_t m_nPrePredictionFlags;
        std::int64_t m_nPostPredictionFlags;

        void* m_pMovementServicesBackup;
        Vector m_vPreAbsOrigin;
        Vector m_vAbsVelocityBackup;
        Vector m_vVelocityBackup;
    };

    int m_nLastSequenceProcessed = 0;
    
    PredictionData_t m_PredictionData;
public:

    void Begin(CUserCmd* pCmd);
    void End(CUserCmd* pCmd);
    void RunClientPeriction(CNetworkGameClient* pNetworkGameClient, int nPredictionReason);
    void RunSimulation(CUserCmd* cmd, int num);

    std::int64_t GetPreFlags() const { return m_PredictionData.m_nPrePredictionFlags; }
    std::int64_t GetPostFlags() const { return m_PredictionData.m_nPostPredictionFlags; }
    PredictionData_t* GetPredictionData() { return &m_PredictionData; };
};

inline auto g_Prediction = std::make_unique<CEnginePrediction>();