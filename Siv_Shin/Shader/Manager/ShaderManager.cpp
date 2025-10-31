#include "ShaderManager.hpp"

bool ShaderManager::initialize()
{
    if (m_initialized)
    {
        return true;
    }
    
    Stopwatch stopwatch{StartImmediately::Yes};
    
    try
    {
        m_holographic.emplace();
        if (!m_holographic->isValid())
        {
            return false;
        }
        m_terastal.emplace();
        if (!m_terastal->isValid())
        {
            return false;
        }
        m_paintSpread.emplace();
        if (!m_paintSpread->isValid())
        {
            return false;
        }
        m_bombBox.emplace();
        stopwatch.pause();
        
        m_initialized = true;
        return true;
    }
    catch (const Error& e)
    {
        stopwatch.pause();
        return false;
    }
}

bool ShaderManager::isAllValid() const noexcept
{
    return m_initialized &&
           m_holographic.has_value() && m_holographic->isValid() &&
           m_terastal.has_value() && m_terastal->isValid() &&
           m_paintSpread.has_value() && m_paintSpread->isValid() &&
           m_bombBox.has_value();
}

Array<String> ShaderManager::getFailedShaders() const
{
    Array<String> failed;
    
    if (!m_holographic.has_value() || !m_holographic->isValid())
    {
        failed.push_back(U"HolographicShader");
    }
    
    if (!m_terastal.has_value() || !m_terastal->isValid())
    {
        failed.push_back(U"TerastalShader");
    }
    
    if (!m_paintSpread.has_value() || !m_paintSpread->isValid())
    {
        failed.push_back(U"PaintSpreadShader");
    }
    
    if (!m_bombBox.has_value())
    {
        failed.push_back(U"BombBoxEffect");
    }
    
    return failed;
}

void ShaderManager::printStatus() const
{
}

HolographicShader& ShaderManager::holographic()
{
    if (!m_initialized || !m_holographic.has_value())
    {
        throw Error{U"[ShaderManager] HolographicShader error"};
    }
    return *m_holographic;
}

const HolographicShader& ShaderManager::holographic() const
{
    if (!m_initialized || !m_holographic.has_value())
    {
        throw Error{U"[ShaderManager] HolographicShader error"};
    }
    return *m_holographic;
}

TerastalShader& ShaderManager::terastal()
{
    if (!m_initialized || !m_terastal.has_value())
    {
        throw Error{U"[ShaderManager] TerastalShader error"};
    }
    return *m_terastal;
}

const TerastalShader& ShaderManager::terastal() const
{
    if (!m_initialized || !m_terastal.has_value())
    {
        throw Error{U"[ShaderManager] TerastalShader error"};
    }
    return *m_terastal;
}

PaintSpreadShader& ShaderManager::paintSpread()
{
    if (!m_initialized || !m_paintSpread.has_value())
    {
        throw Error{U"[ShaderManager] PaintSpreadShader error"};
    }
    return *m_paintSpread;
}

const PaintSpreadShader& ShaderManager::paintSpread() const
{
    if (!m_initialized || !m_paintSpread.has_value())
    {
        throw Error{U"[ShaderManager] PaintSpreadShader error"};
    }
    return *m_paintSpread;
}

BombBoxEffect& ShaderManager::bombBox()
{
    if (!m_initialized || !m_bombBox.has_value())
    {
        throw Error{U"[ShaderManager] BombBoxEffect error"};
    }
    return *m_bombBox;
}

const BombBoxEffect& ShaderManager::bombBox() const
{
    if (!m_initialized || !m_bombBox.has_value())
    {
        throw Error{U"[ShaderManager] BombBoxEffect error"};
    }
    return *m_bombBox;
}
