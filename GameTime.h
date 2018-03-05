#ifndef GAME_TIME
#define GAME_TIME

#include <Windows.h>

class GameTime
{
public:
	GameTime();
	~GameTime();
	bool Initialize();
	void Update();
	float GetTimeElapsed();

private:
	INT64	m_StartTime;
	INT64	m_freq;
	float	m_ticksPerMs;
	float	m_TimeElapsed; // Time elapsed in millisecond
};
#endif