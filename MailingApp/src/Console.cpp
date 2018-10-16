#include "Console.h"
#include "imgui/imgui.h"
#include <stdlib.h> 
#include <ctype.h>
#include <stdio.h>  
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleClient.h"

// Constructors =================================
Console::Console(const ModuleClient* target): target((ModuleClient*)target)
{
	ClearLog();
	memset(InputBuf, 0, sizeof(InputBuf));
	AddLog("--- Console Ready!");
}

// Destructors ==================================
Console::~Console()
{
	ClearLog();
}

// Functionality ================================
void Console::ClearLog()
{
	for (int i = 0; i < Items.Size; i++)
		free(Items[i]);
	Items.clear();
	ScrollToBottom = true;
}

void Console::AddLog(const char * fmt, ...) //IM_PRINTFARGS(2)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	Items.push_back(Strdup(buf));
	ScrollToBottom = true;
}

void Console::_Draw(const char * title, bool * p_open)
{
	//Set window pos and size
	ImGui::SetNextWindowSize(ImVec2(App->modWindow->GetWidth() * 0.85f, App->modWindow->GetHeight() * 0.3f));
	ImGui::SetNextWindowPos(ImVec2(App->modWindow->GetWidth() * 0.4f + 5, App->modWindow->GetHeight() * 0.4f));

	//Init window
	if (!ImGui::Begin(title, p_open, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar))
	{
		//Close if error
		ImGui::End();
		return;
	}

	//Console Buttons
	/*if (ImGui::SmallButton("Clear"))
	{
		ClearLog();
	}*/
	ImGui::SameLine();
	if (ImGui::SmallButton("Scroll to bottom"))
	{
		ScrollToBottom = true;
	}
	ImGui::SameLine();

	ImGui::Separator();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	static ImGuiTextFilter filter;
	ImGui::PopStyleVar();

	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
	
	
	/*
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Clear"))
			{
				ClearLog();
			}
			ImGui::EndPopup();
		}
	*/

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

	for (int i = 0; i < Items.Size; i++)
	{
		const char* item = Items[i];
		if (!filter.PassFilter(item))
			continue;
		ImVec4 col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // A better implementation may store a type per-item. For the sample let's just parse the text.
		if (strstr(item, "[error]")) col = ImColor(1.0f, 0.4f, 0.4f, 1.0f);
		else if (strncmp(item, "# ", 2) == 0) col = ImColor(1.0f, 0.78f, 0.58f, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, col);
		ImGui::TextUnformatted(item);
		ImGui::PopStyleColor();
	}

	if (ScrollToBottom)
	{
		ImGui::SetScrollHere();
	}
	ScrollToBottom = false;
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::Separator();

	if (target != nullptr)
	{
		// Command-line
		strcpy_s(InputBuf, static_buffer.c_str());
		if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue, nullptr, (void*)this))
		{
			char* input_end = InputBuf + strlen(InputBuf);
			while (input_end > InputBuf && input_end[-1] == ' ') input_end--; *input_end = 0;
			if (InputBuf[0])
			{
				ExecCommand(InputBuf);
			}
			strcpy_s(InputBuf, "");
		}
	}

	// Demonstrate keeping auto focus on the input box
	if (ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
	{
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
	}

	ImGui::End();
}

void Console::ExecCommand(const char * command_line)
{
		target->_SendGlobalMessage(command_line);
}