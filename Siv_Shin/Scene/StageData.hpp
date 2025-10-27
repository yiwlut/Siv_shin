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
		U"#######",
		U"###b###",
		U"###B###",
		U"T R Y o",
		U"###G###",
		U"###g###",
		U"#######",
	};



	//8x8
    inline const Array<String> STAGE_2 = {
		U"########",
		U"# vB R #",
		U"# R    #",  
		U"# T    #",
		U"#    Y #",
		U"#  Rr o#",
		U"#      #",  
		U"########"
	};

	//7x7
	inline const Array<String> STAGE_3 = {
		U"#######",
		U"###  ##",
		U"YBR  ##",  
		U" V#  ##",
		U"    T##",  
		U"## OBv#",
		U"#######",
	};

    inline const Array<String> STAGE_4 = {
		U"## VOv#",  
		U"#     #",  
		U" R    #",
		U"T  BO #",  
		U"# Y   #",  
		U"##BV ##",  
		U"##v####",
	};

    inline const Array<String> STAGE_5 = {
		U"#### ####",
		U"###gY ###",  
		U"###R BG##",
		U"# Y T R #",  
		U"#    Y  #",
		U"##BB  R##",
		U"### B v##",  
		U"#### ####",
		U"#### ####",
	};


    inline const Array<String> STAGE_6 = {
		U"###########",
		U"###########",  
		U"###########",
		U"#####v#####",  
		U"###  R  ###",  
		U"#        9#",
		U"##   T   ##",
		U"###     ###",  
		U"####   ####",
		U"#####T#####",  
		U"###########"
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
		U"##6########",
		U"## # #   ##",  
		U"oG R   #  #",
		U"2#       ##",
		U"Y #      #v",
		U" O   o    B",
		U"  #     # B",
		U"#    R     ",
		U"#  #   #  #",
		U"##B      ##",  
		U"##2##T#####"
	};

	inline const Array<String> STAGE_9 = {
		U"#    T #",
		U"# R   B#",
		U"###### #",
		U"##   #O#",
		U" GV  ###",
		U"        ",
		U"     Y  ",
		U"  r     ",
	};
	inline const Array<String> STAGE_9_TILES = {
		U"   ii   ", 
		U" iiiii  ",
		U"      i ",
		U"  iii i ", 
		U" iiii   ",
		U" iiiii  ",
		U"  iii   ",
		U"        ",
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
			U"           ",
			U"           ",
			U"           ",
			U"    ii     ",
			U"   iii     ",
			U"           ",
			U"     iii   ",
			U"     ii    ",
			U"           ",
			U"           ",
			U"           ",
	};
	// 10x5 Final Boss Stage (always referenced as FINAL_STAGE)
    inline const Array<String> FINAL_STAGE = {
		U"              ",
		U"    B    R    ",
		U"              ",
		U"    roybg T   ",
		U"              ",
		U"              "
	};

	// 첫 시작 시 오버레이 
	inline const Array<String> FINAL_STAGE_P0_TILES = {
		U"LL   iiii !!LL",
		U"LL  Lii iL!!LL",
		U"LL        !!LL",
		U"LL  L  L  !!LL",
		U"LL   L    !!LL",
		U"LL        !!LL"
	};

	// 10초 경과 시 타일 오버레이 (용암 첫 상승)
	inline const Array<String> FINAL_STAGE_P1_TILES = {
		U"LL   iiii LLLL",
		U"LL  Lii iLLLLL",
		U"LL        LLLL",
		U"LL  L  L   LLL",
		U"LL   L    LLLL",
		U"LL        LLLL"
	};

	// 20초 경과 시 타일 오버레이 (용암 두 번째 상승)
	inline const Array<String> FINAL_STAGE_P2_TILES = {
		U"LL   iiii   LL",
		U"LL   LiiL	  LL",
		U"LLLL  LLLLLLLL",
		U"LL  L  L    LL",
		U"LL   L      LL",
		U"LL          LL"
	};

	// 30초 경과 시 타일 오버레이 (용암 세 번째 상승)
	inline const Array<String> FINAL_STAGE_P3_TILES = {
		U"LL   iiii   LL",
		U"LL  LiiL    LL",
		U"LLLLLLLLLLLLLL",
		U"LL  LLLLLLL LL",
		U"LL   L      LL",
		U"LL          LL"
	};

	inline Array<String> getFinalStageTileOverlay(double elapsedTime)
	{
		if (elapsedTime >= 30.0)
		{
			return FINAL_STAGE_P3_TILES;
		}
		else if (elapsedTime >= 20.0)
		{
			return FINAL_STAGE_P2_TILES;
		}
		else if (elapsedTime >= 10.0)
		{
			return FINAL_STAGE_P1_TILES;
		}
		else if (elapsedTime >= 0.0)  // ★ 수정: 0초 이상일 때 P0 반환
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
		case 9: return STAGE_9_TILES;
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
