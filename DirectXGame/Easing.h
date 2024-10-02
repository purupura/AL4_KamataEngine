#define _USE_MATH_DEFINES
#include "Vector3.h"
#include <cmath>
#include <numbers>
float EaseInSine(float a);
float EaseOutSine(float x);
float EaseInOutSine(float x);

float EaseInQuad(float x);
float EaseOutQuad(float x);
float EaseInOutQuad(float x);

float EaseInCubic(float x);
float EaseOutCubic(float x);
float EaseInOutCubic(float x);

float EaseInQuart(float x);
float EaseOutQuart(float x);
float EaseInOutQuart(float x);

float EaseInQuint(float x);
float EaseOutQuint(float x);
float EaseInOutQuint(float x);

float EaseInExpo(float x);
float EaseOutExpo(float x);
float EaseInOutExpo(float x);

float EaseInCirc(float x);
float EaseOutCirc(float x);
float EaseInOutCirc(float x);

float EaseInBack(float x);
float EaseOutBack(float x);
float EaseInOutBack(float x);

float EaseInElastic(float x);
float EaseOutElastic(float x);
float EaseInOutElastic(float x);

float EaseInBounce(float x);
float EaseInOutBounce(float x);
float EaseOutBounce(float x);

float EaseInOut(float x1, float x2, float t);
float Lerp(float x1, float x2, float t);
Vector3 LerpVector(const Vector3& v1, const Vector3& v2, float t);