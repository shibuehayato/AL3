#pragma once
#include "Model.h"
#include "WorldTransform.h"

class Player {
public:
	void Initialize(Model* head, Model* body, Model* L_arm, Model* R_arm);

	void Update();

	void Draw(ViewProjection viewProjection);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* headModel_ = nullptr;
	Model* bodyModel_ = nullptr;
	Model* L_armModel_ = nullptr;
	Model* R_armModel_ = nullptr;
};
