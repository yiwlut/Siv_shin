//-----------------------------------------------
// ShaderManager.cpp (안전한 버전)
//-----------------------------------------------
#include "ShaderManager.hpp"

bool ShaderManager::initialize()
{
    if (m_initialized)
    {
        //Console << U"[ShaderManager] 이미 초기화되었습니다.";
        return true;
    }
    
    //Console << U"[ShaderManager] 쉐이더 로딩 시작...";
    Stopwatch stopwatch{StartImmediately::Yes};
    
    try
    {
        // HolographicShader 초기화
        //Console << U"  - HolographicShader 로딩 중...";
        m_holographic.emplace();
        if (!m_holographic->isValid())
        {
            //Console << U"    ✗ HolographicShader 로드 실패";
            return false;
        }
        //Console << U"    ✓ HolographicShader 로드 완료";
        
        // TerastalShader 초기화
        //Console << U"  - TerastalShader 로딩 중...";
        m_terastal.emplace();
        if (!m_terastal->isValid())
        {
            //Console << U"    ✗ TerastalShader 로드 실패";
            return false;
        }
        //Console << U"    ✓ TerastalShader 로드 완료";
        
        // PaintSpreadShader 초기화
        //Console << U"  - PaintSpreadShader 로딩 중...";
        m_paintSpread.emplace();
        if (!m_paintSpread->isValid())
        {
            //Console << U"    ✗ PaintSpreadShader 로드 실패";
            return false;
        }
        //Console << U"    ✓ PaintSpreadShader 로드 완료";
        
        // BombBoxEffect 초기화
        //Console << U"  - BombBoxEffect 로딩 중...";
        m_bombBox.emplace();
        //Console << U"    ✓ BombBoxEffect 로드 완료";
        
        stopwatch.pause();
        
        //Console << U"[ShaderManager] ✓ 모든 쉐이더 로드 완료! (소요 시간: {:.2f}s)"_fmt(stopwatch.sF());
        m_initialized = true;
        return true;
    }
    catch (const Error& e)
    {
        stopwatch.pause();
        //Console << U"[ShaderManager] ✗ 쉐이더 로드 중 예외 발생: {}"_fmt(e.what());
        //Console << U"[ShaderManager] (소요 시간: {:.2f}s)"_fmt(stopwatch.sF());
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
    ////Console << U"========== ShaderManager 상태 ==========";
    ////Console << U"초기화:       " << (m_initialized ? U"✓" : U"✗");
    ////Console << U"Holographic:  " << ((m_holographic.has_value() && m_holographic->isValid()) ? U"✓" : U"✗");
    ////Console << U"Terastal:     " << ((m_terastal.has_value() && m_terastal->isValid()) ? U"✓" : U"✗");
    ////Console << U"PaintSpread:  " << ((m_paintSpread.has_value() && m_paintSpread->isValid()) ? U"✓" : U"✗");
    ////Console << U"BombBox:      " << (m_bombBox.has_value() ? U"✓" : U"✗");
    ////Console << U"========================================";
}

HolographicShader& ShaderManager::holographic()
{
    if (!m_initialized || !m_holographic.has_value())
    {
        throw Error{U"[ShaderManager] HolographicShader가 초기화되지 않았습니다. initialize()를 먼저 호출하세요."};
    }
    return *m_holographic;
}

const HolographicShader& ShaderManager::holographic() const
{
    if (!m_initialized || !m_holographic.has_value())
    {
        throw Error{U"[ShaderManager] HolographicShader가 초기화되지 않았습니다."};
    }
    return *m_holographic;
}

TerastalShader& ShaderManager::terastal()
{
    if (!m_initialized || !m_terastal.has_value())
    {
        throw Error{U"[ShaderManager] TerastalShader가 초기화되지 않았습니다. initialize()를 먼저 호출하세요."};
    }
    return *m_terastal;
}

const TerastalShader& ShaderManager::terastal() const
{
    if (!m_initialized || !m_terastal.has_value())
    {
        throw Error{U"[ShaderManager] TerastalShader가 초기화되지 않았습니다."};
    }
    return *m_terastal;
}

PaintSpreadShader& ShaderManager::paintSpread()
{
    if (!m_initialized || !m_paintSpread.has_value())
    {
        throw Error{U"[ShaderManager] PaintSpreadShader가 초기화되지 않았습니다. initialize()를 먼저 호출하세요."};
    }
    return *m_paintSpread;
}

const PaintSpreadShader& ShaderManager::paintSpread() const
{
    if (!m_initialized || !m_paintSpread.has_value())
    {
        throw Error{U"[ShaderManager] PaintSpreadShader가 초기화되지 않았습니다."};
    }
    return *m_paintSpread;
}

BombBoxEffect& ShaderManager::bombBox()
{
    if (!m_initialized || !m_bombBox.has_value())
    {
        throw Error{U"[ShaderManager] BombBoxEffect가 초기화되지 않았습니다. initialize()를 먼저 호출하세요."};
    }
    return *m_bombBox;
}

const BombBoxEffect& ShaderManager::bombBox() const
{
    if (!m_initialized || !m_bombBox.has_value())
    {
        throw Error{U"[ShaderManager] BombBoxEffect가 초기화되지 않았습니다."};
    }
    return *m_bombBox;
}
