#include "Enemy.h"
#include <cassert>
#include "MathUtilityForText.h"
#include <DebugCamera.h>
#include <d3d12.h>
#include <ImGuiManager.h>

void Enemy::Initialize(Model* model, uint32_t etextureHandle, ViewProjection* viewProjection) {
	assert(model);
	model_ = model;
	enemyTextureHandle_ = etextureHandle;

	viewProjection_ = viewProjection;

	worldTransform_.Initialize();
	worldTransform_.translation_.z = 100;
}

void Enemy::Update() {
	worldTransform_.TransferMatrix(); 
	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};
	// キャラクターの移動の速さ
	const float kEnemySpeed = 0.2f;

	move.z -= kEnemySpeed;

		// 座標移動（ベクトルの加算）
	worldTransform_.translation_ += move;

	worldTransform_.UpdateMatrix();
}

void Enemy::Draw() { 
	model_->Draw(worldTransform_, *viewProjection_, enemyTextureHandle_);
}
