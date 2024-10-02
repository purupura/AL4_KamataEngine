#pragma once
#define NOMINMAX
#include "Input.h"
#include "MathUtilityFortext.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Easing.h"
#include "AABB.h"
#include <algorithm>
#include <cassert>
#include <numbers>
class Enemy;

class MapChipField;

class Player {
public:
	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		Vector3 move;
	};

	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,

		kNumCorner // 要素数

	};

	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection, const Vector3& position);

	///< summary>
	/// 更新
	///</summary>
	void Update();

	///< summary>
	/// 描画
	///</summary>
	void Draw();

	void InputMove();

	const WorldTransform& GetWorldTransform() const { return worldTransform_; };

	const Vector3& GetVelocity() const { return velocity_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void CheckHitMap(CollisionMapInfo& info);

	void CheckMapCeiling(CollisionMapInfo& info);

	void CheckHitMapBottom(CollisionMapInfo& info);

	void CheckHitMapRight(CollisionMapInfo& info);

	void CheckHitMapLeft(CollisionMapInfo& info);

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	void CheckMapMove(const CollisionMapInfo& info);

	void HitMapProc(const CollisionMapInfo& info);

	void CheckMapLanding(CollisionMapInfo& info);

	void CheckHitWall(const CollisionMapInfo& info);

	void AnimateTurn();

	void OnCollision(const Enemy* enemy);

	AABB GetAABB();

	bool IsDead() const { return isDead_; }

		// ワールド座標を取得
	Vector3 GetWorldPosition();

private:
	enum class LRDirection {
		kRight,
		kLeft,
	};

	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	ViewProjection* viewprojection_ = nullptr;

	Vector3 velocity_ = {};

	static inline const float kAcceleration = 0.01f;
	static inline const float kAttenuation = 0.03f;
	static inline const float kLimitSpeed = 4.0f;

	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;

	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	bool onGround_ = true;

	static inline const float kGravityAcceleration = 0.05f;
	static inline const float kJumpAcceleration = 0.5f;
	static inline const float kLimitFallSpeed = 2.0f;

	static inline const float kWidth = 1.0f;
	static inline const float kHeight = 1.0f;

	static inline const float kBlank = 1.0f;
	static inline const float kAttenuationLanding = 0.03f;

	static inline const float kAttenuationWall = 0.5f;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;



	bool isDead_ = false;

};