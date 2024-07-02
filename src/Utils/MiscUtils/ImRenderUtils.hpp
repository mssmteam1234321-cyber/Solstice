#pragma once
#include <imgui.h>
#include <string>
#include <vector>
#include <windows.h>
#include <Utils/ProcUtils.hpp>
//
// Created by vastrakai on 6/29/2024.
//

struct ImVec4;



class ImRenderUtils
{
public:
	static void addBlur(const ImVec4& pos, float strength, float radius = 0.f);

	inline static void drawText(ImVec2 pos, std::string* textStr, const ImColor& color, float textSize, float alpha, bool shadow = false, int index = 0, ImDrawList* d = ImGui::GetForegroundDrawList())
	{
		if (!ImGui::GetCurrentContext())
			return;
		ImFont* font = ImGui::GetFont();



		ImVec2 textPos = ImVec2(pos.x, pos.y);
		constexpr ImVec2 shadowOffset = ImVec2(1.f, 1.f);
		constexpr ImVec2 shadowOffsetMinecraft = ImVec2(1.45f, 1.45f);

		if (shadow)
		{
			ImVec2 added = textPos;
			added.x = added.x + shadowOffset.x;
			added.y = added.y + shadowOffset.y;
			/*if (font == ImGui::GetIO().Fonts->Fonts[4]) {
				d->AddText(font, (textSize * 18), textPos + shadowOffsetMinecraft, ImColor(color.Value.x * 0.2f, color.Value.y * 0.2f, color.Value.z * 0.2f, alpha * 0.7f), textStr->c_str());
			}
			else {*/
				d->AddText(font, (textSize * 18), added, ImColor(color.Value.x * 0.03f, color.Value.y * 0.03f, color.Value.z * 0.03f, alpha * 0.9f), textStr->c_str());
			//}
		}

		d->AddText(font, (textSize * 18), textPos, ImColor(color.Value.x, color.Value.y, color.Value.z, alpha), textStr->c_str());
	};

	static void fillRectangle(ImVec4 pos, const ImColor& color, float alpha, float radius = 0.f, ImDrawList* list = ImGui::GetForegroundDrawList())
	{
		if (!ImGui::GetCurrentContext())
			return;

		list->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.z, pos.w), ImColor(color.Value.x, color.Value.y, color.Value.z, alpha), radius);
	}

	static void fillShadowRectangle(ImVec4 pos, const ImColor& color, float alpha, float thickness, ImDrawFlags flags, float radius = 0.f, ImDrawList* list = ImGui::GetForegroundDrawList())
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImVec2 offset = ImVec2(0, 0);
		list->AddShadowRect(ImVec2(pos.x, pos.y), ImVec2(pos.z, pos.w), ImColor(color.Value.x, color.Value.y, color.Value.z, alpha), thickness, offset, flags, radius);
	}

	static void fillShadowCircle(ImVec2 pos, float radius, const ImColor& color, float alpha, float thickness, ImDrawFlags flags, float segments = 12.f)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetForegroundDrawList();
		ImVec2 offset = ImVec2(0, 0);
		list->AddShadowCircle(ImVec2(pos.x, pos.y), radius, ImColor(color.Value.x, color.Value.y, color.Value.z, alpha), thickness, offset, flags, radius);
	}

	static void drawRoundRect(ImVec4 pos, const ImDrawFlags& flags, float radius, const ImColor& color, float alpha, float lineWidth)
	{
		if (!ImGui::GetCurrentContext())
			return;
		const auto d = ImGui::GetForegroundDrawList();
		d->AddRect(ImVec2(pos.x, pos.y), ImVec2(pos.z, pos.w), ImColor(color.Value.x, color.Value.y, color.Value.z, alpha), radius, flags, lineWidth);
	}

	static void fillGradientOpaqueRectangle(ImVec4 pos, const ImColor& firstColor, const ImColor& secondColor, float firstAlpha, float secondAlpha)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetForegroundDrawList();

		ImVec2 topLeft = ImVec2(pos.x, pos.y);
		ImVec2 bottomRight = ImVec2(pos.z, pos.w);

		list->AddRectFilledMultiColor(topLeft, bottomRight,
			ImColor(firstColor.Value.x, firstColor.Value.y, firstColor.Value.z, secondAlpha),
			ImColor(secondColor.Value.x, secondColor.Value.y, secondColor.Value.z, secondAlpha),
			ImColor(secondColor.Value.x, secondColor.Value.y, secondColor.Value.z, firstAlpha),
			ImColor(firstColor.Value.x, firstColor.Value.y, firstColor.Value.z, firstAlpha));
	}

	static void fillRoundedGradientRectangle(ImVec4 pos, const ImColor& firstColor, const ImColor& secondColor, float radius, float firstAlpha, float secondAlpha)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetForegroundDrawList();

		ImVec2 topLeft = ImVec2(pos.x, pos.y);
		ImVec2 bottomRight = ImVec2(pos.z, pos.w);

		list->AddRectFilledMultiColor(topLeft, bottomRight,
			ImColor(firstColor.Value.x, firstColor.Value.y, firstColor.Value.z, secondAlpha),
			ImColor(secondColor.Value.x, secondColor.Value.y, secondColor.Value.z, secondAlpha),
			ImColor(secondColor.Value.x, secondColor.Value.y, secondColor.Value.z, firstAlpha),
			ImColor(firstColor.Value.x, firstColor.Value.y, firstColor.Value.z, firstAlpha));
	}

	static void fillCircle(ImVec2 center, float radius, const ImColor& color, float alpha, int segments)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetForegroundDrawList();
		list->AddCircleFilled(ImVec2(center.x , center.y), radius, ImColor(color.Value.x, color.Value.y, color.Value.z, alpha), segments);
	}

	static inline ImVec2 getScreenSize() {
		RECT desktop;

		GetWindowRect(ProcUtils::getMinecraftWindow(), &desktop);
		int w = desktop.right - desktop.left;
		int h = desktop.bottom - desktop.top;
		// not in fullscreen
		if (desktop.top != 0 || desktop.left != 0) {
			//w -= 30;
			//h -= 45;
		}
		return ImVec2(w, h);
	}

	static bool isFullScreen() {
		RECT desktop;

		GetWindowRect(ProcUtils::getMinecraftWindow(), &desktop);
		if (desktop.top != 0 || desktop.left != 0)
			return false;
		return true;
	}

	static inline float getTextHeightStr(std::string* textStr, float textSize) {
		return ImGui::GetFont()->CalcTextSizeA(textSize * 18, FLT_MAX, -1, textStr->c_str()).y;
	}

	static inline float getTextWidth(std::string* textStr, float textSize)
	{
		return ImGui::GetFont()->CalcTextSizeA(textSize * 18, FLT_MAX, -1, textStr->c_str()).x;
	}

	static inline float getTextHeight(float textSize)
	{
		return ImGui::GetFont()->CalcTextSizeA(textSize * 18, FLT_MAX, -1, "").y;
	}

	static float getDeltaTime() {
		return ImGui::GetIO().DeltaTime;
	}

	static ImVec2 getMousePos() {
		return ImGui::GetIO().MousePos;
	}

	inline static bool isMouseOver(ImVec4(pos))
	{
		ImVec2 mousePos = getMousePos();
		return mousePos.x >= pos.x && mousePos.y >= pos.y && mousePos.x < pos.z && mousePos.y < pos.w;
	}
};
