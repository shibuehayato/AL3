#pragma once
#include "Model.h"

enum class Phase {
	approach, // 接近する
	leave,    // 離脱する
};

class Enemy {
public:
	void Initialize(Model* model, const Vector3& position);
	void Update();
	void Draw(const ViewProjection& viewProjection);

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Vector3 velocity_;
	Phase phase_ = Phase::approach;
};