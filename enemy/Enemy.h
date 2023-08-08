#pragma once
#include "Model.h"
#include "EnemyBullet.h"
#include <list>

// 自機クラスの前方宣言
class Player;

// GameSceneの前方宣言
class GameScene;

enum class Phase {
	approach, // 接近する
	leave,    // 離脱する
};

class Enemy {
public:
	Enemy();

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

	void SetPlayer(Player* player) { player_ = player; }

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	// 弾リストを取得
	// const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	bool IsDead() const { return isDead_; }

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

	// 自キャラ
	Player* player_ = nullptr;

	// 弾
	EnemyBullet* bullet_ = nullptr;

	// 発射タイマー
	int32_t fireTimer = 0;

	// std::list<EnemyBullet*> bullets_;

	// ゲームシーン
	GameScene* gameScene_ = nullptr;

	bool isDead_ = false;
};