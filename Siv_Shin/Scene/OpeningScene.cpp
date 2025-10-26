#include "OpeningScene.hpp"

OpeningScene::OpeningScene()
	: elapsedTime_(0.0)
	, fadeInDuration_(1.5)      // フェードイン 1.5秒
	, displayDuration_(3.0)     // 表示 3秒
	, fadeOutDuration_(1.5)     // フェードアウト 1.5秒
	, totalDuration_(30.0)
	, haikuFont_(FontMethod::MSDF, 32, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))  // 横書き用に小さく
	, rubyFont_(FontMethod::MSDF, 16, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))   // 루비용 작은 폰트
	, authorFont_(FontMethod::MSDF, 40, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, skipFont_(FontMethod::MSDF, 20, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, openingBgm_(Resource(U"ArtResources/BGM/DeepSea1.mp3"))
	, petalSpawnTimer_(0.0)
	, petalSpawnInterval_(0.3)   // 0.3秒ごとに花びら生成
	, haikuLineDelay_(2.4)       // 各行ごとに2.4秒間隔
	, haikuLineFadeDuration_(2.0) // 各行が2秒でフェードイン
	, typewriterSpeed_(0.1)     // 문자당 0.1초
	, skipPressTime_(0.0)
	, skipRequiredTime_(3.0)     // 3秒押せばスキップ
	, isSkipping_(false)
{
	currentScene_ = SceneType::Opening;
}

void OpeningScene::onEnter()
{
	elapsedTime_ = 0.0;
	petalSpawnTimer_ = 0.0;
	skipPressTime_ = 0.0;
	isSkipping_ = false;

	// 꽃잎 파티클 초기화
	initializePetals();

	// 배경음악 재생
	if (!openingBgm_.isEmpty())
	{
		openingBgm_.setLoop(false);
		openingBgm_.setVolume(0.3);
		openingBgm_.play();
	}
}

void OpeningScene::onExit()
{
	// 배경음악 정지
	if (!openingBgm_.isEmpty() && openingBgm_.isPlaying())
	{
		openingBgm_.stop();
	}

	// 꽃잎 파티클 정리
	petals_.clear();
}

void OpeningScene::update()
{
	const double dt = Scene::DeltaTime();
	elapsedTime_ += dt;

	// F2 디버그 키로 즉시 스킵 (디버그용으로만 유지)
	if (KeyF2.down())
	{
		changeScene(SceneType::InGame);
		return;
	}

	// 스페이스바 누르기 감지
	if (KeySpace.pressed())
	{
		skipPressTime_ += dt;
		isSkipping_ = true;

		// 3초 이상 누르면 스킵
		if (skipPressTime_ >= skipRequiredTime_)
		{
			changeScene(SceneType::InGame);
			return;
		}
	}
	else
	{
		// 스페이스바를 떼면 초기화
		skipPressTime_ = 0.0;
		isSkipping_ = false;
	}

	// 자동 진행 (총 시간 경과 시)
	if (elapsedTime_ >= totalDuration_)
	{
		changeScene(SceneType::InGame);
		return;
	}

	// 꽃잎 파티클 업데이트
	updatePetals();
}

void OpeningScene::draw()
{
	// 검은색 배경
	Scene::SetBackground(ColorF{ 0.0, 0.0, 0.0 });

	// 꽃잎 파티클 그리기 (텍스트 뒤에)
	drawPetals();

	// 하이쿠 그리기
	drawHaiku();

	// 스킵 UI 그리기
	drawSkipUI();
}

double OpeningScene::getFadeAlpha() const
{
	if (elapsedTime_ < fadeInDuration_)
	{
		// 페이드인 구간
		return elapsedTime_ / fadeInDuration_;
	}
	else if (elapsedTime_ < fadeInDuration_ + displayDuration_)
	{
		// 완전히 표시되는 구간
		return 1.0;
	}
	else
	{
		// 페이드아웃 구간
		const double fadeOutElapsed = elapsedTime_ - (fadeInDuration_ + displayDuration_);
		return 1.0 - (fadeOutElapsed / fadeOutDuration_);
	}
}

void OpeningScene::initializePetals()
{
	petals_.clear();

	// Scene::Size()를 사용하여 화면 크기 가져오기
	const double gameWidth = Scene::Size().x;

	// 시작 시 몇 개의 꽃잎을 미리 생성
	for (int i = 0; i < 10; ++i)
	{
		spawnPetal();
		// 화면에 분산되도록 초기 위치 조정
		petals_.back().position.x = Random(0.0, gameWidth);
	}
}

void OpeningScene::updatePetals()
{
	const double dt = Scene::DeltaTime();
	// Scene::Size()를 사용하여 화면 크기 가져오기
	const double gameWidth = Scene::Size().x;

	// 새로운 꽃잎 생성
	petalSpawnTimer_ += dt;
	if (petalSpawnTimer_ >= petalSpawnInterval_)
	{
		spawnPetal();
		petalSpawnTimer_ = 0.0;
	}

	// 기존 꽃잎 업데이트
	for (auto& petal : petals_)
	{
		// 위치 업데이트 (천천히 이동)
		petal.position += petal.velocity * dt;

		// 회전 업데이트
		petal.rotation += petal.rotationSpeed * dt;

		// 부드러운 상하 움직임 추가 (사인파)
		petal.position.y += Math::Sin(petal.position.x * 0.01 + elapsedTime_) * 0.5;
	}

	// 화면 밖으로 나간 꽃잎 제거
	petals_.remove_if([gameWidth](const Petal& petal) {
		return petal.position.x > gameWidth + 100;
	});
}

void OpeningScene::drawPetals()
{
	for (const auto& petal : petals_)
	{
		const double globalAlpha = getFadeAlpha();
		const ColorF petalColor = ColorF{ petal.color, petal.alpha * globalAlpha };

		// 꽃잎 모양 그리기 (타원형 2개를 조합하여 꽃잎 형태)
		const Vec2 pos = petal.position;
		const double size = 12.0 * petal.scale;

		const Transformer2D transform{ Mat3x2::Rotate(petal.rotation, pos) };

		// 꽃잎 본체 (긴 타원)
		Ellipse{ pos, size * 0.4, size }.draw(petalColor);

		// 꽃잎 하이라이트 (작은 타원)
		Ellipse{ pos.movedBy(-size * 0.15, 0), size * 0.25, size * 0.7 }
		.draw(ColorF{ 1.0, 1.0, 1.0, 0.3 * petal.alpha * globalAlpha });
	}
}

void OpeningScene::spawnPetal()
{
	Petal petal;

	// Scene::Size()를 사용하여 화면 크기 가져오기
	const double gameHeight = Scene::Size().y;

	// 화면 왼쪽 밖에서 시작
	petal.position = Vec2{ -50.0, Random(0.0, gameHeight) };

	// 천천히 오른쪽으로 이동 (속도를 느리게)
	petal.velocity = Vec2{ Random(30.0, 60.0), Random(-10.0, 10.0) };

	// 회전
	petal.rotation = Random(0.0, Math::TwoPi);
	petal.rotationSpeed = Random(-1.0, 1.0);

	// 크기
	petal.scale = Random(0.7, 1.3);

	// 색상 (분홍색, 흰색, 연보라색 계열)
	const int colorType = Random(0, 2);
	if (colorType == 0)
	{
		petal.color = ColorF{ 1.0, 0.7, 0.8 }; // 분홍색
	}
	else if (colorType == 1)
	{
		petal.color = ColorF{ 1.0, 0.95, 1.0 }; // 흰색
	}
	else
	{
		petal.color = ColorF{ 0.9, 0.7, 1.0 }; // 연보라색
	}

	// 투명도
	petal.alpha = Random(0.5, 0.9);

	petals_ << petal;
}

void OpeningScene::drawHaiku()
{
	// ハイクテキスト（9行）- 루비 포함 + 강조 단어는 【】로 표시
	Array<String> haikuLines = {
		U"かつて、あの【愚（おろ）】かなる【蛸（たこ）】は、",
		U"深海（しんかい）のすべての【色】を手にしたと自惚（うぬぼ）れていた。",
		U"身の程も知らず、",
		U"【神】の領域（りょういき）たる「【パレット】」を欲（ほっ）したその【罪（つみ）】。",
		U"我（われ）はやつから【光】を奪（うば）い、",
		U"永遠（えいえん）の【闇（やみ）】をくれてやった。",
		U"今や、やつがその狭（せま）い深海（しんかい）で",
		U"できる唯一（ゆいいつ）のこととは…",
		U"【蛸壺（たこつぼ）】やはかなき【夢（ゆめ）】を【夏（なつ）】の【月（つき）】"
	};

	// Scene::Size()를 사용하여 논리적 사이즈 가져오기
	const Vec2 center = Scene::Center();

	// 横書き表示設定
	const double lineSpacing = 60;  // 행간격（上から下へ）
	const size_t totalLines = haikuLines.size();

	// 全体の高さを計算
	const double totalHeight = lineSpacing * (totalLines - 1);

	// 中央配置のための開始位置計算 (더 위쪽에서 시작하도록 오프셋 추가)
	const double startY = center.y - (totalHeight / 2.0) - 100.0;  // 100픽셀 위로 이동
	const double centerX = center.x;

	// 各行を横書きで中央揃えで描画（루비 포함, 타자기 효과 적용）
	for (size_t lineIndex = 0; lineIndex < totalLines; ++lineIndex)
	{
		const String& line = haikuLines[lineIndex];
		const double lineY = startY + (lineIndex * lineSpacing);

		// この行のアルファ値計算（順次フェードイン）
		const double lineAlpha = getHaikuLineAlpha(lineIndex);

		// アルファ値が0なら描画しない
		if (lineAlpha <= 0.0)
		{
			continue;
		}

		// 타자기 효과: 표시할 문자 수 계산
		const size_t visibleCharCount = getVisibleCharCount(lineIndex);

		// 루비를 포함한 텍스트 렌더링 (타자기 효과 적용, 강조 색상 포함)
		drawTextWithRubyPartial(line, Vec2{ centerX, lineY }, lineAlpha, visibleCharCount);
	}
}

size_t OpeningScene::getVisibleCharCount(size_t lineIndex) const
{
	// 3번째 줄(index 2)과 4번째 줄(index 3)을 동시에 표시하기 위해
	size_t adjustedIndex = lineIndex;
	if (lineIndex >= 3)
	{
		adjustedIndex = lineIndex - 1;
	}

	// 이 줄이 시작되는 시간
	double lineStartTime = adjustedIndex * haikuLineDelay_;

	// 3,4번째 줄 이후(index 4 이상)에 2초 딜레이 추가
	if (lineIndex >= 4)
	{
		lineStartTime += 2.0;  // 2초 추가 딜레이
	}

	// 아직 이 줄이 시작되지 않았으면 0
	if (elapsedTime_ < lineStartTime)
	{
		return 0;
	}

	// 줄이 시작된 후 경과 시간
	const double timeSinceLineStart = elapsedTime_ - lineStartTime;

	// 타자기 효과로 표시할 문자 수 계산
	const size_t visibleChars = static_cast<size_t>(timeSinceLineStart / typewriterSpeed_);

	// 최대값은 매우 큰 수로 (실제 문자열 길이로 제한됨)
	return visibleChars;
}

double OpeningScene::getHaikuLineAlpha(size_t lineIndex) const
{
	// 3번째 줄(index 2)과 4번째 줄(index 3)을 동시에 표시하기 위해
	// index 3일 때는 index 2와 같은 타이밍 사용
	size_t adjustedIndex = lineIndex;
	if (lineIndex >= 3)
	{
		adjustedIndex = lineIndex - 1;  // 4번째 줄부터는 한 칸씩 당김
	}

	// 각 줄이 시작되는 시간 계산 (조정된 인덱스 사용)
	double lineStartTime = adjustedIndex * haikuLineDelay_;

	// 3,4번째 줄 이후(index 4 이상)에 2초 딜레이 추가
	if (lineIndex >= 4)
	{
		lineStartTime += 2.0;  // 2초 추가 딜레이
	}

	const double lineEndTime = lineStartTime + haikuLineFadeDuration_;

	// 아직 이 줄이 시작되지 않았으면 0
	if (elapsedTime_ < lineStartTime)
	{
		return 0.0;
	}

	// 페이드인 중이면 0~1 사이의 값
	if (elapsedTime_ < lineEndTime)
	{
		return (elapsedTime_ - lineStartTime) / haikuLineFadeDuration_;
	}

	// 모든 줄이 표시된 후 전체 페이드아웃 시작
	const double totalHaikuDisplayTime = 10.0;  // 모든 줄 표시 후 10초간 유지
	const double fadeOutStartTime = 8 * haikuLineDelay_ + 2.0 + totalHaikuDisplayTime;  // 3,4번째 줄 동시 표시 + 2초 딜레이

	if (elapsedTime_ < fadeOutStartTime)
	{
		return 1.0;
	}

	// 전체 페이드아웃
	const double fadeOutProgress = (elapsedTime_ - fadeOutStartTime) / fadeOutDuration_;
	return Math::Max(0.0, 1.0 - fadeOutProgress);
}

void OpeningScene::drawSkipUI()
{
	// Scene::Size()를 사용하여 논리적 크기 가져오기
	const double gameWidth = Scene::Size().x;
	const double gameHeight = Scene::Size().y;

	// 화면 우측 하단에 스킵 UI 표시
	const Vec2 uiPos = Vec2{ gameWidth - 80, gameHeight - 80 };
	const double radius = 30.0;

	// 기본 원 (배경)
	Circle{ uiPos, radius }.draw(ColorF{ 0.2, 0.2, 0.2, 0.5 });
	Circle{ uiPos, radius }.drawFrame(2.0, ColorF{ 0.6, 0.6, 0.6, 0.7 });

	// 스페이스바를 누르고 있을 때만 프로그레스 표시
	if (isSkipping_ && skipPressTime_ > 0.0)
	{
		const double progress = skipPressTime_ / skipRequiredTime_;

		// 진행률 원 (시계 방향으로 채워짐)
		const double startAngle = -Math::HalfPi;  // 12시 방향부터 시작
		const double sweepAngle = Math::TwoPi * progress;

		// 프로그레스 원 (채워진 부분)
		Circle{ uiPos, radius - 4 }.drawPie(startAngle, sweepAngle, ColorF{ 0.3, 0.8, 1.0, 0.8 });

		// 프로그레스 테두리
		Circle{ uiPos, radius - 4 }.drawArc(startAngle, sweepAngle, 4.0, 4.0, ColorF{ 0.5, 1.0, 1.0, 1.0 });
	}

	// "SPACE" 텍스트 표시
	skipFont_(U"SPACE").drawAt(uiPos, ColorF{ 1.0, 1.0, 1.0, 0.9 });

	// 하단에 일본어 "スキップ" 텍스트
	const Vec2 skipTextPos = Vec2{ gameWidth - 80, gameHeight - 45 };
	skipFont_(U"スキップ").drawAt(skipTextPos, ColorF{ 0.9, 0.9, 0.9, 0.8 });

	// 하단에 안내 텍스트 (작게)
	const Vec2 holdTextPos = Vec2{ gameWidth - 80, gameHeight - 25 };
	skipFont_(U"Hold 3s").drawAt(holdTextPos, ColorF{ 0.8, 0.8, 0.8, 0.6 });
}

Array<OpeningScene::RubyText> OpeningScene::parseRubyText(const String& text) const
{
	Array<RubyText> result;
	String currentBase;
	String currentRuby;
	bool inParenthesis = false;
	bool hasRubyForCurrent = false;
	bool isHighlighted = false;  // 【】로 강조된 텍스트인지

	for (size_t i = 0; i < text.length(); ++i)
	{
		const char32 ch = text[i];

		if (ch == U'【')
		{
			// 강조 시작
			isHighlighted = true;
		}
		else if (ch == U'】')
		{
			// 강조 끝 - 현재 텍스트를 강조 색상으로 저장
			if (!currentBase.isEmpty())
			{
				ColorF highlightColor = ColorF{ 1.0, 1.0, 0.7 };  // 연노란색 (낮은 채도)
				result.push_back(RubyText{ currentBase, currentRuby, highlightColor });
				currentBase.clear();
				currentRuby.clear();
			}
			isHighlighted = false;
			hasRubyForCurrent = false;
		}
		else if (ch == U'（')
		{
			// 괄호 시작 - 이전 텍스트를 base로 저장 준비
			inParenthesis = true;
			hasRubyForCurrent = true;
			currentRuby.clear();
		}
		else if (ch == U'）')
		{
			// 괄호 끝 - base와 ruby를 함께 저장
			if (!currentBase.isEmpty())
			{
				ColorF textColor = isHighlighted ? ColorF{ 1.0, 1.0, 0.7 } : ColorF{ 0.95, 0.95, 0.9 };
				result.push_back(RubyText{ currentBase, currentRuby, textColor });
				currentBase.clear();
				currentRuby.clear();
			}
			inParenthesis = false;
			hasRubyForCurrent = false;
		}
		else if (inParenthesis)
		{
			// 괄호 안 - ruby 텍스트
			currentRuby.push_back(ch);
		}
		else
		{
			// 일반 텍스트
			// 다음 문자를 확인하여 루비가 붙을지 판단
			bool nextIsRuby = (i + 1 < text.length() && text[i + 1] == U'（');
			bool nextIsHighlightEnd = (i + 1 < text.length() && text[i + 1] == U'】');

			if (!nextIsRuby && !currentBase.isEmpty() && !hasRubyForCurrent && !isHighlighted)
			{
				// 루비가 없는 일반 텍스트는 바로 저장
				result.push_back(RubyText{ currentBase, U"", ColorF{ 0.95, 0.95, 0.9 } });
				currentBase.clear();
			}

			currentBase.push_back(ch);

			// 루비가 없고 강조도 아닌 단일 문자면 바로 저장
			if (!nextIsRuby && !hasRubyForCurrent && !isHighlighted && !nextIsHighlightEnd)
			{
				result.push_back(RubyText{ currentBase, U"", ColorF{ 0.95, 0.95, 0.9 } });
				currentBase.clear();
			}
		}
	}

	// 남은 텍스트 처리
	if (!currentBase.isEmpty())
	{
		ColorF textColor = isHighlighted ? ColorF{ 1.0, 1.0, 0.7 } : ColorF{ 0.95, 0.95, 0.9 };
		result.push_back(RubyText{ currentBase, U"", textColor });
	}

	return result;
}

void OpeningScene::drawTextWithRuby(const String& text, const Vec2& basePos, const ColorF& color) const
{
	Array<RubyText> parts = parseRubyText(text);

	// 전체 텍스트 너비 계산 (중앙 정렬용)
	double totalWidth = 0.0;
	for (const auto& part : parts)
	{
		totalWidth += haikuFont_(part.baseText).region().w;
	}

	// 중앙 정렬 시작 위치
	double currentX = basePos.x - (totalWidth / 2.0);

	// 각 파트 그리기
	for (const auto& part : parts)
	{
		// 기본 텍스트의 너비 계산
		const double baseWidth = haikuFont_(part.baseText).region().w;

		// 루비가 있으면 위에 그리기
		if (!part.rubyText.isEmpty())
		{
			const double rubyWidth = rubyFont_(part.rubyText).region().w;

			// 베이스 텍스트의 중앙 좌표
			const double baseCenterX = currentX + (baseWidth / 2.0);
			// 루비 텍스트를 베이스 중앙에 맞춤
			const double rubyX = baseCenterX - (rubyWidth / 2.0);

			// 루비를 베이스 텍스트 위에 배치 (색상도 함께 적용)
			ColorF rubyColor = part.color.withAlpha(color.a);
			rubyFont_(part.rubyText).draw(rubyX, basePos.y - 19.0, rubyColor);
		}

		// 기본 텍스트 그리기 (파트의 색상 사용)
		ColorF textColor = part.color.withAlpha(color.a);
		haikuFont_(part.baseText).draw(currentX, basePos.y, textColor);

		currentX += baseWidth;
	}
}

void OpeningScene::drawTextWithRubyPartial(const String& text, const Vec2& basePos, double alpha, size_t visibleCharCount) const
{
	Array<RubyText> parts = parseRubyText(text);

	// 전체 텍스트 너비 계산 (중앙 정렬용)
	double totalWidth = 0.0;
	for (const auto& part : parts)
	{
		totalWidth += haikuFont_(part.baseText).region().w;
	}

	// 중앙 정렬 시작 위치
	double currentX = basePos.x - (totalWidth / 2.0);

	// 표시할 문자 수 카운터
	size_t displayedChars = 0;

	// 각 파트 그리기
	for (const auto& part : parts)
	{
		// 이미 충분한 문자를 표시했으면 중단
		if (displayedChars >= visibleCharCount)
		{
			break;
		}

		// 기본 텍스트의 너비 계산
		const double baseWidth = haikuFont_(part.baseText).region().w;

		// 이 파트에서 표시할 문자 수 계산
		const size_t charsToShow = Min(part.baseText.length(), visibleCharCount - displayedChars);
		const String partialBase = part.baseText.substr(0, charsToShow);

		// 루비가 있으면 위에 그리기 (완전히 표시된 경우만)
		if (!part.rubyText.isEmpty() && charsToShow == part.baseText.length())
		{
			const double rubyWidth = rubyFont_(part.rubyText).region().w;

			// 베이스 텍스트의 중앙 좌표
			const double baseCenterX = currentX + (baseWidth / 2.0);
			// 루비 텍스트를 베이스 중앙에 맞춤
			const double rubyX = baseCenterX - (rubyWidth / 2.0);

			// 루비를 베이스 텍스트 위에 배치 (색상도 함께 적용)
			ColorF rubyColor = part.color.withAlpha(alpha);
			rubyFont_(part.rubyText).draw(rubyX, basePos.y - 19.0, rubyColor);
		}

		// 기본 텍스트 그리기 (부분 문자열, 파트의 색상 사용)
		ColorF textColor = part.color.withAlpha(alpha);
		haikuFont_(partialBase).draw(currentX, basePos.y, textColor);

		// 표시한 문자 수 증가
		displayedChars += charsToShow;

		currentX += baseWidth;
	}
}
