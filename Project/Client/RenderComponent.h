#pragma once
#include "Component.h"
#include <optional>
#include <map>
#include <deque>

struct AfterImg
{
	static inline const std::array<D3DCOLOR, 28ul> _ColorTable
	{
		D3DCOLOR_ARGB(255,15,55,255),
		D3DCOLOR_ARGB(255,0,42,255),
		D3DCOLOR_ARGB(255,30,116,255),
		D3DCOLOR_ARGB(255,38,179,255),
		D3DCOLOR_ARGB(255,40,230,255),
		D3DCOLOR_ARGB(255,43,255,223),
		D3DCOLOR_ARGB(255,43,255,173),
		D3DCOLOR_ARGB(255,48,255,134),
		D3DCOLOR_ARGB(255,50,255,84),
		D3DCOLOR_ARGB(255,80,255,53),
		D3DCOLOR_ARGB(255,128,255,43),
		D3DCOLOR_ARGB(255,190,255,40),
		D3DCOLOR_ARGB(255,233,255,40),
		D3DCOLOR_ARGB(255,255,221,56),
		D3DCOLOR_ARGB(255,255,154,2),
		D3DCOLOR_ARGB(255,255,46,35),
		D3DCOLOR_ARGB(255,255,28,81),
		D3DCOLOR_ARGB(255,255,28,152),
		D3DCOLOR_ARGB(255,255,33,203),
		D3DCOLOR_ARGB(255,251,22,255),
		D3DCOLOR_ARGB(255,197,25,255),
		D3DCOLOR_ARGB(255,143,15,255),
		D3DCOLOR_ARGB(255,84,22,255),
		D3DCOLOR_ARGB(255,25,82,255),
		D3DCOLOR_ARGB(255,40,144,255),
		D3DCOLOR_ARGB(255,63,200,255),
		D3DCOLOR_ARGB(255,66,255,251),
		D3DCOLOR_ARGB(255,53,255,194),
	};
	static inline uint8_t CurColorIdx = 0;
	vec3 PastPosition{ 0.f,0.f,0.f };
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

	std::deque<AfterImg> _AfterQ;
	Info _Info{};
	vec3 PositionCorrection{ 0.f,0.f,0.f };
	int32_t AnimDir = 1.f;
	bool bAfterImg = false;
	bool bRender{ true };
private:
	bool bRenderControl = false; 
};

