#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include "AABB.h"
#include <cmath>
#include <assert.h>

// 加算
Vector3 Add(const Vector3& v1, const Vector3& v2);

Vector3 Add2(const Vector3 v1, const Vector3 v2);

// 積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

// 1.X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);

// 2.Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);

// 3.Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

// 回転行列の合成
Matrix4x4 RotateXYZMatrix(const Vector3& radian);

// 3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

Vector3 Lerp(const Vector3& a, const Vector3& b, float t);

Vector3 Multiply(float scalar, const Vector3& v);

// 代入演算子オーバーロード
// Vector3の掛け算
Vector3& operator*=(Vector3& v, float s);

// 2項演算子オーバーロード
// Vector3の掛け算
Vector3 operator*(const Vector3& v, float s);

// 代入演算子オーバーロード
// Vector3の足し算
Vector3& operator+=(Vector3& v1, const Vector3& v2);

// 2項演算子オーバーロード
// Vector3の足し算
const Vector3 operator+(Vector3& v1, Vector3& v2);

Vector3& operator-=(Vector3& lhv, const Vector3& rhv);

const Vector3 operator-(Vector3& v1, Vector3& v2);

bool IsCollision(const AABB& aabb1, const AABB& aabb2);

Matrix4x4 MakeRotateZMatrix(float thata);

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);