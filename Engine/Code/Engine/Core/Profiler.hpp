#pragma once
#include "Engine\Time\Clock.hpp"
#include "Engine\Time\Time.hpp"
#include <stdint.h>

struct ProfileMeasurement
{
	char m_id[64];
	uint64_t m_startTime;
	uint64_t m_endTime;

	ProfileMeasurement* m_parent;
	std::vector<ProfileMeasurement*> m_children;

	void SetParent(ProfileMeasurement* parent){ m_parent = parent; }
	void AddChild(ProfileMeasurement* child){m_children.push_back(child);}
};

class Profiler
{
public:
	Profiler();
	~Profiler();

	static Profiler* CreateInstance();
	static Profiler* GetInstance();

	ProfileMeasurement* CreateMeasurement(const char* id);

	void MarkFrame();
	void Push(const char* id);
	void Pop();

public:
	ProfileMeasurement* m_stack;
	ProfileMeasurement* m_previousStack; //we keep N number of these to preserve history
};

