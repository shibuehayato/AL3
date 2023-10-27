#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Input.h"

class Player {
public:
	void Initialize(Model* head, Model* body, Model* L_arm, Model* R_arm);

	void Update();

	void Draw(ViewProjection viewProjection);

	const WorldTransform& GetWorldTransform() { return worldTransformHead_,worldTransformBody_,worldTransformL_arm_,worldTransformR_arm_; }

	void SetViewProjection(const ViewProjection* viewProjection)
	{
		viewProjection_ = viewProjection;
	}

private:
	// ワールド変換データ
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;
	// モデル
	Model* headModel_ = nullptr;
	Model* bodyModel_ = nullptr;
	Model* L_armModel_ = nullptr;
	Model* R_armModel_ = nullptr;

	// キーボード入力
	Input* input_ = nullptr;

	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;
};
