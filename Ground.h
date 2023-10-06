#pragma once
#include "Model.h"
#include "WorldTransform.h"

class Ground {
public:
	void Initialize(Model* model);

	void Update();

	void Draw(ViewProjection viewProjection);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
};
