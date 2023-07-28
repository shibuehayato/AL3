#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete model_;
	delete player_;
	delete debugCamera_;
	delete enemy_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("sample.png");

	// 3Dモデルデータの生成
	model_ = Model::Create();

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize( model_,textureHandle_);

	// 敵キャラの生成
	enemy_ = new Enemy();
	Vector3 position{10, 0, 20};
	// 敵キャラの初期化
	enemy_->Initialize(model_, position);

	// 敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(50, 50);

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する (アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
}

void GameScene::Update() {
	// 自キャラの更新
	player_->Update();

	// 敵キャラの更新
	enemy_->Update();

	CheckAllCollisions();

	// デバッグカメラの更新
	debugCamera_->Update();

   #ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = true;
	}
   #endif

	// カメラの処理
	if (isDebugCameraActive_) {
	    // デバッグカメラの更新
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		// デバッグカメラのプロジェクション行列
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
	        // ビュープロジェクション行列の更新と転送
		    viewProjection_.UpdateMatrix();
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	
	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 自キャラの描画
	player_->Draw(viewProjection_);
	// 敵キャラの描画
	enemy_->Draw(viewProjection_);
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollisions()
{
	float enemyBulletRadius = 0.5f;
	float playerRadius = 1.0f;
	float playerBulletRadius = 0.5f;
	float enemyRadius = 1.0f;

	// 判定対象AとBの座標
	Vector3 posA, posB;

	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

	#pragma region 自キャラと敵弾の当たり判定
	// 自キャラの座標
	posA = player_->GetWorldPosition();

	// 自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets) {
	        // 敵弾の座標
		    posB = bullet->GetWorldPosition();

			// 座標AとBの距離を求める
			Vector3 Distance = {
		        (posA.x - posB.x) * (posA.x - posB.x),
		        (posA.y - posB.y) * (posA.y - posB.y),
		        (posA.z - posB.z) * (posA.z - posB.z)
			};
			
			if (Distance.x + Distance.y + Distance.z <=
		        (playerRadius + enemyBulletRadius) * (playerRadius + enemyBulletRadius)) {
		        // 自キャラの衝突時コールバック関数を呼び出す
			    player_->OnCollision();
				// 敵弾の衝突時コールバック関数を呼び出す
			    bullet->OnCollision();
			}
	}
	#pragma endregion

	#pragma region 自弾と敵キャラの当たり判定
	posA = enemy_->GetWorldPosition();

	for (PlayerBullet* bullet : playerBullets) {
		    posB = bullet->GetWorldPosition();

			Vector3 Distance = {
		        (posA.x - posB.x) * (posA.x - posB.x), 
				(posA.y - posB.y) * (posA.y - posB.y),
		        (posA.z - posB.z) * (posA.z - posB.z)
			};

			if (Distance.x + Distance.y + Distance.z <= 
				(enemyRadius + playerBulletRadius) * (enemyRadius + playerBulletRadius)) {
			    enemy_->OnCollision();
			    bullet->OnCollision();
			}
	}
	#pragma endregion
	
	#pragma region 自弾と敵弾の当たり判定
	for (PlayerBullet* pBullet : playerBullets) {
		    posA = pBullet->GetWorldPosition();

			for (EnemyBullet* eBullet : enemyBullets) {
			    posB = eBullet->GetWorldPosition();

				Vector3 Distance = {
			        (posA.x - posB.x) * (posA.x - posB.x), 
					(posA.y - posB.y) * (posA.y - posB.y),
			        (posA.z - posB.z) * (posA.z - posB.z)
				};

				if (Distance.x + Distance.y + Distance.z <=
			        (playerBulletRadius + enemyBulletRadius) *
			            (playerBulletRadius + enemyBulletRadius)) {
				    pBullet->OnCollision();
				    eBullet->OnCollision();
				}
			}
	}
	#pragma endregion
}