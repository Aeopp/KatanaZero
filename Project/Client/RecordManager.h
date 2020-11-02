#pragma once
#include "singleton_interface.h"

class RecordManager :
    public singleton_interface<RecordManager>
{
public :
    friend class Camera;
    void TimeCameraPosPush( vec3 CameraPos);
     // �ݵ�� ���� ����� ȣ�����ּ���.
    void DeathRecord();
    // ReWind �� ������Ʈ ������ �ѹ� ȣ�����ּ���.
    void Update();
    void ReWindStart();

    bool bReWind = false;
    bool bReplay = false;
private:
    // �Ŵ����� �����ϴ� ���� T�� �ش��ϴ� (�״�� �����߾���) ī�޶� ��ǥ�� ��ȯ.
    vec3 GetCameraPos();
    float AtDeathT = 0.f;
    float ReWindT = 0.f;
    std::vector<std::pair<float, vec3>> _TimeCameraPos;
};

