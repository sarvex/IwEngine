#pragma once

#include "iw/engine/System.h"
#include "Components/CorePixels.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "Events.h"
#include "Helpers.h"
#include "ECS.h"

#include <array>

struct CorePixelsSystem : iw::SystemBase
{
	iw::SandLayer* m_sand;

	CorePixelsSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase ("Core pixels")
		, m_sand         (sand)
	{}

	void Update() override;

	bool On(iw::ActionEvent& e) override;

	void RemoveCorePixel(
		entity entity,
		int index);
};
