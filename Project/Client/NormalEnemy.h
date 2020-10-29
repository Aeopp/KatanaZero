#pragma once
#include "Enemy.h"
class NormalEnemy :
    public Enemy
{
public:
    enum class State : uint8_t
    {
        Idle,
        Detecting,
        Die,
    };
public :
    using Super = Enemy; 
    void Initialize() & noexcept override;
    void LateUpdate()override; 
    void Hit(std::weak_ptr<class object>_Target, math::Collision::HitInfo _CollisionInfo)override;

protected:
    static inline std::pair<float, float> IsSamefloorRange{-169.f,+48.f};
    bool IsSamefloor(vec3 TargetPos);
    NormalEnemy::State _CurState = NormalEnemy::State::Idle;
    
    float DetectionRange = 300.f;
    float PursuitRange = 600.f;

    float AttackRange = 100.f;
    bool IsDetected = false;
    D3DXCOLOR _DetectionRangeColor;
    D3DXCOLOR _DetectionRangeColorGoal;
    float ColorGoalTime= 1.5f;
    float NarrowRange = 150.f;

    bool IsRangeInnerTarget();
    vec3 ToTargetDir{ -1.f,0.f,0.f };
    vec3 ToTarget;
    bool bWalk = false;

    float ToTargetDistance = 0.f;
    bool IsTurn(vec3 Dir);
    vec3 ConvertMoveDir(vec3 Dir);

    virtual void Turn()abstract;
private:

};

