#pragma once
#include "Enemy.h"
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
        Die,
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
    };
public:
    using Super = Enemy;
    void Initialize() & noexcept override;
    void LateUpdate()override;
    void Update()override;
    void Hit(std::weak_ptr<class object>_Target, math::Collision::HitInfo _CollisionInfo)override;
    void MapHit(math::Collision::HitInfo _CollisionInfo)override;
    virtual ~Boss()noexcept = default;
    virtual OBJECT_ID::EID   GetID()override ;
    virtual OBJECT_TAG::ETAG GetTag()override;
    virtual std::wstring_view GetName()const& override;
    void Die()& override ;
    void UpdateBossDir();
public :
    inline bool IsAttacking() const&;
private:
    int32_t AnimAimRifleID();
    void AnyState();
    void FSM();
    Boss::State _BossState = Boss::State::None;
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
    void Dice();
    void PutBackGun();
    void PutBackGunState();
    void PutBackRifle();
    void PutBackRifleState();
    void RecoverState();
    void Recover();
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

    vec3 DashEndLocation{ 0,0,0 };
    vec3 DashStartLocation{ 0 ,0 , 0 };
    vec3 DashDir{ -1,0,0 };
    vec3 WallJumpDir{ 0,0,0 };
    vec3 WallJumpInitPos{ 0,0,0 };
    vec3 CurWallRideLinePos{ 0,0,0 };

    const float BossStateFloor = 1800.f;
    const float BossStageCenterAxisX = 2000.f;

    float WallRideEffectDelta = 0.f;

    float WallJumpT = 0.f;
    bool bWallJumpFire = false;
    float WallJumpFireDt = 0.25f;
    float CurWallJumpFireDt = WallJumpFireDt;
    float CurWallJumpFireAngle = 0.f;
    static inline constexpr float WallJumpFireAngleDiff = math::PI / 20.f;

    int32_t CurHitCount = 0;
    int32_t CurShootCount = 0;
    bool bCurAnimLoopShoot = false;

    inline bool IsHurt()const& 
    {
        return _BossState == Boss::State::Hurt || _BossState == Boss::State::Recover ||
            _BossState == Boss::State::HurtFly;
    };
private:
    static inline constexpr int32_t GrenadeNum = 3;
    std::array< std::shared_ptr<class Grenade>, GrenadeNum > _Grenades;

    int32_t PrevDice = -1;
};


inline bool Boss::IsAttacking() const&
{
    return _BossState == Boss::State::Dash; 
}
//Ÿ�� �ö����϶� ���� Ʈ��
//
//Ÿ�� �ö��� ������Ʈ���� ���� ��Ҵٴ� ���� ����� ���� �޽�
//
//��Ʈ �׶��� ���Խ� ���� ������� Ʈ��
//
//�ִϸ��̼� ��Ƽ���� �ɾ ������ �޽��� �����

