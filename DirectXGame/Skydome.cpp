#include "Skydome.h"

void Skydome::Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewprojection) {

	assert(model);

	model_ = model;
	texturehandle_ = textureHandle;


	viewprojection_ = viewprojection;

	worldTransform_.Initialize();
}

void Skydome::Update() {}

void Skydome::Draw() { model_->Draw(worldTransform_, *viewprojection_); }