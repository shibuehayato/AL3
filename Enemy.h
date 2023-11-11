#pragma once
#include "BaseCharacter.h"

class Enemy : public BaseCharacter {
public:
	void Initialize(const std::vector<Model*>& models) override;
	void Update();
	void Draw(ViewProjection viewProjection);
};