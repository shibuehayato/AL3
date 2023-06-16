#pragma once
#include "Model.h"

enum class Phase {
	approach, // 接近する
	leave,    // 離脱する
};

class Enemy {
public:
	// 自作メンバ関数
	void Approach();
	void Leave();
	void Initialize(Model* model, const Vector3& position);
	void Update();
	void Draw(const ViewProjection& viewProjection);

private:
	// メンバ関数ポインタのテーブル
	static void (Enemy::*spPhaseTable[])();
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Vector3 velocity_;
	Vector3 move_;
	float kCharacterSpeed_;
	Phase phase_ = Phase::approach;
};