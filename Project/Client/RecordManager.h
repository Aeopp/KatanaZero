#pragma once
#include "singleton_interface.h"

class RecordManager :
    public singleton_interface<RecordManager>
{
public :
    friend class Camera;
    void TimeCameraPosPush( vec3 CameraPos);
     // 반드시 죽은 당시의 호출해주세요.
    void DeathRecord();
    // ReWind 시 업데이트 루프당 한번 호출해주세요.
    void Update();
    void ReWindStart();

    bool bReWind = false;
    bool bReplay = false;
private:
    // 매니저가 관리하는 현재 T에 해당하는 (그당시 저장했었던) 카메라 좌표를 반환.
    vec3 GetCameraPos();
    float AtDeathT = 0.f;
    float ReWindT = 0.f;
    std::vector<std::pair<float, vec3>> _TimeCameraPos;
};

