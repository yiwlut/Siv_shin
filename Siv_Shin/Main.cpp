#include <Siv3D.hpp>
#include "Scene/SceneManager.hpp"
#include "Shader/Manager/ShaderManager.hpp"

//OpenGL 사용 명시
SIV3D_SET(EngineOption::Renderer::OpenGL);

void Main()
{
    // 해상도 설정
    Window::Resize(880, 880);
	Graphics::SetVSyncEnabled(true);
    
    // ESC 키로 프로그램이 종료되지 않도록 설정
    System::SetTerminationTriggers(UserAction::CloseButtonClicked);
    
	
	//// 쉐이더 사전 컴파일
	if (!g_Shaders.initialize())
	{
		Console << U"쉐이더 초기화 실패!";
		return;
	}
	
	g_Shaders.printStatus();
	
    // 씬 매니저 초기화
    GameSceneManager sceneManager;
    sceneManager.initialize(SceneType::MainMenu);
    
    // 메인 루프
    while (System::Update())
    {
        sceneManager.update();
        sceneManager.draw();
    }
}
