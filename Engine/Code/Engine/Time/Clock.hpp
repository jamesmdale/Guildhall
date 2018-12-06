#pragma once
#include "Engine\Time\Time.hpp"
#include <vector>
#include "Engine\Math\MathUtils.hpp"

struct TimeUnit
{
	uint64_t hpc; 
	double seconds;  // high precision seconds
	//float seconds;      // convenience float seconds
};

class Clock
{
public:
	Clock( Clock* parent = nullptr );
	~Clock(); // make sure to cleanup the heirarchy

			  // resets the clock - everything should be zeroes out
			  // and the reference hpc variables set 
			  // to the current hpc.

	Clock(Clock const &c) = delete; //prevents a copy constructor
	void Reset(); 

	// used on a root clock
	// calculates elapsed time since last call
	// and calls advance.
	void BeginFrame(); 

	// advance the clock by the given hpc
	void AdvanceClock(uint64_t elapsedHPC, double elapsedSeconds);

	// adds a clock as a child to this clock
	void AddChildClock( Clock* child ); 

	// manipulation
	void SetPaused( bool paused ) { m_isPaused = paused;}; 
	void SetScale( float scale ) { m_timeScale = scale;}; 

	void ClockSystemStartup() { Reset(); };

	float GetDeltaSeconds()
	{ 
		float deltaSeconds = ClampFloat((float)m_frame.seconds, 0.0f, 0.05f);
		return deltaSeconds;
	}; //frame time is deltaseconds

	uint64_t GetStartHPC(){return m_startHPC;}
	uint64_t GetLastHPC(){return m_lastFrameHPC;}

	double GetRunningTimeInSeconds();
	double GetRunningTimeInMilliseconds();

	float GetFPS();


public:
	TimeUnit m_frame; 
	TimeUnit m_total; 

private:
	uint64_t  m_startHPC;
	uint64_t m_lastFrameHPC;

	double m_timeScale;
	int m_frameCount;
	bool m_isPaused;

	//for the hierarchy
	Clock* m_parentClock;
	std::vector<Clock*> m_childrenClocks;
}; 

Clock* GetMasterClock();
float GetMasterFPS();
float GetMasterDeltaSeconds();
float GetUnclampedFPS();
float GetUnclampedMasterDeltaSeconds();
void MasterClockBeginFrame();



// returns time since this clock 
// using the current hpc, and our start hpc
// to get the most up-to-date time.
//double GetCurrentTime();