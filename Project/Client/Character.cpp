#include "stdafx.h"
#include "Character.h"
#include "ComponentManager.h"
#include "RenderComponent.h"
#include "ObjectManager.h"
#include "PhysicTransformComponent.h"
#include "CollisionComponent.h"
#include "Time.h"
#include "EffectManager.h"

OBJECT_ID::EID Character::GetID()
{
    return OBJECT_ID::CHARACTER;
}

OBJECT_TAG::ETAG Character::GetTag()
{
    return OBJECT_TAG::CHARCTER;
}

std::wstring_view Character::GetName() const&
{ 
    return L"Character"sv;
}

void Character::Initialize() & noexcept
{
    Super::Initialize();

    _TransformComp = ComponentManager::instance().Insert< PhysicTransformComponent>(_This);
    _CollisionComp = ComponentManager::instance().Insert<CollisionComponent>(_This);
    _RenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);
    _PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);
};

void Character::Update()
{
    Super::Update();

    if (_TransformComp->Dir.x < 0.f)
    {
        _RenderComp->AnimDir = -1.f;
    }
    else
    {
        _RenderComp->AnimDir= 1.f;
    }
}
void Character::LateUpdate()
{
    const float dt = Time::instance().Delta();

    Super::LateUpdate();

    if (BloodingDelta < 0.f && bBlooding)
    {
        BloodingDelta = 0.020f;
        BloodingPastDir();
    }

    if (BloodingOverHeadDelta < 0.f && bBloodingOverHead)
    {
        BloodingOverHeadDelta = 0.035f;
        BloodingOverHead();
    }

    BloodingDelta -= dt;
    BloodingOverHeadDelta -= dt;
};

void Character::Move(vec3 Dir, const float AddSpeed)
{
    const float DeltaTime = Time::instance().Delta();

    float MoveFactor = 1.f;
    float AddY = 0.f;
    // 선분에 과 충돌중이라면 선분의 기울기만큼 Y좌표를 통제한다.
    if (_TransformComp->bLineMode)
    {
        MoveFactor = 0.6f;
        float m = _TransformComp->CurrentLineDir.y / _TransformComp->CurrentLineDir.x;
        AddY = Dir.x * ((Speed+ AddSpeed) * MoveFactor) *  m;
    }
    vec3 GoalPos = _TransformComp->Position + (Dir *  (( Speed + AddSpeed)* MoveFactor ) );
    GoalPos.y += AddY;
    _TransformComp->Position = math::lerp(_TransformComp->Position, GoalPos, MoveGoalTime, DeltaTime);
}

void Character::BloodingPastDir()
{
    vec3 Pos = _PhysicComp->Position;
    Pos += _PhysicComp->PastDir*30;
    vec3 RandVec = math::RotationVec(_PhysicComp->PastDir, math::Rand<float>({ -55,55}));
    Pos += RandVec * math::Rand<float>({30,50});
    
    int32_t Blood_ID = math::Rand<int32_t>({ 0,2 });
    std::wstring StateKey = L"spr_blood";
    StateKey += std::to_wstring(Blood_ID);

    EffectManager::instance().EffectPush(L"Blood", StateKey, 6,
        0.1f, 6 * 0.1f + 0.01f, OBJECT_ID::EID::WHITE_BLOOD, Pos, RandVec * math::Rand<float>({50,100}),
        { 3,3,3 }, false, false, false, false, 0.f, 0.f, 255, false,
        0.f, atan2f(RandVec.y, RandVec.x), 0.f, 0);
}

void Character::BloodInit(vec3 Dir)
{
    float BloodInitTime = 0.0f;
    float BloodInitNum = 30;
    float BloodInitDelta = 0.0005f;
    float BloodInitEnd = BloodInitTime + BloodInitDelta * BloodInitNum; 
    Time::instance().TimerRegist(
        BloodInitTime, 
        BloodInitDelta, 
        BloodInitEnd,
       [InitPos = _PhysicComp->Position,Dir]() {

        int32_t Blood_ID = math::Rand<int32_t>({ 0, 2 });
        std::wstring StateKey = L"spr_blood";
        StateKey += std::to_wstring(Blood_ID);

        vec3 Pos = InitPos; 
        Pos += Dir * 50.f;
        vec3 RandDir = math::RotationVec(Dir, math::Rand<float>({ -30,30 }));
        Pos += RandDir * math::Rand<float>({ 50,100 });
        vec3 _Dir = RandDir * 100.f;
        Pos.x += math::Rand<float>({ 0,100});

        EffectManager::instance().EffectPush(L"Blood", StateKey, 6,
            0.07f, 6 * 0.07f + 0.01f, OBJECT_ID::EID::WHITE_BLOOD, Pos, _Dir,
            { 3,3,3 }, false, false, false, false, 0.f, 0.f, 255, false,
            0.f, atan2f(_Dir.y, _Dir.x), 0.f, 0);
        return false; });
}

void Character::BloodingOverHead()
{
    vec3 Dir = { 0,-1.f,0.f };
    vec3 Pos = _PhysicComp->Position ;
    Pos += Dir * math::Rand<float>({ 20,40 });
    Pos.x += math::Rand<float>({ -20,20 });
    Pos.y += math::Rand<float>({ -5,5 });
    Dir = math::RotationVec(Dir, math::Rand<float>({ -60,60}));
    Pos += Dir * math::Rand<float>({ 20,40 });

    int32_t Blood_ID = math::Rand<int32_t>({ 0,2 });
    std::wstring StateKey = L"spr_blood";
    StateKey += std::to_wstring(Blood_ID);

    EffectManager::instance().EffectPush(L"Blood", StateKey, 6,
        0.1f, 6 * 0.1f + 0.01f, OBJECT_ID::EID::WHITE_BLOOD, Pos, Dir * 25.f,
        { 3,3,3 }, false, false, false, false, 0.f, 0.f, 255, false,
        0.f, atan2f(Dir.y, Dir.x), 0.f, 0);
}
