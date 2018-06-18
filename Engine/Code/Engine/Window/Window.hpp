#pragma once
#include <vector>
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\AABB2.hpp"

typedef bool (*windows_message_handler_cb)( unsigned int msg, size_t wparam, size_t lparam ); 

class Window
{
private:
	Window( char const* appName, float clientAspect );
	

public:
	~Window(); 
	void* GetHandle() const { return m_hwnd; }
	void AddHandler( windows_message_handler_cb cb ); 
	static Window* CreateInstance( char const* title, float aspect );
	static Window* GetInstance();
	AABB2 GetClientWindow();
	Vector2 GetCenterOfClientWindow();	
	float GetClientWidth() { return m_clientWidth;};
	float GetClientHeight() { return m_clientHeight;};
	Vector2 GetClientDimensions(){return Vector2(m_clientWidth, m_clientHeight);};
	float GetClientAspectRatio() { return m_clientHeight;};

public:
	void* m_hwnd;
	float m_clientHeight = 0.f;
	float m_clientWidth = 0.f;
	float m_aspectRatio = 0.f;
	std::vector<windows_message_handler_cb> m_messageHandlers; 	
}; 