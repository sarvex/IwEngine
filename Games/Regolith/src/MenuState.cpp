#include "MenuState.h"

#define LoadTexture(x) g_app.Asset->Load<iw::Texture>(std::string("textures/SpaceGame/") + x)

std::unordered_map<std::string, void*> g_imgs;
std::unordered_map<std::string, bool>  g_hovered;
iw::AppVars g_app;

void SetMenuStateAppVars(iw::AppVars& app)
{
	g_app = app;
}

bool Button(const std::string& name)
{
	bool isClicked = ImGui::Button(name.c_str());
	PlayHoverOrClickSound(name);
	return isClicked;
}

bool Checkbox(const std::string& name, bool& active)
{
	bool isClicked = ImGui::Checkbox(name.c_str(), &active);
	PlayHoverOrClickSound(name);
	return isClicked;
}

bool SliderFloat(const std::string& name, float& value, float min, float max, const char* fmt)
{
	bool isClicked = ImGui::SliderFloat(name.c_str(), &value, min, max, fmt);
	value = iw::clamp(value, min, max);
	PlayClickSound(name);
	return isClicked;
}

void PlayHoverOrClickSound(const std::string& name)
{
	PlayHoverSound(name);
	PlayClickSound(name);
}

void PlayHoverSound(const std::string& name)
{
	bool isClicked = ImGui::IsItemClicked();
	if (isClicked) {
		g_app.Bus->push<PlaySound_Event>("event:/ui/click");
	}
}

void PlayClickSound(const std::string& name)
{
	bool isHovered = ImGui::IsItemHovered();

	if (isHovered) {
		if (!g_hovered[name]) {
			g_app.Bus->push<PlaySound_Event>("event:/ui/hover");
		}

		g_hovered[name] = true;
	}

	else {
		g_hovered[name] = false;
	}
}

void RegisterImage(const std::string& str)
{
	auto img = LoadTexture(str);
	img->SetFilter(iw::NEAREST);
	img->Initialize(g_app.Renderer->Now->Device);

	RegisterImage(str, (void*)(size_t)img->Handle()->Id());

}

void RegisterImage(const std::string& str, void* img)
{
	g_imgs[str] = img;
}

void* Image(const std::string& str)
{
	return g_imgs.at(str);
}
