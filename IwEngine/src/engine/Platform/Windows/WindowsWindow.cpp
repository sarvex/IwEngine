#include "iw/engine/Platform/Windows/WindowsWindow.h"
#include "EventTranslator.h"
#include "iw/log/logger.h"
#include "gl/glew.h"
#include "gl/wglew.h"
#include <cstdio>
#include <Tchar.h>
#include <thread>

ATOM RegClass(
	HINSTANCE instance,
	WNDPROC wndproc);

void GLAPIENTRY MessageCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	if (type == GL_DEBUG_TYPE_ERROR) {
		LOG_ERROR << "GL ERROR: "
			<< message
			<< " TYPE 0x" << type
			<< " SEVERITY 0x" << severity;
	}

	else {
		LOG_DEBUG << "GL DEBUG: " << message
			<< " TYPE 0x" << type
			<< " SEVERITY 0x" << severity;
	}
}

namespace iw {
namespace Engine {
	IWindow* IWindow::Create() {
		return new WindowsWindow();
	}

	int WindowsWindow::Initialize(
		const WindowOptions& options)
	{
		this->Options = options;

		m_instance = GetModuleHandle(NULL);
		MAKEINTATOM(RegClass(m_instance, _WndProc));

		HWND fake_hwnd = CreateWindow(
			_T("Core"), _T("Fake Window"),
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0, 0,
			1, 1,
			NULL, NULL,
			m_instance, NULL);

		HDC fake_dc = GetDC(fake_hwnd);

		PIXELFORMATDESCRIPTOR fake_pfd;
		ZeroMemory(&fake_pfd, sizeof(fake_pfd));
		fake_pfd.nSize = sizeof(fake_pfd);
		fake_pfd.nVersion = 1;
		fake_pfd.dwFlags = PFD_DRAW_TO_WINDOW
			| PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		fake_pfd.iPixelType = PFD_TYPE_RGBA;
		fake_pfd.cColorBits = 32;
		fake_pfd.cAlphaBits = 8;
		fake_pfd.cDepthBits = 24;

		INT fake_pfdId = ChoosePixelFormat(fake_dc, &fake_pfd);
		if (fake_pfdId == 0) {
			LOG_ERROR << "ChoosePixelFormat() failed!";
			return 1;
		}

		if (!SetPixelFormat(fake_dc, fake_pfdId, &fake_pfd)) {
			LOG_ERROR << "SetPixelFormat() failed!";
			return 1;
		}

		HGLRC fake_rc = wglCreateContext(fake_dc);
		if (fake_rc == 0) {
			LOG_ERROR << "wglCreateContext() failed!";
			return 1;
		}

		if (!wglMakeCurrent(fake_dc, fake_rc)) {
			LOG_ERROR << "wglMakeCurrent() failed!";
			return 1;
		}

		m_window = CreateWindow(
			_T("Core"), _T("Space"),
			WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			100, 100,
			options.Width, options.Height,
			NULL, NULL,
			m_instance, NULL);

		SetWindowLongPtr(m_window, GWLP_USERDATA, (LONG_PTR)this);

		m_device = GetDC(m_window);

		//gl stuff
		if (glewInit() != GLEW_OK) {
			LOG_ERROR << "glewInit() failed!";
			return 1;
		}

		CONST INT pixelAttribs[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_ALPHA_BITS_ARB, 8,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
			WGL_SAMPLES_ARB, 4,
			0
		};

		INT pfid;
		UINT formatCount;
		BOOL status = wglChoosePixelFormatARB(m_device, pixelAttribs,
			NULL, 1, &pfid, &formatCount);

		if (status == FALSE || formatCount == 0) {
			LOG_ERROR << "wglChoosePixelFormatARB() failed!";
			return 1;
		}

		PIXELFORMATDESCRIPTOR pfd;
		DescribePixelFormat(m_device, pfid, sizeof(pfd), &pfd);
		SetPixelFormat(m_device, pfid, &pfd);

		CONST INT major = 4, minor = 4;
		INT contextAttribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, major,
			WGL_CONTEXT_MINOR_VERSION_ARB, minor,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		m_context = wglCreateContextAttribsARB(m_device, 0, contextAttribs);
		if (m_context == NULL) {
			LOG_ERROR << "wglCreateContextAttribsARB() failed!";
			return 1;
		}

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(fake_rc);
		ReleaseDC(fake_hwnd, fake_dc);
		DestroyWindow(fake_hwnd);
		
		TakeOwnership();

		glClearColor(70 / 255.0f, 85 / 255.0f, 100 / 255.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		wglSwapIntervalEXT(0); //-1 for adaptive vsync 0 for off 1 for on

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

#ifdef IW_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
#endif

		SetCursor(options.Cursor);
		SetState(options.State);

		return 0;
	}

	void WindowsWindow::Destroy() {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_context);
		ReleaseDC(m_window, m_device);
		DestroyWindow(m_window);
	}

