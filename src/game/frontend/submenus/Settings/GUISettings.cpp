#include "GUISettings.hpp"
#include "core/commands/ColorCommand.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/frontend/manager/styles/Themes.hpp"
#include "core/i18n/Language.hpp"
#include <regex>

namespace YimMenu
{
	static std::vector<std::unique_ptr<ColorCommand>> g_ColorCommands;
	static std::unordered_map<std::string, float> g_RoundingValues;
	static std::unordered_map<std::string, std::unique_ptr<FloatCommand>> g_FloatCommands; // Global map for all FloatCommands
	static bool g_ColorInit = false, g_RoundingInit = false;

	static const std::string kSettingsFile = [] {
		char* path = nullptr;
		size_t len = 0;
		_dupenv_s(&path, &len, "APPDATA");
		std::string full = std::string(path ? path : "") + "\\YimMenuV2\\themes.json";
		free(path);
		return full;
	}();

	static std::string GetStyleChineseName(const std::string& name)
	{
		static const std::unordered_map<std::string, std::string> map = {
			// Colors
			{"Text", "文本"},
			{"TextDisabled", "禁用文本"},
			{"WindowBg", "窗口背景"},
			{"ChildBg", "子窗口背景"},
			{"PopupBg", "弹窗背景"},
			{"Border", "边框"},
			{"BorderShadow", "边框阴影"},
			{"FrameBg", "框架背景"},
			{"FrameBgHovered", "框架背景（悬停）"},
			{"FrameBgActive", "框架背景（激活）"},
			{"TitleBg", "标题背景"},
			{"TitleBgActive", "标题背景（激活）"},
			{"TitleBgCollapsed", "标题背景（折叠）"},
			{"MenuBarBg", "菜单栏背景"},
			{"ScrollbarBg", "滚动条背景"},
			{"ScrollbarGrab", "滚动条滑块"},
			{"ScrollbarGrabHovered", "滚动条滑块（悬停）"},
			{"ScrollbarGrabActive", "滚动条滑块（激活）"},
			{"CheckMark", "选中标记"},
			{"SliderGrab", "滑块"},
			{"SliderGrabActive", "滑块（激活）"},
			{"Button", "按钮"},
			{"ButtonHovered", "按钮（悬停）"},
			{"ButtonActive", "按钮（激活）"},
			{"Header", "标题栏"},
			{"HeaderHovered", "标题栏（悬停）"},
			{"HeaderActive", "标题栏（激活）"},
			{"Separator", "分隔线"},
			{"SeparatorHovered", "分隔线（悬停）"},
			{"SeparatorActive", "分隔线（激活）"},
			{"ResizeGrip", "调整大小手柄"},
			{"ResizeGripHovered", "调整大小手柄（悬停）"},
			{"ResizeGripActive", "调整大小手柄（激活）"},
			{"TabHovered", "标签页（悬停）"},
			{"Tab", "标签页"},
			{"TabSelected", "标签页（选中）"},
			{"TabSelectedOverline", "标签页（选中覆盖线）"},
			{"TabDimmed", "标签页（暗淡）"},
			{"TabDimmedSelected", "标签页（暗淡选中）"},
			{"TabDimmedSelectedOverline", "标签页（暗淡选中覆盖线）"},
			{"PlotLines", "折线图"},
			{"PlotLinesHovered", "折线图（悬停）"},
			{"PlotHistogram", "柱状图"},
			{"PlotHistogramHovered", "柱状图（悬停）"},
			{"TableHeader", "表格标题"},
			{"TableBorderStrong", "表格边框（粗）"},
			{"TableBorderLight", "表格边框（细）"},
			{"TableRowBg", "表格行背景"},
			{"TableRowBgAlt", "表格行背景（交替）"},
			{"TextLink", "文本链接"},
			{"TextSelectedBg", "文本选中背景"},
			{"DragDropTarget", "拖放目标"},
			{"NavCursorHighlight", "导航光标高亮"},
			{"NavWindowingHighlight", "导航窗口高亮"},
			{"NavWindowingDimBg", "导航窗口暗背景"},
			{"ModalWindowDimBg", "模态窗口暗背景"},
			// Rounding
			{"WindowRounding", "窗口圆角"},
			{"FrameRounding", "框架圆角"},
			{"GrabRounding", "滑块圆角"},
			{"ScrollbarRounding", "滚动条圆角"},
			{"ChildRounding", "子窗口圆角"},
			{"PopupRounding", "弹窗圆角"},
			{"TabRounding", "标签页圆角"},
			// Layout
			{"WindowPadding", "窗口内边距"},
			{"ItemSpacing", "项目间距"},
			{"ItemInnerSpacing", "项目内部间距"},
			{"TouchExtraPadding", "触控额外边距"},
			{"DisplaySafeAreaPadding", "安全区域边距"},
			{"IndentSpacing", "缩进间距"},
			{"ColumnsMinSpacing", "列最小间距"},
			{"WindowTitleAlign", "窗口标题对齐"},
			{"ButtonTextAlign", "按钮文本对齐"},
			{"SelectableTextAlign", "可选项文本对齐"},
			// Border
			{"WindowBorderSize", "窗口边框大小"},
			{"FrameBorderSize", "框架边框大小"},
			{"TabBorderSize", "标签页边框大小"},
			{"PopupBorderSize", "弹窗边框大小"},
			// Global
			{"GlobalAlpha", "全局透明度"},
			{"DisabledAlpha", "禁用透明度"},
			{"MouseCursorScale", "鼠标光标缩放"},
			{"CurveTessellationTol", "曲线细分容差"},
			// Misc
			{"Horizontal", "水平"},
			{"Vertical", "垂直"},
			{"Modify Colors", "修改颜色"},
			{"Adjust Rounding", "调整圆角"},
			{"Layout & Alignment", "布局与对齐"},
			{"Border Sizes", "边框大小"},
			{"Global Settings", "全局设置"},
			{"Font Configuration", "字体配置"},
			{"Current Scale", "当前缩放"},
			{"Font Scale", "字体缩放"},
			{"Apply Font Scale", "应用字体缩放"},
			{"Colors", "颜色"},
			{"Rounding", "圆角"},
			{"Layout", "布局"},
			{"Border", "边框"},
			{"Global", "全局"},
			{"Fonts", "字体"},
		};

		auto it = map.find(name);
		if (it != map.end())
			return it->second;
		return "";
	}

