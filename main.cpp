#pragma comment(lib,"d3d9.lib")
#include "main.hpp"
#include "export.hpp";

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool anyProject = false;

bool pShowProperties = true;
bool pShowToolbar = true;
bool pShowAdvanced = false;

int wasSelected = -1;
int pSelected = -1;
int pMoving = -1;

bool pShowGrid = false;
int pGridSize = 15;

char pElTitle[255];

ImVec4 ConvertToVec4(ImVec4 color)
{ return ImVec4(color.x / 255, color.y / 255, color.z / 255, color.w / 255); }

int Window::FindElement()
{
	for (int i = 0; i < ELEMENTS_COUNT; ++i)
	{
		if (elements[i].element < 0)
			return i;
	}
	return -1;
}

void OnCreateElementButton(int elemID, Window *pWindow)
{
	Element pElement;
	int elID = pWindow->FindElement();
	switch (elemID)
	{
	case 0:
		pElement.element = 0;
		pElement.label = "New Label";
		pElement.pos = ImVec2(50, 50);
		if (elID >= 0)
			pWindow->elements[elID] = pElement;
		pSelected = elID;
		break;
	case 1:
		pElement.element = 1;
		pElement.label = "New Button";
		pElement.pos = ImVec2(50, 50);
		pElement.size = ImVec2(100, 50);
		
		for (int i = 0; i < 3; ++i)
		{
			pElement.color[i][0] = 0.24f;
			pElement.color[i][1] = 0.65f;
			pElement.color[i][2] = 0.87f;
			pElement.color[i][3] = 1.f;
		}
		pElement.color[3][0] = 1.f;
		pElement.color[3][1] = 1.f;;
		pElement.color[3][2] = 1.f;
		pElement.color[3][3] = 1.f;

		if (elID >= 0)
			pWindow->elements[elID] = pElement;
		pSelected = elID;
		break;
	case 2:
		pElement.element = 2;
		pElement.label = "New Checkbox";
		pElement.pos = ImVec2(50, 50);

		for (int i = 0; i < 4; ++i)
		{
			pElement.color[i][0] = 0.f;
			pElement.color[i][1] = 0.f;
			pElement.color[i][2] = 0.f;
			pElement.color[i][3] = 1.f;
		}
		pElement.color[4][0] = 1.f;
		pElement.color[4][1] = 1.f;;
		pElement.color[4][2] = 1.f;
		pElement.color[4][3] = 1.f;

		if (elID >= 0)
			pWindow->elements[elID] = pElement;
		pSelected = elID;
		break;
	case 3:
		pElement.element = 3;
		pElement.label = "New Input";
		pElement.pos = ImVec2(50, 50);
		pElement.size = ImVec2(200, 0);

		for (int i = 0; i < 3; ++i)
		{
			pElement.color[i][0] = 0.f;
			pElement.color[i][1] = 0.f;
			pElement.color[i][2] = 0.f;
			pElement.color[i][3] = 1.f;
		}
		pElement.color[3][0] = 1.f;
		pElement.color[3][1] = 1.f;;
		pElement.color[3][2] = 1.f;
		pElement.color[3][3] = 1.f;

		if (elID >= 0)
			pWindow->elements[elID] = pElement;
		pSelected = elID;
		break;
	}
}

void CreateElement(const char *text, int elemID, Window *pWindow)
{
	bool result = ImGui::Button(text, ImVec2(190, 0));
	if (result) OnCreateElementButton(elemID, pWindow);
}

