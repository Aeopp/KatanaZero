#include "stdafx.h"
#include "GO.h"
#include "TransformComponent.h"
#include "ComponentManager.h"
#include "UIRenderComponent.h"
#include "Time.h"
#include "RecordManager.h"
#include "Texture_Manager.h"
#include "GraphicDevice.h"
#include "ObjectManager.h"
#include <algorithm>
#include "NormalEnemy.h"

void GO::Initialize() & noexcept
{
    UI::Initialize();

    _RenderComp = ComponentManager::instance().Insert<UIRenderComponent>(_This);
    _TransformComp = ComponentManager::instance().Insert<TransformComponent>(_This);
    _CollisionComponent = ComponentManager::instance().Insert<CollisionComponent>(_This);

    _CollisionComponent->bCollision = false;
    _CollisionComponent->_Tag = CollisionComponent::ETag::ETrigger;
    _CollisionComponent->_CollisionInfo.Height = 100;
    _CollisionComponent->_CollisionInfo.Width = 100;

    _RenderComp->Depth = 1;
    _RenderComp->bRender = false;

    _TransformComp->Scale *= 2.f;

    _RenderComp->_Control.bRender = true;
    _RenderComp->_Control._Render = [](Component& Comp) {
        auto& _RenderComp  =dynamic_cast<UIRenderComponent&>(Comp);
        if (!_RenderComp.bRender)return;

        if (_RenderComp._RenderAfterEvent)
        {
            _RenderComp._RenderAfterEvent();
        }
    };
    _RenderComp->_Info._Layer = LAYER::EUI;

    ScreenPos.x = 200;
    ScreenPos.y = 300;


    _RenderComp->_RenderAfterEvent = [this]() 
    {
        {
            auto spTexInfo = TextureManager::instance().Get_TexInfo(L"Go", L"Go", 1);
            if (!spTexInfo)return;

            matrix MScale, MTrans;
            D3DXMatrixScaling(&MScale,std::abs( _TransformComp->Scale.x), std::abs(_TransformComp->Scale.y), 0);
            D3DXMatrixTranslation(&MTrans, ScreenPos.x, ScreenPos.y - 50, 0);
            MScale *= MTrans;
            RECT srcRect = { 0,0,spTexInfo->ImageInfo.Width ,
                                      spTexInfo->ImageInfo.Height };
            vec3 TextureCenter = { spTexInfo->ImageInfo.Width / 2.f,spTexInfo->ImageInfo.Height / 2.f,0.f };
            GraphicDevice::instance().GetSprite()->SetTransform(&MScale);
            GraphicDevice::instance().GetSprite()->Draw(spTexInfo->pTexture, &srcRect, &TextureCenter, nullptr,
                D3DCOLOR_ARGB(255, 255, 255, 255));
        }

        {
            auto spTexInfo = TextureManager::instance().Get_TexInfo(L"Go", L"Go", 0);
            if (!spTexInfo)return;

            matrix MScale, MTrans;
            D3DXMatrixScaling(&MScale, _TransformComp->Scale.x * XDir, _TransformComp->Scale.y* XDir, 0);
            D3DXMatrixTranslation(&MTrans, ScreenPos.x, ScreenPos.y, 0);
            MScale *= MTrans;
            RECT srcRect = { 0,0,spTexInfo->ImageInfo.Width ,
                                      spTexInfo->ImageInfo.Height };
            vec3 TextureCenter = { spTexInfo->ImageInfo.Width / 2.f,spTexInfo->ImageInfo.Height / 2.f,0.f };
            GraphicDevice::instance().GetSprite()->SetTransform(&MScale);
            GraphicDevice::instance().GetSprite()->Draw(spTexInfo->pTexture, &srcRect, &TextureCenter, nullptr,
                D3DCOLOR_ARGB(255, 255, 255, 255));
        }
     
    };

    auto wpThis = _This;
}

void GO::Update()
{
    const float dt = Time::instance().Delta();
    const float T = Time::instance().T();

    ClearCheckTime  -= dt;

    if (ClearCheckTime < 0.f)
    {
        ClearCheckTime = 1.3f;
        
        auto EnemyMapIter = ObjectManager::instance()._ObjectMap.find(OBJECT_TAG::ETAG::EENEMY);

        if (EnemyMapIter == std::end(ObjectManager::instance()._ObjectMap))return;

        if (std::all_of(std::begin(EnemyMapIter->second), std::end(EnemyMapIter->second), 
            [](const auto& _EnemyPair) {
            auto& _EnemyVec = _EnemyPair.second;
            return std::all_of(std::begin(_EnemyVec), std::end(_EnemyVec), [](const std::shared_ptr<class object>& _spEnemy) {
               auto _NormalEnemy = std::dynamic_pointer_cast<NormalEnemy>(_spEnemy);
               if (!_NormalEnemy)return true;
               return _NormalEnemy->_EnemyState == NormalEnemy::State::Die;
            });
        }))
        {
            _CollisionComponent->bCollision = true;
            _RenderComp->bRender = true;
            bClear = true;
        }
    }

    ScreenPos.x = InitScreenPos.x + (std::sinf(T*5.f))* XRange;
}

void GO::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
    UI::Hit(_Target, _CollisionInfo);

    if (_CollisionInfo._ID == OBJECT_ID::EID::EPLAYER)
    {
        if (bClear  )
        {
            RecordManager::instance().ReplayStart();
        }
    }
}

OBJECT_ID::EID GO::GetID()
{
	return OBJECT_ID::EID::GO;
}

OBJECT_TAG::ETAG GO::GetTag()
{
	return OBJECT_TAG::ETAG::EUI;

}

std::wstring_view GO::GetName() const&
{
	return L"GO"sv;
}

void GO::SetUpInitState(float XDir, int32_t InitState)
{
    if (XDir > 0)
    {

        InitScreenPos.x =   ScreenPos.x = 1720.f;
        InitScreenPos.y= ScreenPos.y = 300;
    }
    else
    {
        InitScreenPos.x = ScreenPos.x = 200.f;
        InitScreenPos.y= ScreenPos.y = 300;
    }
    this->XDir = XDir;

  // _TransformComp->Scale*= XDir;
}

