#pragma once
#include <Siv3D.hpp>

using namespace s3d;

struct BossWallPatternFrame
{
	Array<String> pattern;        
	double duration = 1.0;        
};

struct BossWallPattern
{
	Array<BossWallPatternFrame> frames;
	bool looping = true;        
};

namespace BossWallPatternData
{
	static constexpr double kMinFrameDuration = 0.5;

	inline bool isW(char32 c) { return c == U'W'; }
	inline bool isWarn(char32 c) { return c == U'!'; }
	inline bool isDestroy(char32 c) { return c == U'X'; }

	inline void normalizeLineSizes(BossWallPatternFrame& f, size_t rows, size_t cols)
	{
		if (f.pattern.size() < rows) f.pattern.resize(rows, String(cols, U'.'));
		for (auto& line : f.pattern)
		{
			if (line.size() < cols) line.resize(cols, U'.');
		}
	}

	inline BossWallPattern FixPattern(BossWallPattern pattern)
	{
		BossWallPattern out;
		out.looping = pattern.looping;

		if (pattern.frames.isEmpty())
			return out;

		for (auto& f : pattern.frames)
		{
			if (f.duration < kMinFrameDuration)
				f.duration = kMinFrameDuration;
		}

		const size_t baseRows = pattern.frames.front().pattern.size();
		const size_t baseCols = (baseRows > 0 ? pattern.frames.front().pattern.front().size() : 16);

		BossWallPatternFrame prev = pattern.frames.front();
		normalizeLineSizes(prev, baseRows, baseCols);
		out.frames.push_back(prev);

		for (size_t i = 1; i < pattern.frames.size(); ++i)
		{
			BossWallPatternFrame cur = pattern.frames[i];
			normalizeLineSizes(cur, baseRows, baseCols);

			bool needWarn = false;
			BossWallPatternFrame warn = prev;    
			for (size_t y = 0; y < baseRows; ++y)
			{
				String& wLine = warn.pattern[y];
				const String& pLine = prev.pattern[y];
				const String& cLine = cur.pattern[y];

				for (size_t x = 0; x < baseCols; ++x)
				{
					const char32 p = pLine[x];
					const char32 c = cLine[x];
					if (isW(c) && !isW(p))
					{
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

			if (cur.duration < kMinFrameDuration)
				cur.duration = kMinFrameDuration;

			out.frames.push_back(cur);
			prev = out.frames.back();
		}

		return out;
	}

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

	inline BossWallPattern getHorizontalWavePattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		for (int32 row = 0; row < 6; ++row)
		{
			BossWallPatternFrame warning;
			warning.pattern.resize(6);
			for (int32 y = 0; y < 6; ++y)
			{
				if (y == row) warning.pattern[y] = U"!!!!!!!!!!!!!!!!";
				else          warning.pattern[y] = U"................";
			}
			warning.duration = 0.5;
			pattern.frames.push_back(warning);

			BossWallPatternFrame wall;
			wall.pattern.resize(6);
			for (int32 y = 0; y < 6; ++y)
			{
				if (y == row) wall.pattern[y] = U"WWWWWWWWWWWWWWWW";
				else          wall.pattern[y] = U"................";
			}
			wall.duration = 0.8;
			pattern.frames.push_back(wall);

			BossWallPatternFrame destroy;
			destroy.pattern.resize(6);
			for (int32 y = 0; y < 6; ++y)
			{
				if (y == row) destroy.pattern[y] = U"XXXXXXXXXXXXXXXX";
				else          destroy.pattern[y] = U"................";
			}
			destroy.duration = 0.2;    
			pattern.frames.push_back(destroy);
		}

		return FixPattern(std::move(pattern));
	}

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

	inline BossWallPattern getCrossPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

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

	inline BossWallPattern getDiagonalXPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

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

		BossWallPatternFrame frame2;
		frame2.pattern = {
			U"X..X..X.........",
			U".X..X..X........",
			U"..X..X..X.......",
			U"...X..X..X......",
			U"....X..X..X.....",
			U".....X..X..X...."
		};
		frame2.duration = 0.3;    
		pattern.frames.push_back(frame2);

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

		BossWallPatternFrame frame5;
		frame5.pattern = {
			U".........X..X..X",
			U"........X..X..X.",
			U".......X..X..X..",
			U"......X..X..X...",
			U".....X..X..X....",
			U"....X..X..X....."
		};
		frame5.duration = 0.3;    
		pattern.frames.push_back(frame5);

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

		BossWallPatternFrame frame8;
		frame8.pattern = {
			U"X..X.......X..X",
			U".X..X.....X..X.",
			U"..X..X...X..X..",
			U"...X..X.X..X...",
			U"....X..X..X....",
			U".....X...X....."
		};
		frame8.duration = 0.3;    
		pattern.frames.push_back(frame8);

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

		BossWallPatternFrame frame11;
		frame11.pattern = {
			U"XX..XX..XX..XX..",
			U".XX..XX..XX..XX.",
			U"..XX..XX..XX..XX",
			U"X..XX..XX..XX..X",
			U"XX..XX..XX..XX..",
			U".XX..XX..XX..XX."
		};
		frame11.duration = 0.3;    
		pattern.frames.push_back(frame11);

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

		BossWallPatternFrame frame14;
		frame14.pattern = {
			U"..XX..XX..XX..XX",
			U".XX..XX..XX..XX.",
			U"XX..XX..XX..XX..",
			U"X..XX..XX..XX..X",
			U"..XX..XX..XX..XX",
			U".XX..XX..XX..XX."
		};
		frame14.duration = 0.3;    
		pattern.frames.push_back(frame14);

		return FixPattern(std::move(pattern));
	}

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
		frame1.duration = 0.4;    
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
		frame3.duration = 0.2;    
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
		frame5.duration = 0.4;    
		pattern.frames.push_back(frame5);

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

		BossWallPatternFrame frame7;
		frame7.pattern = {
			U"................",
			U"................",
			U"................",
			U".X.X.X.X.X.X.X.X",
			U".X.X.X.X.X.X.X.X",
			U".X.X.X.X.X.X.X.X"
		};
		frame7.duration = 0.2;    
		pattern.frames.push_back(frame7);

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

		BossWallPatternFrame frame9;
		frame9.pattern = {
			U"W.W.W.W.W.W.W.W.",
			U"W.W.W.W.W.W.W.W.",
			U"!.!.!.!.!.!.!.!.",  
			U"................",
			U"................",
			U"................"
		};
		frame9.duration = 0.4;    
		pattern.frames.push_back(frame9);

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

		BossWallPatternFrame frame11;
		frame11.pattern = {
			U"X.X.X.X.X.X.X.X.",
			U"X.X.X.X.X.X.X.X.",
			U"X.X.X.X.X.X.X.X.",
			U"................",
			U"................",
			U"................"
		};
		frame11.duration = 0.2;    
		pattern.frames.push_back(frame11);

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

		BossWallPatternFrame frame13;
		frame13.pattern = {
			U".W.W.W.W.W.W.W.W",
			U".W.W.W.W.W.W.W.W",
			U".!.!.!.!.!.!.!.!",  
			U"................",
			U"................",
			U"................"
		};
		frame13.duration = 0.4;    
		pattern.frames.push_back(frame13);

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

		BossWallPatternFrame frame15;
		frame15.pattern = {
			U".X.X.X.X.X.X.X.X",
			U".X.X.X.X.X.X.X.X",
			U".X.X.X.X.X.X.X.X",
			U"................",
			U"................",
			U"................"
		};
		frame15.duration = 0.2;    
		pattern.frames.push_back(frame15);

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

		BossWallPatternFrame frame17;
		frame17.pattern = {
			U"W.W.W.W.W.W.W.W.",
			U"!.!.!.!.!.!.!.!.",   
			U"................",
			U"................",
			U".!.!.!.!.!.!.!.!",   
			U".W.W.W.W.W.W.W.W"
		};
		frame17.duration = 0.4;    
		pattern.frames.push_back(frame17);

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

		BossWallPatternFrame frame19;
		frame19.pattern = {
			U"X.X.X.X.X.X.X.X.",
			U"X.X.X.X.X.X.X.X.",
			U"................",
			U"................",
			U".X.X.X.X.X.X.X.X",
			U".X.X.X.X.X.X.X.X"
		};
		frame19.duration = 0.3;    
		pattern.frames.push_back(frame19);

		return FixPattern(std::move(pattern));
	}

