#include "iw/input/ContextManager.h"

namespace IwInput {
	void ContextManager::CreateContext(
		unsigned int windowId)
	{
		m_contexts.emplace(windowId);
	}

	void ContextManager::HandleInput(
		InputEvent& input)
	{
		Context& context = m_contexts[input.WindowId];
		context.States[input.Name] = input.State;
		if (input.Device == KEYBOARD) {
			context.KeyCallback(input.Name, input.State);
		}
		
		else if (input.Device == MOUSE) {
			if (input.Name == MOUSE_WHEEL) {
				context.MouseWheelCallback(input.State);
			}

			else if (input.Name == MOUSE_Y_AXIS) {
				context.MouseMovedCallback(
					context.States[MOUSE_X_AXIS], context.States[MOUSE_Y_AXIS]);
			}

			else if (input.Name != MOUSE_X_AXIS) {
				context.MouseButtonCallback(input.Name, input.State);
			}
		}

	}

	void ContextManager::SetMouseWheelCallback(
		unsigned int windowId,
		MouseWheelCallback callback)
	{
		m_contexts[windowId].MouseWheelCallback = callback;
	}

	void ContextManager::SetMouseMovedCallback(
		unsigned int windowId,
		MouseMovedCallback callback)
	{
		m_contexts[windowId].MouseMovedCallback = callback;
	}

	void ContextManager::SetMouseButtonCallback(
		unsigned int windowId,
		MouseButtonCallback callback)
	{
		m_contexts[windowId].MouseButtonCallback = callback;
	}

	void ContextManager::SetKeyCallback(
		unsigned int windowId,
		KeyCallback callback)
	{
		m_contexts[windowId].KeyCallback = callback;
	}
}
