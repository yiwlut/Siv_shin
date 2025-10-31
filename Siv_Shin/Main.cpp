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

	GameSceneManager sceneManager;
	sceneManager.initialize(SceneType::Logo);     

	constexpr Size gameSize{ 880, 880 };

	Scene::Resize(gameSize);
	Scene::SetResizeMode(ResizeMode::Virtual);
	Scene::SetTextureFilter(TextureFilter::Linear);

	Scene::SetBackground(ColorF{ 0.0, 0.0, 0.0 });
	Scene::SetLetterbox(ColorF{ 0.0, 0.0, 0.0 });

	bool wasFullscreen = false;

	while (System::Update())
	{
		if (KeyF11.down())
		{
			Window::SetFullscreen(!Window::GetState().fullscreen);
		}

		const bool isFullscreen = Window::GetState().fullscreen;
		if (wasFullscreen != isFullscreen)
		{
			Scene::Resize(gameSize);
			Scene::SetResizeMode(ResizeMode::Virtual);
			Scene::SetBackground(ColorF{ 0.0, 0.0, 0.0 });
			Scene::SetLetterbox(ColorF{ 0.0, 0.0, 0.0 });
			wasFullscreen = isFullscreen;
		}

		sceneManager.update();
		sceneManager.draw();
	}
}
