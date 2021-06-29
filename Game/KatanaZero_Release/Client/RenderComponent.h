#pragma once
#include "Component.h"
#include <optional>
#include <map>
#include <deque>
#include "Record.h"

struct AfterImg
{
	matrix PastWorld;
	std::wstring ObjKey{};
	std::wstring StateKey{};
	uint8_t ID{};
	float T = 0.f;
	// ���� ���� ���� ���
	float DeltaCoefft = 1.f;
	D3DXCOLOR _Color = D3DCOLOR_ARGB(120, 0, 255, 255);
	D3DXCOLOR _GoalColor = D3DCOLOR_ARGB(0,255, 0, 255);
};

class RenderComponent : public Component
{
public :
	using NotifyType =std::map<uint8_t, std::function<void()>>;
	
	Record _Record;

	void RecordRender();
	void Render()override;
	void Update()override;
	// Y ������ ������Ʈ�� ���ϴ�
	std::optional<float>CalcY();
	//
	struct Info
	{
	friend class RenderComponent;
	private:
		
	public :
		inline uint8_t GetCurFrame() { return CurrentFrame; };

		bool bLoop = false;
		float T = 0.f;
		std::wstring ObjectKey;
		std::wstring StateKey;
		uint8_t CurrentFrame = 0;
		//[begin,end)
		uint8_t End = 0;
		float AnimSpeed = 1.f;
		vec2 SrcScale{ 1.f,1.f };
		D3DCOLOR _Color = D3DCOLOR_ARGB(255, 255, 255, 255);
		LAYER::ELAYER _Layer;
		// �ִϸ��̼��� �ش� �������϶� �뺸
		// ������Ʈ�� Owner�� ��ȿ���� �ʴٸ� ȣ����� ����.
		typename RenderComponent::NotifyType _Nofify;
	};

	void Anim(const bool SamePlayAgain/*����� ���ϴ� �ִϸ��̼��̶�
						���� ������� �ִϸ��̼��� ���Ƶ� �ٽ� ó������ ���?*/,const bool bLoop,
		const std::wstring& StateKey, 
		const uint8_t End, 
		const float AnimSpeed,
		typename RenderComponent::NotifyType _Nofify = {},
		const  D3DCOLOR _Color = D3DCOLOR_ARGB(255, 255, 255, 255) ,
		const float StartT = 0.f,
		vec2 SrcScale = vec2{ -1.f,-1.f },
		const std::wstring& ObjKey=L"",
		LAYER::ELAYER _Layer = LAYER::ELAYER::ENONE);

	void AfterImgOn();
	void AfterImgOff();
	void AfterImgPush(matrix MWorld);

	static inline D3DXCOLOR ReplayColor = (255, 125, 125, 125);

	D3DXCOLOR PlayStartColor = D3DCOLOR_ARGB(125, 255, 0, 0);
	D3DXCOLOR PlayGoalColor = D3DCOLOR_ARGB(0, 255, 255, 255);
	D3DXCOLOR SlowStartColor = D3DCOLOR_ARGB(255, 0, 255, 255);
	D3DXCOLOR SlowGoalColor= D3DCOLOR_ARGB(0, 255, 0, 255);
	D3DXCOLOR SlowColor = D3DCOLOR_ARGB(255, 0, 255, 255);
	D3DXCOLOR GetCurGameStateColor();

	// ������� �ܻ��ֱⰡ ª�����ϴ�.
	float AfterDeltaCoefft = 0.3f;
	// ������� �ܻ��� ���������ϴ�.
	float SlowDeltaCoefft = 0.4f;

	float SlowAfterImgPushDelta = 0.05f;
	float NormalAfterImgPushDelta = 0.0125f;
	
	bool bFixedScale = false;
	vec3 FixedScale = { 1,1,1 };
	bool bSmokeAffected = true;
	bool bSlowRender = true;
	std::function<void()> _RenderAfterEvent;
	std::vector<AfterImg> _AfterImgVec;
	Info _Info{};
	vec3 PositionCorrection{ 0.f,0.f,0.f };
	int32_t AnimDir = 1.f;
	bool bRender{ true };

	matrix CurRenderWorld;
private:
	float SlowAfterImgPushCurrentDelta = 0.f;
	float NormalAfterImgPushCurrentDelta = 0.f;
	bool bAfterRender = false;
	bool bRenderControl = false; 
	void AfterCheckingPush( matrix MWorld);
};