	static std::string PrettyPrintLabel(const std::string& raw)
	{
		std::string out = raw;
		std::string suffix;
		if (out.size() > 2 && out.compare(out.size() - 2, 2, "_X") == 0)
		{
			out.erase(out.size() - 2);
			suffix = (I18n::g_CurrentLanguage == I18n::Language::ZH) ? "（水平）" : " Horizontal";
		}
		else if (out.size() > 2 && out.compare(out.size() - 2, 2, "_Y") == 0)
		{
			out.erase(out.size() - 2);
			suffix = (I18n::g_CurrentLanguage == I18n::Language::ZH) ? "（垂直）" : " Vertical";
		}

		std::string spaced;
		spaced.reserve(out.size() + 10);
		for (size_t i = 0; i < out.size(); ++i)
		{
			if (i > 0 && isupper(out[i]) && islower(out[i - 1]))
				spaced += ' ';
			spaced += out[i];
		}

		std::regex uscore_re("_");
		spaced = std::regex_replace(spaced, uscore_re, " ");

		if (I18n::g_CurrentLanguage == I18n::Language::ZH)
		{
			std::string zh = GetStyleChineseName(out);
			if (!zh.empty())
				return zh + "（" + spaced + "）" + suffix;
		}

		return spaced + suffix;
	}

	void SyncColorCommandsToStyle()
	{
		auto& style = ImGui::GetStyle();
		for (int i = 0; i < ImGuiCol_COUNT; i++)
			style.Colors[i] = g_ColorCommands[i]->GetState();
	}

