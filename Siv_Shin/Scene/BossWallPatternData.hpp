#pragma once
#include <Siv3D.hpp>

// 벽 패턴 프레임 구조체
struct BossWallPatternFrame
{
	Array<String> pattern; // 각 줄이 맵의 한 행
	double duration = 1.0; // 이 프레임이 유지되는 시간 (초)
};

// 전체 패턴 시퀀스
struct BossWallPattern
{
	Array<BossWallPatternFrame> frames;
	bool looping = true; // 패턴 반복 여부
};

namespace BossWallPatternData
{
	// 패턴 1: 양쪽에서 좁혀오기 (16x6)
	inline BossWallPattern getVerticalFillPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 프레임 0: 양 끝 경고
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

		// 프레임 1: 양 끝 벽 생성
		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"W..............W",
			U"W..............W",
			U"W..............W",
			U"W..............W",
			U"W..............W",
			U"W..............W"
		};
		frame1.duration = 1.0;
		pattern.frames.push_back(frame1);

		// 프레임 2: 한 칸 더 안쪽 경고
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"W!............!W",
			U"W!............!W",
			U"W!............!W",
			U"W!............!W",
			U"W!............!W",
			U"W!............!W"
		};
		frame2.duration = 1.0;
		pattern.frames.push_back(frame2);

		// 프레임 3: 벽 추가 생성
		BossWallPatternFrame frame3;
		frame3.pattern = {
			U"WW............WW",
			U"WW............WW",
			U"WW............WW",
			U"WW............WW",
			U"WW............WW",
			U"WW............WW"
		};
		frame3.duration = 1.0;
		pattern.frames.push_back(frame3);

		// 프레임 4: 모든 벽 파괴
		BossWallPatternFrame frame4;
		frame4.pattern = {
			U"XX............XX",
			U"XX............XX",
			U"XX............XX",
			U"XX............XX",
			U"XX............XX",
			U"XX............XX"
		};
		frame4.duration = 0.5;
		pattern.frames.push_back(frame4);

		return pattern;
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
				if (y == row)
					warning.pattern[y] = U"!!!!!!!!!!!!!!!!";
				else
					warning.pattern[y] = U"................";
			}
			warning.duration = 0.5;
			pattern.frames.push_back(warning);

			// 벽 생성
			BossWallPatternFrame wall;
			wall.pattern.resize(6);
			for (int32 y = 0; y < 6; ++y)
			{
				if (y == row)
					wall.pattern[y] = U"WWWWWWWWWWWWWWWW";
				else
					wall.pattern[y] = U"................";
			}
			wall.duration = 0.8;
			pattern.frames.push_back(wall);

			// 벽 파괴
			BossWallPatternFrame destroy;
			destroy.pattern.resize(6);
			for (int32 y = 0; y < 6; ++y)
			{
				if (y == row)
					destroy.pattern[y] = U"XXXXXXXXXXXXXXXX";
				else
					destroy.pattern[y] = U"................";
			}
			destroy.duration = 0.2;
			pattern.frames.push_back(destroy);
		}

		return pattern;
	}

	// 패턴 3: 세로 웨이브 (왼쪽에서 오른쪽으로)
	inline BossWallPattern getVerticalWavePattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		for (int32 col = 0; col < 16; ++col)
		{
			// 경고
			BossWallPatternFrame warning;
			warning.pattern.resize(6);
			for (int32 y = 0; y < 6; ++y)
			{
				String line = U"................";
				line[col] = U'!';
				warning.pattern[y] = line;
			}
			warning.duration = 0.2;
			pattern.frames.push_back(warning);

			// 벽 생성
			BossWallPatternFrame wall;
			wall.pattern.resize(6);
			for (int32 y = 0; y < 6; ++y)
			{
				String line = U"................";
				line[col] = U'W';
				wall.pattern[y] = line;
			}
			wall.duration = 0.6;
			pattern.frames.push_back(wall);

			// 벽 파괴
			BossWallPatternFrame destroy;
			destroy.pattern.resize(6);
			for (int32 y = 0; y < 6; ++y)
			{
				String line = U"................";
				line[col] = U'X';
				destroy.pattern[y] = line;
			}
			destroy.duration = 0.15;
			pattern.frames.push_back(destroy);
		}

		return pattern;
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
		frame1.duration = 0.5;
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

		return pattern;
	}

	// 패턴 5: X자 공격 (대각선)
	inline BossWallPattern getDiagonalXPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 대각선 경고
		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"!..............!",
			U".!............!.",
			U"..!..........!..",
			U"...!........!...",
			U"....!......!....",
			U".....!....!....."
		};
		frame0.duration = 1.0;
		pattern.frames.push_back(frame0);

		// 대각선 벽 생성
		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"W..............W",
			U".W............W.",
			U"..W..........W..",
			U"...W........W...",
			U"....W......W....",
			U".....W....W....."
		};
		frame1.duration = 1.5;
		pattern.frames.push_back(frame1);

		// 대각선 벽 파괴
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"X..............X",
			U".X............X.",
			U"..X..........X..",
			U"...X........X...",
			U"....X......X....",
			U".....X....X....."
		};
		frame2.duration = 0.5;
		pattern.frames.push_back(frame2);

		return pattern;
	}

	// 패턴 6: 스파이크 (아래에서 위로 튀어오름)
	inline BossWallPattern getSpikePattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 하단 경고
		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"................",
			U"................",
			U"................",
			U"................",
			U"................",
			U"!.!.!.!.!.!.!.!."
		};
		frame0.duration = 1.0;
		pattern.frames.push_back(frame0);

		// 스파이크 1단계
		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"................",
			U"................",
			U"................",
			U"................",
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W."
		};
		frame1.duration = 0.5;
		pattern.frames.push_back(frame1);

		// 스파이크 2단계
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"................",
			U"................",
			U"................",
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W."
		};
		frame2.duration = 0.8;
		pattern.frames.push_back(frame2);

		// 스파이크 파괴
		BossWallPatternFrame frame3;
		frame3.pattern = {
			U"................",
			U"................",
			U"................",
			U"X.X.X.X.X.X.X.X.",
			U"X.X.X.X.X.X.X.X.",
			U"X.X.X.X.X.X.X.X."
		};
		frame3.duration = 0.3;
		pattern.frames.push_back(frame3);

		return pattern;
	}

	// 패턴 7: 좌우 교차 공격
	inline BossWallPattern getAlternatingPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 왼쪽 경고
		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"!!!.............",
			U"!!!.............",
			U"!!!.............",
			U"!!!.............",
			U"!!!.............",
			U"!!!............."
		};
		frame0.duration = 0.8;
		pattern.frames.push_back(frame0);

		// 왼쪽 벽
		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"WWW.............",
			U"WWW.............",
			U"WWW.............",
			U"WWW.............",
			U"WWW.............",
			U"WWW............."
		};
		frame1.duration = 1.0;
		pattern.frames.push_back(frame1);

		// 오른쪽 경고
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"WWW............!",
			U"WWW............!",
			U"WWW............!",
			U"WWW............!",
			U"WWW............!",
			U"WWW............!"
		};
		frame2.duration = 0.8;
		pattern.frames.push_back(frame2);

		// 오른쪽 벽 추가
		BossWallPatternFrame frame3;
		frame3.pattern = {
			U"WWW............W",
			U"WWW............W",
			U"WWW............W",
			U"WWW............W",
			U"WWW............W",
			U"WWW............W"
		};
		frame3.duration = 1.0;
		pattern.frames.push_back(frame3);

		// 모두 파괴
		BossWallPatternFrame frame4;
		frame4.pattern = {
			U"XXX............X",
			U"XXX............X",
			U"XXX............X",
			U"XXX............X",
			U"XXX............X",
			U"XXX............X"
		};
		frame4.duration = 0.5;
		pattern.frames.push_back(frame4);

		return pattern;
	}

	// 패턴 8: 미로 패턴 (좁은 통로)
	inline BossWallPattern getMazePattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 경고
		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"!!!!!!.....!!!!!",
			U"!!!!!!.....!!!!!",
			U"!!!!!!.....!!!!!",
			U"!!!!!!.....!!!!!",
			U"!!!!!!.....!!!!!",
			U"!!!!!!.....!!!!!"
		};
		frame0.duration = 1.0;
		pattern.frames.push_back(frame0);

		// 미로 벽 생성
		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"WWWWWW.....WWWWW",
			U"WWWWWW.....WWWWW",
			U"WWWWWW.....WWWWW",
			U"WWWWWW.....WWWWW",
			U"WWWWWW.....WWWWW",
			U"WWWWWW.....WWWWW"
		};
		frame1.duration = 2.0;
		pattern.frames.push_back(frame1);

		// 파괴
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"XXXXXX.....XXXXX",
			U"XXXXXX.....XXXXX",
			U"XXXXXX.....XXXXX",
			U"XXXXXX.....XXXXX",
			U"XXXXXX.....XXXXX",
			U"XXXXXX.....XXXXX"
		};
		frame2.duration = 0.5;
		pattern.frames.push_back(frame2);

		return pattern;
	}

	// 패턴 9: 체커보드 (안전지대 찾기)
	inline BossWallPattern getCheckerboardPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 체커보드 경고
		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"!.!.!.!.!.!.!.!.",
			U".!.!.!.!.!.!.!.!",
			U"!.!.!.!.!.!.!.!.",
			U".!.!.!.!.!.!.!.!",
			U"!.!.!.!.!.!.!.!.",
			U".!.!.!.!.!.!.!.!"
		};
		frame0.duration = 1.0;
		pattern.frames.push_back(frame0);

		// 체커보드 벽 생성
		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"W.W.W.W.W.W.W.W.",
			U".W.W.W.W.W.W.W.W",
			U"W.W.W.W.W.W.W.W.",
			U".W.W.W.W.W.W.W.W",
			U"W.W.W.W.W.W.W.W.",
			U".W.W.W.W.W.W.W.W"
		};
		frame1.duration = 1.2;
		pattern.frames.push_back(frame1);

		// 파괴
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"X.X.X.X.X.X.X.X.",
			U".X.X.X.X.X.X.X.X",
			U"X.X.X.X.X.X.X.X.",
			U".X.X.X.X.X.X.X.X",
			U"X.X.X.X.X.X.X.X.",
			U".X.X.X.X.X.X.X.X"
		};
		frame2.duration = 0.3;
		pattern.frames.push_back(frame2);

		return pattern;
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

		return pattern;
	}

	// 패턴 11: 회전 공격 (시계방향)
	inline BossWallPattern getRotatingPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		// 상단
		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"!!!!!!!!!!!!!!!!", // 경고
			U"................",
			U"................",
			U"................",
			U"................",
			U"................"
		};
		frame0.duration = 0.5;
		pattern.frames.push_back(frame0);

		BossWallPatternFrame frame1;
		frame1.pattern = {
			U"WWWWWWWWWWWWWWWW", // 벽
			U"................",
			U"................",
			U"................",
			U"................",
			U"................"
		};
		frame1.duration = 0.6;
		pattern.frames.push_back(frame1);

		// 우측
		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"WWWWWWWWWWWWWWW!",
			U"...............!",
			U"...............!",
			U"...............!",
			U"...............!",
			U"...............!"
		};
		frame2.duration = 0.5;
		pattern.frames.push_back(frame2);

		BossWallPatternFrame frame3;
		frame3.pattern = {
			U"WWWWWWWWWWWWWWWW",
			U"...............W",
			U"...............W",
			U"...............W",
			U"...............W",
			U"...............W"
		};
		frame3.duration = 0.6;
		pattern.frames.push_back(frame3);

		// 하단
		BossWallPatternFrame frame4;
		frame4.pattern = {
			U"WWWWWWWWWWWWWWWW",
			U"...............W",
			U"...............W",
			U"...............W",
			U"...............W",
			U"!!!!!!!!!!!!!!!W"
		};
		frame4.duration = 0.5;
		pattern.frames.push_back(frame4);

		BossWallPatternFrame frame5;
		frame5.pattern = {
			U"WWWWWWWWWWWWWWWW",
			U"...............W",
			U"...............W",
			U"...............W",
			U"...............W",
			U"WWWWWWWWWWWWWWWW"
		};
		frame5.duration = 0.6;
		pattern.frames.push_back(frame5);

		// 좌측
		BossWallPatternFrame frame6;
		frame6.pattern = {
			U"WWWWWWWWWWWWWWWW",
			U"!..............W",
			U"!..............W",
			U"!..............W",
			U"!..............W",
			U"WWWWWWWWWWWWWWWW"
		};
		frame6.duration = 0.5;
		pattern.frames.push_back(frame6);

		BossWallPatternFrame frame7;
		frame7.pattern = {
			U"WWWWWWWWWWWWWWWW",
			U"W..............W",
			U"W..............W",
			U"W..............W",
			U"W..............W",
			U"WWWWWWWWWWWWWWWW"
		};
		frame7.duration = 0.8;
		pattern.frames.push_back(frame7);

		// 모두 파괴
		BossWallPatternFrame frame8;
		frame8.pattern = {
			U"XXXXXXXXXXXXXXXX",
			U"X..............X",
			U"X..............X",
			U"X..............X",
			U"X..............X",
			U"XXXXXXXXXXXXXXXX"
		};
		frame8.duration = 0.5;
		pattern.frames.push_back(frame8);

		return pattern;
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

		return pattern;
	}
}
