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
    
    bool initialize();
    
    bool isInitialized() const noexcept { return m_initialized; }
    
    bool isAllValid() const noexcept;
    
    Array<String> getFailedShaders() const;
    
    void printStatus() const;
    
    HolographicShader& holographic();
    const HolographicShader& holographic() const;
    
    TerastalShader& terastal();
    const TerastalShader& terastal() const;
    
    PaintSpreadShader& paintSpread();
    const PaintSpreadShader& paintSpread() const;
    
    BombBoxEffect& bombBox();
    const BombBoxEffect& bombBox() const;
    
    static HolographicShader& Holographic() { return Instance().holographic(); }
    static TerastalShader& Terastal() { return Instance().terastal(); }
    static PaintSpreadShader& PaintSpread() { return Instance().paintSpread(); }
    static BombBoxEffect& BombBox() { return Instance().bombBox(); }

private:
    ShaderManager() = default;
    ~ShaderManager() = default;
    
    Optional<HolographicShader> m_holographic;
    Optional<TerastalShader> m_terastal;
    Optional<PaintSpreadShader> m_paintSpread;
    Optional<BombBoxEffect> m_bombBox;
    
    bool m_initialized = false;
};

#define g_Shaders ShaderManager::Instance()
