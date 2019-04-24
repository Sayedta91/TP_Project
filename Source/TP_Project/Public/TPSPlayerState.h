
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TPSPlayerState.generated.h"


UCLASS()
class TP_PROJECT_API ATPSPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	UFUNCTION(Blueprintcallable, Category = "PlayerState")
	void AddScore(float ScoreDelta);
};
