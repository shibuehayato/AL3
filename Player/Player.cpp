#include "Player.h"
#include <cassert>
#include "MyMath.h"
#include <ImGuiManager.h>

void Player::Initialize(Model* head, Model* body, Model* L_arm, Model* R_arm) {
	assert(head);
	modelFighterHead_ = head;
	assert(body);
	modelFighterBody_ = body;
	assert(L_arm);
	modelFighterL_arm_ = L_arm;
	assert(R_arm);
	modelFighterR_arm_ = R_arm;

	worldTransformHead_.translation_ = {0, 1, 0};
	worldTransformBody_.translation_ = {0, 0, 0};
	worldTransformL_arm_.translation_ = {-0.5f, 1.5, 0};
	worldTransformR_arm_.translation_ = {0.5, 1.5, 0};

	worldTransformHead_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();

	// シングルトンインスタンスを取得
	input_ = Input::GetInstance();

	InitializeFloatingGimmick();
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
		worldTransformL_arm_.rotation_.y = std::atan2(move.x, move.z);
		worldTransformR_arm_.rotation_.y = std::atan2(move.x, move.z);

		// 座標移動
		worldTransformHead_.translation_ = Add(worldTransformHead_.translation_, move);
		worldTransformBody_.translation_ = Add(worldTransformBody_.translation_, move);
		worldTransformL_arm_.translation_ = Add(worldTransformL_arm_.translation_, move);
		worldTransformR_arm_.translation_ = Add(worldTransformR_arm_.translation_, move);
	} 

	UpdateFloatingGimmick();

	worldTransformHead_.UpdateMatrix(); 
    worldTransformBody_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

void Player::Draw(ViewProjection viewProjection) {
	// 3Dモデル描画
	modelFighterHead_->Draw(worldTransformHead_, viewProjection);
	modelFighterBody_->Draw(worldTransformBody_, viewProjection);
	modelFighterL_arm_->Draw(worldTransformL_arm_, viewProjection);
	modelFighterR_arm_->Draw(worldTransformR_arm_, viewProjection);
}

void Player::InitializeFloatingGimmick()
{ 
	floatingParameter_ = 0.0f;
}

void Player::UpdateFloatingGimmick() {
	// 浮遊移動のサイクル<frame>
	const uint16_t cycle = 60;
	// 1フレームでのパラメータ加算値
	const float step = 2.0f * 3.14f / cycle;

	// パラメータを1ステップ分加算
	floatingParameter_ += step;
	// 2πを超えたら0に戻す
	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * 3.14f);

	// 浮遊を座標に反映
	worldTransformHead_.translation_.y = std::sin(floatingParameter_) * range_ + head_[1];
	worldTransformBody_.translation_.y = std::sin(floatingParameter_) * range_ + body_[1];
	worldTransformL_arm_.translation_.y = std::sin(floatingParameter_) * range_ + l_arm[1];
	worldTransformR_arm_.translation_.y = std::sin(floatingParameter_) * range_ + r_arm[1];

	// 両腕をぶらぶらさせる
	worldTransformL_arm_.rotation_.x = std::sin(floatingParameter_) * range_;
	worldTransformR_arm_.rotation_.x = std::sin(floatingParameter_) * range_;

	ImGui::Begin("Player");
	ImGui::SliderFloat3("Head Translation", head_, 0.0f, 5.0f);
	ImGui::SliderFloat3("Body Translation", body_, 0.0f, 5.0f);
	ImGui::SliderFloat3("ArmL Translation", l_arm, -5.0f, 5.0f);
	ImGui::SliderFloat3("ArmR Translation", r_arm, -5.0f, 5.0f);
	ImGui::SliderFloat("range", &range_, 0.0f, 5.0f);
	ImGui::End();

}