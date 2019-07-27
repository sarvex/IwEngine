#include "iw/engine/EntryPoint.h"
#include "GameLayer.h"

struct Transform { float x, y, z; };
struct Velocity  { float vx, vy, vz; };
struct Collider  { float count; };

class Game 
	: public IwEngine::Application
{
public:
	Game() {
		InputManager.CreateDevice<IwInput::Mouse>();
		//InputManager.CreateDevice<IwInput::RawKeyboard>();

		PushLayer(new GameLayer());
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
