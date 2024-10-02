
#include "MathUtilityFortext.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Easing.h"

// 前方宣言
class Player;

class CameraController {
public:
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	// 初期化
	void Initialize();

	// 更新
	void Update();

	void SetTarget(Player* target) { target_ = target; };

	ViewProjection& GetViewProjection() { return viewProjection_; }

	void SetMovableArea(Rect area) { movableArea_ = area; }

	void Reset();

private:
	// ビュープロジェクション
	ViewProjection viewProjection_;

	Player* target_ = nullptr;

	// 追従対象とカメラ座標の差(オフセット)
	Vector3 targetOffset_ = {0, 0, -15.0f};

	WorldTransform* worldTransform_;

	Rect movableArea_ = {0, 100, 0, 100};

	Vector3 goalPos_;

	// 座標保管割合
	static inline const float kInterpolationRate = 0.1f;

	// 速度賭け率
	static inline const float kVelocityBias = 30.0f;

	static inline const Rect kMargin = {-8.0f, 8.0f, -8.0f, 8.0f};
};