#include "Player.h"
#include <cassert>
#include "MyMath.h"

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

	// シングルトンインスタンスを取得
	input_ = Input::GetInstance();

}

void Player::Update() { 

	// ゲームパッドの状態を得る変数
	XINPUT_STATE joyState;

	// ゲームパッドが有効の場合if文が通る
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// 速さ
		const float speed = 0.3f;

		// 移動量
		Vector3 move = {0, 0, 0};

		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * speed;
		move.z += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * speed;

		// カメラの角度から回転行列を計算する
		Matrix4x4 RotationMatrix = MakeRotateMatrix(viewProjection_->rotation_);

		// オフセットをカメラの回転に合わせて回転させる
		move = TransformNormal(move, RotationMatrix);

		worldTransformHead_.rotation_.y = std::atan2(move.x, move.z);
		worldTransformBody_.rotation_.y = std::atan2(move.x, move.z);
		worldTransformL_arm_.rotation_.y = std::atan2(move.x, move.y);
		worldTransformR_arm_.rotation_.y = std::atan2(move.x, move.y);

		// 座標移動
		worldTransformHead_.translation_ = Add(worldTransformHead_.translation_, move);
		worldTransformBody_.translation_ = Add(worldTransformBody_.translation_, move);
		worldTransformL_arm_.translation_ = Add(worldTransformL_arm_.translation_, move);
		worldTransformR_arm_.translation_ = Add(worldTransformR_arm_.translation_, move);
	} 

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