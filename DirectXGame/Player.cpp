#include "Player.h"
#include <cassert>
#include "Input.h"
#include "MathUtilityForText.h"
#include <DebugCamera.h>
#include <d3d12.h>
#include <ImGuiManager.h>



void Player::Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection) {

	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;

	viewProjection_ = viewProjection;

	worldTransform_.Initialize();

	input_ = Input::GetInstance();


}

void Player::Update() {
	worldTransform_.TransferMatrix(); 

	//キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};
	//キャラクターの移動の速さ
	const float kCharacterSpeed = 0.2f;

	//押した方向で移動ベクトルを変更（左右）
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}
	// 押した方向で移動ベクトルを変更（上下）
	if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}

	const float kMoveLimitX = 34;
	const float kMoveLimitY = 19;
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	Rotate();

	Attack();

	 for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	//座標移動（ベクトルの加算）
	worldTransform_.translation_ += move;

	worldTransform_.UpdateMatrix();

	ImGui::Begin("debug");
	ImGui::InputFloat("SpriteX", &worldTransform_.translation_.x);
	ImGui::SliderFloat("SliderSpriteX", &worldTransform_.translation_.x, -100.0f, 1000.0f);

	ImGui::InputFloat("SpriteY", &worldTransform_.translation_.y);
	ImGui::SliderFloat("SliderSpriteY", &worldTransform_.translation_.y, -100.0f, 100.0f);

	ImGui::InputFloat("SpriteZ", &worldTransform_.translation_.z);
	ImGui::SliderFloat("SliderSpriteZ", &worldTransform_.translation_.z, -100.0f, 100.0f);

	ImGui::End();

	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	}

void Player::Draw() { 
	model_->Draw(worldTransform_, *viewProjection_, textureHandle_);
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(*viewProjection_);
	}
}

void Player::Rotate() {
	
		// 回転速さ
	    const float kRotSpeed = 0.02f;

	    // 押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_A)) {
		    worldTransform_.rotation_.y -= kRotSpeed;
		}
	    if (input_->PushKey(DIK_D)) {
		    worldTransform_.rotation_.y += kRotSpeed;
	    }

		Attack();

	 for (PlayerBullet* bullet : bullets_) {
		    bullet->Update();   
	 }
}

void Player::Attack() {
	if (input_->TriggerKey(DIK_0)) {

		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, worldTransform_.translation_,velocity);

		bullets_.push_back(newBullet);
	}
}

Player::~Player() {
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}



