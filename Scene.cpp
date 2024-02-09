#include "Scene.h"
#include "Enemy.h"

void Scene::Initialize() { 
	scene = TITLE;
	input_ = Input::GetInstance();
}

void Scene::Update() {
	switch (scene) {
	case Scene::TITLE:
		break;
	case Scene::OPERATION:
		break;
	case Scene::GAME:
		//if (enemy_->GetIsDead() == true) {
			//
		//}
		break;
	case Scene::CLEAR:
		/*if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			if (Input::GetInstance()->GetJoystickStatePrevious(0, prevjoyState)) {
				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A &&
				    !(prevjoyState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
					scene = TITLE;
				}
			}
		}*/
		break;
	}
}
