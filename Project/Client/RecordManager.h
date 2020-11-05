#pragma once
#include "singleton_interface.h"
#include "SceneID.h"
#include <any>

class RecordManager :
    public singleton_interface<RecordManager>
{
public:
    RecordManager();
    friend class Camera;
    int32_t Timing = 0;
    bool       bUpdate = false;

    void Update();

    // ReWind 시 업데이트 루프당 한번 호출해주세요.
    void ReWindStart();
    void ReWindUpdate();
    void ReWindRender();
    void ReWindEnd();
    ////////////////////
    void ReplayStart();
    void ReplayEnd();
    void ReplayUpdate();
    void RePlayRender();

    bool bPause = false;
    // bool bRewindEnd = false;

    bool bReplayInit = false;

    int32_t RewindSpeed = -2;
    int32_t TimingSpeed = 1;
    int32_t EndTiming = 0;
private:
    // 매니저가 관리하는 현재 T에 해당하는 (그당시 저장했었던) 카메라 좌표를 반환.
                        //Timing  Pos
    std::map<int32_t, vec3>_TimingCameraPos;
    std::vector<std::any> _Anys;
};