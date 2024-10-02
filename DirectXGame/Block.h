#pragma once

#include <Model.h>
#include <WorldTransform.h>

class Block {
	ViewProjection* viewProjection_ = nullptr;

public:
	/// <summary>
	/// 初期化
	/// <summary>
	void Initialize(Model* model, ViewProjection* viewProjection);

	/// <summary>
	/// 初期化
	/// <summary>
	void Update();

	/// <summary>
	/// 初期化
	/// <summary>
	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	Block* block_ = nullptr;

	// モデル
	Model* model_ = nullptr;
};
