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
	worldTransform_.translation_ = {0, 1, 0};
	worldTransform_.Initialize();

}

void Player::Update() { worldTransform_.UpdateMatrix(); }

void Player::Draw(ViewProjection viewProjection) {
	// 3Dモデル描画
	headModel_->Draw(worldTransform_, viewProjection);
	bodyModel_->Draw(worldTransform_, viewProjection);
	L_armModel_->Draw(worldTransform_, viewProjection);
	R_armModel_->Draw(worldTransform_, viewProjection);
}