// Main code
int main(int, char**)
{
	// Create application window
	//ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGUI Editor 0.0.1 by Pakulichev"), NULL };
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("ImGUI Editor 0.0.1 by Pakulichev"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Hide the console
	::ShowWindow(GetConsoleWindow(), FALSE);

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);
	io.IniFilename = NULL;

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Our state
	static ImWchar ranges[3] = {0xf000, 0xf83e, 0};
	ImFontConfig config;
	config.MergeMode = true;

	io.Fonts->ClearFonts();
	io.Fonts->AddFontDefault();
	ImFont* pFontBD = io.Fonts->AddFontFromFileTTF("tahoma.ttf", 22.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	ImFont* pFont = io.Fonts->AddFontFromFileTTF("tahoma.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	ImFont* FontAwesome = io.Fonts->AddFontFromFileTTF("fa-solid-900.ttf", 14.0f, &config, ranges);
	io.Fonts->Build();

	Window pWindow;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::PushFont(pFont);

		{
			static float f = 0.0f;
			static int counter = 0;

			RECT windowConf;
			GetWindowRect(hwnd, &windowConf);

			int width = windowConf.right - windowConf.left;
			int height = windowConf.bottom - windowConf.top;

			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2((float)width, (float)height), ImGuiCond_Always);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(1, 1, 1, 1));
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1, 1, 1, 1));
			ImGui::Begin("ImGUI Editor 0.0.1 by Pakulichev", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Create New Project"))
					{
						anyProject = true;
						clear(pWindow); // Just swap the class
						strcpy_s(pElTitle, sizeof(pElTitle), pWindow.title.c_str());
					}
					ImGui::MenuItem("Save Current Project", '\0', (bool*)false, anyProject);
					if (ImGui::MenuItem("Export Current Project", '\0', (bool*)false, anyProject))
						ExportProject(hwnd, pWindow);
					ImGui::MenuItem("Close Current Project", '\0', (bool*)false, anyProject);
					if (ImGui::MenuItem("Exit ImGUI Editor"))
						::PostQuitMessage(0);
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Environment"))
				{
					ImGui::MenuItem("Show the Grid", '\0', &pShowGrid);
					if (pShowGrid)
					{
					ImGui::Text("Grid Size:");
					ImGui::SameLine();
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
					ImGui::PushItemWidth(30);
					ImGui::InputInt("##GridSize", &pGridSize, 0, 0);
					ImGui::AlignTextToFramePadding();
					ImGui::PopItemWidth();
					ImGui::PopStyleVar();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Window"))
				{
					ImGui::MenuItem("Show Toolbar", '\0', &pShowToolbar);
					ImGui::MenuItem("Show Properties", '\0', &pShowProperties);
					ImGui::MenuItem("Show Additions", '\0', &pShowAdvanced);
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("About ImGui Editor"))
				{
					ImGui::MenuItem("Developers Team");
					ImGui::MenuItem("What is ImGUI Editor");
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			ImGui::PopStyleVar(1);

			ImGui::SetCursorPosX(0);
			ImVec2 sPos = ImGui::GetCursorPos();
			ImGui::BeginChild("WorkArea", ImVec2(ImGui::GetWindowWidth() - sPos.x, ImGui::GetWindowHeight() - sPos.y), FALSE);
			ImGui::SetCursorPos(ImVec2(0, 0));
			sPos = ImGui::GetCursorPos();
			ImVec2 wSize = ImGui::GetWindowSize();
			wSize.y -= sPos.y;

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
			if (pShowToolbar)
			{
				ImGui::SetCursorPos(ImVec2(0, sPos.y));
				ImGui::PushStyleColor(ImGuiCol_ChildBg, ConvertToVec4(ImVec4(66, 66, 66, 255)));
				ImGui::BeginChild("Toolbar", ImVec2(200, wSize.y), false);

				ImGui::PushFont(pFontBD);
				ImGui::SetCursorPos(ImVec2(40, 5));
				ImGui::Text("ImGUI Toolbar");
				ImGui::PopFont();
				ImGui::PushFont(FontAwesome);
				ImGui::SetCursorPos(ImVec2(5, 35));
				ImGui::BeginGroup();
				CreateElement("\xef\x80\xb1 Text Label", 0, &pWindow);
				CreateElement("\xef\x83\x88 Flat Button", 1, &pWindow);
				CreateElement("\xef\x85\x8a Checkable Flag", 2, &pWindow);
				CreateElement("\xef\x81\x84 InputText Field", 3, &pWindow);
				ImGui::EndGroup();
				ImGui::PopFont();

				ImGui::EndChild();
				ImGui::PopStyleColor();
			}

			if (pShowProperties)
			{
				ImGui::SetCursorPos(ImVec2(wSize.x - 215, sPos.y));
				ImGui::PushStyleColor(ImGuiCol_ChildBg, ConvertToVec4(ImVec4(66, 66, 66, 255)));
				ImGui::BeginChild("Properties", ImVec2(200, wSize.y), false);

				ImGui::PushFont(pFontBD);
				ImGui::SetCursorPos(ImVec2(28, 5));
				ImGui::Text("Element Properties");
				ImGui::PopFont();

				/*char* buffer = new char[10 + 128];
				strcat_s(buffer, 11, "Selected: ");
				if (pSelected == -1)
					strcat_s(buffer, 11, "Window [0]");*/
				std::string editBuffer = "Selected: ";
				if (!anyProject)
					editBuffer += "None";
				else
				{
					if (pSelected == -1)
						editBuffer += "Window Form";
					else
					{
						switch (pWindow.elements[pSelected].element)
						{
						case 0:
							editBuffer += "Text Label [" + std::to_string(pSelected) + ']';
							break;
						case 1:
							editBuffer += "Flat Button [" + std::to_string(pSelected) + ']';
							break;
						case 2:
							editBuffer += "Checkbox [" + std::to_string(pSelected) + ']';
							break;
						case 3:
							editBuffer += "InputText [" + std::to_string(pSelected) + ']';
							break;
						}
					}
				}

				if (wasSelected != pSelected)
				{
					if (pSelected == -1)
						strcpy_s(pElTitle, sizeof(pElTitle), pWindow.title.c_str());
					else
					{
						strcpy_s(pElTitle, sizeof(pElTitle), pWindow.elements[pSelected].label.c_str());
						/*switch (pWindow.elements[pSelected].element)
						{

						}*/
					}
					wasSelected = pSelected;
				}

				ImVec2 tLen = ImGui::CalcTextSize(editBuffer.c_str());
				ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - tLen.x / 2);
				ImGui::Text(editBuffer.c_str());
				ImGui::SetCursorPosX(5);
				if (anyProject)
				{
					ImGui::BeginGroup();
					if (pSelected == -1)
					{
						ImGui::Text("Window Title");
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
						ImGui::PushItemWidth(190);
						if (ImGui::InputText("##WindowTitle", pElTitle, IM_ARRAYSIZE(pElTitle), ImGuiInputTextFlags_EnterReturnsTrue))
							pWindow.title = pElTitle;
						ImGui::PopItemWidth();
						ImGui::PopStyleColor();
						ImGui::Checkbox("Window Closable", &pWindow.canClose);
						ImGui::Checkbox("Window Collapsible", &pWindow.canMinimize);
						ImGui::Checkbox("Window Movable", &pWindow.canMove);
						ImGui::Checkbox("Window Resizable", &pWindow.canResize);
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
						ImGui::PushItemWidth(190);
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
						ImGui::Text("Title Background");
						ImGui::PopStyleColor();
						ImGui::ColorEdit4("##TitleBG", pWindow.titleCol);
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
						ImGui::Text("Window Background");
						ImGui::PopStyleColor();
						ImGui::ColorEdit4("##WindowBG", pWindow.windowCol);
						ImGui::PopItemWidth();
						ImGui::PopStyleColor();
					}
					else
					{
						float posx = pWindow.elements[pSelected].pos.x;
						float posy = pWindow.elements[pSelected].pos.y;
						float sizex = pWindow.elements[pSelected].size.x;
						float sizey = pWindow.elements[pSelected].size.y;
						switch (pWindow.elements[pSelected].element)
						{
						case 0:
							ImGui::Text("Text Label");
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
							ImGui::PushItemWidth(190);
							if (ImGui::InputText("##TextLabel", pElTitle, IM_ARRAYSIZE(pElTitle), ImGuiInputTextFlags_EnterReturnsTrue))
								pWindow.elements[pSelected].label = pElTitle;
							ImGui::PopItemWidth();
							ImGui::PushItemWidth(190);
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
							ImGui::Text("Text Color");
							ImGui::PopStyleColor();
							ImGui::ColorEdit4("##Color", pWindow.elements[pSelected].color[0]);
							ImGui::PopItemWidth();
							ImGui::PopStyleColor();
							ImGui::Text("Position");
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));

							ImGui::PushItemWidth(90);
							ImGui::InputFloat("##XPos", &posx, 0.0, 0.0);
							ImGui::SameLine();
							ImGui::InputFloat("##YPos", &posy, 0.0, 0.0);
							ImGui::PopItemWidth();
							
							pWindow.elements[pSelected].pos.x = posx;
							pWindow.elements[pSelected].pos.y = posy;
							
							ImGui::PopStyleColor();
							break;
						case 1:
							ImGui::Text("Button Label");
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
							ImGui::PushItemWidth(190);
							if (ImGui::InputText("##TextLabel", pElTitle, IM_ARRAYSIZE(pElTitle), ImGuiInputTextFlags_EnterReturnsTrue))
								pWindow.elements[pSelected].label = pElTitle;
							ImGui::PopItemWidth();
							ImGui::PushItemWidth(190);
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
							ImGui::Text("Button Colors");
							ImGui::PopStyleColor();
							ImGui::ColorEdit4("##Color", pWindow.elements[pSelected].color[0]);
							ImGui::ColorEdit4("##Color2", pWindow.elements[pSelected].color[1]);
							ImGui::ColorEdit4("##Color3", pWindow.elements[pSelected].color[2]);
							ImGui::ColorEdit4("##Color4", pWindow.elements[pSelected].color[3]);
							ImGui::PopItemWidth();
							ImGui::PopStyleColor();
							ImGui::Text("Position & Size");
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));

							ImGui::PushItemWidth(90);
							ImGui::InputFloat("##XPos", &posx, 0.0, 0.0);
							ImGui::SameLine();
							ImGui::InputFloat("##YPos", &posy, 0.0, 0.0);
							ImGui::InputFloat("##XSize", &sizex, 0.0, 0.0);
							ImGui::SameLine();
							ImGui::InputFloat("##YSize", &sizey, 0.0, 0.0);
							ImGui::PopItemWidth();

							pWindow.elements[pSelected].pos.x = posx;
							pWindow.elements[pSelected].pos.y = posy;
							pWindow.elements[pSelected].size.x = sizex;
							pWindow.elements[pSelected].size.y = sizey;

							ImGui::PopStyleColor();
							break;
						case 2:
							ImGui::Text("Checkbox Label");
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
							ImGui::PushItemWidth(190);
							if (ImGui::InputText("##TextLabel", pElTitle, IM_ARRAYSIZE(pElTitle), ImGuiInputTextFlags_EnterReturnsTrue))
								pWindow.elements[pSelected].label = pElTitle;
							ImGui::PopItemWidth();
							ImGui::PushItemWidth(190);
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
							ImGui::Text("Checkbox Colors");
							ImGui::PopStyleColor();
							ImGui::ColorEdit4("##Color", pWindow.elements[pSelected].color[0]);
							ImGui::ColorEdit4("##Color2", pWindow.elements[pSelected].color[1]);
							ImGui::ColorEdit4("##Color3", pWindow.elements[pSelected].color[2]);
							ImGui::ColorEdit4("##Color4", pWindow.elements[pSelected].color[3]);
							ImGui::ColorEdit4("##Color5", pWindow.elements[pSelected].color[4]);
							ImGui::PopItemWidth();
							ImGui::PopStyleColor();
							ImGui::Checkbox("Is Flag Checked", &pWindow.elements[pSelected].checked);
							ImGui::Text("Position");
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));

							ImGui::PushItemWidth(90);
							ImGui::InputFloat("##XPos", &posx, 0.0, 0.0);
							ImGui::SameLine();
							ImGui::InputFloat("##YPos", &posy, 0.0, 0.0);
							ImGui::PopItemWidth();

							pWindow.elements[pSelected].pos.x = posx;
							pWindow.elements[pSelected].pos.y = posy;

							ImGui::PopStyleColor();
							break;
						case 3:
							ImGui::Text("InputText Label");
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
							ImGui::PushItemWidth(190);
							if (ImGui::InputText("##TextLabel", pElTitle, IM_ARRAYSIZE(pElTitle), ImGuiInputTextFlags_EnterReturnsTrue))
								pWindow.elements[pSelected].label = pElTitle;
							ImGui::PopItemWidth();
							ImGui::PushItemWidth(190);
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
							ImGui::Text("InputText Colors");
							ImGui::PopStyleColor();
							ImGui::ColorEdit4("##Color", pWindow.elements[pSelected].color[0]);
							ImGui::ColorEdit4("##Color2", pWindow.elements[pSelected].color[1]);
							ImGui::ColorEdit4("##Color3", pWindow.elements[pSelected].color[2]);
							ImGui::ColorEdit4("##Color4", pWindow.elements[pSelected].color[3]);
							ImGui::PopItemWidth();
							ImGui::PopStyleColor();
							ImGui::Text("Position & Size");
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));

							ImGui::PushItemWidth(90);
							ImGui::InputFloat("##XPos", &posx, 0.0, 0.0);
							ImGui::SameLine();
							ImGui::InputFloat("##YPos", &posy, 0.0, 0.0);
							ImGui::PopItemWidth();
							ImGui::PushItemWidth(190);
							ImGui::InputFloat("##XSize", &sizex, 0.0, 0.0);
							ImGui::PopItemWidth();

							pWindow.elements[pSelected].pos.x = posx;
							pWindow.elements[pSelected].pos.y = posy;
							pWindow.elements[pSelected].size.x = sizex;

							ImGui::PopStyleColor();
							break;
						}
					}
					ImGui::EndGroup();
				}
				ImGui::EndChild();
				ImGui::PopStyleColor();
			}

			if (pShowToolbar)
				ImGui::SetCursorPos(ImVec2(200, sPos.y));
			else
				ImGui::SetCursorPos(ImVec2(sPos.x, sPos.y));
			float centerSize = ImGui::GetWindowWidth() - 15;
			if (pShowProperties) centerSize -= 200.0f;
			if (pShowToolbar) centerSize -= 200.0f;

			ImGui::BeginChild("Main", ImVec2(centerSize, wSize.y));

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ConvertToVec4(ImVec4(66, 66, 66, 255)));
			ImGui::BeginChild("Main Title", ImVec2(ImGui::GetWindowWidth(), 50), false);
			ImGui::SetCursorPos(ImVec2(5, 2));
			ImGui::BeginGroup();
			ImGui::PushFont(pFontBD);
			ImGui::Text("ImGUI Editor 0.0.1");
			ImGui::PopFont();
			ImGui::Text("Developer: Pakulichev");
			ImGui::EndGroup();
			ImGui::EndChild();
			ImGui::PopStyleColor();

			ImGui::SetCursorPos(ImVec2(0, 50));
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ConvertToVec4(ImVec4(88, 88, 88, 255)));
			ImGui::BeginChild("Main Box", ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 50), false);

			ImGui::EndChild();
			ImGui::PopStyleColor();

			ImGui::EndChild();
			ImGui::PopStyleColor();

			ImGui::EndChild();
			ImGui::End();
			ImGui::PopStyleVar(3); // Window Rounding and Border Size & Padding
			ImGui::PopStyleColor(2); // Menu Bar Background & Window Background
		
			// Creation Window
			if (anyProject)
			{
				ImGui::SetNextWindowSize(pWindow.size, ImGuiCond_Always);
				ImGui::SetNextWindowPos(ImVec2(210, 90), ImGuiCond_FirstUseEver);
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(pWindow.windowCol[0], pWindow.windowCol[1], pWindow.windowCol[2], pWindow.windowCol[3]));
				ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(pWindow.titleCol[0], pWindow.titleCol[1], pWindow.titleCol[2], pWindow.titleCol[3]));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

				bool winCross = true;
				
				ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove;
				if (!pWindow.canMinimize) window_flags |= ImGuiWindowFlags_NoCollapse;
				if (!pWindow.canResize) window_flags |= ImGuiWindowFlags_NoResize;
				
				ImGui::Begin(pWindow.title.c_str(), pWindow.canClose ? &winCross : NULL, window_flags);

				ImVec2 wpos = ImGui::GetCursorScreenPos();
				ImVec2 mCur = ImGui::GetMousePos();

				if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) pSelected = -1;

				for (int i = 0; i < ELEMENTS_COUNT; ++i)
				{
					if (pWindow.elements[i].element < 0) continue;
					if (!ImGui::IsMouseDown(1) && pMoving >= 0) pMoving = -1;

					bool wasChecked = pWindow.elements[i].checked;
					std::string text = "##" + std::to_string(i);
					char buf[2] = { 0 };

					ImGui::SetCursorPos(pWindow.elements[i].pos);
					switch (pWindow.elements[i].element)
					{
					case 0:
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(pWindow.elements[i].color[0][0], pWindow.elements[i].color[0][1], pWindow.elements[i].color[0][2], pWindow.elements[i].color[0][3]));
						ImGui::Text(pWindow.elements[i].label.c_str());
						ImGui::PopStyleColor();
						if (!ImGui::IsMouseDown(1) && pMoving >= 0) pMoving = -1;
						if ((pSelected == i && ImGui::IsItemHovered() || pMoving == i) && ImGui::IsMouseDown(1)) pMoving = i;
						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0, false)) pSelected = i;
						if (pMoving == i)
							pWindow.elements[i].pos = ImVec2(mCur.x - wpos.x - 10, mCur.y - wpos.y + 10);
						break;
					case 1:
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(pWindow.elements[i].color[0][0], pWindow.elements[i].color[0][1], pWindow.elements[i].color[0][2], pWindow.elements[i].color[0][3]));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(pWindow.elements[i].color[1][0], pWindow.elements[i].color[1][1], pWindow.elements[i].color[1][2], pWindow.elements[i].color[1][3]));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(pWindow.elements[i].color[2][0], pWindow.elements[i].color[2][1], pWindow.elements[i].color[2][2], pWindow.elements[i].color[2][3]));
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(pWindow.elements[i].color[3][0], pWindow.elements[i].color[3][1], pWindow.elements[i].color[3][2], pWindow.elements[i].color[3][3]));
						ImGui::Button(pWindow.elements[i].label.c_str(), pWindow.elements[i].size);
						ImGui::PopStyleColor(4);
						if ((pSelected == i && ImGui::IsItemHovered() || pMoving == i) && ImGui::IsMouseDown(1)) pMoving = i;
						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0, false)) pSelected = i;
						if (pMoving == i)
							pWindow.elements[i].pos = ImVec2(mCur.x - wpos.x - 10, mCur.y - wpos.y + 10);
						break;
					case 2:
						ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(pWindow.elements[i].color[0][0], pWindow.elements[i].color[0][1], pWindow.elements[i].color[0][2], pWindow.elements[i].color[0][3]));
						ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(pWindow.elements[i].color[1][0], pWindow.elements[i].color[1][1], pWindow.elements[i].color[1][2], pWindow.elements[i].color[1][3]));
						ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(pWindow.elements[i].color[2][0], pWindow.elements[i].color[2][1], pWindow.elements[i].color[2][2], pWindow.elements[i].color[2][3]));
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(pWindow.elements[i].color[3][0], pWindow.elements[i].color[3][1], pWindow.elements[i].color[3][2], pWindow.elements[i].color[3][3]));
						ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(pWindow.elements[i].color[4][0], pWindow.elements[i].color[4][1], pWindow.elements[i].color[4][2], pWindow.elements[i].color[4][3]));
						
						ImGui::Checkbox(pWindow.elements[i].label.c_str(), &pWindow.elements[i].checked);
						pWindow.elements[i].checked = wasChecked;
						ImGui::PopStyleColor(5);
						if ((pSelected == i && ImGui::IsItemHovered() || pMoving == i) && ImGui::IsMouseDown(1)) pMoving = i;
						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0, false)) pSelected = i;
						if (pMoving == i)
							pWindow.elements[i].pos = ImVec2(mCur.x - wpos.x - 10, mCur.y - wpos.y + 10);
						break;
					case 3:
						ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(pWindow.elements[i].color[0][0], pWindow.elements[i].color[0][1], pWindow.elements[i].color[0][2], pWindow.elements[i].color[0][3]));
						ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(pWindow.elements[i].color[1][0], pWindow.elements[i].color[1][1], pWindow.elements[i].color[1][2], pWindow.elements[i].color[1][3]));
						ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(pWindow.elements[i].color[2][0], pWindow.elements[i].color[2][1], pWindow.elements[i].color[2][2], pWindow.elements[i].color[2][3]));
						ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(pWindow.elements[i].color[3][0], pWindow.elements[i].color[3][1], pWindow.elements[i].color[3][2], pWindow.elements[i].color[3][3]));
						ImGui::PushItemWidth(pWindow.elements[i].size.x);
						ImGui::InputTextWithHint(text.c_str(), pWindow.elements[i].label.c_str(), buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_ReadOnly);
						ImGui::PopItemWidth();
						ImGui::PopStyleColor(4);
						if ((pSelected == i && ImGui::IsItemHovered() || pMoving == i) && ImGui::IsMouseDown(1)) pMoving = i;
						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0, false)) pSelected = i;
						if (pMoving == i)
							pWindow.elements[i].pos = ImVec2(mCur.x - wpos.x - 10, mCur.y - wpos.y + 10);
						break;
					}
				}

				if (pShowGrid)
				{
					float count = ImGui::GetWindowHeight() / pGridSize;
					for (int i = 0; i < count; ++i)
					{
						int offset = i * pGridSize;
						ImGui::GetWindowDrawList()->AddLine(ImVec2(wpos.x, wpos.y + offset), ImVec2(wpos.x + ImGui::GetWindowWidth(), wpos.y + offset), 0x30000000, 1.f);
					}
					count = ImGui::GetWindowWidth() / pGridSize;
					for (int i = 0; i < count; ++i)
					{
						int offset = i * pGridSize;
						ImGui::GetWindowDrawList()->AddLine(ImVec2(wpos.x + offset, wpos.y), ImVec2(wpos.x + offset, wpos.y + ImGui::GetWindowHeight()), 0x30000000, 1.f);
					}
				}

				ImGui::End();
				ImGui::PopStyleColor(2);
				ImGui::PopStyleVar();
			}
		}

		ImGui::PopFont();
		ImGui::EndFrame();
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * 255.0f), (int)(clear_color.y * 255.0f), (int)(clear_color.z * 255.0f), (int)(clear_color.w * 255.0f));
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}
		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice();
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}


bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	// Create the D3DDevice
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;

	return true;
}

void CleanupDeviceD3D()
{
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}