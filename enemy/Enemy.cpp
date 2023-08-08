#include "Enemy.h"
#include "Matrix.h"
#include "Player.h"
#include <cassert>
#include "GameScene.h"
#include "ImGuiManager.h"

Enemy::Enemy() {}

void Enemy::Initialize(Model* model, const Vector3& position)
{
	// Nullポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("uvChecker.png");

	worldTransform_.Initialize();

	move_ = {0, 0, 0};
	kCharacterSpeed_ = 0.002f;

	static_cast<size_t>(phase_);

	// 引数で初期座標をリセット
	worldTransform_.translation_ = position;

	// 接近フェーズ初期化
	ApproachInitialize();
}

// フェーズの関数テーブル
void (Enemy::*Enemy::spPhaseTable[])() = {
	&Enemy::Approach,
	&Enemy::Leave
};

void Enemy::Approach()
{
	move_.z -= kCharacterSpeed_;

		if (worldTransform_.translation_.z < 0.0f) {
		(this->*spPhaseTable[1])();
		}

}

void Enemy::Leave()
{
	    move_.x -= kCharacterSpeed_;
	    move_.y += kCharacterSpeed_;
}

void Enemy::Update()
{ 	    
	// 発射タイマーカウントダウン
	    --fireTimer;
	    // 指定時間に達した
	    if (fireTimer <= 0) {
		// 弾を発射
		Fire();
		// 発射タイマーを初期化
		fireTimer = kFireInterval;
	    }

	(this->*spPhaseTable[0])();
	
	worldTransform_.UpdateMatrix();
}

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得 (ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	
	return worldPos;
}

void Enemy::Fire()
{
	assert(player_);
	
	// 弾の速度
	const float kBulletSpeed = 1.0f;

	Vector3 playerPosition = player_->GetWorldPosition();
	Vector3 enemyPosition = this->GetWorldPosition();
	Vector3 velocity = Sub(playerPosition,enemyPosition);
	velocity = Normalize(velocity);
	velocity.x *= kBulletSpeed;
	velocity.y *= kBulletSpeed;
	velocity.z *= kBulletSpeed;

	// 弾を発生し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);
	
	// 弾を登録
	gameScene_->AddEnemyBullet(newBullet);
}

void Enemy::ApproachInitialize()
{
	// 発射タイマーを初期化
	fireTimer = kFireInterval;
}

void Enemy::OnCollision()
{
	isDead_ = true;
}

void Enemy::Draw(ViewProjection viewProjection) 
{
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}