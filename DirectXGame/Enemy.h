#pragma once

#define NOMINMAX
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <algorithm>
#include"Input.h"
#include <cassert>
#include<numbers>
#include"easing.h"
#include"MathUtilityFortext.h"

#include "AABB.h"
class Player;

class Enemy {

	public:

		void Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection, const Vector3& position);

		void Update();

		void Draw();

		AABB GetAABB();

	    Vector3 GetWorldPosition();

	    void OnCollision(const Player* player);

	private:

		// ワールド変換データ
	    WorldTransform worldTransform_;
	    // モデル
	    Model* model_ = nullptr;

	    ViewProjection* viewprojection_ = nullptr;

		// テクスチャハンドル
	    uint32_t textureHandle_ = 0u;

		// 歩行の速さ
	    static inline const float kWalkSpeed = 0.01f;

	    // 速度
	    Vector3 velocity_ = {};

	    // 最初の角度
	    static inline const float kWalkMotionAngleStart = 0;

	    // 最後の角度
	    static inline const float kWalkMotionAngleEnd = 30.0f;

	    // アニメーションの周期となる時間[秒]
	    static inline const float kWalkMotionTime = 1.0f;

	    float walkTimer_ = 0.0f;

};
