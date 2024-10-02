#include "Vector3.h"

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
public:
	float x;
	float y;
	float z;
	Vector3 operator+(const Vector3& other) const { return Vector3{x + other.x, y + other.y, z + other.z}; }
};