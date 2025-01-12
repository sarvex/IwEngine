﻿#include "ECS.h"
#include <chrono>

struct Timer
{
	using clock_t     = std::chrono::high_resolution_clock;
	using res_t       = std::chrono::nanoseconds;
	using duration_t  = std::chrono::duration<double, std::milli>;
	using timepoint_t = std::chrono::time_point<clock_t>;

	timepoint_t m_beginPoint;
	std::string m_name;

	Timer(
		const std::string& name
	)
		: m_beginPoint (clock_t::now())
		, m_name       (name)
	{}

	~Timer()
	{
		timepoint_t endPoint = clock_t::now();

		auto begin = std::chrono::time_point_cast<res_t>(m_beginPoint);
		auto end   = std::chrono::time_point_cast<res_t>(endPoint);

		duration_t duration = end - begin;
		printf("\n[TIMER] %s took %2.2f", m_name.c_str(), (float)duration.count());
	}
};

#include "App.h"
#include <combaseapi.h>

struct test_handler
{
	const char* m_name;

	test_handler(const char* name) : m_name(name) {}

	void on(int event)
	{
		printf("\n%s -> %d", m_name, event);
	}

	void on(float event)
	{
		printf("\n%s -> %f", m_name, event);
	}
};

int App::Initialize(
	iw::InitOptions& options)
{
	SetMenuStateAppVars(APP_VARS_LIST);

	PushLayer<StaticLayer>(this);
	PushLayer<Audio_Layer>();

	PushLayer<iw::DebugLayer>();

	int err = Application::Initialize(options);
	if (err) return err;

	m_fonts = new iw::FontMap();
	m_fonts->Load("verdana",   18, "fonts/ttf/verdana.ttf");
	m_fonts->Load("verdana",   36, "fonts/ttf/verdana.ttf");
	m_fonts->Load("verdana",   92, "fonts/ttf/verdana.ttf");
	m_fonts->Load("Quicksand", 50, "fonts/ttf/Quicksand-Regular.ttf");
	m_fonts->Load("Quicksand", 24, "fonts/ttf/Quicksand-Regular.ttf");
	m_fonts->Load("Quicksand", 30, "fonts/ttf/Quicksand-Medium.ttf");
	m_fonts->Load("Quicksand", 40, "fonts/ttf/Quicksand-Medium.ttf");
	m_fonts->Load("Quicksand", 60, "fonts/ttf/Quicksand-Medium.ttf");
	m_fonts->Load("Quicksand", 120, "fonts/ttf/Quicksand-Medium.ttf");

	PushLayer<iw::ImGuiLayer>(Window, m_fonts)->BindContext();

	iw::ref<iw::Context> context_game = Input->CreateContext("game");
	context_game->MapButton(iw::InputName::D     , "+right");
	context_game->MapButton(iw::InputName::A     , "-right");
	context_game->MapButton(iw::InputName::W     , "+up");
	context_game->MapButton(iw::InputName::S     , "-up");
	context_game->MapButton(iw::InputName::LMOUSE, "+fire");
	context_game->MapButton(iw::InputName::RMOUSE, "+alt-fire");
	context_game->MapButton(iw::InputName::ESCAPE, "escape");
	context_game->MapButton(iw::InputName::X     , "toolbox");

	context_game->MapButton(iw::InputName::E , "+a"); // buttons for upgrades
	context_game->MapButton(iw::InputName::Q , "+b");
	context_game->MapButton(iw::InputName::F , "+x");
	context_game->MapButton(iw::InputName::C , "+y");

	iw::ref<iw::Context> context_menu = Input->CreateContext("menu");
	context_menu->MapButton(iw::InputName::LMOUSE, "+execute");
	context_menu->MapButton(iw::InputName::ESCAPE, "escape");

	iw::ref<iw::Device> mouse        = Input->CreateDevice<iw::Mouse>();
	iw::ref<iw::Device> keyboard_raw = Input->CreateDevice<iw::RawKeyboard>();

	context_game->AddDevice(mouse);
	context_game->AddDevice(keyboard_raw);

	context_menu->AddDevice(mouse);
	context_menu->AddDevice(keyboard_raw);

	//return 0;

	// menu states could be switched to through commands

	// game-play   -- starts game
	// game-end    -- ends game, brings you to post game menus
	// game-exit   -- exits game to main menu
	// game-exit d -- exits game to desktop
	// game-pause  -- opens escape menu

	// todo: all the m_menus-> calls should be handled by the manu layer itself
	//		 all the Input-> calls should be handled in a event listener for state changes

	Console->QueueCommand("set-state menus");

	// add default cells

	iw::Cell c;
	c.Type        = iw::CellType::LIGHTNING;
	c.Props       = iw::CellProp::NONE;
	c.Style       = iw::CellStyle::SHIMMER;
	c.StyleColor  = iw::Color(.1, .1, .1, 0);
	c.Color       = iw::Color::From255(212, 194, 252);
	c.life        = .1;
	
	iw::Cell::SetDefault(iw::CellType::LIGHTNING, c);

	//m_gamePause = new GameState("Pause menu", StateName::GAME_PAUSE_STATE);
	//m_gamePause->Layers.push_back(new Menu_Pause_Layer());
	//m_gamePause->OnChange = [&]()
	//{
	//	Physics->Paused = true;
	//	//Window()->SetCursor(true);
	//	//Renderer->Device->SetVSync();
	//	Input->SetContext("Menu");
	//};

	//Console->QueueCommand("set-state title");

	// create a guid for the installation of this app

	//GUID install_id;

	//if (iw::FileExists("install_id.txt"))
	//{
	//	std::string str = iw::ReadFile("install_id.txt");
	//	std::wstring s;
	//	s.assign(str.begin(), str.end());

	//	install_id = s;
	//}

	//else
	//{
	//	GUID& id = install_id;
	//	char szGuid[40] = { 0 };
	//	CoCreateGuid(&id);

	//	sprintf(szGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", 
	//		id.Data1,    id.Data2,    id.Data3, 
	//		id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3],
	//		id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);

	//	iw::WriteFile("install_id.txt", szGuid);
	//}

	Console->AddHandler([&](
		const iw::Command& command)
	{
		LOG_INFO << command.Original;

		if (command.Verb == "escape")
		{
			switch (m_state)
			{
				case StateName::IN_MENU:
					m_menus->Escape();
					break;
				case StateName::IN_GAME:
					ChangeToPauseMenu();
					break;
				case StateName::IN_GAME_THEN_MENU:
					if (m_menus->target_menu != MenuTarget::PAUSE)
						m_menus->Escape();
					else
						ChangeBackToGame();
					break;
				default:
					break;
			}
		}

		//else
		//if (command.Verb == "fade")
		//{
		//	float fadeTime  = command.TokenCount > 0 ? command.Tokens[0].Float : 1.f;
		//	float fadeDelay = command.TokenCount > 1 ? command.Tokens[1].Float : 0.f;
		//	float fadeTotalTime = fadeTime + fadeDelay;

		//	iw::Layer* fadeLayer = PushLayer<Menu_Fadeout_Layer>(fadeTime, fadeDelay);

		//	float time = iw::RawTotalTime();
		//	Task->coroutine(
		//		[this, time, fadeTotalTime, fadeLayer]()
		//	{
		//		bool done = iw::RawTotalTime() - time > fadeTotalTime;
		//		if (done) PopLayer(fadeLayer);
		//		return done;
		//	});
		//}

		else
		if (command.Verb == "set-state")
		{
			std::string name  = command.Str(0);
			float       delay = command.Num(1);

			float time = iw::TotalTime();
			Task->coroutine(
				[=]()
			{
				bool finished = iw::TotalTime() - time > delay;
				if (finished)
				{
					SetState(name);
				}

				return finished;
			});
		}

		else
		if (command.Verb == "set-death-movie-frame")
		{
			m_menus->deathMovieFrame = command.Num(0);
		}

		//else
		//if (command.Verb == "push-state")
		//{
		//	std::string stateName = command.Tokens[0].String;

		//	     if (stateName == "settings")   PushState(MakeState_Settings());
		//	else if (stateName == "highscores") PushState(MakeState_Highscores());
		//	//else if (stateName == "settings") PushState(MakeState_Settings());

		//	else
		//	{
		//		LOG_ERROR << "[push-state] invalid state";
		//	}
		//}

		//else
		//if (command.Verb == "push-state")
		//{
		//	PopState();
		//}

		else
		if (command.Verb == "final-score")
		{
			m_menus->m_finalScore = command.Num(0);
		}

		else
		if (command.Verb == "exit")
		{
			Stop();
		}

		return false;
	});

	/*

	Task->coroutine(
		[log]()
		{
			bool log = false;
			if (log)
			{
				const auto& queue = entities_defer().m_queue;

				if (queue.size() > 0)
				{
					printf("\nEntity Command Buffer:");
				}

				for (command_buffer::command* command : queue)
				{
					switch (command->m_type)
					{
					case entity_command::DESTROY:
					{
						command_buffer::command_destroy* c = (command_buffer::command_destroy*)command;
						printf("\n\t[DESTROY] %d, %d - %s", c->m_handle.m_index, c->m_handle.m_version, c->m_where);
						break;
					}
					default:
					{
						printf("\n\t[none   ] %s", command->m_where);
						break;
					}
					}
				}
			}

			entities_defer().execute();

			if (log)
			{
				printf("\nEntity System:");

				for (auto& [hash, storage] : entities().m_storage)
				{
					printf("\n\t Storage [Components: ", storage.m_archetype.m_hash);

					const auto& components = storage.m_archetype.m_components;

					for (int i = 0; i < components.size(); i++)
					{
						printf("%s ", components.at(i).m_name);
						if (i < components.size() - 1)
						{
							printf(" ");
						}
					}

					printf("]");

					for (int i = 0; i < storage.m_entities.size(); i++)
					{
						const entity_storage::entity_data& e = storage.m_entities.at(i);
						printf("\n\t\t [Index: %4d Version %4d Addresss %p]", e.m_index, e.m_version, e.m_addr);
					}
				}
			}

			return false;
		}
	);*/

	return 0;
}

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	options.WindowOptions = iw::WindowOptions {
		816,
		1039,
		true,
		iw::DisplayState::NORMAL
	};

	return new App();
}

//std::vector<int> results;
//for (int i = 0; i < 1e6; i++)
//{
//	float x = 10;
//	
//	int c = 0;
//	while (x > 0.00001f)
//	{
//		x *= iw::randf();
//		c++;
//	}

//	results.push_back(c);
//}

//std::unordered_map<int, int> counts;
//for (int& r : results)
//{
//	counts[r] += 1;
//}

//std::vector<std::pair<int, int>> sorted_counts(counts.begin(), counts.end());

//std::sort(sorted_counts.begin(), sorted_counts.end());

//for (auto x : sorted_counts)
//{
//	printf("\n%d", x.first);
//}
//printf("\n\n");
//for (auto x : sorted_counts)
//{
//	printf("\n%d", x.second);
//}
