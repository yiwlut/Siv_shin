//-----------------------------------------------
// ShaderManager.hpp (안전한 버전)
//-----------------------------------------------
#pragma once
#include <Siv3D.hpp>
#include "../2DShaders/HolographicShader.hpp"
#include "../2DShaders/TerastalShader.hpp"
#include "../2DShaders/PaintSpreadShader.hpp"
#include "../2DShaders/BombBoxEffect.hpp"

class ShaderManager
{
public:
    static ShaderManager& Instance() noexcept
    {
        static ShaderManager instance;
        return instance;
    }
    
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;
    ShaderManager(ShaderManager&&) = delete;
    ShaderManager& operator=(ShaderManager&&) = delete;
    
    /// @brief 모든 쉐이더 초기화 (사전 컴파일 포함)
    bool initialize();
    
    /// @brief 초기화 여부 확인
    bool isInitialized() const noexcept { return m_initialized; }
    
    /// @brief 모든 쉐이더가 정상적으로 로드되었는지 확인
    bool isAllValid() const noexcept;
    
    /// @brief 로드 실패한 쉐이더 목록 반환
    Array<String> getFailedShaders() const;
    
    /// @brief 로드 상태 출력 (디버그용)
    void printStatus() const;
    
    // 쉐이더 접근자 (초기화 확인 포함)
    HolographicShader& holographic();
    const HolographicShader& holographic() const;
    
    TerastalShader& terastal();
    const TerastalShader& terastal() const;
    
    PaintSpreadShader& paintSpread();
    const PaintSpreadShader& paintSpread() const;
    
    BombBoxEffect& bombBox();
    const BombBoxEffect& bombBox() const;
    
    // 간편한 전역 함수 스타일 접근
    static HolographicShader& Holographic() { return Instance().holographic(); }
    static TerastalShader& Terastal() { return Instance().terastal(); }
    static PaintSpreadShader& PaintSpread() { return Instance().paintSpread(); }
    static BombBoxEffect& BombBox() { return Instance().bombBox(); }

private:
    ShaderManager() = default;
    ~ShaderManager() = default;
    
    // Optional로 지연 초기화
    Optional<HolographicShader> m_holographic;
    Optional<TerastalShader> m_terastal;
    Optional<PaintSpreadShader> m_paintSpread;
    Optional<BombBoxEffect> m_bombBox;
    
    bool m_initialized = false;
};

#define g_Shaders ShaderManager::Instance()
