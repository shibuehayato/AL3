#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player.h"
#include <memory>
#include "Skydome.h"
#include "Ground.h"
#include "DebugCamera.h"
#include "WorldTransform.h"
#include "FollowCamera.h"
#include "Enemy.h"
#include "Scene.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

    /// <summary>
    /// 衝突判定と応答
    /// </summary>
	void CheckCollision();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	//3Dモデルデータ
	std::unique_ptr<Model> model_;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 自キャラ
	std::unique_ptr<Player> player_;
	// 3Dモデルの生成
	std::unique_ptr<Model> modelFighterHead_;
	std::unique_ptr<Model> modelFighterBody_;
	std::unique_ptr<Model> modelFighterL_arm_;
	std::unique_ptr<Model> modelFighterR_arm_;

	// 敵キャラ
	std::unique_ptr<Enemy> enemy_;
	// 3Dモデルの生成
	std::unique_ptr<Model> modelEnemyBody_;
	std::unique_ptr<Model> modelEnemyL_arm_;
	std::unique_ptr<Model> modelEnemyR_arm_;

	// 3Dモデル
	std::unique_ptr<Model> modelHammer_;

	// 天球
	std::unique_ptr<Skydome> skydome_;

	// 3Dモデルの生成
	std::unique_ptr<Model> modelSkydome_;

	// 地面
	std::unique_ptr<Ground> ground_;

	// 3Dモデルの生成
	std::unique_ptr<Model> modelGround_;

	// デバッグカメラ
	std::unique_ptr<DebugCamera> debugCamera_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// 追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;

	std::unique_ptr<Scene> scene_;

	// シーン切り替え
	/*enum Scene3 { TITLE, OPERATION, GAME, CLEAR };
	Scene3 scene3 = TITLE;*/
	// テクスチャハンドル
	uint32_t TitleTexture_ = 0;
	uint32_t OperationTexture_ = 0;
	uint32_t ClearTexture_ = 0;
	// シーン画面のスプライト
	std::unique_ptr<Sprite> TitleSprite_ = nullptr;
	std::unique_ptr<Sprite> OperationSprite_ = nullptr;
	std::unique_ptr<Sprite> ClearSprite_ = nullptr;

    // ゲームパッドの状態を得る変数
	XINPUT_STATE joyState;
	XINPUT_STATE prevjoyState;

	int deathTimer_ = 60;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
