#include "Enemy.h"

void Enemy::Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;

	textureHandle_ = textureHandle;

	// ワールド変換の初期化
	viewprojection_ = viewProjection;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;

	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.0f;
}

void Enemy::Update() {

	// 移動
	worldTransform_.translation_ += velocity_;

	// タイマーを加算
	walkTimer_ += 1.0f / 60.0f;

	worldTransform_.rotation_.x = std::sin(2.0f * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);

	// 行列計算
	worldTransform_.UpdateMatrix();
}

void Enemy::Draw() {
 model_->Draw(worldTransform_, *viewprojection_,textureHandle_); 
}

AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;
	aabb.min = {worldPos.x - 0.5f, worldPos.y - 0.5f, worldPos.z - 0.5f};
	aabb.max = {worldPos.x + 0.5f, worldPos.y + 0.5f, worldPos.z + 0.5f};
	return aabb;
}

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;
	// ワールド行列から平行移動成分を取り出す
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

void Enemy::OnCollision(const Player* player) { (void)player; }