#include "GameOverScene.hpp"

GameOverScene::GameOverScene()
    : titleFont_(48)
    , scoreFont_(32)
    , buttonFont_(24)
    , statFont_(20)
    , helpFont_(16)
    , bgm_(Resource(U"ArtResources/BGM/DeepSea1.mp3"), Loop::Yes)     
{
    currentScene_ = SceneType::GameOver;
    initializeUI();
}

void GameOverScene::onEnter()
{
    animTimer_ = 0.0;
    scoreCountUp_ = 0.0;
    scoreAnimFinished_ = false;
    particles_.clear();
    
    if (gameResult_.isNewRecord)
    {
        createParticles();
    }
    
    if (!bgm_.isEmpty())
    {
        bgm_.setVolume(0.2);         
        bgm_.play();
    }
    
}

void GameOverScene::onExit()
{
    if (!bgm_.isEmpty() && bgm_.isPlaying())
    {
        bgm_.stop();
    }
    
}

void GameOverScene::setGameResult(int32 score, double time, int32 moves)
{
    gameResult_.finalScore = score;
    gameResult_.playTime = time;
    gameResult_.totalMoves = moves;
    gameResult_.isNewRecord = (score > 100);
}

void GameOverScene::initializeUI()
{
    const double buttonWidth = 150;
    const double buttonHeight = 50;
    const double centerX = 256;
    
    retryButton_ = Rect{
        static_cast<int32>(centerX - buttonWidth - 10), 400,
        static_cast<int32>(buttonWidth), static_cast<int32>(buttonHeight)
    };
    
    menuButton_ = Rect{
        static_cast<int32>(centerX + 10), 400,
        static_cast<int32>(buttonWidth), static_cast<int32>(buttonHeight)
    };
}

void GameOverScene::update()
{
    const double deltaTime = Scene::DeltaTime();
    
    updateAnimations();
    updateParticles();
    
    retryHovered_ = retryButton_.mouseOver();
    menuHovered_ = menuButton_.mouseOver();
    
    if (retryButton_.leftClicked())
    {
        changeScene(SceneType::InGame);
    }
    else if (menuButton_.leftClicked())
    {
        changeScene(SceneType::MainMenu);
    }
    
    if (KeyEnter.down() || KeySpace.down())
    {
        changeScene(SceneType::InGame);
    }
    else if (KeyEscape.down())
    {
        changeScene(SceneType::MainMenu);
    }
}

void GameOverScene::updateAnimations()
{
    const double deltaTime = Scene::DeltaTime();
    animTimer_ += deltaTime;
    
    if (!scoreAnimFinished_)
    {
        const double animDuration = 2.0;
        if (animTimer_ < animDuration)
        {
            double progress = animTimer_ / animDuration;
            progress = 1.0 - Math::Pow(1.0 - progress, 3.0);
            scoreCountUp_ = gameResult_.finalScore * progress;
        }
        else
        {
            scoreCountUp_ = gameResult_.finalScore;
            scoreAnimFinished_ = true;
        }
    }
}

void GameOverScene::updateParticles()
{
    const double deltaTime = Scene::DeltaTime();
    
    for (auto& particle : particles_)
    {
        particle.pos += particle.velocity * deltaTime;
        particle.life -= deltaTime;
        particle.velocity.y += 100 * deltaTime;
        
        double alpha = particle.life / particle.maxLife;
        particle.color.a = alpha;
    }
    
    particles_.remove_if([](const Particle& p) { return p.life <= 0; });
    
    if (gameResult_.isNewRecord && particles_.size() < 50)
    {
        createParticles();
    }
}

void GameOverScene::createParticles()
{
    for (int i = 0; i < 5; i++)
    {
        Particle p;
        p.pos = Vec2{Random(100, 412), Random(100, 200)};
        p.velocity = Vec2{Random(-100, 100), Random(-200, -50)};
        p.color = HSV{Random(0, 360), 0.8, 1.0, 1.0}.toColorF();
        p.life = p.maxLife = Random(1.0, 3.0);
        particles_.push_back(p);
    }
}

void GameOverScene::draw()
{
    Scene::SetBackground(ColorF{ 0.05, 0.05, 0.1 });
    
    for (int y = 0; y < 512; y++)
    {
        double t = static_cast<double>(y) / 512;
        ColorF color = ColorF{0.05, 0.05, 0.1}.lerp(ColorF{0.1, 0.05, 0.15}, t);
        Rect{0, y, 512, 1}.draw(color);
    }
    
    drawResults();
    drawButtons();
    drawParticles();
}

void GameOverScene::drawResults()
{
    String titleText = gameResult_.isNewRecord ? U"NEW RECORD!" : U"GAME OVER";
    ColorF titleColor = gameResult_.isNewRecord ? 
        ColorF{1.0, 0.8, 0.2} : ColorF{0.8, 0.4, 0.4};
    
    double titleScale = 1.0 + 0.1 * Math::Sin(animTimer_ * 3.0);
    titleFont_(titleText).drawAt(256, 120, titleColor);
    
    scoreFont_(U"Score: {}"_fmt(static_cast<int32>(scoreCountUp_)))
        .drawAt(256, 200, ColorF{1.0, 1.0, 0.5});
    
    statFont_(U"Play Time: {:.1f}s"_fmt(gameResult_.playTime))
        .drawAt(256, 240, ColorF{0.8, 0.8, 1.0});
    
    statFont_(U"Total Moves: {}"_fmt(gameResult_.totalMoves))
        .drawAt(256, 270, ColorF{0.8, 0.8, 1.0});
    
    helpFont_(U"Enter/Space: Retry | ESC: Menu")
        .drawAt(256, 350, ColorF{0.6, 0.6, 0.7});
}

void GameOverScene::drawButtons()
{
    ColorF retryColor = retryHovered_ ? 
        ColorF{0.3, 0.7, 0.3} : ColorF{0.2, 0.6, 0.2};
    retryButton_.draw(retryColor);
    retryButton_.drawFrame(2, Palette::White);
    buttonFont_(U"Retry").drawAt(retryButton_.center(), 
        retryHovered_ ? Palette::Yellow : Palette::White);
    
    ColorF menuColor = menuHovered_ ? 
        ColorF{0.3, 0.5, 0.7} : ColorF{0.2, 0.4, 0.6};
    menuButton_.draw(menuColor);
    menuButton_.drawFrame(2, Palette::White);
    buttonFont_(U"Menu").drawAt(menuButton_.center(), 
        menuHovered_ ? Palette::Yellow : Palette::White);
}

void GameOverScene::drawParticles()
{
    for (const auto& particle : particles_)
    {
        Circle{particle.pos, 3}.draw(particle.color);
    }
}
