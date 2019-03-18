#include "iw/engine/Application.h"
#include "iw/log/logger.h"
#include "iw/events/functional/callback.h"

namespace IwEngine {
	Application::Application()
		: m_window (IWindow::Create())
		, m_running(false)
	{}

	Application::~Application() {
		delete m_window;
	}

	int Application::Initilize(
		const WindowOptions& windowOptions)
	{
		LOG_SINK(iwlog::stdout_sink, iwlog::INFO);
		LOG_SINK(iwlog::stderr_sink, iwlog::ERR);
		LOG_SINK(iwlog::file_sink,   iwlog::DEBUG, "sandbox.log");

		m_window->SetCallback(
			iwevents::make_callback(&Application::HandleEvent, this));
		int status;
		LOG_DEBUG << "Initilizing window...";
		if (status = m_window->Initilize(windowOptions)) {
			LOG_ERROR << "Window failed to initilize with error code "
				<< status;
			return status;
		}

		LOG_DEBUG << "Binding input manager to window...";

		m_window->SetInputManager(m_inputManager);
		m_inputManager.CreateDevice<IwInput::Mouse>();
		m_inputManager.CreateDevice<IwInput::Keyboard>();

		for (Layer* layer : m_layerStack) {
			LOG_DEBUG << "Initilizing " << layer->Name() << " layer...";
			if (status = layer->Initilize()) {
				LOG_ERROR << layer->Name()
					<< " layer initilization failed with error code "
					<< status;
				return status;
			}
		}

		m_window->SetState(windowOptions.state);

		return 0;
	}

	void Application::Run() {
		m_running = true;
		while (m_running) {
			Update();
		}
	}

	void Application::Update() {
		m_window->Clear();

		for (Layer* layer : m_layerStack) {
			layer->Update();
		}

		m_window->Update();
		m_window->Render();
	}

	void Application::Destroy() {
		m_running = false;
		m_window->Destroy();
		for (Layer* layer : m_layerStack) {
			layer->Destroy();
		}
	}

	void Application::HandleEvent(Event& e) {
		if (e.Type == WindowResized) {
			DispatchEvent((WindowResizedEvent&)e);
		}

		else if (e.Type == MouseWheel) {
			DispatchEvent((MouseWheelEvent&)e);
		}

		else if (e.Type == MouseMoved) {
			DispatchEvent((MouseMovedEvent&)e);
		}

		else if (e.Type == MouseButton) {
			DispatchEvent((MouseButtonEvent&)e);
		}

		else if (e.Type == Key) {
			DispatchEvent((KeyEvent&)e);
		}

		else if (e.Type == WindowClosed) {
			Destroy();
		}
	}

	void Application::PushLayer(
		Layer* layer)
	{
		m_layerStack.PushLayer(layer);
	}

	void Application::PushOverlay(
		Layer* overlay)
	{
		m_layerStack.PushOverlay(overlay);
	}

	void Application::PopLayer(
		Layer* layer)
	{
		m_layerStack.PopLayer(layer);
	}

	void Application::PopOverlay(
		Layer* overlay)
	{
		m_layerStack.PopOverlay(overlay);
	}
}
