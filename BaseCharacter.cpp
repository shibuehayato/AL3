#include "BaseCharacter.h"

void BaseCharacter::Initialize(const std::vector<Model*>& models) 
{ 
	models_ = models;
	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();
	worldTransformHammer_.Initialize();
}

void BaseCharacter::Update() 
{ 
	worldTransformBody_.UpdateMatrix(); 
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
	worldTransformHammer_.UpdateMatrix();
}

void BaseCharacter::Draw(const ViewProjection& viewProjection)
{
	// モデル描画
	for (Model* model : models_)
	{
		model->Draw(worldTransformBody_, viewProjection);
	}
}