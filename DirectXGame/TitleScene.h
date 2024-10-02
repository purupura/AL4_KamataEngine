#include "GameScene.h"
#include "Input.h"
#include "TextureManager.h"
class TitleScene {

public:
	TitleScene();

	~TitleScene();

	void Intialize();

	void Update();

	void Draw();

	bool IsFinished() const { return finished_; }

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// 終了フラグ
	bool finished_ = false;

	// ワールド変換データ
	WorldTransform worldTransform_;

	ViewProjection viewProjection_;

	uint32_t textureHandle_ = 0;

	Model* model_ = nullptr;
};