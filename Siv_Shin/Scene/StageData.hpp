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
		U"   v   ",
		U"       ",
		U"      ",
		U"   V   ",
        U"   Y   ",
		U"   T   ",
		U"       ",

    };


	inline const Array<String> STAGE_1_TILES = {
		U"       ",  // 첫 줄은 변경 없음
		U"iiiiii ",
		U"iiiiii ",
		U"iiiiii ",  // T, R, Y 위치를 얼음으로, o는 변경 없음
		U"       ",
		U"       ",
		U"       ",
	};

	//inline const Array<String> STAGE_2 = {
	//	U"  o",
	//	U"TRY",
	//	U"   ",
	//};

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

	// 10x5 Final Boss Stage (always referenced as FINAL_STAGE)
    inline const Array<String> FINAL_STAGE = {
		U"     iiii     ",
		U"    BiiiiR    ",
		U"     ii i     ",
		U"    roybg T   ",
		U"          "
	};

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
        case 9: return FINAL_STAGE; // Final Boss Stage (명명: finalStage)
        default: return STAGE_1; // 기본값: 스테이지 1
        }
    }

	inline Array<String> getStageTileOverlay(int32 stageNumber) {
		switch (stageNumber) {
		case 1: return STAGE_1_TILES;
		// case 4: return STAGE_4_TILES;
		// case 5: return STAGE_5_TILES;
		default: return {};
		}
	}

    inline constexpr int32 getTotalStageCount()
    {
        return 9;
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
