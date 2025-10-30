#pragma once
#include <Siv3D.hpp>

struct BossWallPatternFrame
{
	Array<String> pattern;
	double duration = 1.0;
};

// 전체 패턴 시퀀스
struct BossWallPattern
{
	Array<BossWallPatternFrame> frames;
	bool looping = true;
};

namespace BossWallPatternData
{

	inline BossWallPattern getVerticalFillPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 프레임 0: 양 끝 경고
		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"!.........!",
			U"!.........!",
			U"!.........!",
			U"!.........!",
			U"!.........!"
		};
		frame0.duration = 1.0;
		pattern.frames.push_back(frame0);

		// 프레임 1: 양 끝 벽 생성
		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"W.........W",
			U"W.........W",
			U"W.........W",
			U"W.........W",
			U"W.........W"
		};
		frame1.duration = 1.0;
		pattern.frames.push_back(frame1);

		// 프레임 2: 한 칸 더 안쪽 경고
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"W!.......!W",
			U"W!.......!W",
			U"W!.......!W",
			U"W!.......!W",
			U"W!.......!W"
		};
		frame2.duration = 1.0;
		pattern.frames.push_back(frame2);

		// 프레임 3: 벽 추가 생성
		BossWallPatternFrame frame3;
		frame3.pattern = {
			U"WW.......WW",
			U"WW.......WW",
			U"WW.......WW",
			U"WW.......WW",
			U"WW.......WW"
		};
		frame3.duration = 1.0;
		pattern.frames.push_back(frame3);

		// 프레임 4: 모든 벽 파괴
		BossWallPatternFrame frame4;
		frame4.pattern = {
			U"XX.......XX",
			U"XX.......XX",
			U"XX.......XX",
			U"XX.......XX",
			U"XX.......XX"
		};
		frame4.duration = 0.5;
		pattern.frames.push_back(frame4);

		return pattern;
	}

	// 예시: 랜덤 벽 생성 패턴
	inline BossWallPattern getRandomWallPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 프레임 0: 랜덤 위치 경고
		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"..!...!....",
			U"...!....!..",
			U".!.......!.",
			U"....!.!....",
			U"..!.....!.."
		};
		frame0.duration = 1.0;
		pattern.frames.push_back(frame0);

		// 프레임 1: 경고 위치에 벽 생성
		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"..W...W....",
			U"...W....W..",
			U".W.......W.",
			U"....W.W....",
			U"..W.....W.."
		};
		frame1.duration = 2.0;
		pattern.frames.push_back(frame1);

		// 프레임 2: 벽 파괴
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"..X...X....",
			U"...X....X..",
			U".X.......X.",
			U"....X.X....",
			U"..X.....X.."
		};
		frame2.duration = 0.5;
		pattern.frames.push_back(frame2);

		return pattern;
	}

	// 예시: 가로줄 이동 패턴
	inline BossWallPattern getHorizontalWavePattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 위에서 아래로 벽이 이동
		for (int32 row = 0; row < 5; ++row)
		{
			BossWallPatternFrame warning;
			warning.pattern.resize(5);
			for (int32 y = 0; y < 5; ++y)
			{
				if (y == row)
					warning.pattern[y] = U"!!!!!!!!!!!!";
				else
					warning.pattern[y] = U"............";
			}
			warning.duration = 0.5;
			pattern.frames.push_back(warning);

			BossWallPatternFrame wall;
			wall.pattern.resize(5);
			for (int32 y = 0; y < 5; ++y)
			{
				if (y == row)
					wall.pattern[y] = U"WWWWWWWWWWWW";
				else
					wall.pattern[y] = U"............";
			}
			wall.duration = 1.0;
			pattern.frames.push_back(wall);

			BossWallPatternFrame destroy;
			destroy.pattern.resize(5);
			for (int32 y = 0; y < 5; ++y)
			{
				if (y == row)
					destroy.pattern[y] = U"XXXXXXXXXXXX";
				else
					destroy.pattern[y] = U"............";
			}
			destroy.duration = 0.3;
			pattern.frames.push_back(destroy);
		}

		return pattern;
	}
}
