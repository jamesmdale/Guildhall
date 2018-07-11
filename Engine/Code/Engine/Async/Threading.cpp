//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
//#include "Engine\Async\Threading.hpp"
//#include <chrono>
//
//
//ThreadHandle ThreadCreate(ThreadCallback callback, void* arguments)
//{
//	std::thread* threadObject = new std::thread(callback, arguments);
//
//	return threadObject->get_id();
//}
//
////  =========================================================================================
//void ThreadJoin(ThreadHandle handle)
//{
//	std::thread* thread = GetThreadByHandle(handle);
//
//	if(thread != nullptr)
//		thread->join();
//
//	thread = nullptr;
//}
//
////  =========================================================================================
//void ThreadDetach(ThreadHandle handle)
//{
//	std::thread* thread = GetThreadByHandle(handle);
//
//	if (thread != nullptr)
//	{		
//		thread->detach();
//		RemoveThreadByHandle(handle);
//	}
//		
//	thread = nullptr;
//}
//
////  =========================================================================================
//void ThreadCreateAndDetach(ThreadCallback callback, void* userData)
//{
//	ThreadHandle handle = ThreadCreate(callback, userData);
//	ThreadDetach(handle);
//}
//
////  =========================================================================================
//void ThreadSleep(int milliseconds)
//{
//	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
//}
//
////  =========================================================================================
//void ThreadYield()
//{
//	std::this_thread::yield();
//}
//
////  =============================================================================