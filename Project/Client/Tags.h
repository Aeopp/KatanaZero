#pragma once

namespace OBJECT_ID
{
	using uint8_t = unsigned int;

	// 태그 다음 집합
	enum EID : uint8_t
	{
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

	// 집합
	enum ETAG : uint8_t
	{
		ENEMY,
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
		EOBJECT,
		EEFFECT,
		EUI,
		EMOUSE,

		ENONE,
	};
};