	inline BossWallPattern getAlternatingPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		for (int32 col = 0; col < 16; ++col)
		{
			int32 row = (col / 3) % 6;

			BossWallPatternFrame frameEmpty;
			frameEmpty.pattern = {
				U"................",
				U"................",
				U"................",
				U"................",
				U"................",
				U"................"
			};
			frameEmpty.duration = 0.2;    
			pattern.frames.push_back(frameEmpty);

			BossWallPatternFrame frameWarn;
			frameWarn.pattern.resize(6);
			for (int32 y = 0; y < 6; ++y)
			{
				String line = U"................";

				if (y == row)
				{
					for (int32 x = 0; x < 16; ++x)
						line[x] = U'!';
				}

				line[col] = U'!';

				frameWarn.pattern[y] = line;
			}
			frameWarn.duration = 0.5;
			pattern.frames.push_back(frameWarn);

			BossWallPatternFrame frameWall;
			frameWall.pattern.resize(6);
			for (int32 y = 0; y < 6; ++y)
			{
				String line = U"................";

				if (y == row)
				{
					for (int32 x = 0; x < 16; ++x)
						line[x] = U'W';
				}

				line[col] = U'W';

				frameWall.pattern[y] = line;
			}
			frameWall.duration = 0.5;
			pattern.frames.push_back(frameWall);

			BossWallPatternFrame frameEnd;
			frameEnd.pattern = {
				U"XXXXXXXXXXXXXXXX",
				U"XXXXXXXXXXXXXXXX",
				U"XXXXXXXXXXXXXXXX",
				U"XXXXXXXXXXXXXXXX",
				U"XXXXXXXXXXXXXXXX",
				U"XXXXXXXXXXXXXXXX"
			};
			frameEnd.duration = 0.3;    
			pattern.frames.push_back(frameEnd);
		}

