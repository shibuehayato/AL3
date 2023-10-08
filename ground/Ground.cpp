#include "Ground.h"
#include <cassert>

void Ground::Initialize(Model* model) {
	assert(model);

	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.5, 0.5, 0.5};

}

void Ground::Update() { worldTransform_.UpdateMatrix(); }

void Ground::Draw(ViewProjection viewProjection) {
	// 3Dモデル描画
	model_->Draw(worldTransform_, viewProjection);
}