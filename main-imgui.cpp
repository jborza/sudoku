// based on Dear ImGui: standalone example application for DirectX 9
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include <stdio.h>
#include "Game.h"
#include "Solver.h"
#include "sudoku.h"
#include "set_tools.h"
#include "imgui_internal.h"

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

static ImFont* AddDefaultFont(float pixel_size)
{
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig config;
	config.SizePixels = pixel_size;
	config.OversampleH = config.OversampleV = 1;
	config.PixelSnapH = true;
	ImFont* font = io.Fonts->AddFontDefault(&config);
	return font;
}

float sudokuScale = 3.0f;
HintData currentHintData;

// Main code
#if 1
int main(int, char**)
#else
int main_(int, char**)
#endif
{
	// Create application window
	//ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Sudoku ImGui"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}

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
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

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


	ImFont* fontA = AddDefaultFont(13*sudokuScale);
	ImFont* fontSmall = AddDefaultFont(7 * sudokuScale);
	ImFont* fontLarge = AddDefaultFont(21 * sudokuScale);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	auto color_yellow = IM_COL32(231, 255, 0, 40);
	auto color_magenta = IM_COL32(190, 0, 255, 60);
	auto color_grayblue = IM_COL32(88, 216, 245, 110);

	// TODO refactor
	Game game;
	game.Load("test-hiddensingle.txt");
	game.grid.AutoNoteUser();
	//game.grid.AutoNoteUser();	

	//GAME control globals
	int currentlySelectedNumber = 0;

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGuiStyle& style = ImGui::GetStyle();
		style.CellPadding.x = 0;
		style.CellPadding.y = 0;
		style.FramePadding.x = 0;
		style.FramePadding.y = 0;
		style.WindowPadding.x = 0;
		style.WindowPadding.y = 0;
		style.Colors[ImGuiCol_TableRowBg] = style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0, 0, 0, 0);// IM_COL32(0, 0, 0, 0);


		// 3. Show another simple window.
		if (show_another_window)
		{


			ImGui::SetNextWindowPos(ImVec2(50, 30));
			ImGui::SetNextWindowSize(ImVec2(320 * sudokuScale, 240 * sudokuScale));
			ImGui::Begin("Game Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			//HelpMarker("This demonstrate embedding a table into another table cell.");


			//create numbers table @ 217, 100
			static ImGuiTableFlags flags = ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX;// ImGuiTableFlags_BordersOuter;// ImGuiTableFlags_Borders;

			static int selected = 0;

			ImGui::BeginChild("left pane", ImVec2(218 * sudokuScale, 0 * sudokuScale), false);
			if (ImGui::BeginTable("grid", 3, ImGuiTableFlags_Borders))
			{
				for (int row = 0; row < 3; row++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < 3; column++)
					{
						ImGui::TableSetColumnIndex(column);

						//create sub-table for the house
						static ImGuiTableFlags innerFlags = ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_BordersInner;
						if (ImGui::BeginTable("table_nested", 3, innerFlags)) {
							for (int houserow = 0; houserow < 3; houserow++) {
								ImGui::TableNextRow();
								for (int housecolumn = 0; housecolumn < 3; housecolumn++) {
									int cellrow = row * 3 + houserow;
									int cellcol = column * 3 + housecolumn;
									Cell* cell = game.grid.GetCell(cellrow, cellcol);

									ImGui::TableNextColumn();
									if (contains(currentHintData.cellsToHighlight, cell)) {
										ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color_grayblue);
									}
									if (cell->hasValue()) {
										char buf[32];
										sprintf(buf, "%d", cell->value);
										ImGui::PushFont(fontLarge);
										if (ImGui::Selectable(buf)) {
											//replace with a different number
											cell->SetValue(currentlySelectedNumber);
										}
										//ImGui::Text(cell->hasValue ? buf : "");
										ImGui::PopFont();
									}
									else {
										//draw tiny options https://github.com/ocornut/imgui/issues/1018 
										//TODO draw selectable over this cell - https://github.com/ocornut/imgui/issues/2184
										//TODO see Using explicit size:
#if 0
										bool selected;
										if (ImGui::Selectable("##title", &selected, ImGuiSelectableFlags_AllowItemOverlap, ImVec2(8, 8))) {
											//todo do something
											cell->SetValue(currentlySelectedNumber);
										}
										ImGui::SameLine();
										ImGui::SetItemAllowOverlap();

#endif

										ImGuiTableFlags optionFlags = ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_RowBg;// | ImGuiTableFlags_BordersOuter;
										if (ImGui::BeginTable("table_options", 3, optionFlags)) {
											ImGui::PushFont(fontSmall);
											for (int optionRow = 0; optionRow < 3; optionRow++) {
												ImGui::TableNextRow();
												for (int optionCol = 0; optionCol < 3; optionCol++) {
													ImGui::TableNextColumn();
													int option = optionRow * 3 + optionCol + 1;
													if (contains(cell->hints, option)) {
														if (contains(cell->crossedOutHints, option))
															ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(255, 255, 0, 255));
														char hintBuf[2];
														sprintf(hintBuf, "%d", option);
														//TODO highlight user options, deleted options, hints
														if (option == currentlySelectedNumber) {
															ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color_magenta);
														}

														if (cell->crossedOutHints.count(option)) {
															ImGui::PushStyleColor(ImGuiCol_Text, color_yellow);
														}
														if (ImGui::Selectable(hintBuf, false, ImGuiSelectableFlags_AllowItemOverlap)) {
															cell->SetValue(currentlySelectedNumber);
														}
														if (cell->crossedOutHints.count(option)) {
															ImGui::PopStyleColor();
														}

													}
													else {
														ImGui::Text(" ");
													}
												}
											}
											ImGui::PopFont();
											ImGui::EndTable();
										}
									}

								}
							}
							ImGui::EndTable();
						}
					}
				}
				ImGui::EndTable();
			}
			ImGui::EndChild();

			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Text("Time: 03:14");
			ImGui::Text("Score: 1234");
			if (ImGui::Button("Hint")) {
				//TODO number clicked
				Solver solver;
				auto hint = solver.Hint(&(game.grid));
				currentHintData = hint;
			}
			ImGui::SameLine();
			if (ImGui::Button("Apply")) {
				//TODO number clicked
				Solver solver;
				auto hint = solver.Hint(&(game.grid));
				currentHintData = hint;
				solver.ApplyHint(&game.grid, hint);
			}
			if (ImGui::Button("Undo")) {
				//TODO number clicked
			}
			if (ImGui::Button("Autonote")) {
				game.grid.AutoNoteUser();
			}
			// numbers table
			// TODO highligh action, button disable
			if (ImGui::BeginTable("numbers", 3, ImGuiTableFlags_Borders))
			{
				for (int row = 0; row < 3; row++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < 3; column++)
					{
						ImGui::TableSetColumnIndex(column);
						char buf[32];
						int num = row * 3 + column + 1;
						snprintf(buf, 32, "%d", num);

						// simulated "disabled" number 7 when we get there
						//if (num == 7)
							//ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
						if (ImGui::Button(buf, ImVec2(35*sudokuScale, 35 * sudokuScale))) {
							//TODO number clicked
							currentlySelectedNumber = num;
						}
						//if (num == 7)
							//ImGui::PopStyleVar();
					}
				}
				ImGui::EndTable();
				char buf2[32];
				sprintf(buf2, "Selected: %d", currentlySelectedNumber);
				ImGui::Text(buf2);
			}

			ImGui::EndGroup();
			ImGui::PushFont(fontSmall);

			ImGui::Text(currentHintData.message.c_str());
			ImGui::PopFont();
			ImGui::End();

		}

		// Rendering
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

		// Handle loss of D3D9 device
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

// Helper functions

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
