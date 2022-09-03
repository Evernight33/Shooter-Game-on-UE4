// Shoot Them up Game, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "STUPlayerState.generated.h"

UCLASS()
class SHOOTTHEMUP_API ASTUPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
    void SetTeamId(int32 ID) { TeamId = ID; }
    int32 GetTeamID() const { return TeamId; }

    void SetTeamColor(const FLinearColor& Color) { TeamColor = Color; }
    FLinearColor GetTeamColor() const { return TeamColor; }

    void AddKill() { ++KillsNum; }
    void AddDeath() { ++DeathsNum; }

    void LogInfo() const;
    int32 GetKillsNum() const;
    int32 GetDeathsNum() const;

private:
    int32 TeamId;
    FLinearColor TeamColor;

    int32 KillsNum = 0;
    int32 DeathsNum = 0;
};
