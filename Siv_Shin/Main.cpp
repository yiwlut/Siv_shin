#include <Siv3D.hpp>
#include "Scene/SceneManager.hpp"
#include "Shader/Manager/ShaderManager.hpp"

SIV3D_SET(EngineOption::Renderer::OpenGL);

void Main()
{
	Window::SetStyle(WindowStyle::Sizable);
	Window::Resize(880, 880);
	Window::SetMinimumFrameBufferSize(Size{ 880, 880 });
	Graphics::SetVSyncEnabled(true);

	System::SetTerminationTriggers(UserAction::CloseButtonClicked);

	if (!g_Shaders.initialize())
	{
		Console << U"쉐이더 초기화 실패!";
		return;
	}

	g_Shaders.printStatus();

	GameSceneManager sceneManager;
	sceneManager.initialize(SceneType::MainMenu);

	constexpr Size gameSize{ 880, 880 };

	// ★ Scene의 논리적 크기를 880x880으로 고정
	Scene::Resize(gameSize);
	Scene::SetResizeMode(ResizeMode::Virtual);

	// ★ 배경을 항상 검정색으로 고정
	Scene::SetBackground(ColorF{ 0.0, 0.0, 0.0 });
	Scene::SetLetterbox(ColorF{ 0.0, 0.0, 0.0 });

	while (System::Update())
	{
		// F11 키로 전체화면 토글
		if (KeyF11.down())
		{
			Window::SetFullscreen(!Window::GetState().fullscreen);
		}

		// Scene::Size()는 항상 880x880을 반환
		sceneManager.update();
		sceneManager.draw();
	}
}
