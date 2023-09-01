#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include <fstream>

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete model_;
	delete player_;
	delete debugCamera_;
	delete skydome_;
	delete modelSkydome_;
	delete railCamera_;
	delete sprite_;

	for (Enemy* enemy : enemys_)
	{
		delete enemy;
	}

	for (EnemyBullet* bullet : enemyBullets_)
	{
		delete bullet;
	}
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("sample.png");

	textureHandle1_ = TextureManager::Load("Sprit.png");
	sprite_ = Sprite::Create(textureHandle1_, {0, 0});

	// レティクルのテクスチャ
	TextureManager::Load("reticle.png");

	// 3Dモデルデータの生成
	model_ = Model::Create();

	worldTransform_.Initialize();

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// 自キャラの生成
	player_ = new Player();
	Vector3 playerPosition(0, 0, 30);
	// 自キャラの初期化
	player_->Initialize( model_,textureHandle_, playerPosition);

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_);

	railCamera_ = new RailCamera();
	railCamera_->Initialize({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f});

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(50, 50);

	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParnet(&railCamera_->GetWorldTransform());

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する (アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	LoadEnemyPopData();
}

void GameScene::Update() {
	// 自キャラの更新
	player_->Update(viewProjection_);

	// 敵キャラの更新
	UpdateEnemyPopCommands();
	for (Enemy* enemy : enemys_)
	{
		enemy->Update();
	}
	
	// 敵弾の更新
	for (EnemyBullet* bullet : enemyBullets_)
	{
		bullet->Update();
	}

	// デスフラグの立った敵を削除
	enemys_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	// デスフラグの立った弾を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	CheckAllCollisions();

	// 天球の更新
	skydome_->Update();
	
	// デバッグカメラの更新
	debugCamera_->Update();
	
   #ifdef _DEBUG
	if (input_->TriggerKey(DIK_RETURN)) {
		if (isDebugCameraActive_ == false)
		{
			isDebugCameraActive_ = true;
		} else {
			isDebugCameraActive_ = false;
		}
	}
   #endif

	// カメラの処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
	    // デバッグカメラの更新
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		// デバッグカメラのプロジェクション行列
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
		railCamera_->Update();
		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
	        // ビュープロジェクション行列の更新と転送
		    viewProjection_.TransferMatrix();
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
	for (Enemy* enemy : enemys_)
	{
		    enemy->Draw(viewProjection_);
	}
	for (EnemyBullet* bullet : enemyBullets_)
	{
		    bullet->Draw(viewProjection_);
	}

	// 天球の描画
	skydome_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	player_->DrawUI();
	
	if (railCamera_->GetWorldTransform().translation_.z >= 10.0f) {
		    sprite_->Draw();
	}

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet)
{
	// リストに登録する
	enemyBullets_.push_back(enemyBullet);
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

	#pragma region 自キャラと敵弾の当たり判定
	// 自キャラの座標
	posA = player_->GetWorldPosition();

	// 自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets_) {
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
	for (Enemy* enemy : enemys_) 
	{
		    for (PlayerBullet* bullet : playerBullets) {
			    posA = enemy->GetWorldPosition();
			    posB = bullet->GetWorldPosition();

			    Vector3 Distance = {
			        (posA.x - posB.x) * (posA.x - posB.x), (posA.y - posB.y) * (posA.y - posB.y),
			        (posA.z - posB.z) * (posA.z - posB.z)};

			    if (Distance.x + Distance.y + Distance.z <=
			        (enemyRadius + playerBulletRadius) * (enemyRadius + playerBulletRadius)) {
				    enemy->OnCollision();
				    bullet->OnCollision();
			    }
		    }
	}
	#pragma endregion
	
	#pragma region 自弾と敵弾の当たり判定
	for (PlayerBullet* pBullet : playerBullets) {
		    posA = pBullet->GetWorldPosition();

			for (EnemyBullet* eBullet : enemyBullets_) {
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

void GameScene::LoadEnemyPopData()
{
	// ファイルを開く
	std::ifstream file;
	file.open("Resources/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::EnemyPop(Vector3 position)
{
	// 敵キャラの生成
	enemy_ = new Enemy();
	// 敵キャラの初期化
	enemy_->Initialize(model_, position);
	// 敵キャラにゲームシーンを渡す
	enemy_->SetGameScene(this);
	// 敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);
	enemys_.push_back(enemy_);
}

void GameScene::UpdateEnemyPopCommands()
{
	// 待機処理
	if (isWaiting_) {
		waitTimer_--;
		    if (waitTimer_ <= 0) {
				// 待機完了
			    isWaiting_ = false;
			}
		    return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopCommands,line)) {
		    // 1行分の文字列をストリームに変換して解析しやすくする
		    std::istringstream line_stream(line);

			std::string word;
			// ,区切りで行の先頭文字列を取得
		    getline(line_stream, word, ',');

			// "//"から始まる行はコメント
		    if (word.find("//") == 0) {
				// コメント行を飛ばす
			    continue;
			}

			// POPコマンド
			if (word.find("POP") == 0) {
				// x座標
			    getline(line_stream, word, ',');
			    float x = (float)std::atof(word.c_str());

				// y座標
			    getline(line_stream, word, ',');
			    float y = (float)std::atof(word.c_str());

				// z座標
			    getline(line_stream, word, ',');
			    float z = (float)std::atof(word.c_str());

				// 敵を発生させる
			    EnemyPop(Vector3(x, y, z));
			}

			// WAITコマンド
			else if (word.find("WAIT") == 0) {
			    getline(line_stream, word, ',');

				// 待ち時間
			    int32_t waitTime = atoi(word.c_str());

				// 待機開始
			    isWaiting_ = true;
			    waitTimer_ = waitTime;

				// コマンドループを抜ける
			    break;
			}
	}
}