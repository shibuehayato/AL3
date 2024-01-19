#include "Player.h"
#include <cassert>
#include "MyMath.h"
#include <ImGuiManager.h>
#include <functional>

void Player::Initialize(const std::vector<Model*>& models) {
	// 基底クラスの初期化
	BaseCharacter::Initialize(models);

	worldTransformHead_.translation_ = {0, 1.8f, 0};
	worldTransformBody_.translation_ = {0, 0, 0};
	worldTransformL_arm_.translation_ = {-0.5f, 1.5, 0};
	worldTransformR_arm_.translation_ = {0.5, 1.5, 0};
	worldTransformHammer_.translation_ = {0, 1.5f, 0};

	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;
	worldTransformHammer_.parent_ = &worldTransformBody_;

	// シングルトンインスタンスを取得
	input_ = Input::GetInstance();

	InitializeFloatingGimmick();
}

void Player::Update() { 
		// 基底クラスの更新
	BaseCharacter::Update();

	if (behaviorRequest_) {
		// 振るまいを変更する
		behavior_ = behaviorRequest_.value();
		// 各振るまいごとの初期化
		switch (behavior_) { 
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kJump:
			BehaviorJumpInitialize();
			break;
		}
		// 振るまいリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
		// 通常行動
	case Behavior::kRoot:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;
	}

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

	if (behavior_ == Behavior::kAttack) {
		models_[4]->Draw(worldTransformHammer_, viewProjection);
	}
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

void Player::BehaviorRootUpdate() 
{
	// ゲームパッドの状態を得る変数
	XINPUT_STATE joyState;

	// ゲームパッドが有効の場合if文が通る
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// 速さ
		const float speed = 0.3f;

		// 移動量
		velocity_ = {
		    (float)joyState.Gamepad.sThumbLX / SHRT_MAX * speed, 0.0f,
		    (float)joyState.Gamepad.sThumbLY / SHRT_MAX * speed};

		// カメラの角度から回転行列を計算する
		Matrix4x4 RotationMatrix = MakeRotateMatrix(viewProjection_->rotation_);

		// オフセットをカメラの回転に合わせて回転させる
		velocity_ = TransformNormal(velocity_, RotationMatrix);

		worldTransformBody_.rotation_.y = std::atan2(velocity_.x, velocity_.z);

		// 座標移動
		worldTransformBody_.translation_ = Add(worldTransformBody_.translation_, velocity_);
	}

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		if (joyState.Gamepad.wButtons == XINPUT_GAMEPAD_A) {
			behaviorRequest_ = Behavior::kAttack;
		}
	}

	// Bボタンを押したらジャンプ
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		if (joyState.Gamepad.wButtons == XINPUT_GAMEPAD_B) {
			behaviorRequest_ = Behavior::kJump;
		}
	}

	UpdateFloatingGimmick();
}

void Player::BehaviorAttackUpdate() 
{
	std::function<float(float)> easeInBack = [](float t) -> float {
		const float c1 = 1.70158f;
		const float c2 = c1 * 1.525f;

		return t * t * ((c2 + 1) * t - c2);
	};

	const float kMaxAttackAnim = 60.0f;

	if (time_ <= 65)
	{
		time_++;
	}

	AfterAttackCooldown_++;

	if (AfterAttackCooldown_ > 120)
	{
		behavior_ = Behavior::kRoot;
		behaviorRequest_ = Behavior::kRoot;
	}

	worldTransformHammer_.rotation_.x = easeInBack(time_/ kMaxAttackAnim);
	worldTransformL_arm_.rotation_.x = 3 + easeInBack(time_ / kMaxAttackAnim);
	worldTransformR_arm_.rotation_.x = 3 + easeInBack(time_ / kMaxAttackAnim);
}

void Player::BehaviorRootInitialize() 
{ 
	AfterAttackCooldown_ = 0;
	time_ = 0;
	worldTransformHammer_.rotation_.x = 0;
	worldTransformL_arm_.rotation_.x = 0;
	worldTransformR_arm_.rotation_.x = 0;
}

void Player::BehaviorAttackInitialize() 
{}

void Player::BehaviorJumpInitialize() 
{ 
	worldTransformBody_.translation_.y = 0;
	worldTransformL_arm_.rotation_.x = 0;
	worldTransformR_arm_.rotation_.x = 0;

	//　ジャンプ初速
	const float kJumpFirstSpeed = 1.0f;
	// ジャンプ初速を与える
	velocity_.y = kJumpFirstSpeed;
}

void Player::BehaviorJumpUpdate() 
{
	// 移動
	worldTransformBody_.translation_ = Add(worldTransformBody_.translation_, velocity_);
	// 重力加速度
	const float kGravityAcceleration = 0.05f;
	// 加速度ベクトル
	Vector3 accelerationVector = {0, -kGravityAcceleration, 0};
	// 加速する
	velocity_ = Add(velocity_, accelerationVector);

	// 着地
	if (worldTransformBody_.translation_.y <= 0.0f) {
		worldTransformBody_.translation_.y = 0;
		// ジャンプ終了
		behaviorRequest_ = Behavior::kRoot;
	}
}