#include "Enemy.h"
#include "MyMath.h"
#include <ImGuiManager.h>

void Enemy::Initialize(const std::vector<Model*>& models) 
{
	// 基底クラスの初期化
	BaseCharacter::Initialize(models);
	worldTransformBody_.translation_ = {0.0f, 1.0f, 0.0f};
	worldTransformL_arm_.translation_ = {-1.0f, 0.8f, 0};
	worldTransformR_arm_.translation_ = {1.0f, 0.8f, 0};

	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;
}

void Enemy::Update() { 
	//基底クラスの更新
	BaseCharacter::Update();

	ImGui::Begin("enemy");
	ImGui::DragFloat3("pos", &worldTransformBody_.translation_.x);
	ImGui::End();

	// 敵の速度
	float EnemySpeed = 0;
	Vector3 velocity(0, 0, EnemySpeed);
	// 速度ベクトルを敵の向きに合わせて回転させる
	velocity = TransformNormal(velocity, worldTransformBody_.matWorld_);
	//worldTransformBody_.rotation_.y += 0.06f;
	worldTransformBody_.translation_ = Add(worldTransformBody_.translation_, velocity);
}

void Enemy::Draw(ViewProjection viewProjection) 
{
	if (isDead == false) {

		// 基底クラスの描画
		models_[0]->Draw(worldTransformBody_, viewProjection);
		models_[1]->Draw(worldTransformL_arm_, viewProjection);
		models_[2]->Draw(worldTransformR_arm_, viewProjection);
	}
}

Vector3 Enemy::GetWorldPos() { 
	Vector3 pos;

	pos.x = worldTransformBody_.matWorld_.m[3][0];
	pos.y = worldTransformBody_.matWorld_.m[3][1];
	pos.z = worldTransformBody_.matWorld_.m[3][2];

	return pos;
}

void Enemy::OnCollision() { isDead = true; }

void Enemy::Reset() {
	worldTransformBody_.translation_ = {0.0f, 1.0f, 0.0f};
	worldTransformL_arm_.translation_ = {-1.0f, 0.8f, 0};
	worldTransformR_arm_.translation_ = {1.0f, 0.8f, 0};

	isDead = false;
}