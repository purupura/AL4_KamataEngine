#pragma once

#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "matrix.h"
#include "MathUtilityForText.h"
#include <array>
#include <algorithm>

class DeathParticles {

public:
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Update();

	void Draw();

	bool IsFinished() const { return isFinished_; }

private:
	// モデルのポインタ
	Model* model_ = nullptr;

	// ビュープロジェクションのポインタ
	ViewProjection* viewProjection_ = nullptr;

	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;

	std::array<WorldTransform, kNumParticles> worldTransform_;

	// 存続時間(消滅までの時間)<秒>
	static inline const float kDuration =2;
	// 移動の速さ
	static inline const float kSpeed = 0.05f;
	// 分割した1個分の角度
	static inline const float kAngleUint = 2.0f * 3.14f / kNumParticles;
	// 終了フラグ
	bool isFinished_ = false;
	// 経過時間カウント
	float counter_ = 0.0f;
	// 色変更オブジェクト
	ObjectColor objectColor_;
	// 色の数値
	Vector4 color_;


};
