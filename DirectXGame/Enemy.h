#pragma once

#include <Model.h>
#include "WorldTransform.h"
#include <list>
#include "EnemyBullet.h"


class Enemy {

	public:
	/// <summary>
	/// 初期化
	/// <summary>
	void Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	///  <summary>
	/// 描画
	/// </summary>
	void Draw();

	//敵の接近処理
	void Approach();
	//敵の離脱処理
	void Leave();
	//敵の弾発射処理
	void Fire();

	//接近フェーズの初期化
	void approachInitialize();

	//デストラクタ
	~Enemy();

	static const int kFireInterval = 60;

	private:
	// 　ワールド変換データ
	WorldTransform worldTransform_;
	// 　モデル
	Model* model_ = nullptr;
	// 　テクスチャハンドル
	uint32_t enemyTextureHandle_ = 0u;

	ViewProjection* viewProjection_ = nullptr;

	enum class Phase {
		Approach,//接近
		Leave,//離脱
	};

	Phase phase_ = Phase::Approach;

	std::list<EnemyBullet*> bullets_;

	int32_t fireTimmer = 0;
};
