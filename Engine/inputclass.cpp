////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"

InputClass::InputClass()
{

}

InputClass::InputClass(const InputClass& other)
{
}

InputClass::~InputClass()
{

}

void InputClass::Initialize()
{
	int i;

	// Initialize all the keys to being releasef and not pressed.
	for(i=0; i<256;i++)
	{
		m_keys[i] =false;
	}

	return;
}

void InputClass::KeyDown(unsigned int input)
{
	// If a key is pressed the save that state in the key array.
	m_keys[input]=true;
	return;
}

void InputClass::KeyUp(unsigned int input)
{
	// If  a key is released then clear that state in hte key array.
	m_keys[input]= false;
	return;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	// return what state hte key is in (pressed/not pressed).
	return m_keys[key];
}