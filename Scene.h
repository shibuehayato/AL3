#pragma once
#include "Input.h"

class Enemy;

class Scene {
private:
	Input* input_ = nullptr;

	// ゲームパッドの状態を得る変数
	XINPUT_STATE joyState;
	XINPUT_STATE prevjoyState;

	Enemy* enemy_;

public:
	void Initialize();
	void Update();

	// シーン切り替え
	enum Scene2 { TITLE, OPERATION, GAME, CLEAR };
	Scene2 scene;

	Scene2 GetScene() { return scene; }
	Scene2 SetScene(Scene2 scene2) { return scene=scene2; }

};
