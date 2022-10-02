// Shoot Them up Game, All rights reserved.


#include "STUGameModeBase.h"
#include "Player/STUBaseCharacter.h"
#include "Player/STUPlayerController.h"
#include "AI/STUAIController.h"
#include "AI/STUAICharacter.h"
#include "UI/STUGameHUD.h"
#include "AIController.h"
#include "Player/STUPlayerState.h"
#include "Components/STURespawnComponent.h"
#include "UI/STUPlayerHUDWidget.h"
#include "EngineUtils.h"
#include "STUGameInstance.h"
#include "Components/AudioComponent.h"
#include "Player/STUPlayerCharacter.h"
#include "Weapon/STURifleWeapon.h"
#include "Components/STUWeaponComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSTUGameModeBase, All, All)

constexpr static int32 MinRoundTimeForRespawn = 10;

ASTUGameModeBase::ASTUGameModeBase()
{
    DefaultPawnClass = ASTUBaseCharacter::StaticClass();
    PlayerControllerClass = ASTUPlayerController::StaticClass();
    HUDClass = ASTUGameHUD::StaticClass();
    PlayerStateClass = ASTUPlayerState::StaticClass();
}

void ASTUGameModeBase::StartPlay() 
{
    Super::StartPlay();

    SpawnBots();
    CreateTeamsInfo();

    CurrentRound = 1;
    StartRound();

    SetMatchState(ESTUMatchState::InProgress);
}

void ASTUGameModeBase::Killed(AController* KillerController, AController* VictimController)
{
    const auto KillerPlayerState = KillerController ? Cast<ASTUPlayerState>(KillerController->PlayerState) : nullptr;
    const auto VictimPlayerState = VictimController ? Cast<ASTUPlayerState>(VictimController->PlayerState) : nullptr;

    if (KillerPlayerState)
    {
        KillerPlayerState->AddKill();       
    }
    if (VictimPlayerState) 
    {
        VictimPlayerState->AddDeath();
    }

    StartRespawn(VictimController);
}

void ASTUGameModeBase::RespawnRequst(AController* Controller) 
{
    ResetOnePlayer(Controller);
}

UClass* ASTUGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
    UClass* ReturnVal = nullptr;

    ReturnVal = Super::GetDefaultPawnClassForController_Implementation(InController);
    if (InController != nullptr && InController->IsA<AAIController>())
    {
        ReturnVal = AIPawnClass;
    }

    return ReturnVal;
}

void ASTUGameModeBase::SpawnBots() 
{
    if (GetWorld())
    {
        for (int i = 0; i < GameData.PlayersNum - 1; i++)
        {
            FActorSpawnParameters SpawnInfo;
            SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            const auto STUAIController = GetWorld()->SpawnActor<AAIController>(AIControllerClass, SpawnInfo);
            RestartPlayer(STUAIController);
        }
    }
}

void ASTUGameModeBase::StartRound() 
{
    RoundCountDown = GameData.RoundTime;
    GetWorldTimerManager().SetTimer(GameRoundTimerHandle, this, &ASTUGameModeBase::GameTimerUpdate, 1.0f, true);
}

void ASTUGameModeBase::GameTimerUpdate() 
{
    UE_LOG(LogSTUGameModeBase, Display, TEXT("Time: %i / Round: %i/%i"), RoundCountDown, CurrentRound, GameData.RoundsNum);

    //const auto TimerRate = GetWorldTimerManager().GetTimerRate(GameRoundTimerHandle);
    if (--RoundCountDown == 0) 
    {
        GetWorldTimerManager().ClearTimer(GameRoundTimerHandle);

        if (CurrentRound + 1 <= GameData.RoundsNum)
        {
            ++CurrentRound;
            ResetPlayers();
            StartRound();
        }
        else
        {            
            GameOver();
        }
    }
}

void ASTUGameModeBase::ResetPlayers() 
{
    if (GetWorld())
    {
        StopRifleFire();

        for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
        {
            ResetOnePlayer(It->Get());
        }
    }
}
void ASTUGameModeBase::ResetOnePlayer(AController* Controller) 
{
    if (Controller && Controller->GetPawn())
    {
        Controller->GetPawn()->Reset();
    }

    RestartPlayer(Controller);
    SetPlayerColor(Controller); 
}

void ASTUGameModeBase::CreateTeamsInfo()
{
    if (GetWorld())
    {
        int32 TeamID = 1;
        for (auto It = GetWorld()->GetControllerIterator(); It; It++)
        {
            const auto Controller = It->Get();
            if (Controller)
            {
                const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
                if (PlayerState)
                {
                    PlayerState->SetTeamId(TeamID);
                    PlayerState->SetTeamColor(DetermineColorByTeamID(TeamID));
                    PlayerState->SetPlayerName(Controller->IsPlayerController() ? "Player" : "Bot");
                    SetPlayerColor(Controller);
                    TeamID = TeamID == 1 ? 2 : 1;
                }
            }
        }
    }
}

