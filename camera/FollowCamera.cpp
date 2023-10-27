#include "FollowCamera.h"
#include "MyMath.h"
#include "Input.h"

void FollowCamera::Initialize()
{ 
	viewProjection_.Initialize();
}

void FollowCamera::Update() {
	// 追従対象がいれば
	if (target_) {
		// 追従対象からカメラまでのオフセット
		Vector3 offset = {0.0f, 2.0f, -10.0f};

		// カメラの角度から回転行列を計算する
		Matrix4x4 RotationMatrix = MakeRotateMatrix(viewProjection_.rotation_);

		// オフセットをカメラの回転に合わせて回転させる
		offset = TransformNormal(offset, RotationMatrix);

		// 座標をコピーしてオフセット分ずらす
		viewProjection_.translation_ = Add(target_->translation_, offset);
	}

	// ゲームパッドの状態を得る変数
	XINPUT_STATE joyState;

	// ゲームパッドが有効の場合if文が通る
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// 回転する速さ
		const float speed = 0.1f;

		viewProjection_.rotation_.y += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * speed;

	}

	// ビュー行列の更新
	viewProjection_.UpdateMatrix();
}