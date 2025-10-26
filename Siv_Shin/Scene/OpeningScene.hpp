#pragma once
#include "SceneManager.hpp"

class OpeningScene : public GameScene
{
public:
    OpeningScene();
    ~OpeningScene() override = default;

    void update() override;
    void draw() override;
    void onEnter() override;
    void onExit() override;

private:
    // 꽃잎 파티클 구조체
    struct Petal
    {
        Vec2 position;
        Vec2 velocity;
        double rotation;
        double rotationSpeed;
        double scale;
        ColorF color;
        double alpha;
    };

    // 루비(후리가나) 텍스트 구조체
    struct RubyText
    {
        String baseText;  // 한자
        String rubyText;  // 후리가나
        ColorF color;     // 텍스트 색상 (기본은 흰색)
        
        RubyText(const String& base, const String& ruby, const ColorF& col = ColorF{0.95, 0.95, 0.9, 1.0})
            : baseText(base), rubyText(ruby), color(col) {}
    };

    // 타이머
    double elapsedTime_;
    double fadeInDuration_;
    double displayDuration_;
    double fadeOutDuration_;
    double totalDuration_;

    // 폰트
    Font haikuFont_;  // 하이쿠용 폰트
    Font rubyFont_;   // 루비(후리가나)용 작은 폰트
    Font authorFont_; // 작가명용 폰트 (하이쿠보다 작게)
    Font skipFont_;   // 스킵 안내용 폰트

    // 배경음악
    Audio openingBgm_;

    // 꽃잎 파티클
    Array<Petal> petals_;
    double petalSpawnTimer_;
    double petalSpawnInterval_;
    
    // 하이쿠 애니메이션
    double haikuLineDelay_;      // 각 줄 사이의 딜레이
    double haikuLineFadeDuration_;  // 각 줄의 페이드인 시간
    double typewriterSpeed_;     // 타자기 효과 속도 (문자당 초)
    
    // 스킵 기능
    double skipPressTime_;       // 스페이스바를 누르고 있는 시간
    double skipRequiredTime_;    // 스킵에 필요한 시간 (3초)
    bool isSkipping_;            // 현재 스킵 중인지

    // 페이드 효과
    double getFadeAlpha() const;
    
    // 꽃잎 파티클 관련 함수
    void initializePetals();
    void updatePetals();
    void drawPetals();
    void spawnPetal();
    
    // 하이쿠 그리기
    void drawHaiku();
    double getHaikuLineAlpha(size_t lineIndex) const;
    size_t getVisibleCharCount(size_t lineIndex) const;  // 타자기 효과용
    
    // 루비 텍스트 파싱 및 렌더링
    Array<RubyText> parseRubyText(const String& text) const;
    void drawTextWithRuby(const String& text, const Vec2& basePos, const ColorF& color) const;
    void drawTextWithRubyPartial(const String& text, const Vec2& basePos, double alpha, size_t visibleCharCount) const;
    
    // 스킵 UI 그리기
    void drawSkipUI();
};
