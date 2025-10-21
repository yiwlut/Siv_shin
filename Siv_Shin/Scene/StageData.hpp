#pragma once
#include <Siv3D.hpp>

/**
 * @file StageData.hpp
 * @brief 레벨 디자이너용 스테이지 맵 데이터 정의
 * 
 * 이 파일은 레벨 디자이너가 수정할 수 있도록 분리된 맵 데이터를 포함합니다.
 * 맵 편집 가이드:
 * 
 * 맵 기호 설명:
 * '#' = 벽 (Wall)
 * 'T' = 플레이어 시작 위치 (Taco)
 * 
 * 상자 (대문자):
 * 'R' = 빨간 상자 (Red Box)
 * 'Y' = 노란 상자 (Yellow Box)  
 * 'B' = 파란 상자 (Blue Box)
 * 'O' = 주황 상자 (Orange Box)
 * 'G' = 초록 상자 (Green Box)
 * 'V' = 보라 상자 (Violet Box)
 * 'K' = 검은 상자 (Black Box)
 * 
 * 목표 지점 (소문자):
 * 'r' = 빨간 목표 (Red Goal)
 * 'y' = 노란 목표 (Yellow Goal)
 * 'b' = 파간 목표 (Blue Goal)
 * 'o' = 주황 목표 (Orange Goal)
 * 'g' = 초록 목표 (Green Goal)
 * 'v' = 보라 목표 (Violet Goal)
 * 'k' = 검은 목표 (Black Goal)
 * 
 * 빈 공간:
 * ' ' 또는 '.' = 빈 공간 (Empty)
 * 
 * 색상 조합 규칙:
 * - 1차 색상: R + Y = O, Y + B = G, B + R = V
 * - 2차 색상: 2차 + 2차 = K (검은색)
 * - 1차 + 2차: 구성 요소가 아닌 경우만 K 생성
 */

namespace StageData
{
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
		U"##########",
		U"#  R  Y  #",
		U"#  B  G  #",
		U"# roybg T#",
		U"##########"
	};

    /// @brief 모든 스테이지 맵 데이터를 반환하는 함수
    /// @param stageNumber 스테이지 번호 (1-8) + 마지막은 final stage
    /// @return 해당 스테이지의 맵 데이터 (빈 배열이면 스테이지 없음)
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

    /// @brief 스테이지 개수 반환
    /// @return 총 스테이지 개수
    inline constexpr int32 getTotalStageCount()
    {
        return 9;
    }

    /// @brief 파이널 스테이지(보스) 맵 반환 - 번호와 무관하게 명시적으로 접근
    inline Array<String> getFinalStageMap()
    {
        return FINAL_STAGE;
    }

    /// @brief 파이널 스테이지인지 여부 (현재는 마지막 스테이지)
    inline constexpr bool isFinalStage(int32 stageNumber)
    {
        return (stageNumber == getTotalStageCount());
    }

    /// @brief 파이널 스테이지의 인덱스(번호) 반환
    inline constexpr int32 getFinalStageIndex()
    {
        return getTotalStageCount();
    }

    /// @brief 스테이지가 유효한지 확인
    /// @param stageNumber 확인할 스테이지 번호
    /// @return 유효하면 true, 아니면 false
    inline bool isValidStage(int32 stageNumber)
    {
        return stageNumber >= 1 && stageNumber <= getTotalStageCount();
    }
}

/*
====================================================
레벨 디자이너를 위한 맵 편집 가이드 (11x11 버전)
====================================================

1. 맵 크기: 11x11 고정
2. 반드시 '#'으로 테두리를 둘러싸야 함
3. 플레이어 시작 위치 'T'는 반드시 1개만 배치
4. 상자와 목표는 1:1 매칭되어야 함 (개수 동일)

맵 테스트 방법:
1. 이 파일을 수정 후 저장
2. 게임을 다시 빌드
3. 해당 스테이지에서 테스트

예시 - 새로운 스테이지 추가 (11x11):
inline const Array<String> STAGE_7 = {
    U"###########",
    U"#T        #",
    U"#   R  Y  #",
    U"#         #",
    U"#    o    #",
    U"#         #",
    U"#         #",
    U"#         #",
    U"#         #",
    U"#         #",
    U"###########"
};

주의사항:
- 맵이 해결 불가능하지 않도록 주의
- 상자가 벽에 끼이지 않도록 경로 확보
- 목표 지점에 도달 가능한지 확인
- 11x11 크기를 정확히 유지
*/
