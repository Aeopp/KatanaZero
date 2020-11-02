#pragma once
#include "singleton_interface.h"
#include "SceneID.h"
#include <any>

class RecordManager :
    public singleton_interface<RecordManager>
{
public :
    RecordManager();
    friend class Camera;
    int32_t Timing = 0;
    void Update();

    // ReWind �� ������Ʈ ������ �ѹ� ȣ�����ּ���.
    void ReWindStart();
    void ReWindUpdate();
    void ReWindRender();
    void ReWindEnd();
    ////////////////////
    void ReplayStart(ESceneID _SceneID);
    void ReplayEnd();
    void ReplayUpdate();
    void RePlayRender();

    bool bPause = false;

    int32_t RewindSpeed = -2;
    int32_t TimingSpeed = 1;
private:
    ESceneID _AtReplayEndChangeSceneID;
    // �Ŵ����� �����ϴ� ���� T�� �ش��ϴ� (�״�� �����߾���) ī�޶� ��ǥ�� ��ȯ.
                        //Timing  Pos
    std::map<int32_t,vec3>_TimingCameraPos;
    std::vector<std::any> _Anys;
};

