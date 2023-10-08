#include "Player.h"
#include <cassert>

void Player::Initialize(Model* head, Model* body, Model* L_arm, Model* R_arm) {
	assert(head);
	headModel_ = head;
	assert(body);
	bodyModel_ = body;
	assert(L_arm);
	L_armModel_ = L_arm;
	assert(R_arm);
	R_armModel_ = R_arm;

	worldTransformHead_.translation_ = {0, 1.5, 0};
	worldTransformBody_.translation_ = {0, 0, 0};
	worldTransformL_arm_.translation_ = {-0.5, 1.5, 0};
	worldTransformR_arm_.translation_ = {0.5, 1.5, 0};

	worldTransformHead_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();

}

void Player::Update() { 
	worldTransformHead_.UpdateMatrix(); 
    worldTransformBody_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

void Player::Draw(ViewProjection viewProjection) {
	// 3Dモデル描画
	headModel_->Draw(worldTransformHead_, viewProjection);
	bodyModel_->Draw(worldTransformBody_, viewProjection);
	L_armModel_->Draw(worldTransformL_arm_, viewProjection);
	R_armModel_->Draw(worldTransformR_arm_, viewProjection);
}