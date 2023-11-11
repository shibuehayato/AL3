#include "Player.h"
#include <cassert>
#include "MyMath.h"
#include <ImGuiManager.h>

void Player::Initialize(const std::vector<Model*>& models) {
	// 基底クラスの初期化
	BaseCharacter::Initialize(models);

	worldTransformHead_.translation_ = {0, 1.8f, 0};
	worldTransformBody_.translation_ = {0, 0, 0};
	worldTransformL_arm_.translation_ = {-0.5f, 1.5, 0};
	worldTransformR_arm_.translation_ = {0.5, 1.5, 0};

	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;

	// シングルトンインスタンスを取得
	input_ = Input::GetInstance();

	InitializeFloatingGimmick();
}

void Player::Update() { 

	// 基底クラスの更新
	BaseCharacter::Update();

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
	
		worldTransformBody_.rotation_.y = std::atan2(move.x, move.z);

		// 座標移動
		worldTransformBody_.translation_ = Add(worldTransformBody_.translation_, move);
	} 

	UpdateFloatingGimmick();

    worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix(); 
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

void Player::Draw(ViewProjection viewProjection) {
	// 3Dモデル描画
	models_[0]->Draw(worldTransformBody_, viewProjection);
	models_[1]->Draw(worldTransformHead_, viewProjection);
	models_[2]->Draw(worldTransformL_arm_, viewProjection);
	models_[3]->Draw(worldTransformR_arm_, viewProjection);
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
	worldTransformBody_.translation_.y = std::sin(floatingParameter_) * range_;

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