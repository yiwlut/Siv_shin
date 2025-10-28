#pragma once
#include <Siv3D.hpp>

using namespace s3d;

// 벽 패턴 프레임 구조체
struct BossWallPatternFrame
{
	Array<String> pattern;   // 각 줄이 맵의 한 행
	double duration = 1.0;   // 이 프레임이 유지되는 시간 (초)
};

// 전체 패턴 시퀀스
struct BossWallPattern
{
	Array<BossWallPatternFrame> frames;
	bool looping = true;     // 패턴 반복 여부
};

namespace BossWallPatternData
{
	// 안전 보정 파라미터
	static constexpr double kMinFrameDuration = 0.5;

	// 내부 유틸
	inline bool isW(char32 c) { return c == U'W'; }
	inline bool isWarn(char32 c) { return c == U'!'; }
	inline bool isDestroy(char32 c) { return c == U'X'; }

	// 프레임 라인 길이 정규화 (행 길이 불일치 대비)
	inline void normalizeLineSizes(BossWallPatternFrame& f, size_t rows, size_t cols)
	{
		if (f.pattern.size() < rows) f.pattern.resize(rows, String(cols, U'.'));
		for (auto& line : f.pattern)
		{
			if (line.size() < cols) line.resize(cols, U'.');
		}
	}

	// 직전 프레임 대비 이번 프레임에서 새로 'W'가 등장하는 칸에 경고('!') 프레임 자동 삽입
	// - 경고 프레임은 직전 프레임을 복제한 뒤, 새로 'W'가 생길 위치만 '!'로 덮는다
	// - 경고가 이미 있었던 칸은 중복 삽입하지 않음
	// - 모든 삽입 프레임은 최소 0.5초
	inline BossWallPattern FixPattern(BossWallPattern pattern)
	{
		BossWallPattern out;
		out.looping = pattern.looping;

		if (pattern.frames.isEmpty())
			return out;

		// 1차: 최소 지속시간 보정
		for (auto& f : pattern.frames)
		{
			if (f.duration < kMinFrameDuration)
				f.duration = kMinFrameDuration;
		}

		// 기준 행/열 크기 추정 (첫 프레임 기준)
		const size_t baseRows = pattern.frames.front().pattern.size();
		const size_t baseCols = (baseRows > 0 ? pattern.frames.front().pattern.front().size() : 16);

		// 2차: 경고 프레임 삽입
		BossWallPatternFrame prev = pattern.frames.front();
		normalizeLineSizes(prev, baseRows, baseCols);
		out.frames.push_back(prev);

		for (size_t i = 1; i < pattern.frames.size(); ++i)
		{
			BossWallPatternFrame cur = pattern.frames[i];
			normalizeLineSizes(cur, baseRows, baseCols);

			// 새로 W가 생기는 칸 탐지
			bool needWarn = false;
			BossWallPatternFrame warn = prev; // 직전 프레임 복제
			for (size_t y = 0; y < baseRows; ++y)
			{
				String& wLine = warn.pattern[y];
				const String& pLine = prev.pattern[y];
				const String& cLine = cur.pattern[y];

				for (size_t x = 0; x < baseCols; ++x)
				{
					const char32 p = pLine[x];
					const char32 c = cLine[x];
					// 이전에 W가 아니었는데 이번에 W가 생김
					if (isW(c) && !isW(p))
					{
						// 직전이 이미 경고면 그대로 두고, 아니면 경고 추가
						if (!isWarn(p))
						{
							wLine[x] = U'!';
							needWarn = true;
						}
					}
				}
			}

			if (needWarn)
			{
				warn.duration = kMinFrameDuration;
				out.frames.push_back(std::move(warn));
			}

			// 현재 프레임도 최소 지속시간 보장
			if (cur.duration < kMinFrameDuration)
				cur.duration = kMinFrameDuration;

			out.frames.push_back(cur);
			prev = out.frames.back();
		}

		// 3차: 파괴→경고 사이의 최소 간격은 위 0.5초 하한으로 충족
		//      (필요 시 추가 규칙 삽입 가능: 이동형 세부 파괴 프레임 강제 등)

		return out;
	}

	// -------------------------
	// 이하: 원본 패턴 정의
	// 모든 getXXXPattern은 마지막에 FixPattern을 통해 자동 보정되어 반환된다
	// -------------------------

