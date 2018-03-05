#include "GameTime.h"


GameTime::GameTime()
{
	m_StartTime = 0;
}

bool GameTime::Initialize()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_freq);
	if (m_freq == 0)
	{
		return false;
	}

	m_ticksPerMs = static_cast<float>(m_freq / 1000);
	QueryPerformanceCounter((LARGE_INTEGER*)&m_StartTime);
	return true;
}

void GameTime::Update()
{
	INT64 currentTime;
	INT64 difference;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	difference = currentTime - m_StartTime;

	m_TimeElapsed = difference / m_ticksPerMs;

	m_StartTime = currentTime;

	return;
}

float GameTime::GetTimeElapsed()
{
	return m_TimeElapsed;
}

GameTime::~GameTime()
{

}