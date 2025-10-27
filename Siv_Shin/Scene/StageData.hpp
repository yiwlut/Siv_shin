#pragma once
#include <Siv3D.hpp>

namespace StageData
{

	struct TileModification {
		int32 x, y;
		char tileChar;  // 'i' = Ice, 'l' = Lava
	};

	

	//7x7
	inline const Array<String> STAGE_1 = {
		U"###g###",
		U"###B###",
		U"###Y###",
		U"T R Y o",
		U"###R###",
		U"###B###",
		U"###v###",
	};
	//8x8
    inline const Array<String> STAGE_2 = {
		U"########",
		U"# vB ###",
		U"# R   ##",  
		U"# T R  #",
		U"#    Y #",
		U"## Rr o#",
		U"###    #",  
		U"########"
	};

	//7x7
	inline const Array<String> STAGE_3 = {
		U"LL####L",
		U"###  #L",
		U"YBR  #L",  
		U" V#  #L",
		U"    T##",  
		U"## OBv#",
		U"L######",
	};

    inline const Array<String> STAGE_4 = {
		U"L# VOvL",  
		U"#     L",  
		U" R    L",
		U"T  BO L",  
		U"# Y   L",  
		U"##BV LL",  
		U"LLvLLLL",
	};

	/*
    inline const Array<String> STAGE_5 = {
		U"iiiii#iiiii",
		U"iiiii#iiiii",
		U"iiiii#iiiii",
		U"iiiiiviiiii",  
		U"iii  R  iii",  
		U"L        9L",
		U"#L   T   LL",
		U"LLL     LLL",  
		U"LLLL   LLLL",
		U"LLLLLTLLLLL",  
		U"LLLLLLLLLLL"
	};

	*/
	inline const Array<String> STAGE_5 = {
		U"       #   ",
		U" ## # ## # ",
		U" #  #    # ",
		U" # #  # ## ",
		U" ######### ",
		U" #VYoTR#GR ",
		U" #######4# ",
		U" # ## # ## ",
		U" #    #    ",
		U" ## # # ## ",
		U"    #    # ",
	};

	inline const Array<String> STAGE_6 = {
		U"LL## ##ii",
		U"###gY ###",
		U"###R BG##",
		U"# Y T R #",
		U"#    Y  #",
		U"##BB  R##",
		U"### B v##",
		U"#### ###L",
		U"LL## ##LL",
	};

    inline const Array<String> STAGE_7 = {
		U"###########",
		U"# R R R R #",  
		U"#R R R6R R#",
		U"# R R R R #",  
		U"#R R R R R#",  
		U"g R R R R T",
		U"#R R R R R#",
		U"# R R R R #",  
		U"#R R8R R R#",
		U"# R R R R #",  
		U"###########"
	};



	inline const Array<String> STAGE_8 = {
		U"#    T #",
		U"# R   B#",
		U"###### #",
		U"##   #O#",
		U" GV  ###",
		U"        ",
		U"     Y  ",
		U"  r     ",
	};
	inline const Array<String> STAGE_8_TILES = {
		U"   ii   ", 
		U" iiiii  ",
		U"      i ",
		U"  iii i ", 
		U" iiii   ",
		U" iiiii  ",
		U"  iii   ",
		U"        ",
	};

	inline const Array<String> STAGE_9 = {
		U"##6########",
		U"## # #   ##",
		U"oG R   #  #",
		U"2#       ##",
		U"Y #      #v",
		U" O   o    B",
		U"  #     # B",
		U"#    R     ",
		U"#  #iii#  #",
		U"##B iii  ##",
		U"##2##T#####"
	};

	inline const Array<String> STAGE_10 = {
		U"   ####    ",
		U" ##        ",
		U" #   R ##  ",
		U"#     #8O# ",
		U"# V   ##  #",
		U"# R##b##Y #",
		U"#  ##     #",
		U" # 7#   T #",
		U"  ## O   # ",
		U"        ## ",
		U"    ####   ",
	};


	inline const Array<String> STAGE_10_TILES = {
		U"iii    iiii",
		U"i        ii",
		U"i         i",
		U"    ii     ",
		U"   iii     ",
		U"           ",
		U"     iii   ",
		U"     ii    ",
		U"i         i",
		U"iii       i",
		U"iiii    iii",
	};

	inline const Array<String> FINAL_STAGE = {
		U"     r          ",
		U"    R v         ",
		U"     R y        ",
		U"    GG Tg       ",
		U"         b      ",
		U"          o     ",

	};

