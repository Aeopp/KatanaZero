#pragma once
#include "Enemy.h"
#include <queue>

class Boss :
    public Enemy
{
private:
    enum class State : uint8_t
    {
        None,
        Idle,
        PreDash,
        Dash,
        DashEndGround,
        TakeOutRifle,
        AimRifle,
        PreJump,
        PreJumpState,
        Jump,
        WallIdle,
        WallJump,
        WallJumpLand,
        TakeOutGun,
        Shoot,
        Hurt,
        HurtFly,
        PutBackGun,
        PutBackRifle,
        Recover,
        TelePortIn,
        TelePortOut,
        TelePortInGround,
        TelePortOutGround,
        DodgeRoll,
        Exit,
        DieFly,
        DieLand,
        Dead,
    };
public:
    using Super = Enemy;
    void Initialize() & noexcept override;
    void LateUpdate()override;
    void Update()override;
    void Hit(std::weak_ptr<class object>_Target, math::Collision::HitInfo _CollisionInfo)override;
    void MapHit(math::Collision::HitInfo _CollisionInfo)override;
    bool IsAlive()const& override;
    virtual ~Boss()noexcept = default;
    virtual OBJECT_ID::EID   GetID()override ;
    virtual OBJECT_TAG::ETAG GetTag()override;
    virtual std::wstring_view GetName()const& override;
    virtual void Die()& override;
    void UpdateBossDir();
public :
    inline bool IsAttacking() const&;
private:
    int32_t AnimAimRifleID();
    void AnyState();
    void FSM();
    void Dice();
    void Escape();
    Boss::State _BossState = Boss::State::Recover;
private:
    void PreDash();
    void PreDashState();
    void Idle();
    void IdleState();
    void Dash();
    void DashState();
    void DieState();
    void DashAndGround();
    void DashAndGroundState();
    void TakeOutRifle();
    void TakeOutRifleState();
    void AimRifle();
    void AimRifleState();
    void PreJump();
    void PreJumpState();
    void Jump();
    void JumpState();
    void WallIdle();
    void WallIdleState();
    void WallJump();
    void WallJumpEnd();
    void WallJumpState();
    void WallJumpLandState();
    void WallJumpLand();
    void TakeOutGun();
    void TakeOutGunState();
    void Shoot();
    void ShootState();
    void Hurt();
    void HurtState();
    void HurtFly();
    void HurtFlyState();
   
    void PutBackGun();
    void PutBackGunState();
    void PutBackRifle();
    void PutBackRifleState();
    void RecoverState();
    void Recover();
    void TelePortIn();
    void TelePortInState();
    void TelePortOut();
    void TelePortOutState();
    void TelePortInGround();
    void TelePortInGroundState();
    void TelePortOutGround();
    void TelePortOutGroundState();

    void DoegeRoll();
    void DoegeRollState();
    void Exit();
    void ExitState();
    void DieFly();
    void DieFlyState();
    void DieLand();
    void DieLandState();
    void Dead();
    void DeadState();
private:
    inline bool IsTelePortable()const&;
    inline bool IsFly()const&;
private:
    bool bPreDashEnd{ false };
    bool bDashEnd{ false };
    bool bDashAndGroundEnd{ false };
    bool bTakeOutRifleEnd{ false };
    bool bPreJumpEnd{ false };
    bool bWallIdleEnd{ false };
    bool bWallJumpEnd{ false };
    bool bWallJumpLandEnd{ false };
    bool bTakeOutGunEnd{ false };
    bool bShootEnd{ false };
    bool bAimRifleEnd{ false };
    bool bHurtEnd{ false };
    bool bPutBackGunEnd{ false };
    bool bPutBackRifleEnd{ false };
    bool bRecoverEnd{ false };
    bool bTelePortInEnd{ false };
    bool bTelePortOutEnd{ false };
    bool bTelePortInGroundEnd{ false };
    bool bTelePortOutGroundEnd{ false };
    bool bDodgeRollEnd{ false };
    bool bExitEnd{ false };
    bool bDieLandEnd{ false };
    bool bDeadEnd{ false };

    vec3 DashEndLocation{ 0,0,0 };
    vec3 DashStartLocation{ 0 ,0 , 0 };
    vec3 DashDir{ -1,0,0 };
    vec3 WallJumpDir{ 0,0,0 };
    vec3 WallJumpInitPos{ 0,0,0 };
    vec3 CurWallRideLinePos{ 0,0,0 };

    const float BossStateFloor = 1800.f;
    const float BossStageCenterAxisX = 2000.f;
    
    static inline constexpr float StageStandY = 1747;
    const std::pair<float, float> StageStandRangeX = { 1366 ,2648};
    static inline constexpr float CeilY = { 1230.f };
    const std::pair<float, float> TpXRange = { 1217.f,2717.f };

    float WallRideEffectDelta = 0.f;

    float WallJumpT = 0.f;
    bool bWallJumpFire = false;
    float WallJumpFireDt = 0.25f;
    float CurWallJumpFireDt = WallJumpFireDt;
    float CurWallJumpFireAngle = 0.f;
    static inline constexpr float WallJumpFireAngleDiff = math::PI / 20.f;

    int32_t CurHitCount = 0;
    int32_t CurShootCount = 0;
    int32_t CurTpCount = 5;
    bool bCurAnimLoopShoot = false;

    inline bool IsHurt()const& 
    {
        return _BossState == Boss::State::Hurt || _BossState == Boss::State::Recover ||
             _BossState == Boss::State::HurtFly  || _BossState == Boss::State::Dead 
            || _BossState == Boss::State::DieFly|| _BossState == Boss::State::DieLand;
    };
private:
    static inline constexpr int32_t GrenadeNum = 3;
    std::array< std::shared_ptr<class Grenade>, GrenadeNum > _Grenades;
    bool bEvasion = true;
    int32_t PrevDice = -1;
    std::list<int> PatternQ
    {
        1,0,2,1,2,0
    };
};

inline bool Boss::IsAttacking() const&
{
    return _BossState == Boss::State::Dash;
};

inline bool Boss::IsTelePortable() const&
{
    return _BossState == Boss::State::TelePortOut ||
              _BossState == Boss::State::TelePortOutGround ||
            _BossState == Boss::State::Recover ||
            _BossState == Boss::State::Exit;
}
inline bool Boss::IsFly() const&
{
    return _BossState == Boss::State::WallJump ||
        _BossState == Boss::State::WallIdle ||
        _BossState == Boss::State::Jump;
};
//타격 플라이일때 블러딩 트루
//
//타격 플라이 스테이트에서 땅에 닿았다는 판정 생기면 블러딩 펄스
//
//허트 그라운드 진입시 블러딩 오버헤드 트루