	static void SyncRoundingToStyle()
	{
		auto& s = ImGui::GetStyle();
		for (auto& [k, v] : g_RoundingValues)
			if (k == "WindowRounding")
				s.WindowRounding = v;
			else if (k == "FrameRounding")
				s.FrameRounding = v;
			else if (k == "GrabRounding")
				s.GrabRounding = v;
			else if (k == "ScrollbarRounding")
				s.ScrollbarRounding = v;
			else if (k == "ChildRounding")
				s.ChildRounding = v;
			else if (k == "PopupRounding")
				s.PopupRounding = v;
			else if (k == "TabRounding")
				s.TabRounding = v;
	}

	static void LoadSettings()
	{
		if (!std::filesystem::exists(kSettingsFile))
			return;
		std::ifstream file(kSettingsFile);
		nlohmann::json json;
		file >> json;

		// Load colors
		for (int i = 0; i < ImGuiCol_COUNT; ++i)
			if (auto it = json.find(ImGui::GetStyleColorName(i)); it != json.end() && it->is_array())
				g_ColorCommands[i]->SetState(ImVec4((*it)[0], (*it)[1], (*it)[2], (*it)[3]));

		// Load rounding values
		for (const char* key : {"WindowRounding", "FrameRounding", "GrabRounding", "ScrollbarRounding", "ChildRounding", "PopupRounding", "TabRounding"})
			if (auto it = json.find(key); it != json.end())
				g_RoundingValues[key] = *it;

		// Load all float command values (except colors and rounding, which are handled)
		for (auto& [key, cmd] : g_FloatCommands)
		{
			if (auto it = json.find(key); it != json.end() && it->is_number())
			{
				cmd->SetState(it->get<float>());
			}
		}
	}

	static void SaveSettings()
	{
		nlohmann::json json;

		// Save colors
		for (int i = 0; i < ImGuiCol_COUNT; ++i)
		{
			auto c = g_ColorCommands[i]->GetState();
			json[ImGui::GetStyleColorName(i)] = {c.x, c.y, c.z, c.w};
		}

		// Save rounding
		for (auto& [k, v] : g_RoundingValues)
			json[k] = v;

		// Save floats
		for (auto& [k, cmd] : g_FloatCommands)
			json[k] = cmd->GetState();

		std::filesystem::create_directories(std::filesystem::path(kSettingsFile).parent_path());
		std::ofstream(kSettingsFile) << json.dump(4);
	}

	void ApplyThemeToImGui()
	{
		auto& style = ImGui::GetStyle();

		for (int i = 0; i < ImGuiCol_COUNT; ++i)
			style.Colors[i] = g_ColorCommands[i]->GetState();

		style.WindowRounding = g_RoundingValues["WindowRounding"];
		style.FrameRounding = g_RoundingValues["FrameRounding"];
		style.GrabRounding = g_RoundingValues["GrabRounding"];
		style.ScrollbarRounding = g_RoundingValues["ScrollbarRounding"];
		style.ChildRounding = g_RoundingValues["ChildRounding"];
		style.PopupRounding = g_RoundingValues["PopupRounding"];
		style.TabRounding = g_RoundingValues["TabRounding"];
	}


	void InitializeColorCommands()
	{
		if (g_ColorInit)
			return;
		auto& style = ImGui::GetStyle();
		for (int i = 0; i < ImGuiCol_COUNT; ++i)
			g_ColorCommands.emplace_back(std::make_unique<ColorCommand>(
			    "ColorCommand." + std::string(ImGui::GetStyleColorName(i)),
			    ImGui::GetStyleColorName(i),
			    "Edit color for " + std::string(ImGui::GetStyleColorName(i)),
			    style.Colors[i]));

		if (!g_RoundingInit)
		{
			g_RoundingInit = true;
			g_RoundingValues = {
			    {"WindowRounding", style.WindowRounding},
			    {"FrameRounding", style.FrameRounding},
			    {"GrabRounding", style.GrabRounding},
			    {"ScrollbarRounding", style.ScrollbarRounding},
			    {"ChildRounding", style.ChildRounding},
			    {"PopupRounding", style.PopupRounding},
			    {"TabRounding", style.TabRounding}};
		}

		LoadSettings();
		SyncColorCommandsToStyle();
		SyncRoundingToStyle();
		g_ColorInit = true;
	}