FLinearColor ASTUGameModeBase::DetermineColorByTeamID(int32 TeamID) const 
{
    FLinearColor ReturnVal;
    if (TeamID - 1 < GameData.TeamColors.Num())
    {
        ReturnVal = GameData.TeamColors[TeamID - 1];
    }
    else
    {
        UE_LOG(LogSTUGameModeBase, Warning, TEXT("No color for team id: %i, set to default: %s"), TeamID,
            *GameData.DefaultTeamColor.ToString());
        ReturnVal = GameData.DefaultTeamColor;
    }

    return ReturnVal;
}

void ASTUGameModeBase::SetPlayerColor(AController* Controller) 
{
    if (Controller)
    {
        const auto Character = Cast<ASTUBaseCharacter>(Controller->GetPawn());
        if (Character)
        {
            const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
            if (PlayerState)
            {
                Character->SetPlayerColor(PlayerState->GetTeamColor());
            }
        }
    }
}

void ASTUGameModeBase::LogPlayerInfo() 
{
    if (GetWorld())
    {
        for (auto It = GetWorld()->GetControllerIterator(); It; It++)
        {
            const auto Controller = It->Get();
            if (Controller)
            {
                const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
                if (PlayerState)
                {
                    PlayerState->LogInfo();
                }
            }
        }
    }
}

void ASTUGameModeBase::StartRespawn(AController* Controller) 
{
    bool RespawnAvaible = RoundCountDown > MinRoundTimeForRespawn + GameData.RespawnTime;

    if (RespawnAvaible)
    {
        const auto RespawnComponent = USTUPlayerHUDWidget::GetComponent<USTURespawnComponent>(Controller);
        if (RespawnComponent)
        {
            RespawnComponent->Respawn(GameData.RespawnTime);
        }
    }
    
}

void ASTUGameModeBase::GameOver() 
{
    StopRifleFire();

    UE_LOG(LogSTUGameModeBase, Display, TEXT("====== GAME OVER ======"));
    LogPlayerInfo();

    for (auto Pawn : TActorRange<APawn>(GetWorld()))
    {
        if (Pawn)
        {
            Pawn->TurnOff();
            Pawn->DisableInput(nullptr);
        }
    }

    SetMatchState(ESTUMatchState::GameOver);
}

void ASTUGameModeBase::SetMatchState(ESTUMatchState State) 
{
    if (MatchState != State)
    {
        MatchState = State;
        OnMatchStateChange.Broadcast(MatchState);
    }
}

bool ASTUGameModeBase::SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate) 
{
    bool PauseSet = Super::SetPause(PC, CanUnpauseDelegate);

    if (PauseSet)
    {
        SetMatchState(ESTUMatchState::Pause);
    }
    return PauseSet;
}

bool ASTUGameModeBase::ClearPause()
{
    bool PauseCleared = Super::ClearPause();

    if (PauseCleared == true)
    {
        SetMatchState(ESTUMatchState::InProgress);
    }

    return PauseCleared;
}

void ASTUGameModeBase::StopRifleFire() 
{
    for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        AController* Controller = (It->Get());

        if (Controller)
        {
            if (ASTUPlayerController* PlayerController = Cast<ASTUPlayerController>(Controller))
            {
                if (ASTUPlayerCharacter* PlayerCharacter = Cast<ASTUPlayerCharacter>(PlayerController->GetCharacter()))
                {
                    if (USTUWeaponComponent* WeaponComponent = Cast<USTUWeaponComponent>(PlayerCharacter->GetWeaponComponent()))
                    {
                        if (ASTURifleWeapon* RifleWeapon = Cast<ASTURifleWeapon>(WeaponComponent->GetCurrentWeapon()))
                        {
                            RifleWeapon->StopFire();
                        }
                    }
                }
            }
            else if (ASTUAIController* AIController = Cast<ASTUAIController>(Controller))
            {
                if (ASTUAICharacter* AICharacter = Cast<ASTUAICharacter>(AIController->GetCharacter()))
                {
                    if (USTUWeaponComponent* WeaponComponent = Cast<USTUWeaponComponent>(AICharacter->GetWeaponComponent()))
                    {
                        if (ASTURifleWeapon* RifleWeapon = Cast<ASTURifleWeapon>(WeaponComponent->GetCurrentWeapon()))
                        {
                            RifleWeapon->StopFire();
                        }
                    }
                }
            }
        }
    }  
}