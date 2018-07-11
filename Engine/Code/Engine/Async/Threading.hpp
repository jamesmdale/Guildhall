//#pragma once
//#include <thread>
//#include <map>
//
////struct ThreadHandleType;
//typedef std::thread::id ThreadHandle;
//
//typedef void(*ThreadCallback)(void* userData);
//
//// functions =========================================================================================
//ThreadHandle ThreadCreate(ThreadCallback callback, void* arguments = nullptr);
//
//void ThreadJoin();
//void ThreadDetach();
//void ThreadCreateAndDetach(ThreadCallback callback, void* userData = nullptr);
//
//void ThreadSleep(int milliseconds);
//void ThreadYield();
//
//static std::map<ThreadHandle, std::thread*> s_threads;
//
