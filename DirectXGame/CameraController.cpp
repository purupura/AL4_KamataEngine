#define NOMINMAX
#include "CameraController.h"
#include "Player.h"

void CameraController::Initialize() { viewProjection_.Initialize(); }

void CameraController::Update() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// 追従対象とオフセットと追従対象からカメラの目標を計算
	const Vector3& targetVelocity = target_->GetVelocity();
	goalPos_ = Add2(targetWorldTransform.translation_, targetOffset_);
	goalPos_ = Add2(goalPos_, Multiply(kVelocityBias, targetVelocity));

	// 座標補間によりゆったり追従
	viewProjection_.translation_ = LerpVector(viewProjection_.translation_, goalPos_, kInterpolationRate);

	// 追従対象がカメラの外に出ないように補正
	viewProjection_.translation_.x = std::max(viewProjection_.translation_.x, targetWorldTransform.translation_.x + kMargin.left);
	viewProjection_.translation_.x = std::min(viewProjection_.translation_.x, targetWorldTransform.translation_.x + kMargin.right);
	viewProjection_.translation_.y = std::max(viewProjection_.translation_.y, targetWorldTransform.translation_.y + kMargin.bottom);
	viewProjection_.translation_.y = std::min(viewProjection_.translation_.y, targetWorldTransform.translation_.y + kMargin.top);
	// 移動範囲制限
	viewProjection_.translation_.x = std::max(viewProjection_.translation_.x, movableArea_.left);
	viewProjection_.translation_.x = std::min(viewProjection_.translation_.x, movableArea_.right);
	viewProjection_.translation_.y = std::max(viewProjection_.translation_.y, movableArea_.bottom);
	viewProjection_.translation_.y = std::min(viewProjection_.translation_.y, movableArea_.top);

	// 行列を更新する
	viewProjection_.UpdateMatrix();
}

void CameraController::Reset() {}