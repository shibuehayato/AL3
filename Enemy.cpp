#include "Enemy.h"
#include "Matrix.h"
#include <cassert>

void Enemy::Initialize(Model* model, const Vector3& position)
{
	// Nullポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("uvChecker.png");

	worldTransform_.Initialize();

	// 引数で初期座標をリセット
	worldTransform_.translation_ = position;

}

void Enemy::Update()
{ 
	// 敵の移動ベクトル
	Vector3 move = {0, 0, 0};
	// 敵の移動速さ
	const float kCharacterSpeed = 0.2f;
	switch (phase_) { 
	case Phase::approach:
	default:
		move.z -= kCharacterSpeed;
		// 規定の位置に到達したら削除
		if (worldTransform_.translation_.z < 0.0f) {
			phase_ = Phase::leave;
		}
		break;
	case Phase::leave:
		move.x -= kCharacterSpeed;
		move.y += kCharacterSpeed;
		break;
	}
	// 移動ベクトルを加算
	worldTransform_.translation_ = VectorAdd(worldTransform_.translation_, move);
	worldTransform_.UpdateMatrix();
}

void Enemy::Draw(const ViewProjection& viewProjection) 
{
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}