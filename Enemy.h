#pragma once
#include "BaseCharacter.h"

class Enemy : public BaseCharacter {
public:
	void Initialize(const std::vector<Model*>& models) override;
	void Update();
	void Draw(ViewProjection viewProjection);

	bool isDead = false;

	bool GetIsDead() { return isDead; }

	Vector3 GetWorldPos();

	void Reset();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();
};