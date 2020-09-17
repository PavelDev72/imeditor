#pragma once
#include "imgui/imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include <string>
#include <fstream>

#define ELEMENTS_COUNT 100

template<class C>
C& clear(C& container) {
	C empty;
	using std::swap;
	swap(empty, container);
	return container;
}

struct Element
{
	int element = -1;
	std::string label;
	ImVec2 pos;
	ImVec2 size;
	float color[5][4] = { 0.f, 0.f, 0.f, 1.f };
	bool checked = false;
};

class Window
{
public:
	std::string title = "New Window";
	bool canClose = false;
	bool canMinimize = true;
	bool canMove = true;
	bool canResize = false;
	float titleCol[4] = { 0.8f, 0.8f, 0.8f, 1.f };
	float windowCol[4] = { 1.f, 1.f, 1.f, 1.f };
	ImVec2 size = ImVec2(840, 650);
	Element elements[ELEMENTS_COUNT];
	int FindElement();
};