#pragma once
#include <optional>

class Record
{
public:
	struct Info
	{
		// �ǰ��� ���� ���İ� 0�� �Ǹ� ����
		float OwnerY = 0.f;
		float Alpha = 255;
		int32_t Timing;
		matrix MWorld;
		std::wstring ObjKey;
		std::wstring StateKey;
		uint8_t DrawID;
		D3DXCOLOR _Color;
	};
public:
	// ��ƽ���� ȣ�����ּ���.
	void Render();
	std::multimap<int32_t, typename Record::Info> _Infos;
};
