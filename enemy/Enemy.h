#pragma once
#include "Model.h"
#include "EnemyBullet.h"
#include <list>

enum class Phase {
	approach, // 接近する
	leave,    // 離脱する
};

class Enemy {
public:

	Enemy();
	~Enemy();

	// 自作メンバ関数
	void Approach();
	void Leave();

	// 接近フェーズ初期化
	void ApproachInitialize();

	void Initialize(Model* model, const Vector3& position);
	void Update();
	void Draw(ViewProjection viewProjection);

	// 敵の攻撃処理
	void Fire();

	// 発射間隔
	static const int kFireInterval = 60;

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
	
	// 弾
	EnemyBullet* bullet_ = nullptr;

	// 発射タイマー
	int32_t fireTimer = 0;

	std::list<EnemyBullet*> bullets_;
};