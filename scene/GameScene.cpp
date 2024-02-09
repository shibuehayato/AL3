#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include <ImGuiManager.h>

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("sample.png");

	// 3Dモデルデータの生成
	model_.reset(Model::Create());

	TitleTexture_ = TextureManager::Load("scene/title.png");
	OperationTexture_ = TextureManager::Load("scene/operation.png");
	ClearTexture_ = TextureManager::Load("scene/clear.png");

	TitleSprite_ = std::make_unique<Sprite>();
	OperationSprite_ = std::make_unique<Sprite>();
	ClearSprite_ = std::make_unique<Sprite>();

	TitleSprite_.reset(Sprite::Create(TitleTexture_, {0, 0}));
	OperationSprite_.reset(Sprite::Create(OperationTexture_, {0, 0}));
	ClearSprite_.reset(Sprite::Create(ClearTexture_, {0, 0}));

	// ビュープロジェクションの初期化
	viewProjection_.translation_ = {0, 1, -10};
	viewProjection_.Initialize();

	// 自キャラの生成
	player_ = std::make_unique<Player>();
	// 3Dモデルの生成
	modelFighterHead_.reset(Model::CreateFromOBJ("float_Head", true));
	modelFighterBody_.reset(Model::CreateFromOBJ("float_Body", true));
	modelFighterL_arm_.reset(Model::CreateFromOBJ("float_L_arm", true));
	modelFighterR_arm_.reset(Model::CreateFromOBJ("float_R_arm", true));
	modelHammer_.reset(Model::CreateFromOBJ("hammer", true));
	// 自キャラモデル
	std::vector<Model*> playerModels = {
	    modelFighterBody_.get(), modelFighterHead_.get(), modelFighterL_arm_.get(),
	    modelFighterR_arm_.get(), modelHammer_.get()};
	// 自キャラの初期化
	player_->Initialize(playerModels);

	// 敵キャラの生成
	enemy_ = std::make_unique<Enemy>();
	// 3Dモデルの生成
	modelEnemyBody_.reset(Model::CreateFromOBJ("needle_Body", true));
	modelEnemyL_arm_.reset(Model::CreateFromOBJ("needle_L_arm", true));
	modelEnemyR_arm_.reset(Model::CreateFromOBJ("needle_R_arm", true));
	// 敵キャラモデル
	std::vector<Model*> enemyModels = {
	    modelEnemyBody_.get(), modelEnemyL_arm_.get(), modelEnemyR_arm_.get()};
	// 敵キャラの初期化
	enemy_->Initialize(enemyModels);

	// 3Dモデルの生成
	modelSkydome_.reset(Model::CreateFromOBJ("skydome", true));
	// 天球の生成
	skydome_ = std::make_unique<Skydome>();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_.get());

	// 3Dモデルの生成
	modelGround_.reset(Model::CreateFromOBJ("ground", true));
	// 地面の生成
	ground_ = std::make_unique<Ground>();
	// 地面の初期化
	ground_->Initialize(modelGround_.get());

	// デバッグカメラの生成
	debugCamera_ = std::make_unique<DebugCamera>(2000, 2000);

	scene_ = std::make_unique<Scene>();

	// 追従カメラの生成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();

		// 自キャラに追従カメラのビュープロジェクションをアドレス渡しする
	player_->SetViewProjection(&followCamera_->GetViewProjection());

	// 自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&player_->GetWorldTransform());

	//// 軸方向表示を有効にする
	//AxisIndicator::GetInstance()->SetVisible(true);
	//// 軸方向表示が参照するビュープロジェクションを指定する (アドレス渡し)
	//AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

}

