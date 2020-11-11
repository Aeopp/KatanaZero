#include "stdafx.h"
#include "sound_mgr.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include "math.h"

void sound_mgr::initialize()
{
	Init();

	namespace fs = std::filesystem;

	for (const fs::directory_entry& entry :
		fs::directory_iterator(fs::current_path() / ".."  / "Resource" / "Sound")) {
		bool bLoad = Load(entry.path().string(), entry.path().stem().string());
	}
}

bool sound_mgr::Play(std::string SoundKey,
	bool IsBgm, const float Volume) {

	if (Sounds.empty()) {
		return false;
	}

	if (IsBgm)
	{
		CurrentBgmKey = SoundKey;
	}

	FMOD_RESULT HR;
	bool isplay = false;

	if (auto iter = Sounds.find(SoundKey);
		iter != std::end(Sounds))
	{
		auto& [System, Sound, Channel] = iter->second;
		{
			Channel->isPlaying(&isplay);

			/*	if (isplay == true) {
					Channel->stop();
				};*/
			HR = System->playSound(Sound, nullptr, false,
				&Channel);
			Channel->setVolume(Volume == DefaultVolume ? DefaultVolume : Volume);

			if (HR != FMOD_OK)
			{
				return  false;
			}
		}
		return  true;
	}
	return true;
}
void sound_mgr::Stop(std::string SoundKey)
{
	auto iter = Sounds.find(SoundKey);
	if (iter == std::end(Sounds))
		return;

	auto& Channel = std::get<2>(iter->second);
	Channel->stop();
};

bool sound_mgr::Load(std::string FullPath, std::string Key) {

	if (FMOD_System == nullptr) {
		return false;
	};
	// 사운드 새로 생성해서 삽입
	if (auto iter = Sounds.find(Key);
		iter == std::end(Sounds) == true) {
		SoundType InsertSound;
		auto& [System, Sound, Channel] = InsertSound;
		System = FMOD_System;

		FMOD_RESULT HR = System->createSound(FullPath.c_str(), FMOD_DEFAULT, 0, &Sound);

		if (HR != FMOD_OK) {
			return false;
		};

		Channel->setVolume(DefaultVolume);
		Sounds.insert(iter, { std::move(Key)  , InsertSound });
	}
	return true;
};

bool sound_mgr::Frame(const float DeltaTime) {
	if (FMOD_System == nullptr) return false;
	// update 함수는 매프레임마다 반드시 호출해줘야함을 요구함
	FMOD_System->update();

	// BGM 재생 루프
	auto iter = Sounds.find(CurrentBgmKey);
	if (iter != std::end(Sounds))
	{
		auto& [System, Sound, Channel] = iter->second;

		bool bPlayedBgm = false;
		Channel->isPlaying(&bPlayedBgm);
		if (!bPlayedBgm)
		{
			System->playSound(Sound, nullptr, false, &Channel);
		}
	};
	return true;
}
bool sound_mgr::Release()
{
	for (auto& [key, SoundObject] : Sounds) {
		auto& [System, Sound, Channel] = SoundObject;
		Sound->release();
	}
	Sounds.clear();
	return true;
}
bool sound_mgr::IsPlay(const std::string& SoundKey)
{
	bool bPlay{};

	auto SoundIter = Sounds.find(SoundKey);

	if (std::end(Sounds) != SoundIter) {
		auto& [Key, SoundObject] = *SoundIter;
		auto& [System, Sound, Channel] = SoundObject;
		Channel->isPlaying(&bPlay);
	};

	return bPlay;
}
void sound_mgr::RandSoundKeyPlay(const std::string& SoundKey, std::pair<uint8_t, uint8_t> Range,
	float Volume)
{
	int n = math::Rand<uint8_t>(std::move(Range));

	Play(SoundKey + "_" + std::to_string(n), false, Volume);
}
bool sound_mgr::Init()
{
	FMOD_RESULT F_Result;

	F_Result = FMOD::System_Create(&FMOD_System);

	{
		// FMOD 시스템 로딩 실패
		if (F_Result != FMOD_OK)
			return false;
		F_Result = FMOD_System->init(128, FMOD_INIT_NORMAL, 0);
		if (F_Result != FMOD_OK)
			return false;
	}

	return true;
};

void SOUNDPLAY(const std::string& Key, float Volume, bool IsBgm)
{
	sound_mgr::instance().Play(Key, IsBgm, Volume);
}

void RAND_SOUNDPLAY(const std::string& Key, std::pair<uint8_t, uint8_t> Range, float Volume, bool IsBgm)
{
	sound_mgr::instance().RandSoundKeyPlay(Key, Range, Volume);
}

