#pragma once

#include "iw/engine/Layer.h"

namespace IwEngine {
	class IWENGINE_API ImGuiLayer
		: public Layer
	{
	private:
		int counter = 0;
	public:
		ImGuiLayer();
		~ImGuiLayer();

		int Initilize();
		void Destroy();
		void Update();

		bool On(WindowResizedEvent& event);
		bool On(MouseMovedEvent&    event);
		bool On(MouseButtonEvent&   event);
	};
}
