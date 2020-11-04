#pragma once
#include "singleton_interface.h"
#include "CollisionComponent.h"
#include "Record.h"

#include <string_view>
// 50 - 20       / 50 
struct EffectInfo
{
	static int32_t MakeID() { static int32_t ID = 0; return ++ID; };

	// 루프 결정
	bool bLoop = false;
	// 충돌 판정 허용할지 말지
	bool bPhysic = false;
	// 충돌판정시 사용할 크기
	float Width = 0.f;
	float Height = 0.f;
	bool bMapSlide = false;
	bool bHitNotify = false;
	bool bRecord = false;

	std::wstring ObjKey = L"";
	std::wstring StateKey = L"";
	uint8_t DrawID = 0;
	// [Begin,End)
	uint8_t End = 1;
	// 시작 위치
	vec3 Pos = { 0,0,0 };
	// 스케일 렌더와 콜리전 둘다 적용
	vec3 Scale{ 1.f,1.f,1.f };
	// 나아가는 방향
	vec3 Dir = { 0,0,0 };
	// Z에 지속적으로 더해지는 값
	float RotZAcc = 0.f;
	// 회전
	float RotZ = 0.f;
	// 지속시간
	float MaxT = 1.f;
	// T가 MaxT에 도달시 중지
	float T = 0.f;
	// 프레임간의 델타
	float AnimDelta = 0.1f;
	float CurrentDelta = 0.f;

	// 알파를 이펙트 시간에 맞춰 보간할까요 ?? 
	bool bAlphaLerp = false;
	int32_t Alpha = 255;
	OBJECT_ID::EID OBJ_ID = OBJECT_ID::EID::ENONE;

	int32_t _ID = EffectInfo::MakeID();
};

class EffectManager : public singleton_interface<EffectManager>
{
public :
	void Initialize() & noexcept;
	void Render();
	void Update();
	void RecordRender();
	void Clear();
	void RecordClear();
	void HitEvent(EffectInfo& _Effect ,
		math::Collision::HitInfo _HitInfo);

	bool bRender = true;
	D3DXCOLOR SwitchColorFromGameState(D3DXCOLOR _Color);

	D3DXCOLOR SwitchColorFromEffectID(OBJECT_ID::EID _EffectID,D3DXCOLOR _Color);

	void EffectPush(
		const std::wstring_view& _ObjKey,
		const std::wstring_view& _StateKey,
		uint8_t _End,
		float AnimDelta,
		float MaxT,
		OBJECT_ID::EID _EffectID,
		bool bRecord,
		vec3 Pos,
		vec3 Dir={ 0,0,0 },
		vec3 Scale = { 1,1,1 },
		bool bLoop = false,
		bool bPhysic = false,
		bool bMapSlide = false,
		bool bHitNotify = false,
		float Width = 0.f,
		float Height = 0.f,
		int32_t Alpha=255,
		bool bAlphaLerp=false ,
		float T = 0,
		float RotZ = 0,
		float RotZAcc = 0,
		uint8_t _StartID = 0
	);
	std::set<CollisionComponent::ETag> _EffectCollisionTagSet;

	Record _Record;

	inline std::vector<EffectInfo>& EffectsRef(){return _Effects;}
private:
	std::vector<EffectInfo> _Effects;
};

