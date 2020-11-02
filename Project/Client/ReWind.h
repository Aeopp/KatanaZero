#pragma once

class ReWind
{
public:
	struct Info
	{
		// �ǰ��� ���� ���İ� 0�� �Ǹ� ����
		float OwnerY = 0.f;
		float Alpha = 255;
		float T = 0.f;
		matrix MWorld;
		std::wstring_view ObjKey;
		std::wstring_view StateKey;
		uint8_t DrawID;
	};
public:
	// �ǰ��� ���� �ƴҶ��� ȣ��
	void Push(float T, matrix MWorld, std::wstring_view ObjKey, std::wstring_view StateKey,
		uint8_t DrawID, float OwnerY);
	// ��ƽ���� ȣ�����ּ���.
	void Update();
	void Render();
	bool bUpdate = false;
private:
	
	float UpdateDelta = 0.1f;
	std::list<typename ReWind::Info> _Infos;
};
