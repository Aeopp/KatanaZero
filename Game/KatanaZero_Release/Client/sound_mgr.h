#pragma once
#include "singleton_interface.h"
#include "fmod.hpp"
#include <unordered_map>

class sound_mgr : public singleton_interface<sound_mgr>
{
public:
	using SoundType = std::tuple<FMOD::System*, FMOD::Sound*, FMOD::Channel*>;
	std::unordered_map<std::string, SoundType> Sounds;
	static inline constexpr float DefaultVolume = 1.f;
	FMOD::System* FMOD_System = nullptr;
public:
	void initialize();

	// 확장자없이 파일이름만 입력해주세요.
	bool Play(std::string SoundKey, bool IsBgm = false, const float Volume = DefaultVolume);
	void Stop(std::string SoundKey);
	bool Load(std::string FullPath, std::string Key);
	bool Init();
	bool Frame(const float DeltaTime);
	bool Release();
	bool IsPlay(const std::string& SoundKey);

	void RandSoundKeyPlay(const std::string& SoundKey,
		std::pair<uint8_t, uint8_t> Range, float Volume);

	std::string CurrentBgmKey{};
private:
};

void SOUNDPLAY(const std::string& Key, float Volume = 1.f, bool IsBgm = false);
void RAND_SOUNDPLAY(const std::string& Key, std::pair<uint8_t, uint8_t> Range,
	float Volume = 1.f, bool IsBgm = false);
