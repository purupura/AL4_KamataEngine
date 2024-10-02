#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	delete debugCamera_;

	delete modelSkydome_;

	delete mapChipField_;

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	delete deathParticle_;

}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイルを指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("player.png");

	// ファイルを指定してテクスチャを読み込む
	enemyTextureHandle_ = TextureManager::Load("enemy/enemy.png");

	// 3Dモデルの生成
	model_ = Model::CreateFromOBJ("player");

	enemyModel_ = Model::CreateFromOBJ("enemy");

	modelBlock_ = Model::CreateFromOBJ("block");

	modelParticles_ = Model::CreateFromOBJ("deathParticle", true);

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションンの初期化

	viewProjection_.Initialize();

	mapChipField_ = new MapChipField();

	mapChipField_->Initialize();

	// 自キャラの生成
	player_ = new Player();



	// モデルデータの生成

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 天球モデルの生成
	modelSkydome_ = Model::CreateFromOBJ("SkyDome", true);



	skydome_ = new Skydome();

	skydome_->Initialize(modelSkydome_, skyDometextureHandle_, &viewProjection_);

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	GenerateBlocks();

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(3, 18);

	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, &viewProjection_, playerPosition);

		// パーティクル生成
	deathParticle_ = new DeathParticles;
	deathParticle_->Initialize(modelParticles_, &viewProjection_, playerPosition);


	player_->SetMapChipField(mapChipField_);



	for (int32_t i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(11 + i * 3, 18 - i);
		
		newEnemy->Initialize(enemyModel_, enemyTextureHandle_, &viewProjection_, enemyPosition);
	
	enemies_.push_back(newEnemy);
	}
	
	

	// カメラコントローラの初期化
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->Reset();

	CameraController::Rect cameraArea = {12.0f, 100.0f - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);

	phase_ = Phase::kPlay;

}

void GameScene::Update() {

	CameraController::Rect cameraArea = {12.0f, 100.0f - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);

	switch (phase_) {
	case Phase::kPlay:
		// 自キャラの更新
		player_->Update();

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		cameraController_->Update();

		if (isDebugCameraActive_) {
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			viewProjection_.TransferMatrix();
		} else {
			viewProjection_.matView = cameraController_->GetViewProjection().matView;
			viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の更新と転送
			viewProjection_.TransferMatrix();
		}

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}

				worldTransformBlock->UpdateMatrix();
			}
		}

		CheckAllCollisions();

		break;

	case Phase::kDeath:

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// デスパーティクルが存在するなら
		if (deathParticle_) {

			deathParticle_->Update();
		}

		if (isDebugCameraActive_) {
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			viewProjection_.TransferMatrix();
		} else {
			viewProjection_.matView = cameraController_->GetViewProjection().matView;
			viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の更新と転送
			viewProjection_.TransferMatrix();
		}

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}

				worldTransformBlock->UpdateMatrix();
			}
		}

		break;
	}

	ChangePhase();

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	///

	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 3Dモデル描画
	/*model_->Draw(worldTransform_, viewProjection_, textureHandle_);*/
	// 自キャラの描画
	switch (phase_) {

	case Phase::kPlay:

		// 自キャラの描画
		player_->Draw();

		break;

	case Phase::kDeath:

		break;
	}

	// 敵の描画
	// enemy_->Draw();

	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	skydome_->Draw();

	// 縦横ブロック描画
	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko)
				continue;

			modelBlock_->Draw(*worldTransformBlockYoko, viewProjection_);
		}
	}

	if (deathParticle_) {

		deathParticle_->Draw();
	}


	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::GenerateBlocks() {
	// 要素数　
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizonal = mapChipField_->GetNumBlockHorizonal();

	// 要素数を変更する
	// 列数を設定

	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizonal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizonal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}


}

void GameScene::CheckAllCollisions() {

#pragma region

	// 判定対象1と2の座標
	AABB aabb1, aabb2;

	// 自キャラの座標
	aabb1 = player_->GetAABB();
	for (Enemy* enemy : enemies_) {
		// 敵弾の座標
		aabb2 = enemy->GetAABB();

		// AABB同士の交差判定(
		if (IsCollision(aabb1, aabb2)) {
			// 自キャラの衝突判定コールバックを呼び出す
			player_->OnCollision(enemy);
			// 敵弾の衝突判定コールバックを呼び出す
			enemy->OnCollision(player_);
		}
	}
#pragma endregion
}

void GameScene::ChangePhase() {
	switch (phase_) {

	case Phase::kPlay:

		if (player_->IsDead()) {

			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;

			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			// 生成処理
			deathParticle_ = new DeathParticles;

			deathParticle_->Initialize(modelParticles_, &viewProjection_, deathParticlesPosition);
		}

		break;

	case Phase::kDeath:

		if (deathParticle_ && deathParticle_->IsFinished()) {

			finished_ = true;
		}

		break;
	}
}