#pragma once

namespace OBJECT_ID
{
	using uint8_t = unsigned int;

	// �±� ���� ����
	enum EID : uint8_t
	{
		DOOR,
		DRAGON_DASH,
		SMOKE_CLOUD,
		REFLECT,
		DRUM,
		EXPLOSION,
		ITEM,
		GO,
		GANGSTER_GUN,
		REFLECT_BULLET,
		BULLET,
		HIT_EFFECT,
		GANGSTER,
		HIT_IMPACT,
		STOMPER_CLOUD,
		JUMP_CLOUD,
		LAND_CLOUD,
		SLASH_FX,
		WHITE_BLOOD,
		GRUNT_SLASH,
		GRUNT,
		DustCloud ,
		Effect, 
		ATTACK,
		ATTACK_SLASH,
		ELINE,
		ETILE,
		EDOWNJUMPTILE,
		EWALLRIDELINE,
		EPLAYER,
		CHARACTER,
		CAMERA,
		UI_TIMER,
		UI_ITEMICONS,
		HUD,
		BATTERY,
		MOUSE,
		EENEMY1,
		EENEMY2,
		EENEMY3,
		ENONE,
	};
};

namespace OBJECT_TAG
{
	using uint8_t = unsigned int;

	// ����
	enum ETAG : uint8_t
	{
		SMOKE_CLOUD,
		INTERACTION_OBJECT,
		ITEM,
		ATTACK,
		PLAYER_ATTACK,
		ENEMY_ATTACK,
		ETERRAIN,
		CHARCTER,
		EOBJECT,
		EENEMY,
		EPLAYER,
		EBOSS,
		EEFFECT,
		EUI,
		ENONE,
	};
};

namespace LAYER
{
	using uint8_t = unsigned int;

	enum ELAYER : uint8_t
	{
		ETERRAIN,
		EOBJECT_UNDER,
		EOBJECT,
		EOBJECT_OVER,
		EEFFECT,
		EEFFECT_OVER,
		EUI,
		EMOUSE,

		ENONE,
	};
};