	static void DrawStyleVec2(const char* label, float& x, float& y, float min, float max)
	{
		std::string nameX = std::string(label) + "_X";
		std::string nameY = std::string(label) + "_Y";

		// Use global float commands map
		if (!g_FloatCommands.count(nameX))
			g_FloatCommands[nameX] = std::make_unique<FloatCommand>(nameX.c_str(), nameX.c_str(), "Adjust " + nameX, min, max, x);
		if (!g_FloatCommands.count(nameY))
			g_FloatCommands[nameY] = std::make_unique<FloatCommand>(nameY.c_str(), nameY.c_str(), "Adjust " + nameY, min, max, y);

		float newX = g_FloatCommands[nameX]->GetState();
		float newY = g_FloatCommands[nameY]->GetState();

		bool changed = false;
		changed |= ImGui::SliderFloat(PrettyPrintLabel(nameX).c_str(), &newX, min, max, "%.1f");
		changed |= ImGui::SliderFloat(PrettyPrintLabel(nameY).c_str(), &newY, min, max, "%.1f");

		if (changed)
		{
			g_FloatCommands[nameX]->SetState(newX);
			g_FloatCommands[nameY]->SetState(newY);
			x = newX;
			y = newY;
			SaveSettings();
		}
	}

	static void DrawStyleFloat(const char* label, float& v, float min, float max)
	{
		std::string name(label);

		if (!g_FloatCommands.count(name))
			g_FloatCommands[name] = std::make_unique<FloatCommand>(name.c_str(), name.c_str(), "Adjust " + name, min, max, v);

		float newVal = g_FloatCommands[name]->GetState();
		if (ImGui::SliderFloat(PrettyPrintLabel(name).c_str(), &newVal, min, max, "%.1f"))
		{
			g_FloatCommands[name]->SetState(newVal);
			v = newVal;
			SaveSettings();
		}
	}

	static void DrawColorsTab()
	{
		bool changed = false;
		ImGui::Text("%s", L("gui.modify_colors", "Modify Colors:").c_str());
		ImGui::Separator();
		for (int i = 0; i < ImGuiCol_COUNT; ++i)
		{
			auto& cmd = g_ColorCommands[i];
			auto col = cmd->GetState();

			if (ImGui::ColorEdit4(PrettyPrintLabel(ImGui::GetStyleColorName(i)).c_str(), (float*)&col))
			{
				cmd->SetState(col);
				changed = true;
			}
		}
		if (changed)
		{
			SyncColorCommandsToStyle();
			SaveSettings();
		}
	}

	static void DrawRoundingTab()
	{
		bool changed = false;
		ImGui::Text("%s", L("gui.adjust_rounding", "Adjust Rounding:").c_str());
		ImGui::Separator();
		for (auto& [k, v] : g_RoundingValues)
			if (ImGui::SliderFloat(PrettyPrintLabel(k).c_str(), &v, 0.0f, 20.0f, "%.1f"))
				changed = true;

		if (changed)
		{
			SyncRoundingToStyle();
			SaveSettings();
		}
	}

