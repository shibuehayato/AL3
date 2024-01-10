#pragma once
#include "BaseCharacter.h"

class Enemy : public BaseCharacter {
public:
	void Initialize(const std::vector<Model*>& models) override;
	void Update();
	void Draw(ViewProjection viewProjection);

	bool const GetIsDead() const { return isDead_; }

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	void Reset();

private:
	// デスフラグ
	bool isDead_ = false;
	int deathTimer_ = 60;
};