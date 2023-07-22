#include "Enemy.h"
#include "Matrix.h"
#include "Player.h"
#include <cassert>

Enemy::Enemy() {}

Enemy::~Enemy(){
	// bullet_の解放
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
}

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
	// 敵の移動ベクトル
	//Vector3 move = {0, 0, 0};
	// 敵の移動速さ
	//const float kCharacterSpeed = 0.2f;
	
	//switch (phase_) { 
	//case Phase::approach:
	//default:
	//	move.z -= kCharacterSpeed;
	//	// 規定の位置に到達したら削除
	//	if (worldTransform_.translation_.z < 0.0f) {
	//		phase_ = Phase::leave;
	//	}
	//	break;
	//case Phase::leave:
	//	move.x -= kCharacterSpeed;
	//	move.y += kCharacterSpeed;
	//	break;
	//}
	    
	// 発射タイマーカウントダウン
	    --fireTimer;
	    // 指定時間に達した
	    if (fireTimer <= 0) {
		// 弾を発射
		Fire();
		// 発射タイマーを初期化
		fireTimer = kFireInterval;
	    }

	// デスフラグの立った弾を削除
	    bullets_.remove_if([](EnemyBullet* bullet) {
		    if (bullet->IsDead()) {
			    delete bullet;
			    return true;
		    }
		    return false;
	    });

	// 弾更新
	    for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
		}

	(this->* spPhaseTable[0])();
	// 移動ベクトルを加算
	worldTransform_.translation_ = VectorAdd(worldTransform_.translation_, move_);
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
	bullets_.push_back(newBullet);
}

void Enemy::ApproachInitialize()
{
	// 発射タイマーを初期化
	fireTimer = kFireInterval;
}

void Enemy::Draw(ViewProjection viewProjection) 
{
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// 弾描画
    for (EnemyBullet* bullet : bullets_) {
    		bullet->Draw(viewProjection);
    	}
}