﻿#pragma once
#include "model.h"
#include "WorldTransform.h"
#include "Input.h"
#include "PlayerBullet.h"
#include <list>
class Player {
public:

	Player();

	~Player();

	void Initialize(Model* model, uint32_t textureHandle);

	void Update();

	void Draw(ViewProjection viewProjection);

	void Rotate();

	void Attack();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// キーボード入力
	Input* input_ = nullptr;
	// 弾
	PlayerBullet* bullet_ = nullptr;

	std::list<PlayerBullet*> bullets_;
};