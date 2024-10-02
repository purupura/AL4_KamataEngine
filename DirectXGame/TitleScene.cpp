#include "TitleScene.h"

TitleScene::TitleScene() {}

TitleScene::~TitleScene() { delete model_; }

void TitleScene::Intialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	model_ = Model::CreateFromOBJ("titleFont");
	textureHandle_ = TextureManager::Load("white1x1.png");
	viewProjection_.Initialize();
	worldTransform_.Initialize();
}

void TitleScene::Update() {

	if (Input::GetInstance()->PushKey(DIK_SPACE)) {

		finished_ = true;
	}
}

void TitleScene::Draw() {

#pragma endregion

#pragma region 3Dオブジェクト描画
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	model_->Draw(worldTransform_, viewProjection_, textureHandle_);

	Model::PostDraw();
}