	// 패턴 1: 양쪽에서 채우기 (세로 채움)
	inline BossWallPattern getVerticalFillPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"!..............!",
			U"!..............!",
			U"!..............!",
			U"!..............!",
			U"!..............!",
			U"!..............!"
		};
		frame0.duration = 1.0;
		pattern.frames.push_back(frame0);

		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"W..............W",
			U"W..............W",
			U"W..............W",
			U"W..............W",
			U"W..............W",
			U"W..............W"
		};
		frame1.duration = 0.5;
		pattern.frames.push_back(frame1);

		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"W!............!W",
			U"W!............!W",
			U"W!............!W",
			U"W!............!W",
			U"W!............!W",
			U"W!............!W"
		};
		frame2.duration = 0.5;
		pattern.frames.push_back(frame2);

		BossWallPatternFrame frame3;
		frame3.pattern = {
			U"WW............WW",
			U"WW............WW",
			U"WW............WW",
			U"WW............WW",
			U"WW............WW",
			U"WW............WW"
		};
		frame3.duration = 0.5;
		pattern.frames.push_back(frame3);

		BossWallPatternFrame frame4;
		frame4.pattern = {
			U"WW!..........!WW",
			U"WW!..........!WW",
			U"WW!..........!WW",
			U"WW!..........!WW",
			U"WW!..........!WW",
			U"WW!..........!WW"
		};
		frame4.duration = 0.5;
		pattern.frames.push_back(frame4);

		BossWallPatternFrame frame5;
		frame5.pattern = {
			U"WWW..........WWW",
			U"WWW..........WWW",
			U"WWW..........WWW",
			U"WWW..........WWW",
			U"WWW..........WWW",
			U"WWW..........WWW"
		};
		frame5.duration = 0.5;
		pattern.frames.push_back(frame5);

		BossWallPatternFrame frame6;
		frame6.pattern = {
			U"WWW!........!WWW",
			U"WWW!........!WWW",
			U"WWW!........!WWW",
			U"WWW!........!WWW",
			U"WWW!........!WWW",
			U"WWW!........!WWW"
		};
		frame6.duration = 0.5;
		pattern.frames.push_back(frame6);

		BossWallPatternFrame frame7;
		frame7.pattern = {
			U"WWWW........WWWW",
			U"WWWW........WWWW",
			U"WWWW........WWWW",
			U"WWWW........WWWW",
			U"WWWW........WWWW",
			U"WWWW........WWWW"
		};
		frame7.duration = 0.5;
		pattern.frames.push_back(frame7);

		BossWallPatternFrame frame8;
		frame8.pattern = {
			U"WWWW!......!WWWW",
			U"WWWW!......!WWWW",
			U"WWWW!......!WWWW",
			U"WWWW!......!WWWW",
			U"WWWW!......!WWWW",
			U"WWWW!......!WWWW"
		};
		frame8.duration = 0.5;
		pattern.frames.push_back(frame8);

		BossWallPatternFrame frame9;
		frame9.pattern = {
			U"WWWWW......WWWWW",
			U"WWWWW......WWWWW",
			U"WWWWW......WWWWW",
			U"WWWWW......WWWWW",
			U"WWWWW......WWWWW",
			U"WWWWW......WWWWW"
		};
		frame9.duration = 0.5;
		pattern.frames.push_back(frame9);

		BossWallPatternFrame frame10;
		frame10.pattern = {
			U"WWWWW!....!WWWWW",
			U"WWWWW!....!WWWWW",
			U"WWWWW!....!WWWWW",
			U"WWWWW!....!WWWWW",
			U"WWWWW!....!WWWWW",
			U"WWWWW!....!WWWWW"
		};
		frame10.duration = 0.5;
		pattern.frames.push_back(frame10);

		BossWallPatternFrame frame11;
		frame11.pattern = {
			U"WWWWWW....WWWWWW",
			U"WWWWWW....WWWWWW",
			U"WWWWWW....WWWWWW",
			U"WWWWWW....WWWWWW",
			U"WWWWWW....WWWWWW",
			U"WWWWWW....WWWWWW"
		};
		frame11.duration = 0.5;
		pattern.frames.push_back(frame11);

		BossWallPatternFrame frame12;
		frame12.pattern = {
			U"WWWWWW!..!WWWWWW",
			U"WWWWWW!..!WWWWWW",
			U"WWWWWW!..!WWWWWW",
			U"WWWWWW!..!WWWWWW",
			U"WWWWWW!..!WWWWWW",
			U"WWWWWW!..!WWWWWW"
		};
		frame12.duration = 0.5;
		pattern.frames.push_back(frame12);

		BossWallPatternFrame frame13;
		frame13.pattern = {
			U"WWWWWWW..WWWWWWW",
			U"WWWWWWW..WWWWWWW",
			U"WWWWWWW..WWWWWWW",
			U"WWWWWWW..WWWWWWW",
			U"WWWWWWW..WWWWWWW",
			U"WWWWWWW..WWWWWWW"
		};
		frame13.duration = 0.5;
		pattern.frames.push_back(frame13);

		BossWallPatternFrame frame14;
		frame14.pattern = {
			U"XXXXXXX..XXXXXXX",
			U"XXXXXXX..XXXXXXX",
			U"XXXXXXX..XXXXXXX",
			U"XXXXXXX..XXXXXXX",
			U"XXXXXXX..XXXXXXX",
			U"XXXXXXX..XXXXXXX"
		};
		frame14.duration = 0.5;
		pattern.frames.push_back(frame14);

		return FixPattern(std::move(pattern));
	}

	// 패턴 2: 가로 웨이브 (위에서 아래로 한 줄씩)
	inline BossWallPattern getHorizontalWavePattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		for (int32 row = 0; row < 6; ++row)
		{
			// 경고
			BossWallPatternFrame warning;
			warning.pattern.resize(6);
			for (int32 y = 0; y < 6; ++y)
			{
				if (y == row) warning.pattern[y] = U"!!!!!!!!!!!!!!!!";
				else          warning.pattern[y] = U"................";
			}
			warning.duration = 0.5;
			pattern.frames.push_back(warning);

			// 벽 생성
			BossWallPatternFrame wall;
			wall.pattern.resize(6);
			for (int32 y = 0; y < 6; ++y)
			{
				if (y == row) wall.pattern[y] = U"WWWWWWWWWWWWWWWW";
				else          wall.pattern[y] = U"................";
			}
			wall.duration = 0.8;
			pattern.frames.push_back(wall);

			// 벽 파괴
			BossWallPatternFrame destroy;
			destroy.pattern.resize(6);
			for (int32 y = 0; y < 6; ++y)
			{
				if (y == row) destroy.pattern[y] = U"XXXXXXXXXXXXXXXX";
				else          destroy.pattern[y] = U"................";
			}
			destroy.duration = 0.2; // FixPattern이 0.5로 상향
			pattern.frames.push_back(destroy);
		}

		return FixPattern(std::move(pattern));
	}

	// 패턴 3: 세로 웨이브 (왼쪽에서 오른쪽으로)
	inline BossWallPattern getVerticalWavePattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		BossWallPatternFrame f0;
		f0.pattern = {
			U"!.!.!.!.!.!.!.!.",
			U"!.!.!.!.!.!.!.!.",
			U"!.!.!.!.!.!.!.!.",
			U"!.!.!.!.!.!.!.!.",
			U"!.!.!.!.!.!.!.!.",
			U"!.!.!.!.!.!.!.!.",
		};
		f0.duration = 1.0;
		pattern.frames.push_back(f0);

		BossWallPatternFrame f1;
		f1.pattern = {
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W.",
		};
		f1.duration = 1.0;
		pattern.frames.push_back(f1);

		BossWallPatternFrame f2;
		f2.pattern = {
			U"X.X.X.X.X.X.X.X.",
			U"X.X.X.X.X.X.X.X.",
			U"X.X.X.X.X.X.X.X.",
			U"X.X.X.X.X.X.X.X.",
			U"X.X.X.X.X.X.X.X.",
			U"X.X.X.X.X.X.X.X.",
		};
		f2.duration = 1.0;
		pattern.frames.push_back(f2);

		BossWallPatternFrame f3;
		f3.pattern = {
			U"................",
			U"................",
			U"................",
			U"................",
			U"................",
			U"................",
		};
		f3.duration = 1.0;
		pattern.frames.push_back(f3);

		BossWallPatternFrame f4;
		f4.pattern = {
			U".!.!.!.!.!.!.!.!",
			U".!.!.!.!.!.!.!.!",
			U".!.!.!.!.!.!.!.!",
			U".!.!.!.!.!.!.!.!",
			U".!.!.!.!.!.!.!.!",
			U".!.!.!.!.!.!.!.!",
		};
		f4.duration = 1.0;
		pattern.frames.push_back(f4);

		BossWallPatternFrame f5;
		f5.pattern = {
			U".W.W.W.W.W.W.W.W",
			U".W.W.W.W.W.W.W.W",
			U".W.W.W.W.W.W.W.W",
			U".W.W.W.W.W.W.W.W",
			U".W.W.W.W.W.W.W.W",
			U".W.W.W.W.W.W.W.W",
		};
		f5.duration = 1.0;
		pattern.frames.push_back(f5);

		BossWallPatternFrame f6;
		f6.pattern = {
			U".X.X.X.X.X.X.X.X",
			U".X.X.X.X.X.X.X.X",
			U".X.X.X.X.X.X.X.X",
			U".X.X.X.X.X.X.X.X",
			U".X.X.X.X.X.X.X.X",
			U".X.X.X.X.X.X.X.X",
		};
		f6.duration = 1.0;
		pattern.frames.push_back(f6);

		BossWallPatternFrame f7;
		f7.pattern = {
			U"................",
			U"................",
			U"................",
			U"................",
			U"................",
			U"................",
		};
		f7.duration = 1.0;;
		pattern.frames.push_back(f7);

		return FixPattern(std::move(pattern));
	}

	// 패턴 4: 십자가 공격 (중앙 십자)
	inline BossWallPattern getCrossPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 세로선 경고
		BossWallPatternFrame frame0;
		frame0.pattern = {
			U".......!!.......",
			U".......!!.......",
			U".......!!.......",
			U".......!!.......",
			U".......!!.......",
			U".......!!......."
		};
		frame0.duration = 1.0;
		pattern.frames.push_back(frame0);

		// 세로선 생성
		BossWallPatternFrame frame1;
		frame1.pattern = {
			U".......WW.......",
			U".......WW.......",
			U".......WW.......",
			U".......WW.......",
			U".......WW.......",
			U".......WW......."
		};
		frame1.duration = 1.0;
		pattern.frames.push_back(frame1);

		// 가로선 경고 추가
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U".......WW.......",
			U".......WW.......",
			U"!!!!!!!WW!!!!!!!",
			U"!!!!!!!WW!!!!!!!",
			U".......WW.......",
			U".......WW......."
		};
		frame2.duration = 1.0;
		pattern.frames.push_back(frame2);

		// 십자 완성
		BossWallPatternFrame frame3;
		frame3.pattern = {
			U".......WW.......",
			U".......WW.......",
			U"WWWWWWWWWWWWWWWW",
			U"WWWWWWWWWWWWWWWW",
			U".......WW.......",
			U".......WW......."
		};
		frame3.duration = 1.2;
		pattern.frames.push_back(frame3);

		// 모두 파괴
		BossWallPatternFrame frame4;
		frame4.pattern = {
			U".......XX.......",
			U".......XX.......",
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX",
			U".......XX.......",
			U".......XX......."
		};
		frame4.duration = 0.5;
		pattern.frames.push_back(frame4);

		return FixPattern(std::move(pattern));
	}

	// 패턴 5: X자 공격 (대각선)
	inline BossWallPattern getDiagonalXPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// ===== 1단계: 왼쪽 대각선(/) 3줄 겹침 =====
		// 경고
		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"!..!..!.........",
			U".!..!..!........",
			U"..!..!..!.......",
			U"...!..!..!......",
			U"....!..!..!.....",
			U".....!..!..!...."
		};
		frame0.duration = 0.8;
		pattern.frames.push_back(frame0);

		// 벽 생성
		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"W..W..W.........",
			U".W..W..W........",
			U"..W..W..W.......",
			U"...W..W..W......",
			U"....W..W..W.....",
			U".....W..W..W...."
		};
		frame1.duration = 1.0;
		pattern.frames.push_back(frame1);

		// 벽 파괴
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"X..X..X.........",
			U".X..X..X........",
			U"..X..X..X.......",
			U"...X..X..X......",
			U"....X..X..X.....",
			U".....X..X..X...."
		};
		frame2.duration = 0.3; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame2);

		// ===== 2단계: 오른쪽 대각선(\) 3줄 겹침 =====
		// 경고
		BossWallPatternFrame frame3;
		frame3.pattern = {
			U".........!..!..!",
			U"........!..!..!.",
			U".......!..!..!..",
			U"......!..!..!...",
			U".....!..!..!....",
			U"....!..!..!....."
		};
		frame3.duration = 0.8;
		pattern.frames.push_back(frame3);

		// 벽 생성
		BossWallPatternFrame frame4;
		frame4.pattern = {
			U".........W..W..W",
			U"........W..W..W.",
			U".......W..W..W..",
			U"......W..W..W...",
			U".....W..W..W....",
			U"....W..W..W....."
		};
		frame4.duration = 1.0;
		pattern.frames.push_back(frame4);

		// 벽 파괴
		BossWallPatternFrame frame5;
		frame5.pattern = {
			U".........X..X..X",
			U"........X..X..X.",
			U".......X..X..X..",
			U"......X..X..X...",
			U".....X..X..X....",
			U"....X..X..X....."
		};
		frame5.duration = 0.3; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame5);

		// ===== 3단계: X자 2개 동시 공격 =====
		// 경고
		BossWallPatternFrame frame6;
		frame6.pattern = {
			U"!..!.......!..!",
			U".!..!.....!..!.",
			U"..!..!...!..!..",
			U"...!..!.!..!...",
			U"....!..!..!....",
			U".....!...!....."
		};
		frame6.duration = 1.0;
		pattern.frames.push_back(frame6);

		// 벽 생성
		BossWallPatternFrame frame7;
		frame7.pattern = {
			U"W..W.......W..W",
			U".W..W.....W..W.",
			U"..W..W...W..W..",
			U"...W..W.W..W...",
			U"....W..W..W....",
			U".....W...W....."
		};
		frame7.duration = 1.2;
		pattern.frames.push_back(frame7);

		// 벽 파괴
		BossWallPatternFrame frame8;
		frame8.pattern = {
			U"X..X.......X..X",
			U".X..X.....X..X.",
			U"..X..X...X..X..",
			U"...X..X.X..X...",
			U"....X..X..X....",
			U".....X...X....."
		};
		frame8.duration = 0.3; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame8);

		// ===== 4단계: 조밀한 대각선 망 (/) 4개 =====
		// 경고
		BossWallPatternFrame frame9;
		frame9.pattern = {
			U"!!..!!..!!..!!..",
			U".!!..!!..!!..!!.",
			U"..!!..!!..!!..!!",
			U"!..!!..!!..!!..!",
			U"!!..!!..!!..!!..",
			U".!!..!!..!!..!!."
		};
		frame9.duration = 1.0;
		pattern.frames.push_back(frame9);

		// 벽 생성
		BossWallPatternFrame frame10;
		frame10.pattern = {
			U"WW..WW..WW..WW..",
			U".WW..WW..WW..WW.",
			U"..WW..WW..WW..WW",
			U"W..WW..WW..WW..W",
			U"WW..WW..WW..WW..",
			U".WW..WW..WW..WW."
		};
		frame10.duration = 1.5;
		pattern.frames.push_back(frame10);

		// 벽 파괴
		BossWallPatternFrame frame11;
		frame11.pattern = {
			U"XX..XX..XX..XX..",
			U".XX..XX..XX..XX.",
			U"..XX..XX..XX..XX",
			U"X..XX..XX..XX..X",
			U"XX..XX..XX..XX..",
			U".XX..XX..XX..XX."
		};
		frame11.duration = 0.3; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame11);

		// ===== 5단계: 조밀한 대각선 망 (\) 4개 =====
		// 경고
		BossWallPatternFrame frame12;
		frame12.pattern = {
			U"..!!..!!..!!..!!",
			U".!!..!!..!!..!!.",
			U"!!..!!..!!..!!..",
			U"!..!!..!!..!!..!",
			U"..!!..!!..!!..!!",
			U".!!..!!..!!..!!."
		};
		frame12.duration = 1.0;
		pattern.frames.push_back(frame12);

		// 벽 생성
		BossWallPatternFrame frame13;
		frame13.pattern = {
			U"..WW..WW..WW..WW",
			U".WW..WW..WW..WW.",
			U"WW..WW..WW..WW..",
			U"W..WW..WW..WW..W",
			U"..WW..WW..WW..WW",
			U".WW..WW..WW..WW."
		};
		frame13.duration = 1.5;
		pattern.frames.push_back(frame13);

		// 벽 파괴
		BossWallPatternFrame frame14;
		frame14.pattern = {
			U"..XX..XX..XX..XX",
			U".XX..XX..XX..XX.",
			U"XX..XX..XX..XX..",
			U"X..XX..XX..XX..X",
			U"..XX..XX..XX..XX",
			U".XX..XX..XX..XX."
		};
		frame14.duration = 0.3; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame14);

		return FixPattern(std::move(pattern));
	}

	// 패턴 6: 스파이크 연속
	inline BossWallPattern getSpikePattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"................",
			U"................",
			U"................",
			U"................",
			U"................",
			U"!.!.!.!.!.!.!.!."
		};
		frame0.duration = 0.8;
		pattern.frames.push_back(frame0);

		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"................",
			U"................",
			U"................",
			U"!.!.!.!.!.!.!.!.",
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W."
		};
		frame1.duration = 0.4; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame1);

		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"................",
			U"................",
			U"................",
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W."
		};
		frame2.duration = 0.6;
		pattern.frames.push_back(frame2);

		BossWallPatternFrame frame3;
		frame3.pattern = {
			U"................",
			U"................",
			U"................",
			U"X.X.X.X.X.X.X.X.",
			U"X.X.X.X.X.X.X.X.",
			U"X.X.X.X.X.X.X.X."
		};
		frame3.duration = 0.2; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame3);

		BossWallPatternFrame frame4;
		frame4.pattern = {
			U"................",
			U"................",
			U"................",
			U"................",
			U"................",
			U".!.!.!.!.!.!.!.!"
		};
		frame4.duration = 0.8;
		pattern.frames.push_back(frame4);

		BossWallPatternFrame frame5;
		frame5.pattern = {
			U"................",
			U"................",
			U"................",
			U".!.!.!.!.!.!.!.!",
			U".W.W.W.W.W.W.W.W",
			U".W.W.W.W.W.W.W.W"
		};
		frame5.duration = 0.4; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame5);

		// 스파이크 2단계
		BossWallPatternFrame frame6;
		frame6.pattern = {
			U"................",
			U"................",
			U"................",
			U".W.W.W.W.W.W.W.W",
			U".W.W.W.W.W.W.W.W",
			U".W.W.W.W.W.W.W.W"
		};
		frame6.duration = 0.6;
		pattern.frames.push_back(frame6);

		// 스파이크 파괴
		BossWallPatternFrame frame7;
		frame7.pattern = {
			U"................",
			U"................",
			U"................",
			U".X.X.X.X.X.X.X.X",
			U".X.X.X.X.X.X.X.X",
			U".X.X.X.X.X.X.X.X"
		};
		frame7.duration = 0.2; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame7);

		// ===== 3단계: 상단 역스파이크 (짝수 칸) =====
		// 상단 경고
		BossWallPatternFrame frame8;
		frame8.pattern = {
			U"!.!.!.!.!.!.!.!.",
			U"................",
			U"................",
			U"................",
			U"................",
			U"................"
		};
		frame8.duration = 0.8;
		pattern.frames.push_back(frame8);

		// 역스파이크 1단계 (상단 2줄) - 아래쪽에 경고
		BossWallPatternFrame frame9;
		frame9.pattern = {
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W.",
			U"!.!.!.!.!.!.!.!.", // 경고
			U"................",
			U"................",
			U"................"
		};
		frame9.duration = 0.4; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame9);

		// 역스파이크 2단계 (상단 3줄)
		BossWallPatternFrame frame10;
		frame10.pattern = {
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W.",
			U"................",
			U"................",
			U"................"
		};
		frame10.duration = 0.6;
		pattern.frames.push_back(frame10);

		// 역스파이크 파괴
		BossWallPatternFrame frame11;
		frame11.pattern = {
			U"X.X.X.X.X.X.X.X.",
			U"X.X.X.X.X.X.X.X.",
			U"X.X.X.X.X.X.X.X.",
			U"................",
			U"................",
			U"................"
		};
		frame11.duration = 0.2; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame11);

		// ===== 4단계: 상단 역스파이크 (홀수 칸 - 한 칸 옆) =====
		// 상단 경고 (홀수)
		BossWallPatternFrame frame12;
		frame12.pattern = {
			U".!.!.!.!.!.!.!.!",
			U"................",
			U"................",
			U"................",
			U"................",
			U"................"
		};
		frame12.duration = 0.8;
		pattern.frames.push_back(frame12);

		// 역스파이크 1단계 - 아래쪽에 경고
		BossWallPatternFrame frame13;
		frame13.pattern = {
			U".W.W.W.W.W.W.W.W",
			U".W.W.W.W.W.W.W.W",
			U".!.!.!.!.!.!.!.!", // 경고
			U"................",
			U"................",
			U"................"
		};
		frame13.duration = 0.4; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame13);

		// 역스파이크 2단계
		BossWallPatternFrame frame14;
		frame14.pattern = {
			U".W.W.W.W.W.W.W.W",
			U".W.W.W.W.W.W.W.W",
			U".W.W.W.W.W.W.W.W",
			U"................",
			U"................",
			U"................"
		};
		frame14.duration = 0.6;
		pattern.frames.push_back(frame14);

		// 역스파이크 파괴
		BossWallPatternFrame frame15;
		frame15.pattern = {
			U".X.X.X.X.X.X.X.X",
			U".X.X.X.X.X.X.X.X",
			U".X.X.X.X.X.X.X.X",
			U"................",
			U"................",
			U"................"
		};
		frame15.duration = 0.2; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame15);

		// ===== 5단계: 상하 동시 공격 (최종) =====
		// 상하 동시 경고
		BossWallPatternFrame frame16;
		frame16.pattern = {
			U"!.!.!.!.!.!.!.!.",
			U"................",
			U"................",
			U"................",
			U"................",
			U".!.!.!.!.!.!.!.!"
		};
		frame16.duration = 1.0;
		pattern.frames.push_back(frame16);

		// 상하 동시 스파이크 1단계 - 중간에 경고
		BossWallPatternFrame frame17;
		frame17.pattern = {
			U"W.W.W.W.W.W.W.W.",
			U"!.!.!.!.!.!.!.!.", // 상단 경고
			U"................",
			U"................",
			U".!.!.!.!.!.!.!.!", // 하단 경고
			U".W.W.W.W.W.W.W.W"
		};
		frame17.duration = 0.4; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame17);

		// 상하 동시 스파이크 2단계 (중앙으로 압박)
		BossWallPatternFrame frame18;
		frame18.pattern = {
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W.",
			U"................",
			U"................",
			U".W.W.W.W.W.W.W.W",
			U".W.W.W.W.W.W.W.W"
		};
		frame18.duration = 0.8;
		pattern.frames.push_back(frame18);

		// 상하 동시 파괴
		BossWallPatternFrame frame19;
		frame19.pattern = {
			U"X.X.X.X.X.X.X.X.",
			U"X.X.X.X.X.X.X.X.",
			U"................",
			U"................",
			U".X.X.X.X.X.X.X.X",
			U".X.X.X.X.X.X.X.X"
		};
		frame19.duration = 0.3; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame19);

		return FixPattern(std::move(pattern));
	}

	// 패턴 7: 세로/가로 교대 이동
	inline BossWallPattern getAlternatingPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 세로선이 열 0부터 15까지 이동 (총 16단계)
		for (int32 col = 0; col < 16; ++col)
		{
			// 가로선 위치 계산 (세로선 3칸 이동마다 가로선 1칸 하강)
			int32 row = (col / 3) % 6;

			// 1단계: 모두 제거 (빈 화면)
			BossWallPatternFrame frameEmpty;
			frameEmpty.pattern = {
				U"................",
				U"................",
				U"................",
				U"................",
				U"................",
				U"................"
			};
			frameEmpty.duration = 0.2; // FixPattern이 0.5로 상향
			pattern.frames.push_back(frameEmpty);

			// 2단계: 경고 프레임
			BossWallPatternFrame frameWarn;
			frameWarn.pattern.resize(6);
			for (int32 y = 0; y < 6; ++y)
			{
				String line = U"................";

				// 가로선 경고 (전체 행)
				if (y == row)
				{
					for (int32 x = 0; x < 16; ++x)
						line[x] = U'!';
				}

				// 세로선 경고 (전체 열)
				line[col] = U'!';

				frameWarn.pattern[y] = line;
			}
			frameWarn.duration = 0.5;
			pattern.frames.push_back(frameWarn);

			// 3단계: 벽 생성 프레임
			BossWallPatternFrame frameWall;
			frameWall.pattern.resize(6);
			for (int32 y = 0; y < 6; ++y)
			{
				String line = U"................";

				// 가로선
				if (y == row)
				{
					for (int32 x = 0; x < 16; ++x)
						line[x] = U'W';
				}

				// 세로선
				line[col] = U'W';

				frameWall.pattern[y] = line;
			}
			frameWall.duration = 0.5;
			pattern.frames.push_back(frameWall);

			// 최종 파괴
			BossWallPatternFrame frameEnd;
			frameEnd.pattern = {
				U"XXXXXXXXXXXXXXXX",
				U"XXXXXXXXXXXXXXXX",
				U"XXXXXXXXXXXXXXXX",
				U"XXXXXXXXXXXXXXXX",
				U"XXXXXXXXXXXXXXXX",
				U"XXXXXXXXXXXXXXXX"
			};
			frameEnd.duration = 0.3; // FixPattern이 0.5로 상향
			pattern.frames.push_back(frameEnd);
		}

		return FixPattern(std::move(pattern));
	}

	// 패턴 8: 미로형 통로 이동
	inline BossWallPattern getMazePattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 1단계: 왼쪽 통로
		// 경고
		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"!!!!!.....!!!!!!",
			U"!!!!!.....!!!!!!",
			U"!!!!!.....!!!!!!",
			U"!!!!!.....!!!!!!",
			U"!!!!!.....!!!!!!",
			U"!!!!!.....!!!!!!"
		};
		frame0.duration = 0.6;
		pattern.frames.push_back(frame0);

		// 벽 생성
		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"WWWWW.....WWWWWW",
			U"WWWWW.....WWWWWW",
			U"WWWWW.....WWWWWW",
			U"WWWWW.....WWWWWW",
			U"WWWWW.....WWWWWW",
			U"WWWWW.....WWWWWW"
		};
		frame1.duration = 0.8;
		pattern.frames.push_back(frame1);

		// 2단계: 중앙-왼쪽 통로로 이동 (경고)
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"WWWWW!!...!!WWWW",
			U"WWWWW!!...!!WWWW",
			U"WWWWW!!...!!WWWW",
			U"WWWWW!!...!!WWWW",
			U"WWWWW!!...!!WWWW",
			U"WWWWW!!...!!WWWW"
		};
		frame2.duration = 0.5;
		pattern.frames.push_back(frame2);

		// 벽 변경
		BossWallPatternFrame frame3;
		frame3.pattern = {
			U"WWWWWWW...WWWWWW",
			U"WWWWWWW...WWWWWW",
			U"WWWWWWW...WWWWWW",
			U"WWWWWWW...WWWWWW",
			U"WWWWWWW...WWWWWW",
			U"WWWWWWW...WWWWWW"
		};
		frame3.duration = 0.8;
		pattern.frames.push_back(frame3);

		// 3단계: 중앙 통로 (경고)
		BossWallPatternFrame frame4;
		frame4.pattern = {
			U"WWWWWWW!!.!!WWWW",
			U"WWWWWWW!!.!!WWWW",
			U"WWWWWWW!!.!!WWWW",
			U"WWWWWWW!!.!!WWWW",
			U"WWWWWWW!!.!!WWWW",
			U"WWWWWWW!!.!!WWWW"
		};
		frame4.duration = 0.5;
		pattern.frames.push_back(frame4);

		// 벽 변경 (좁은 중앙 통로)
		BossWallPatternFrame frame5;
		frame5.pattern = {
			U"WWWWWWWW..WWWWWW",
			U"WWWWWWWW..WWWWWW",
			U"WWWWWWWW..WWWWWW",
			U"WWWWWWWW..WWWWWW",
			U"WWWWWWWW..WWWWWW",
			U"WWWWWWWW..WWWWWW"
		};
		frame5.duration = 0.8;
		pattern.frames.push_back(frame5);

		// 4단계: 중앙-오른쪽 통로 (경고)
		BossWallPatternFrame frame6;
		frame6.pattern = {
			U"WWWWWWWW!!..!WWW",
			U"WWWWWWWW!!..!WWW",
			U"WWWWWWWW!!..!WWW",
			U"WWWWWWWW!!..!WWW",
			U"WWWWWWWW!!..!WWW",
			U"WWWWWWWW!!..!WWW"
		};
		frame6.duration = 0.5;
		pattern.frames.push_back(frame6);

		// 벽 변경
		BossWallPatternFrame frame7;
		frame7.pattern = {
			U"WWWWWWWWWW...WWW",
			U"WWWWWWWWWW...WWW",
			U"WWWWWWWWWW...WWW",
			U"WWWWWWWWWW...WWW",
			U"WWWWWWWWWW...WWW",
			U"WWWWWWWWWW...WWW"
		};
		frame7.duration = 0.8;
		pattern.frames.push_back(frame7);

		// 5단계: 오른쪽 통로 (경고)
		BossWallPatternFrame frame8;
		frame8.pattern = {
			U"WWWWWWWWWW!!..WW",
			U"WWWWWWWWWW!!..WW",
			U"WWWWWWWWWW!!..WW",
			U"WWWWWWWWWW!!..WW",
			U"WWWWWWWWWW!!..WW",
			U"WWWWWWWWWW!!..WW"
		};
		frame8.duration = 0.5;
		pattern.frames.push_back(frame8);

		// 벽 변경
		BossWallPatternFrame frame9;
		frame9.pattern = {
			U"WWWWWWWWWWWW..WW",
			U"WWWWWWWWWWWW..WW",
			U"WWWWWWWWWWWW..WW",
			U"WWWWWWWWWWWW..WW",
			U"WWWWWWWWWWWW..WW",
			U"WWWWWWWWWWWW..WW"
		};
		frame9.duration = 0.8;
		pattern.frames.push_back(frame9);

		// 6단계: 다시 왼쪽으로 (역순)
		// 중앙-오른쪽 경고
		BossWallPatternFrame frame10;
		frame10.pattern = {
			U"WWWWWWWWW!!...WW",
			U"WWWWWWWWW!!...WW",
			U"WWWWWWWWW!!...WW",
			U"WWWWWWWWW!!...WW",
			U"WWWWWWWWW!!...WW",
			U"WWWWWWWWW!!...WW"
		};
		frame10.duration = 0.5;
		pattern.frames.push_back(frame10);

		// 벽 변경
		BossWallPatternFrame frame11;
		frame11.pattern = {
			U"WWWWWWWWW....WWW",
			U"WWWWWWWWW....WWW",
			U"WWWWWWWWW....WWW",
			U"WWWWWWWWW....WWW",
			U"WWWWWWWWW....WWW",
			U"WWWWWWWWW....WWW"
		};
		frame11.duration = 0.8;
		pattern.frames.push_back(frame11);

		// 중앙 경고
		BossWallPatternFrame frame12;
		frame12.pattern = {
			U"WWWWWWW!!....WWW",
			U"WWWWWWW!!....WWW",
			U"WWWWWWW!!....WWW",
			U"WWWWWWW!!....WWW",
			U"WWWWWWW!!....WWW",
			U"WWWWWWW!!....WWW"
		};
		frame12.duration = 0.5;
		pattern.frames.push_back(frame12);

		// 벽 변경
		BossWallPatternFrame frame13;
		frame13.pattern = {
			U"WWWWWWW.....WWWW",
			U"WWWWWWW.....WWWW",
			U"WWWWWWW.....WWWW",
			U"WWWWWWW.....WWWW",
			U"WWWWWWW.....WWWW",
			U"WWWWWWW.....WWWW"
		};
		frame13.duration = 0.8;
		pattern.frames.push_back(frame13);

		// 모두 파괴
		BossWallPatternFrame frame14;
		frame14.pattern = {
			U"XXXXXXX.....XXXX",
			U"XXXXXXX.....XXXX",
			U"XXXXXXX.....XXXX",
			U"XXXXXXX.....XXXX",
			U"XXXXXXX.....XXXX",
			U"XXXXXXX.....XXXX"
		};
		frame14.duration = 0.5;
		pattern.frames.push_back(frame14);

		return FixPattern(std::move(pattern));
	}

	// 패턴 9: 체커보드 (안전지대 찾기)
	inline BossWallPattern getCheckerboardPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 1단계: 체커보드 패턴 1 (왼쪽 위 시작)
		// 경고
		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"!!..!!..!!..!!..",
			U"!!..!!..!!..!!..",
			U"..!!..!!..!!..!!",
			U"..!!..!!..!!..!!",
			U"!!..!!..!!..!!..",
			U"!!..!!..!!..!!.."
		};
		frame0.duration = 0.8;
		pattern.frames.push_back(frame0);

		// 벽 생성
		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"WW..WW..WW..WW..",
			U"WW..WW..WW..WW..",
			U"..WW..WW..WW..WW",
			U"..WW..WW..WW..WW",
			U"WW..WW..WW..WW..",
			U"WW..WW..WW..WW.."
		};
		frame1.duration = 1.0;
		pattern.frames.push_back(frame1);

		// 2단계: 반전 (역체커보드)
		// 경고 (이전 벽 유지하며 빈 칸 경고)
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"WW!!WW!!WW!!WW!!",
			U"WW!!WW!!WW!!WW!!",
			U"!!WW!!WW!!WW!!WW",
			U"!!WW!!WW!!WW!!WW",
			U"WW!!WW!!WW!!WW!!",
			U"WW!!WW!!WW!!WW!!"
		};
		frame2.duration = 0.8;
		pattern.frames.push_back(frame2);

		// 반전 벽 생성
		BossWallPatternFrame frame3;
		frame3.pattern = {
			U"XXWWXXWWXXWWXXWW",
			U"XXWWXXWWXXWWXXWW",
			U"WWXXWWXXWWXXWWXX",
			U"WWXXWWXXWWXXWWXX",
			U"XXWWXXWWXXWWXXWW",
			U"XXWWXXWWXXWWXXWW"
		};
		frame3.duration = 1.0;
		pattern.frames.push_back(frame3);

		// 3단계: 다시 원래 패턴
		// 경고
		BossWallPatternFrame frame4;
		frame4.pattern = {
			U"!!WWWW!!WWWW!!WW",
			U"!!WWWW!!WWWW!!WW",
			U"WW!!WWWW!!WWWW!!",
			U"WW!!WWWW!!WWWW!!",
			U"!!WWWW!!WWWW!!WW",
			U"!!WWWW!!WWWW!!WW"
		};
		frame4.duration = 0.6;
		pattern.frames.push_back(frame4);

		// 원래 패턴
		BossWallPatternFrame frame5;
		frame5.pattern = {
			U"WWXXWWXXWWXXWWXX",
			U"WWXXWWXXWWXXWWXX",
			U"XXWWXXWWXXWWXXWW",
			U"XXWWXXWWXXWWXXWW",
			U"WWXXWWXXWWXXWWXX",
			U"WWXXWWXXWWXXWWXX"
		};
		frame5.duration = 0.8;
		pattern.frames.push_back(frame5);

		// 모두 파괴
		BossWallPatternFrame frame6;
		frame6.pattern = {
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX"
		};
		frame6.duration = 0.3; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame6);

		return FixPattern(std::move(pattern));
	}

	// 패턴 10: 랜덤 탄막
	inline BossWallPattern getRandomBulletPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 랜덤 경고 1
		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"..!...!...!.....",
			U"...!.....!.....!",
			U".!....!.........",
			U"......!....!....",
			U"..!.........!...",
			U"....!....!......"
		};
		frame0.duration = 0.8;
		pattern.frames.push_back(frame0);

		// 랜덤 벽 1
		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"..W...W...W.....",
			U"...W.....W.....W",
			U".W....W.........",
			U"......W....W....",
			U"..W.........W...",
			U"....W....W......"
		};
		frame1.duration = 1.0;
		pattern.frames.push_back(frame1);

		// 랜덤 경고 2 (다른 위치)
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"..W.!.W.!.W..!..",
			U"!..W..!..W...!.W",
			U".W.!..W.!.......",
			U"...!..W.!..W....",
			U"..W..!...!..W...",
			U".!..W..!.W.!...."
		};
		frame2.duration = 0.8;
		pattern.frames.push_back(frame2);

		// 랜덤 벽 2
		BossWallPatternFrame frame3;
		frame3.pattern = {
			U"..WWW.WWW.WW.W..",
			U"W..W..W..W...W.W",
			U".W.W..W.W.......",
			U"...W..W.W..W....",
			U"..W..W...W..W...",
			U".W..W..W.W.W...."
		};
		frame3.duration = 1.2;
		pattern.frames.push_back(frame3);

		// 모두 파괴
		BossWallPatternFrame frame4;
		frame4.pattern = {
			U"..XXX.XXX.XX.X..",
			U"X..X..X..X...X.X",
			U".X.X..X.X.......",
			U"...X..X.X..X....",
			U"..X..X...X..X...",
			U".X..X..X.X.X...."
		};
		frame4.duration = 0.5;
		pattern.frames.push_back(frame4);

		return FixPattern(std::move(pattern));
	}

	// 패턴 11: 펄스 웨이브 (중앙에서 퍼져나감)
	inline BossWallPattern getRotatingPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 웨이브 1: 중앙점 (경고→중앙 W)
		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"................",
			U"................",
			U".......!!.......",
			U".......!!.......",
			U"................",
			U"................"
		};
		frame0.duration = 0.3; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame0);

		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"................",
			U"................",
			U".......WW.......",
			U".......WW.......",
			U"................",
			U"................"
		};
		frame1.duration = 0.4; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame1);

		// 웨이브 2: 십자 확장
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U".......WW.......",
			U".......WW.......",
			U".WWWWWWWWWWWWWW.",
			U".WWWWWWWWWWWWWW.",
			U".......WW.......",
			U".......WW......."
		};
		frame2.duration = 0.5;
		pattern.frames.push_back(frame2);

		// 웨이브 3: X자 추가
		BossWallPatternFrame frame3;
		frame3.pattern = {
			U"W......WW......W",
			U".W.....WW.....W.",
			U".WWWWWWWWWWWWWW.",
			U".WWWWWWWWWWWWWW.",
			U".W.....WW.....W.",
			U"W......WW......W"
		};
		frame3.duration = 0.5;
		pattern.frames.push_back(frame3);

		// 웨이브 4: 외곽 확장
		BossWallPatternFrame frame4;
		frame4.pattern = {
			U"WWWWWWWWWWWWWWWW",
			U"WW.....WW.....WW",
			U"WWWWWWWWWWWWWWWW",
			U"WWWWWWWWWWWWWWWW",
			U"WW.....WW.....WW",
			U"WWWWWWWWWWWWWWWW"
		};
		frame4.duration = 0.6;
		pattern.frames.push_back(frame4);

		// 모두 파괴
		BossWallPatternFrame frame5;
		frame5.pattern = {
			U"XXXXXXXXXXXXXXXX",
			U"XX.....XX.....XX",
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX",
			U"XX.....XX.....XX",
			U"XXXXXXXXXXXXXXXX"
		};
		frame5.duration = 0.3; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame5);

		// 웨이브 2회: 더 빠르게 (원본에는 경고 누락 → FixPattern이 중앙 W 전에 경고 삽입)
		// 중앙
		BossWallPatternFrame frame6;
		frame6.pattern = {
			U"................",
			U"................",
			U".......WW.......",
			U".......WW.......",
			U"................",
			U"................"
		};
		frame6.duration = 0.3; // FixPattern이 0.5로 상향 (경고 자동 삽입)
		pattern.frames.push_back(frame6);

		// 십자
		BossWallPatternFrame frame7;
		frame7.pattern = {
			U".......WW.......",
			U".......WW.......",
			U".WWWWWWWWWWWWWW.",
			U".WWWWWWWWWWWWWW.",
			U".......WW.......",
			U".......WW......."
		};
		frame7.duration = 0.4; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame7);

		// X자
		BossWallPatternFrame frame8;
		frame8.pattern = {
			U"W......WW......W",
			U".W.....WW.....W.",
			U".WWWWWWWWWWWWWW.",
			U".WWWWWWWWWWWWWW.",
			U".W.....WW.....W.",
			U"W......WW......W"
		};
		frame8.duration = 0.4; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame8);

		// 외곽
		BossWallPatternFrame frame9;
		frame9.pattern = {
			U"WWWWWWWWWWWWWWWW",
			U"WW.....WW.....WW",
			U"WWWWWWWWWWWWWWWW",
			U"WWWWWWWWWWWWWWWW",
			U"WW.....WW.....WW",
			U"WWWWWWWWWWWWWWWW"
		};
		frame9.duration = 0.5;
		pattern.frames.push_back(frame9);

		// 파괴
		BossWallPatternFrame frame10;
		frame10.pattern = {
			U"XXXXXXXXXXXXXXXX",
			U"XX.....XX.....XX",
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX",
			U"XX.....XX.....XX",
			U"XXXXXXXXXXXXXXXX"
		};
		frame10.duration = 0.3; // FixPattern이 0.5로 상향
		pattern.frames.push_back(frame10);

		return FixPattern(std::move(pattern));
	}

	// 패턴 12: 박스 압축 (사방에서 좁혀오기)
	inline BossWallPattern getBoxShrinkPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 외곽 경고
		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"!!!!!!!!!!!!!!!!",
			U"!..............!",
			U"!..............!",
			U"!..............!",
			U"!..............!",
			U"!!!!!!!!!!!!!!!!"
		};
		frame0.duration = 1.0;
		pattern.frames.push_back(frame0);

		// 외곽 벽
		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"WWWWWWWWWWWWWWWW",
			U"W..............W",
			U"W..............W",
			U"W..............W",
			U"W..............W",
			U"WWWWWWWWWWWWWWWW"
		};
		frame1.duration = 0.8;
		pattern.frames.push_back(frame1);

		// 한 칸 안쪽 경고
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"WWWWWWWWWWWWWWWW",
			U"W!!!!!!!!!!!!!!W",
			U"W!............!W",
			U"W!............!W",
			U"W!!!!!!!!!!!!!!W",
			U"WWWWWWWWWWWWWWWW"
		};
		frame2.duration = 1.0;
		pattern.frames.push_back(frame2);

		// 한 칸 더 압축
		BossWallPatternFrame frame3;
		frame3.pattern = {
			U"WWWWWWWWWWWWWWWW",
			U"WWWWWWWWWWWWWWWW",
			U"WW............WW",
			U"WW............WW",
			U"WWWWWWWWWWWWWWWW",
			U"WWWWWWWWWWWWWWWW"
		};
		frame3.duration = 1.0;
		pattern.frames.push_back(frame3);

		// 모두 파괴
		BossWallPatternFrame frame4;
		frame4.pattern = {
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX",
			U"XX............XX",
			U"XX............XX",
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX"
		};
		frame4.duration = 0.5;
		pattern.frames.push_back(frame4);

		return FixPattern(std::move(pattern));
	}
}
