﻿#pragma once
#include "model.h"
#include "WorldTransform.h"
#include "Input.h"
#include "PlayerBullet.h"
#include <list>
#include "Sprite.h"

class Player {
public:

	Player();

	~Player();

	void Initialize(Model* model, uint32_t textureHandle, Vector3 pos);

	void Update(ViewProjection viewProjection);

	void Draw(ViewProjection viewProjection);

	void Rotate();

	void Attack();

	/// <summary>
	/// 親となるワールドトランスフォームをセット
	/// </summary>
	/// <param name="parent">親となるワールドトランスフォーム</param>
	void SetParnet(const WorldTransform* parent);

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

    /// <summary>
    /// UI描画
    /// </summary>
	void DrawUI();

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

	// 3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;

	// 2Dレティクル用スプライト
	Sprite* sprite2DReticle_ = nullptr;

	std::list<PlayerBullet*> bullets_;
};