	// 첫 시작 시 오버레이 
	inline const Array<String> FINAL_STAGE_P0_TILES = {
		U"                ",
		U"                ",
		U"                ",
		U"                ",
		U"                ",
		U"                ",
	};
	//U"L!! iiiiii LLL",
	//U"LL! iiiiii LLL",
	//U"LL!        !LL",
	//U"LL!!!      !LL",
	//U"LL!        !LL",
	//U"LLL!      !!!L",
	// 10초 경과 시 타일 오버레이 (용암 첫 상승)
	inline const Array<String> FINAL_STAGE_P1_TILES = {
		U"LLL!iiiii!!LLL",
		U"LLL!!iiiii!LLL",
		U"LLL!!     !LLL",
		U"LLLLL!   !!LLL",
		U"LLL!      !LLL",
		U"LLLL!    !LLLL",

	};

	// 20초 경과 시 타일 오버레이 (용암 두 번째 상승)
	inline const Array<String> FINAL_STAGE_P2_TILES = {
		U"LLLL!ii!!LLLLL",
		U"LLLLL!ii!!LLLL",
		U"LLLLL!   !LLLL",
		U"LLLLLL  !LLLLL",
		U"LLLL!!   !LLLL",
		U"LLLLL! !!LLLLL",
	};

	// 30초 경과 시 타일 오버레이 (용암 세 번째 상승)
	inline const Array<String> FINAL_STAGE_P3_TILES = {
		U"LLLLL!!LLLLLLL",
		U"LLLLLL!!LLLLLL",
		U"LLLLLL!!!LLLLL",
		U"LLLLLL!!LLLLLL",
		U"LLLLLL!!!LLLLL",
		U"LLLLLL!LLLLLLL",
	};

	inline const Array<String> FINAL_STAGE_P4_TILES = {
		U"LLLLLLLLLLLLLL",
		U"LLLLLLLLLLLLLL",
		U"LLLLLLLLLLLLLL",
		U"LLLLLLLLLLLLLL",
		U"LLLLLLLLLLLLLL",
		U"LLLLLLLLLLLLLL",
	};

	inline constexpr double getNextPhaseTime(int32 phase)
	{
		double nextPhaseTime = 10.0;

		return nextPhaseTime * phase;
	}

	inline constexpr double getPhaseTimeByGameTime(double elapsedTime)
	{
		if (elapsedTime >= getNextPhaseTime(4)) return 4;
		if (elapsedTime >= getNextPhaseTime(3)) return 3;
		if (elapsedTime >= getNextPhaseTime(2)) return 2;
		if (elapsedTime >= getNextPhaseTime(1)) return 1;

		return 0;
	}

	inline Array<String> getFinalStageTileOverlay(double elapsedTime)
	{
		

		if (elapsedTime >= getNextPhaseTime(4))  // ★ 18초 (6초 * 3)
		{
			return FINAL_STAGE_P4_TILES;
		}
		else if (elapsedTime >= getNextPhaseTime(3))  // ★ 12초 (6초 * 2)
		{
			return FINAL_STAGE_P3_TILES;
		}
		else if (elapsedTime >= getNextPhaseTime(2))  // ★ 12초 (6초 * 2)
		{
			return FINAL_STAGE_P2_TILES;
		}
		else if (elapsedTime >= getNextPhaseTime(1))  // ★ 6초 (6초 * 1)
		{
			return FINAL_STAGE_P1_TILES;
		}
		else if (elapsedTime >= 0.0)  // 0초 (시작)
		{
			return FINAL_STAGE_P0_TILES;
		}
		return Array<String>{};
	}

    inline Array<String> getStageMap(int32 stageNumber)
    {
        switch (stageNumber)
        {
        case 1: return STAGE_1;
        case 2: return STAGE_2;
        case 3: return STAGE_3;
        case 4: return STAGE_4;
        case 5: return STAGE_5;
        case 6: return STAGE_6;
        case 7: return STAGE_7;
        case 8: return STAGE_8;
        case 9: return STAGE_9;
        case 10: return STAGE_10;
        case 11: return FINAL_STAGE; // Final Boss Stage
        default: return STAGE_1; // 기본값: 스테이지 1
        }
    }

	inline Array<String> getStageTileOverlay(int32 stageNumber) {
		switch (stageNumber) {
		case 8: return STAGE_8_TILES;
		case 10: return STAGE_10_TILES;
		default: return {};
		}
	}

    inline constexpr int32 getTotalStageCount()
    {
        return 11;  // 10 normal stages + 1 final stage
    }

    inline Array<String> getFinalStageMap()
    {
        return FINAL_STAGE;
    }

    inline constexpr bool isFinalStage(int32 stageNumber)
    {
        return (stageNumber == getTotalStageCount());
    }

    inline constexpr int32 getFinalStageIndex()
    {
        return getTotalStageCount();
    }


    inline bool isValidStage(int32 stageNumber)
    {
        return stageNumber >= 1 && stageNumber <= getTotalStageCount();
    }
}
