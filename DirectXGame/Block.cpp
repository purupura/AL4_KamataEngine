#include "Block.h"
#include <cassert>
void Block::Initialize(Model* model, ViewProjection* viewProjection) {
	assert(model);
	model_ = model;
	viewProjection_ = viewProjection;

	worldTransform_.Initialize();
}

void Block::Update() { worldTransform_.TransferMatrix(); }

void Block::Draw() { model_->Draw(worldTransform_, *viewProjection_); }