void GameScene::Update() {

	scene_->Update();

	if (scene_->GetScene() == scene_->TITLE) {
		if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			if (Input::GetInstance()->GetJoystickStatePrevious(0, prevjoyState)) {
				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A &&
				    !(prevjoyState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
					isFade = true;
				}
			}
		}
		if (isFade == true) {
			fadeColor_.w -= 0.005f;
			TitleSprite_->SetColor(fadeColor_);
		}
		if (fadeColor_.w <= 0) {
			scene_->SetScene(scene_->OPERATION);
			fadeColor_.w = 1.0f;
			ClearSprite_->SetColor(fadeColor_);
			isFade = false;
		}
	}

	if (scene_->GetScene() == scene_->OPERATION) {
		if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			if (Input::GetInstance()->GetJoystickStatePrevious(0, prevjoyState)) {
				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A &&
				    !(prevjoyState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
					isFade = true;
					scene_->SetScene(scene_->GAME);
				}
			}
		}
	}

	//switch (scene) {
	//case GameScene::TITLE: // タイトルシーン
	//	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
	//		if (Input::GetInstance()->GetJoystickStatePrevious(0, prevjoyState)) {
	//			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A &&
	//			    !(prevjoyState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
	//				scene = OPERATION;
	//			}
	//		}
	//	}
	//	break;
	//case GameScene::OPERATION: // 操作説明
	//	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
	//		if (Input::GetInstance()->GetJoystickStatePrevious(0, prevjoyState)) {
	//			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A &&
	//			    !(prevjoyState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
	//				scene = GAME;
	//			}
	//		}
	//	}
	//	break;
	//case GameScene::GAME:

	if (scene_->GetScene() == scene_->GAME) {

		if (isFade == true) {
			fadeColor_.w -= 0.005f;
			OperationSprite_->SetColor(fadeColor_);
		}
		if (fadeColor_.w <= 0) {
			fadeColor_.w = 1.0f;
			isFade = false;
		}

		// 自キャラの更新
		player_->Update();

		// 敵キャラの更新
		enemy_->Update();

		// 天球の更新
		skydome_->Update();

		// 地面の更新
		ground_->Update();

		// デバッグカメラの更新
		debugCamera_->Update();

#ifdef _DEBUG
			if (input_->TriggerKey(DIK_RETURN)) {
				isDebugCameraActive_ = true;
			}
#endif

			// カメラの処理
			if (isDebugCameraActive_) {
				debugCamera_->Update();
				viewProjection_.matView = debugCamera_->GetViewProjection().matView;
				viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
				// ビュープロジェクション行列の転送
				viewProjection_.TransferMatrix();
			} else {
				// 追従カメラの更新
				followCamera_->Update();
				viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
				viewProjection_.matView = followCamera_->GetViewProjection().matView;
				viewProjection_.TransferMatrix();
			}

			if (enemy_->GetIsDead() == true) {
				scene_->SetScene(scene_->CLEAR);
			}
			CheckCollision();
	}

		//if (enemy_->GetIsDead() == true) {
		//	scene = CLEAR;
		//}

	/*	if (enemy_->GetIsDead() == true) {
			deathTimer_--;
		}
		if (deathTimer_ <= 0) {
			scene = CLEAR;
		}*/

		//break;
	//case GameScene::CLEAR:*/

	if (scene_->GetScene() == scene_->CLEAR) {
		    player_->Reset();
		    enemy_->Reset();
		    deathTimer_ = 60;

		   if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			   if (Input::GetInstance()->GetJoystickStatePrevious(0, prevjoyState)) {
				   if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A &&
				    !(prevjoyState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
					isFade = true;
				   }
			   }
		   }

		    if (isFade == true) {
			    fadeColor_.w -= 0.005f;
			    ClearSprite_->SetColor(fadeColor_);
			    Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
			    TitleSprite_->SetColor(color);
		    }
		    if (fadeColor_.w <= 0) {
			    fadeColor_.w = 1.0f;
			    scene_->SetScene(scene_->TITLE);
			    OperationSprite_->SetColor(fadeColor_);
			    isFade = false;
		    }
	}

		/*if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			if (Input::GetInstance()->GetJoystickStatePrevious(0, prevjoyState)) {
				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A &&
				    !(prevjoyState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
					scene = TITLE;
				}
			}
		}
		break;*/
	//}
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

	//if (scene == GAME) {
	if (scene_->GetScene() == scene_->GAME) {
		// 自キャラの描画
		player_->Draw(viewProjection_);

		// 敵キャラの描画
		enemy_->Draw(viewProjection_);

		// 天球の描画
		skydome_->Draw(viewProjection_);

		// 地面の描画
		ground_->Draw(viewProjection_);
	}
	//}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	
	OperationSprite_->Draw();
	TitleSprite_->Draw();
	
	if (scene_->GetScene()==scene_->CLEAR) {
	ClearSprite_->Draw();
	
	}

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckCollision()
{
	float enemyRadius = 1.0f;
	float hammerRadius = 1.0f;

	// 判定対象AとBの座標
	Vector3 posA, posB;

	#pragma region 敵とハンマーの当たり判定
	// 敵の座標
	posA = enemy_->GetWorldPosition();
	posB = player_->GetWorldHammerPosition();

	// 座標AとBの距離を求める
	Vector3 Distance = {
	    (posA.x - posB.x) * (posA.x - posB.x), (posA.y - posB.y) * (posA.y - posB.y),
	    (posA.z - posB.z) * (posA.z - posB.z)};

	if (Distance.x + Distance.y + Distance.z <=
		(enemyRadius + hammerRadius) * (enemyRadius + hammerRadius))
	{
		// 敵の衝突時コールバック関数を呼び出す
		enemy_->OnCollision();
	}
}