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
	worldTransform_.UpdateMatrix();
	
	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	move.z -= kCharacterSpeed;

	worldTransform_.translation_ = VectorAdd(worldTransform_.translation_, move);
}

void Enemy::Draw(const ViewProjection& viewProjection) 
{
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}