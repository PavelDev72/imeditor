#include "main.hpp"
#include "export.hpp"

void ExportProject(HWND hwnd, Window pWindow)
{
	std::string exportText = "-- Export Info: ImGUI Editor 0.0.1 by Pakulichev\n\nlocal imgui = require 'mimgui';\nlocal ffi = require 'ffi';\nlocal encoding = require 'encoding';\n";
	exportText += "encoding.default = 'CP1251';\nlocal u8 = encoding.UTF8;\n\nlocal pWindow = imgui.new.bool(true); -- main window handle, change to show & hide\n";

	for (int i = 0; i < ELEMENTS_COUNT; ++i)
	{
		if (pWindow.elements[i].element < 0) continue;
		std::string varName;
		switch (pWindow.elements[i].element)
		{
		case 1:
			exportText += "local button" + std::to_string(i) + "Callback = function() end\n";
			break;
		case 2:
			exportText += "local checkbox" + std::to_string(i) + "Checked = imgui.new.bool(" + (pWindow.elements[i].checked ? "true" : "false") + ");\n";
			break;
		case 3:
			exportText += "local inputtext" + std::to_string(i) + "Value = imgui.new.char[128]();\n";
			break;
		}
	}
	exportText += "\nimgui.OnInitialize(function()\n\timgui.StyleColorsLight();\n\timgui.GetIO().IniFilename = nil\n\tscrX, scrY = getScreenResolution()\n\t";
	exportText += "local glyph_ranges = imgui.GetIO().Fonts:GetGlyphRangesCyrillic();\n\t--imgui.GetIO().Fonts:Clear();\n\timgui.GetIO().Fonts:AddFontFromFileTTF(getFolderPath(0x14) .. '\\arial.ttf', 16, nil, glyph_ranges);\n\t";
	exportText += "imgui.InvalidateFontsTexture();\nend);\n\nimgui.OnFrame(function() return pWindow[0] end,\nfunction(self)\n\tself.HideCursor = false;\n\tself.LockPlayer = true;\n\t";
	exportText += "imgui.SetNextWindowPos(imgui.ImVec2(scrX / 2, scrY / 2), imgui.Cond.FirstUseEver, imgui.ImVec2(0.5, 0.5));\n\timgui.SetNextWindowSize(imgui.ImVec2(" + std::to_string(pWindow.size.x) + ", " + std::to_string(pWindow.size.y) + "), imgui.Cond.FirstUseEver);\n\t";

	std::string flags;
	if (!pWindow.canMinimize)
	{
		if (flags.length() < 1) flags += "imgui.WindowFlags.NoCollapse";
		else flags += "+ imgui.WindowFlags.NoCollapse";
	}
	if (!pWindow.canMove)
	{
		if (flags.length() < 1) flags += "imgui.WindowFlags.NoMove";
		else flags += "+ imgui.WindowFlags.NoMove";
	}
	if (!pWindow.canResize)
	{
		if (flags.length() < 1) flags += "imgui.WindowFlags.NoResize";
		else flags += "+ imgui.WindowFlags.NoResize";
	}

	std::string colorStr;
	colorStr = std::to_string(pWindow.titleCol[0]);
	for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.titleCol[ii]);
	exportText += "imgui.PushStyleColor(imgui.Col.TitleBgActive, imgui.ImVec4("+colorStr+"));\n\t";
	colorStr = std::to_string(pWindow.windowCol[0]);
	for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.windowCol[ii]);
	exportText += "imgui.PushStyleColor(imgui.Col.WindowBg, imgui.ImVec4(" + colorStr + "));\n\t";
	exportText += "imgui.Begin('" + pWindow.title + "', " + (pWindow.canClose ? "pWindow" : "nil") + (flags.length() < 1 ? ")" : ", " + flags + ");\n");

	for (int i = 0; i < ELEMENTS_COUNT; ++i)
	{
		if (pWindow.elements[i].element < 0) continue;
		switch (pWindow.elements[i].element)
		{
		case 0:
			colorStr = std::to_string(pWindow.elements[i].color[0][0]);
			for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.elements[i].color[0][ii]);
			exportText += "\timgui.PushStyleColor(imgui.Col.Text, imgui.ImVec4(" + colorStr + "));\n";
			exportText += "\timgui.SetCursorPos(imgui.ImVec2(" + std::to_string(pWindow.elements[i].pos.x) + "," + std::to_string(pWindow.elements[i].pos.y) + "));\n";
			exportText += "\timgui.Text('" + pWindow.elements[i].label + "');\n";
			exportText += "\timgui.PopStyleColor();\n";
			break;
		case 1:
			colorStr = std::to_string(pWindow.elements[i].color[0][0]);
			for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.elements[i].color[0][ii]);
			exportText += "\timgui.PushStyleColor(imgui.Col.Button, imgui.ImVec4(" + colorStr + "));\n";
			colorStr = std::to_string(pWindow.elements[i].color[1][0]);
			for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.elements[i].color[1][ii]);
			exportText += "\timgui.PushStyleColor(imgui.Col.ButtonActive, imgui.ImVec4(" + colorStr + "));\n";
			colorStr = std::to_string(pWindow.elements[i].color[2][0]);
			for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.elements[i].color[2][ii]);
			exportText += "\timgui.PushStyleColor(imgui.Col.ButtonHovered, imgui.ImVec4(" + colorStr + "));\n";
			colorStr = std::to_string(pWindow.elements[i].color[3][0]);
			for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.elements[i].color[3][ii]);
			exportText += "\timgui.PushStyleColor(imgui.Col.Text, imgui.ImVec4(" + colorStr + "));\n";
			exportText += "\timgui.SetCursorPos(imgui.ImVec2(" + std::to_string(pWindow.elements[i].pos.x) + "," + std::to_string(pWindow.elements[i].pos.y) + "));\n";
			exportText += "\tif(imgui.Button('" + pWindow.elements[i].label + "', imgui.ImVec2(" + std::to_string(pWindow.elements[i].size.x) + "," + std::to_string(pWindow.elements[i].size.y) + "))) then\n";
			exportText += "\t\tbutton" + std::to_string(i) + "Callback();\n\tend\n";
			exportText += "\timgui.PopStyleColor(4);\n";
			break;
		case 2:
			colorStr = std::to_string(pWindow.elements[i].color[0][0]);
			for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.elements[i].color[0][ii]);
			exportText += "\timgui.PushStyleColor(imgui.Col.FrameBg, imgui.ImVec4(" + colorStr + "));\n";
			colorStr = std::to_string(pWindow.elements[i].color[1][0]);
			for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.elements[i].color[1][ii]);
			exportText += "\timgui.PushStyleColor(imgui.Col.FrameBgActive, imgui.ImVec4(" + colorStr + "));\n";
			colorStr = std::to_string(pWindow.elements[i].color[2][0]);
			for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.elements[i].color[2][ii]);
			exportText += "\timgui.PushStyleColor(imgui.Col.FrameBgHovered, imgui.ImVec4(" + colorStr + "));\n";
			colorStr = std::to_string(pWindow.elements[i].color[3][0]);
			for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.elements[i].color[3][ii]);
			exportText += "\timgui.PushStyleColor(imgui.Col.Text, imgui.ImVec4(" + colorStr + "));\n";
			colorStr = std::to_string(pWindow.elements[i].color[4][0]);
			for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.elements[i].color[4][ii]);
			exportText += "\timgui.PushStyleColor(imgui.Col.CheckMark, imgui.ImVec4(" + colorStr + "));\n";
			exportText += "\timgui.SetCursorPos(imgui.ImVec2(" + std::to_string(pWindow.elements[i].pos.x) + "," + std::to_string(pWindow.elements[i].pos.y) + "));\n";
			exportText += "\timgui.Checkbox('" + pWindow.elements[i].label + "', checkbox" + std::to_string(i) + "Checked);\n";
			exportText += "\timgui.PopStyleColor(5);\n";
			break;
		case 3:
			colorStr = std::to_string(pWindow.elements[i].color[0][0]);
			for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.elements[i].color[0][ii]);
			exportText += "\timgui.PushStyleColor(imgui.Col.FrameBg, imgui.ImVec4(" + colorStr + "));\n";
			colorStr = std::to_string(pWindow.elements[i].color[1][0]);
			for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.elements[i].color[1][ii]);
			exportText += "\timgui.PushStyleColor(imgui.Col.FrameBgActive, imgui.ImVec4(" + colorStr + "));\n";
			colorStr = std::to_string(pWindow.elements[i].color[2][0]);
			for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.elements[i].color[2][ii]);
			exportText += "\timgui.PushStyleColor(imgui.Col.FrameBgHovered, imgui.ImVec4(" + colorStr + "));\n";
			colorStr = std::to_string(pWindow.elements[i].color[3][0]);
			for (int ii = 1; ii < 4; ++ii) colorStr += ',' + std::to_string(pWindow.elements[i].color[3][ii]);
			exportText += "\timgui.PushStyleColor(imgui.Col.Text, imgui.ImVec4(" + colorStr + "));\n";
			exportText += "\timgui.SetCursorPos(imgui.ImVec2(" + std::to_string(pWindow.elements[i].pos.x) + "," + std::to_string(pWindow.elements[i].pos.y) + "));\n";
			exportText += "\timgui.PushItemWidth(" + std::to_string(pWindow.elements[i].size.x) + ");\n";
			exportText += "\timgui.InputTextWithHint('##input"+std::to_string(i)+"', '" + pWindow.elements[i].label + "', inputtext" + std::to_string(i) + "Value, ffi.sizeof(inputtext" + std::to_string(i) + "Value)-1);\n";
			exportText += "\timgui.PopItemWidth();\n";
			exportText += "\timgui.PopStyleColor(4);\n";
			break;
		}
	}

	exportText += "\timgui.End();\n\timgui.PopStyleColor(2);\nend);";

	std::ofstream exportFile("export.lua");
	exportFile << exportText;
	exportFile.close();

	MessageBoxA(hwnd, (LPCSTR)"ImGUI Code was successfully exported to export.lua!", (LPCSTR)"Code Exported!", 0);
}