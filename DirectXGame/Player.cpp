#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include "Vector3.h"

void Player::Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;

	textureHandle_ = textureHandle;

	// ワールド変換の初期化
	viewprojection_ = viewProjection;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {

	// 1,移動入力
	InputMove();

	// 2マップ衝突チェック
	// 衝突情報を初期化
	//   衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;

	// マップ衝突チェック
	CheckHitMap(collisionMapInfo);

	CheckMapMove(collisionMapInfo);

	CheckMapCeiling(collisionMapInfo);

	CheckHitWall(collisionMapInfo);

	CheckMapLanding(collisionMapInfo);

	// bool landing = false;

	//// 地面との当たり判定
	//// 下降中？
	// if (velocity_.y < 0) {
	//	// Y座標が地面以下になったら着地
	//	if (worldTransform_.translation_.y <= 1.0f) {
	//		landing = true;
	//	}
	// }

	//// 接地判定
	// if (onGround_) {
	//	// ジャンプ開始
	//	if (velocity_.y > 0.0f) {
	//		// 空中状態に移行
	//		onGround_ = false;
	//	}
	// } else {
	//	// 着地
	//	if (landing) {
	//		// めり込み排斥
	//		worldTransform_.translation_.y = 1.0f;
	//		// 摩擦で横方向速度が減衰する
	//		velocity_.x *= (1.0f - kAttenuation);
	//		// 下方向速度をリセット
	//		velocity_.y = 0.0f;
	//		// 接地状態に移行
	//		onGround_ = true;
	//	}
	// }

	AnimateTurn();

	// 行列計算
	worldTransform_.UpdateMatrix();
}

void Player::Draw() { model_->Draw(worldTransform_, *viewprojection_, textureHandle_); }

void Player::InputMove() {
	// 移動入力
	if (onGround_) {
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {

				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x += kAcceleration;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}

			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {

				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x -= kAcceleration;

				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}
			// 加速/減速
			velocity_ = velocity_ + acceleration;

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitSpeed, kLimitSpeed);
		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			velocity_.y += kJumpAcceleration;
		}
		// 空中
	} else {
		// 落下速度
		velocity_.y += -kGravityAcceleration;
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::CheckHitMap(CollisionMapInfo& info) {

	CheckMapCeiling(info);
	CheckHitMapBottom(info);
	CheckHitMapRight(info);
	CheckHitMapLeft(info);
	/*CheckMapCeiling(info);
	CheckHitWall(info);*/
}

void Player::CheckMapCeiling(CollisionMapInfo& info) {
	// 上昇あり?
	if (info.move.y <= 0) {
		return;
	}

	// 移動後の4つのかどの座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を指定する
		Vector3 test = {0, kHeight / 2.0f, 0};
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + test);
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		// 天井に当たった事を記録する
		info.ceiling = true;
	}
}

void Player::CheckHitMapBottom(CollisionMapInfo& info) {
	// 下降あり?
	if (info.move.y >= 0) {
		return;
	}

	// 移動後の4つのかどの座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真下の当たり判定を行う
	bool hit = false;
	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット?
	if (hit) {
		Vector3 test = {0, kHeight / 2.0f, 0};

		// めり込みを排除する方向に移動量を指定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + test);
		// めり込み先ブロックの範囲矩形z
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		// 天井に当たった事を記録する
		info.landing = true;
	}
}

// void Player::CheckHitMapRight(CollisionMapInfo& info) {
//	if (velocity_.x > 0.0f) {
//		return;
//	}
//
//	// 移動後の4つのかどの座標
//	std::array<Vector3, kNumCorner> positionNew;
//
//	for (uint32_t i = 0; i < positionNew.size(); ++i) {
//		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
//	}
//
//	MapChipType mapChipType;
//	// 右の当たり判定を行う
//	bool hit = false;
//	// 右上点の判定
//	MapChipField::IndexSet indexSet;
//	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
//	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
//	if (mapChipType == MapChipType::kBlock) {
//		hit = true;
//	}
//	// 右下点の判定
//	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
//	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
//	if (mapChipType == MapChipType::kBlock) {
//		hit = true;
//	}
//
//	// ブロックにヒット?
//	if (hit) {
//		Vector3 test = {kWidth / 2.0f, 0, 0};
//		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + test);
//		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
//		info.move.x = std::max(0.0f, rect.right - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
//		info.hitWall = true;
//	}
// }

