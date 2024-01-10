#include "Enemy.h"
#include "MyMath.h"

void Enemy::Initialize(const std::vector<Model*>& models) 
{
	// 基底クラスの初期化
	BaseCharacter::Initialize(models);
	worldTransformL_arm_.translation_ = {-1.0f, 0.8f, 0};
	worldTransformR_arm_.translation_ = {1.0f, 0.8f, 0};

	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;
}

void Enemy::Update() { 
	//基底クラスの更新
	BaseCharacter::Update();

	// 敵の速度
	float EnemySpeed = 0.1f;
	Vector3 velocity(0, 0, EnemySpeed);
	// 速度ベクトルを敵の向きに合わせて回転させる
	velocity = TransformNormal(velocity, worldTransformBody_.matWorld_);
	worldTransformBody_.rotation_.y += 0.06f;
	worldTransformBody_.translation_ = Add(worldTransformBody_.translation_, velocity);
}

void Enemy::Draw(ViewProjection viewProjection) 
{
	//if (isDead_ == false) {
	//	// 基底クラスの描画
	//	models_[0]->Draw(worldTransformBody_, viewProjection);
	//	models_[1]->Draw(worldTransformL_arm_, viewProjection);
	//	models_[2]->Draw(worldTransformR_arm_, viewProjection);
	//}

	if (deathTimer_ > 30) {
		// 基底クラスの描画
		models_[0]->Draw(worldTransformBody_, viewProjection);
		models_[1]->Draw(worldTransformL_arm_, viewProjection);
		models_[2]->Draw(worldTransformR_arm_, viewProjection);
	}
}

Vector3 Enemy::GetWorldPosition()
{ 
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得 (ワールド座標)
	worldPos.x = worldTransformBody_.matWorld_.m[3][0];
	worldPos.y = worldTransformBody_.matWorld_.m[3][1];
	worldPos.z = worldTransformBody_.matWorld_.m[3][2];

	return worldPos;
}

void Enemy::OnCollision() { 
	isDead_ = true;
	deathTimer_--;
}

void Enemy::Reset() { 
	isDead_ = false;
	deathTimer_ = 60;
}