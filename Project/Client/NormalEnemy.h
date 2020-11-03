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
        Walk,
        Die,
    };
public :
    using Super = Enemy; 
    void Initialize() & noexcept override;
    void LateUpdate()override; 
    void Hit(std::weak_ptr<class object>_Target, math::Collision::HitInfo _CollisionInfo)override;

    virtual void SetUpInitState(float DirX,int32_t StateID)abstract;
    virtual ~NormalEnemy()noexcept = default;
protected:
    static inline std::pair<float, float> IsSamefloorRange{-169.f,+48.f};
    bool IsSamefloor(vec3 TargetPos);
    NormalEnemy::State _EnemyState = NormalEnemy::State::Idle;
    

    std::shared_ptr<class RenderComponent> _FollowRenderComp;
    std::vector<vec3> Paths;
    vec3 GoalPos{ 0.f,0.f,0.f };
    static inline const float PathFindCheckMinDistanceMin = 100.f;
    //static inline const float GoalPosCheckMinDistance = 3.f;
    static inline const float NextPathCheckMinDistance = 5.f;
    static inline const float DoorPathCheckMinDistance = 36.f *3.f;
 
    bool bEnterStairMotionEnd = false;
    bool bLeaveStairMotionEnd = false;
    bool bHurtGroundMotionEnd = false;

    float DetectionRange = 300.f;
    float PursuitRange = 600.f;
    float DelayAfterAttack = 0.4f;

    float AttackRange = 100.f;
    bool IsDetected = false;
    bool bLaziness = false;
    D3DXCOLOR _DetectionRangeColor;
    D3DXCOLOR _DetectionRangeColorGoal;
    float _DetectionRangeColorGoalTime= 1.0f;
    float NarrowRange = 200.f;

    bool IsRangeInnerTarget();
    vec3 ToTargetDir{ -1.f,0.f,0.f };
    vec3 AtTheAttackDir{ -1.f,0.f,0.f };
    vec3 CurMoveMark{ 0.f,0.f,0.f };

    vec3 LeaveDoorLocation{ 0.f,0.f,0.f };
    vec3 ToTarget;
    bool bWalk = false;

    float ToTargetDistance = 0.f;
    
    vec3 ConvertXAxisDir(vec3 Dir);

    virtual void Turn()abstract;
private:

};

