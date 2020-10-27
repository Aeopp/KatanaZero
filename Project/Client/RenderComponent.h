#pragma once
#include "Component.h"
#include <optional>
#include <map>
#include <deque>

struct AfterImg
{
	static inline const std::array<D3DCOLOR, 20ul> _GradationTable
	{
		D3DCOLOR_ARGB(200,0,255,246),
		D3DCOLOR_ARGB(190,0,210,246),
		D3DCOLOR_ARGB(180,0,170,246),
		D3DCOLOR_ARGB(170,0,130,246),
		D3DCOLOR_ARGB(160,0,90,246),
		D3DCOLOR_ARGB(150,0,50,246),
		D3DCOLOR_ARGB(140,0,10,246),
		D3DCOLOR_ARGB(130,30,0,246),
		D3DCOLOR_ARGB(120,70,0,246),
		D3DCOLOR_ARGB(110,110,0,246),
		D3DCOLOR_ARGB(100,150,0,246),
		D3DCOLOR_ARGB(90,190,0,255),
		D3DCOLOR_ARGB(80,230,0,255),
		D3DCOLOR_ARGB(70,255,0,240),
		D3DCOLOR_ARGB(60,255,0,200),
		D3DCOLOR_ARGB(50,255,0,160),
		D3DCOLOR_ARGB(40,255,0,120),
		D3DCOLOR_ARGB(30,255,0,80),
		D3DCOLOR_ARGB(20,255,0,40),
		D3DCOLOR_ARGB(10,255,0,0)
	};
	static inline uint8_t CurColorIdx = 0;
	matrix PastWorld;
	std::wstring StateKey{};
	uint8_t ID{};
	D3DCOLOR _Color = D3DCOLOR_ARGB(255, 255, 255, 255);
};
class RenderComponent : public Component
{
public :
	using NotifyType =std::map<uint8_t, std::function<void()>>;

	void Render()override;
	void Update()override;
	// Y 기준은 오브젝트의 최하단
	std::optional<float>CalcY();
	//
	struct Info
	{
	friend class RenderComponent;
	private:
		uint8_t CurrentFrame = 0.f;
	public :
		inline uint8_t GetCurFrame() { return CurrentFrame; };

		bool bLoop = false;
		float T = 0.f;
		std::wstring ObjectKey;
		std::wstring StateKey;
		//[begin,end)
		uint8_t End = 0;
		float AnimSpeed = 1.f;
		vec2 SrcScale{ 1.f,1.f };
		D3DCOLOR _Color = D3DCOLOR_ARGB(255, 255, 255, 255);
		LAYER::ELAYER _Layer;
		// 애니메이션이 해당 프레임일때 통보
		// 컴포넌트의 Owner가 유효하지 않다면 호출되지 않음.
		typename RenderComponent::NotifyType _Nofify;
	};

	void Anim(const bool SamePlayAgain/*재생을 원하는 애니메이션이랑
						현재 재생중인 애니메이션이 같아도 다시 처음부터 재생?*/,const bool bLoop,
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

	std::deque<AfterImg> _AfterQ;
	Info _Info{};
	vec3 PositionCorrection{ 0.f,0.f,0.f };
	int32_t AnimDir = 1.f;
	int32_t AfterImgCount = 15ul;
	bool bRender{ true };
private:
	bool bAfterRender = false;
	bool bRenderControl = false; 
};

