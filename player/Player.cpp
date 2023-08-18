#include "Player.h"
#include <cassert>
#include "Matrix.h"
#include "ImGuiManager.h"
#include "TextureManager.h"

Player::Player() {}

Player::~Player() { 
	// bullet_の解放
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
	delete sprite2DReticle_;
}

void Player::Initialize(Model *model,uint32_t textureHandle, Vector3 pos) {

	assert(model);

	model_ = model;

	textureHandle_ = textureHandle;

	worldTransform_.Initialize();

	// 3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	// レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("reticle.png");

	// スプライト生成
	sprite2DReticle_ =
	    Sprite::Create(textureReticle, {640.0f, 360.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	worldTransform_.translation_ = Add(worldTransform_.translation_, pos);
}

void Player::Update(ViewProjection viewProjection) {

	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	// 押した方向で移動ベクトルを変更 (左右)
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}
	// 押した方向で移動ベクトルを変更 (上下)
	if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}

	worldTransform_.translation_ = VectorAdd(worldTransform_.translation_, move);

	float moves[3] = {
		worldTransform_.translation_.x, worldTransform_.translation_.y,
	worldTransform_.translation_.z};

	// キャラクターの座標を画面表示する処理
	ImGui::Begin("Player");
	ImGui::InputFloat3("InputFloat3", moves);
	ImGui::SliderFloat3("SliderFloat3", moves, 0.0f, 30.0f);
	ImGui::Text("DebugCamera ENTER");
	ImGui::End();
	
	worldTransform_.translation_.x = moves[0];
	worldTransform_.translation_.y = moves[1];
	worldTransform_.translation_.z = moves[2];

	GetMousePos(viewProjection);

	// 旋回処理
	Rotate();

	// キャラクター攻撃処理
	Attack();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	// 移動限界座標
	const float kMoveLimitX = 20;
	const float kMoveLimitY = 10;
	// 範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	worldTransform_.UpdateMatrix();

	// 自機のワールド座標から3Dレティクルのワールド座標を計算
	{
		// 自機から3Dレティクルへの距離
		const float kDistancePlayerTo3DReticle = 50.0f;
		// 自機から3Dレティクルへのオフセット(z+向き)
		Vector3 offset = {0, 0, 1.0f};
		// 自機のワールド行列の回転を反映
		offset = TransformNormal(offset, worldTransform_.matWorld_);
		// ベクトルの長さを整える
		offset = Normalize(offset);
		offset.x *= kDistancePlayerTo3DReticle;
		offset.y *= kDistancePlayerTo3DReticle;
		offset.z *= kDistancePlayerTo3DReticle;

		// 3Dレティクルの座標を設定
		worldTransform3DReticle_.translation_.x = offset.x + worldTransform_.matWorld_.m[3][0];
		worldTransform3DReticle_.translation_.y = offset.y + worldTransform_.matWorld_.m[3][1];
		worldTransform3DReticle_.translation_.z = offset.z + worldTransform_.matWorld_.m[3][2];

		worldTransform3DReticle_.UpdateMatrix();
		worldTransform3DReticle_.TransferMatrix();
	}

	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	{
		Vector3 positionReticle;
		positionReticle.x = worldTransform3DReticle_.matWorld_.m[3][0];
		positionReticle.y = worldTransform3DReticle_.matWorld_.m[3][1];
		positionReticle.z = worldTransform3DReticle_.matWorld_.m[3][2];

		// ビューポート行列
		Matrix4x4 matViewport =
		    MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matViewprojectionViewport =
		    Multiply(viewProjection.matView, Multiply(viewProjection.matProjection, matViewport));

		// ワールド→スクリーン座標変換　(ここから3Dから2Dになる)
		positionReticle = Transform(positionReticle, matViewprojectionViewport);

		// スプライトのレティクルに座標設定
		sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
	}

}

void Player::Rotate()
{
	// 回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y += kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	}
}

void Player::Attack() {
	if (input_->PushKey(DIK_SPACE)) {

		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		// 速度ベクトルを自機の向きに合わせて回転させる
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, GetWorldPosition(), velocity);

		// 弾を登録
		bullets_.push_back(newBullet);

		velocity.z = worldTransform3DReticle_.translation_.z - worldTransform_.translation_.z;
		velocity = Normalize(velocity);
		velocity.x *= kBulletSpeed;
		velocity.y *= kBulletSpeed;
		velocity.z *= kBulletSpeed;
	}
}

Vector3 Player::GetWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得　(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	
	return worldPos;
}

void Player::OnCollision(){}

void Player::SetParnet(const WorldTransform* parent)
{ 
	// 親関係を結ぶ
	worldTransform_.parent_ = parent; 
}

void Player::Draw(ViewProjection viewProjection) { 
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// 3Dレティクルを描画
	model_->Draw(worldTransform3DReticle_, viewProjection);

	// 弾描画
	for (PlayerBullet* bullet : bullets_) {

		bullet->Draw(viewProjection);
	}
}

void Player::DrawUI() 
{ 
	sprite2DReticle_->Draw(); 
}

void Player::GetMousePos(ViewProjection viewProjection)
{
	POINT mousePosition;

	GetCursorPos(&mousePosition);

	HWND hwnd = WinApp::GetInstance()->GetHwnd();

	ScreenToClient(hwnd, &mousePosition);

	sprite2DReticle_->SetPosition({(float)mousePosition.x, (float)mousePosition.y});

	// ビュープロジェクションビューポート合成行列
	Matrix4x4 matVPV = Multiply(
	    viewProjection.matView,
	    Multiply(
	        viewProjection.matProjection,
	        MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1)));
	// 合成行列の逆行列を計算する
	Matrix4x4 matInverseVPV = Inverse(matVPV);

	// スクリーン座標
	Vector3 posNear = Vector3(sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y, 0);
	Vector3 posFar = Vector3(sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y, 1);

	// スクリーン座標系からワールド座標系へ
	posNear = Transform(posNear, matInverseVPV);
	posFar = Transform(posFar, matInverseVPV);

	// マウスレイの方向
	Vector3 mouseDirection = Add(posFar, posNear);
	mouseDirection = Normalize(mouseDirection);
	// カメラから照準オブジェクトの距離
	const float kDistanceTestObject = 50.0f;
	worldTransform3DReticle_.translation_.x = posNear.x + mouseDirection.x * kDistanceTestObject;
	worldTransform3DReticle_.translation_.y = posNear.y + mouseDirection.y * kDistanceTestObject;
	worldTransform3DReticle_.translation_.z = posNear.z + mouseDirection.z * kDistanceTestObject;

	worldTransform3DReticle_.UpdateMatrix();
	worldTransform3DReticle_.TransferMatrix();

	ImGui::Begin("Player");
	ImGui::Text("Near:(%+.2f,%+.2f,%.2f)", posNear.x, posNear.y, posNear.z);
	ImGui::Text("Far:(%+.2f,%+.2f,%.2f)", posFar.x, posFar.y, posFar.z);
	ImGui::Text(
	    "3DRetixle:(%+.2f,%+.2f,%.2f)", worldTransform3DReticle_.translation_.x,
	    worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
	ImGui::End();
}