	void WindowsWindow::Update() {
		MSG msg;
		if (GetMessage(&msg, m_window, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void WindowsWindow::SwapBuffers() {
		::SwapBuffers(m_device);
	}

	bool WindowsWindow::TakeOwnership() {
		if (!wglMakeCurrent(m_device, m_context)) {
			LOG_ERROR << "wglMakeCurrent(" << m_device << ", " << m_context << ") failed.";
			return false;
		}

		return true;
	}

	bool WindowsWindow::ReleaseOwnership() {
		if (!wglMakeCurrent(NULL, NULL)) {
			LOG_ERROR << "wglMakeCurrent(NULL, NULL) failed.";
			return false;
		}

		return true;
	}

	void WindowsWindow::SetState(
		DisplayState state)
	{
		int wstate = -1;
		switch (state) {
			case DisplayState::HIDDEN:     wstate = SW_HIDE;       break;
			case DisplayState::MINIMIZED:  wstate = SW_MINIMIZE;   break;
			case DisplayState::MAXIMIZED:  wstate = SW_MAXIMIZE;   break;
			case DisplayState::NORMAL:     wstate = SW_SHOWNORMAL; break;
			case DisplayState::BORDERLESS: {
				int width  = GetSystemMetrics(SM_CXSCREEN);
				int height = GetSystemMetrics(SM_CYSCREEN);

				SetWindowLongPtr(m_window, GWL_STYLE, WS_VISIBLE | WS_POPUP);
				SetWindowPos(m_window, HWND_TOP, 0, 0, width, height, SWP_FRAMECHANGED);

				break;
			}
		}

		if (wstate != -1) {
			ShowWindow(m_window, wstate);
		}

		Options.State = state;
	}


	void WindowsWindow::SetCursor(
		bool show)
	{
		ShowCursor(show);
		Options.Cursor = show;
	}

	LRESULT CALLBACK WindowsWindow::_WndProc(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam)
	{
		WindowsWindow* me = reinterpret_cast<WindowsWindow*>(
			GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (me) {
			return me->HandleEvent(hwnd, msg, wParam, lParam);
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK WindowsWindow::HandleEvent(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam)
	{
		OsEvent ose(Id(), msg, wParam, lParam);
		
		switch (msg) {
			case WM_INPUT: 
				ose.Type = iw::val(OsEventType::INPUT);
				break;
		}

		Bus->send<OsEvent>(ose);

		switch (msg) {
			case WM_SIZE:
				Bus->push<WindowResizedEvent>(Translate<WindowResizedEvent>(msg, Id(), wParam, lParam));
				break;
			case WM_CLOSE:
				Bus->push<WindowEvent>(Closed, Id());
				break;
			case WM_DESTROY:
				Bus->push<WindowEvent>(Destroyed, Id());
				break;
			default:
				//Bus->push(iw::event(iw::NOT_HANDLED, 0));
				return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		return 0;
	}
}
}

ATOM RegClass(
	HINSTANCE instance,
	WNDPROC wndproc)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = wndproc;
	wcex.hInstance = instance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = _T("Core");

	return RegisterClassEx(&wcex);
}