		return FixPattern(std::move(pattern));
	}

	inline BossWallPattern getMazePattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

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

	inline BossWallPattern getCheckerboardPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

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

		BossWallPatternFrame frame6;
		frame6.pattern = {
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX"
		};
		frame6.duration = 0.3;    
		pattern.frames.push_back(frame6);

		return FixPattern(std::move(pattern));
	}

	inline BossWallPattern getRandomBulletPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

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

	inline BossWallPattern getRotatingPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

		BossWallPatternFrame frame0;
		frame0.pattern = {
			U"................",
			U"................",
			U".......!!.......",
			U".......!!.......",
			U"................",
			U"................"
		};
		frame0.duration = 0.3;    
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
		frame1.duration = 0.4;    
		pattern.frames.push_back(frame1);

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

		BossWallPatternFrame frame5;
		frame5.pattern = {
			U"XXXXXXXXXXXXXXXX",
			U"XX.....XX.....XX",
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX",
			U"XX.....XX.....XX",
			U"XXXXXXXXXXXXXXXX"
		};
		frame5.duration = 0.3;    
		pattern.frames.push_back(frame5);

		BossWallPatternFrame frame6;
		frame6.pattern = {
			U"................",
			U"................",
			U".......WW.......",
			U".......WW.......",
			U"................",
			U"................"
		};
		frame6.duration = 0.3;       
		pattern.frames.push_back(frame6);

		BossWallPatternFrame frame7;
		frame7.pattern = {
			U".......WW.......",
			U".......WW.......",
			U".WWWWWWWWWWWWWW.",
			U".WWWWWWWWWWWWWW.",
			U".......WW.......",
			U".......WW......."
		};
		frame7.duration = 0.4;    
		pattern.frames.push_back(frame7);

		BossWallPatternFrame frame8;
		frame8.pattern = {
			U"W......WW......W",
			U".W.....WW.....W.",
			U".WWWWWWWWWWWWWW.",
			U".WWWWWWWWWWWWWW.",
			U".W.....WW.....W.",
			U"W......WW......W"
		};
		frame8.duration = 0.4;    
		pattern.frames.push_back(frame8);

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

		BossWallPatternFrame frame10;
		frame10.pattern = {
			U"XXXXXXXXXXXXXXXX",
			U"XX.....XX.....XX",
			U"XXXXXXXXXXXXXXXX",
			U"XXXXXXXXXXXXXXXX",
			U"XX.....XX.....XX",
			U"XXXXXXXXXXXXXXXX"
		};
		frame10.duration = 0.3;    
		pattern.frames.push_back(frame10);

		return FixPattern(std::move(pattern));
	}

	inline BossWallPattern getBoxShrinkPattern()
	{
		BossWallPattern pattern;
		pattern.looping = true;

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
