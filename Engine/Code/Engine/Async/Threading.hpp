#pragma once
#include <thread>
#include <map>

//struct ThreadHandleType;
typedef std::thread::id ThreadHandle;

typedef void(*ThreadCallback)(void* userData);

// functions =========================================================================================
ThreadHandle ThreadCreate(ThreadCallback callback, void* arguments = nullptr);

void ThreadJoin(ThreadHandle thread);
void ThreadDetach(ThreadHandle thread);
void ThreadCreateAndDetach(ThreadCallback callback, void* userData = nullptr);

void ThreadSleep(int milliseconds);
void ThreadYield();

std::thread* GetThreadByHandle(ThreadHandle handle);
void RemoveThreadByHandle(ThreadHandle handle);

static std::map<ThreadHandle, std::thread*> s_threads;

