#include "Enemy.h"
#include <cassert>
#include "MathUtilityForText.h"
#include <DebugCamera.h>
#include <d3d12.h>
#include <ImGuiManager.h>

void Enemy::Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection) {
	assert(model);
	model_ = model;
	enemyTextureHandle_ = textureHandle;

	viewProjection_ = viewProjection;

	worldTransform_.Initialize();
	worldTransform_.translation_.z = 100;
	worldTransform_.translation_.x = 30;

	approachInitialize();
}

void Enemy::Update() {
	worldTransform_.TransferMatrix(); 
	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};
	// キャラクターの移動の速さ
	//const float kEnemySpeed =  0.2f;
	//move.z -= kEnemySpeed;
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	switch (phase_) {
	case Enemy::Phase::Approach:
	default:
		Approach();
		break;
	case Enemy::Phase::Leave:
		Leave();
		break;
	}

	worldTransform_.UpdateMatrix();

	ImGui::Begin("debug");
	ImGui::InputFloat("SpriteX", &worldTransform_.translation_.x);
	ImGui::SliderFloat("SliderSpriteX", &worldTransform_.translation_.x, -100.0f, 1000.0f);

	ImGui::InputFloat("SpriteY", &worldTransform_.translation_.y);
	ImGui::SliderFloat("SliderSpriteY", &worldTransform_.translation_.y, -100.0f, 100.0f);

	ImGui::InputFloat("SpriteZ", &worldTransform_.translation_.z);
	ImGui::SliderFloat("SliderSpriteZ", &worldTransform_.translation_.z, -100.0f, 100.0f);

	ImGui::End();
}

void Enemy::Draw() { 
	model_->Draw(worldTransform_, *viewProjection_, enemyTextureHandle_); 
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(*viewProjection_);
	}
}

void Enemy::Approach() {
	Vector3 approachSpeed = {0, 0, -0.2f};

	worldTransform_.translation_ += approachSpeed;

	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}

	fireTimmer--;

	if (fireTimmer <= 0) {
		
		Fire();

		fireTimmer = kFireInterval;
	
	}

	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}
}

void Enemy::Leave() { 
	Vector3 leaveSpeed = {-0.2f, 0.2f, 0};
	worldTransform_.translation_ += leaveSpeed; 

}

void Enemy::Fire() {
	const float kBulletSpeed = 1.0f;
	Vector3 velocity(0, 0, kBulletSpeed);

	velocity = TransformNormal(velocity, worldTransform_.matWorld_);

	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	bullets_.push_back(newBullet);
}

void Enemy::approachInitialize() {
	//発射タイマーの初期化
	fireTimmer = kFireInterval; }

Enemy::~Enemy() {}
