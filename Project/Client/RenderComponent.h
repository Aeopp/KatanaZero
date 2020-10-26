#pragma once
#include "Component.h"
#include <optional>
#include <map>

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

	Info _Info{};

	int32_t AnimDir = 1.f;
private:
	bool bRenderControl = false; 
};

