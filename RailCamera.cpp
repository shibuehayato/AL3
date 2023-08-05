#include "RailCamera.h"
#include "Matrix.h"
#include "ImGuiManager.h"

void RailCamera::Initialize(const Vector3& pos, const Vector3& rotation) {
	// ワールドトランスフォームの初期設定
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.translation_ = pos;
	worldTransform_.rotation_ = rotation;

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void RailCamera::Update() { 
	worldTransform_.rotation_.y += 0.001f;
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    {1.0f, 1.0f, 1.0f}, worldTransform_.rotation_, worldTransform_.translation_);

	// カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);
	                         
	// カメラの座標を画面表示する処理
	ImGui::Begin("Camera");
	ImGui::DragFloat3("CameraTranslation", &worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("CameraRotate", &worldTransform_.rotation_.x, 0.01f);
	ImGui::End();
}