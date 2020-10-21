#pragma once

namespace OBJECT_ID
{
	using uint8_t = unsigned int;

	// �±� ���� ����
	enum EID : uint8_t
	{
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

