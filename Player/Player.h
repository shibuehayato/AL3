﻿#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Input.h"
#include "BaseCharacter.h"
#include <optional>

class Player : public BaseCharacter{
private:
	// 振る舞い
	enum class Behavior {
		kRoot, // 通常状態
		kAttack, // 攻撃中
	};

	// 振るまい
	Behavior behavior_ = Behavior::kRoot;

	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	// 攻撃後硬直
	float AfterAttackCooldown_ = 0;

public:
	void Initialize(const std::vector<Model*>& models) override;

	void Update() override;

	void Draw(ViewProjection viewProjection);

	const WorldTransform& GetWorldTransform() { return worldTransformBody_; }

	void SetViewProjection(const ViewProjection* viewProjection)
	{
		viewProjection_ = viewProjection;
	}

	// 浮遊ギミック初期化
	void InitializeFloatingGimmick();
	// 浮遊ギミック更新
	void UpdateFloatingGimmick();
	
	// 通常行動更新
	void BehaviorRootUpdate();
	// 攻撃更新
	void BehaviorAttackUpdate();

	// 通常行動初期化
	void BehaviorRootInitialize();
	// 攻撃行動初期化
	void BehaviorAttackInitialize();

private:
	// キーボード入力
	Input* input_ = nullptr;

	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	// 浮遊ギミックの媒介変数
	float floatingParameter_ = 0.0f;

	// 浮遊の振幅<m>
	float range_ = 0.4f;

	float time_ = 0.0f;

	float head_[3] = {0, 1.4f, 0};
	float body_[3] = {0, 0.2f, 0};
	float l_arm[3] = {-0.6f, 1.5f, 0};
	float r_arm[3] = {0.6f, 1.5f, 0};
};