void Player::CheckHitMapRight(CollisionMapInfo& info) {
	// 右方向への移動があるか？
	if (info.move.x <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 右の当たり判定を行う
	bool hit = false;
	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を指定する
		Vector3 test = {kWidth / 2.0f, 0, 0};
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + test);
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::min(0.0f, rect.left - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank));
		// 右に当たったことを記録する
		info.hitWall = true;
	}
}

void Player::CheckHitMapLeft(CollisionMapInfo& info) {
	// 左方向への移動があるか？
	if (info.move.x >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 左の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を指定する
		Vector3 test = {-kWidth / 2.0f, 0, 0};
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + test);
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x + (kWidth / 2.0f - kBlank));
		// 左に当たったことを記録する
		info.hitWall = true;
	}
}

// void Player::CheckHitMapLeft(CollisionMapInfo& info) {
//	if (velocity_.x > 0.0f) {
//		return;
//	}
//
//	// 移動後の4つのかどの座標
//	std::array<Vector3, kNumCorner> positionNew;
//
//	for (uint32_t i = 0; i < positionNew.size(); ++i) {
//		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
//	}
//
//	MapChipType mapChipType;
//	// 左の当たり判定を行う
//	bool hit = false;
//	// 左上点の判定
//	MapChipField::IndexSet indexSet;
//	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
//	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
//	if (mapChipType == MapChipType::kBlock) {
//		hit = true;
//	}
//	// 左下点の判定
//	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
//	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
//	if (mapChipType == MapChipType::kBlock) {
//		hit = true;
//	}
//
//	// ブロックにヒット?
//	// 左に当たった場合の処理
//	if (hit) {
//
//		Vector3 test = {kWidth / 2.0f, 0, 0};
//		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + test);
//		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
//		info.move.x = std::min(0.0f, rect.left - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank));
//		info.hitWall = true;
//	}
// }

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, //  kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}, //  kLeftTop
	};

	return Add(center, offsetTable[static_cast<uint32_t>(corner)]);
}

void Player::CheckMapMove(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_ = Add(worldTransform_.translation_, info.move);
}

void Player::HitMapProc(const CollisionMapInfo& info) {
	// 天井に当たった?
	if (info.ceiling) {
		velocity_.y = 0.0f;
	}
}

void Player::CheckMapLanding(CollisionMapInfo& info) {
	// 自キャラが接地状態？
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {

			// 移動後の4つのかどの座標
			std::array<Vector3, kNumCorner> positionNew;

			for (uint32_t i = 0; i < positionNew.size(); ++i) {
				positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			}

			MapChipType mapChipType;
			bool hit = false;

			// 左下点の判定
			MapChipField::IndexSet indexSet;
			float miNum = 0.001f;
			Vector3 kShift = {0, -miNum, 0};
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom] + kShift);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom] + kShift);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			if (!hit) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	} else {
		// 接地フラグ
		if (info.landing) {
			// 接地状態に切り替える
			onGround_ = true;
			// 移動時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロにする
			velocity_.y = 0.0f;
		} 
	}
}

void Player::CheckHitWall(const CollisionMapInfo& info) {
	// 壁接触による減速
	if (info.hitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

void Player::AnimateTurn() {
	// 旋回制御
	if (turnTimer_ > 0.0f) {

		turnTimer_ -= 1.0f / 60.0f;

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,
		    std::numbers::pi_v<float> * 3.0f / 2.0f,
		};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}
}

void Player::OnCollision(const Enemy* enemy) { 
	(void)enemy; 
	
	isDead_ = true;
}

Vector3 Player::GetWorldPosition() { 
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;
	aabb.min = {worldPos.x - 0.5f, worldPos.y - 0.5f, worldPos.z - 0.5f};
	aabb.max = {worldPos.x + 0.5f, worldPos.y + 0.5f, worldPos.z + 0.5f};
	return aabb;
}