	static void DrawLayoutTab()
	{
		auto& s = ImGui::GetStyle();
		ImGui::Text("%s", L("gui.layout_alignment", "Layout & Alignment:").c_str());
		ImGui::Separator();
		DrawStyleVec2("WindowPadding", s.WindowPadding.x, s.WindowPadding.y, 0.f, 32.f);
		DrawStyleVec2("ItemSpacing", s.ItemSpacing.x, s.ItemSpacing.y, 0.f, 32.f);
		DrawStyleVec2("ItemInnerSpacing", s.ItemInnerSpacing.x, s.ItemInnerSpacing.y, 0.f, 32.f);
		DrawStyleVec2("TouchExtraPadding", s.TouchExtraPadding.x, s.TouchExtraPadding.y, 0.f, 32.f);
		DrawStyleVec2("DisplaySafeAreaPadding", s.DisplaySafeAreaPadding.x, s.DisplaySafeAreaPadding.y, 0.f, 32.f);

		DrawStyleFloat("IndentSpacing", s.IndentSpacing, 0.f, 64.f);
		DrawStyleFloat("ColumnsMinSpacing", s.ColumnsMinSpacing, 0.f, 64.f);

		DrawStyleVec2("WindowTitleAlign", s.WindowTitleAlign.x, s.WindowTitleAlign.y, 0.f, 1.f);
		DrawStyleVec2("ButtonTextAlign", s.ButtonTextAlign.x, s.ButtonTextAlign.y, 0.f, 1.f);
		DrawStyleVec2("SelectableTextAlign", s.SelectableTextAlign.x, s.SelectableTextAlign.y, 0.f, 1.f);
	}

	static void DrawBorderTab()
	{
		auto& s = ImGui::GetStyle();
		ImGui::Text("%s", L("gui.border_sizes", "Border Sizes:").c_str());
		ImGui::Separator();
		DrawStyleFloat("WindowBorderSize", s.WindowBorderSize, 0.f, 8.f);
		DrawStyleFloat("FrameBorderSize", s.FrameBorderSize, 0.f, 8.f);
		DrawStyleFloat("TabBorderSize", s.TabBorderSize, 0.f, 8.f);
		DrawStyleFloat("PopupBorderSize", s.PopupBorderSize, 0.f, 8.f);
	}

	static void DrawGlobalTab()
	{
		auto& s = ImGui::GetStyle();
		ImGui::Text("%s", L("gui.global_settings", "Global Settings:").c_str());
		ImGui::Separator();
		DrawStyleFloat("GlobalAlpha", s.Alpha, 0.1f, 1.f);
		DrawStyleFloat("DisabledAlpha", s.DisabledAlpha, 0.f, 1.f);
		DrawStyleFloat("MouseCursorScale", s.MouseCursorScale, 0.5f, 2.f);
		DrawStyleFloat("CurveTessellationTol", s.CurveTessellationTol, 0.1f, 10.f);
	}

	static void DrawFontTab()
	{
		ImGuiIO& io = ImGui::GetIO();
		static float scale = io.FontGlobalScale;
		ImGui::Text("%s", L("gui.font_config", "Font Configuration:").c_str());
		ImGui::Separator();
		ImGui::Text(L("gui.current_scale", "Current Scale: %.2f").c_str(), io.FontGlobalScale);
		ImGui::SliderFloat(L("gui.font_scale", "Font Scale").c_str(), &scale, 0.5f, 2.0f, "%.2f");
		if (ImGui::Button(L("gui.apply_font_scale", "Apply Font Scale").c_str()))
			io.FontGlobalScale = scale;
	}

	std::shared_ptr<Category> DrawGUISettingsMenu()
	{
		InitializeColorCommands();
		auto imGuiCustomStyle = std::make_shared<Category>(L("category.customize", "Customize"));
		imGuiCustomStyle->AddItem(std::make_unique<ImGuiItem>([] {
		ImGui::Text("%s", L("gui.imgui_style_editor", "ImGui Style Editor").c_str());
		ImGui::Separator();
		if (ImGui::BeginTabBar("StyleTabs"))
		{
			if (ImGui::BeginTabItem(L("tab.colors", "Colors").c_str()))
			{
				DrawColorsTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(L("tab.rounding", "Rounding").c_str()))
			{
				DrawRoundingTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(L("tab.layout", "Layout").c_str()))
			{
				DrawLayoutTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(L("tab.border", "Border").c_str()))
			{
				DrawBorderTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(L("tab.global", "Global").c_str()))
			{
				DrawGlobalTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(L("tab.fonts", "Fonts").c_str()))
			{
				DrawFontTab();
				ImGui::EndTabItem();
			}
				ImGui::EndTabBar();
			}
		}));
		return imGuiCustomStyle;
	}
}
