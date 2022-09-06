// Shoot Them up Game, All rights reserved.

#include "UI/STUGameOverWidget.h"
#include "STUGameModeBase.h"
#include "Player/STUPlayerState.h"
#include "UI/STUPlayerStatRowWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "STUGameInstance.h"

void USTUGameOverWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GetWorld())
    {
        const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());

        if (GameMode)
        {
            GameMode->OnMatchStateChange.AddUObject(this, &USTUGameOverWidget::OnMatchStateChanged);
        }
    }

    if (ResetLevelButton)
    {
        ResetLevelButton->OnClicked.AddDynamic(this, &USTUGameOverWidget::OnResetLevel);
    }
}

void USTUGameOverWidget::OnMatchStateChanged(ESTUMatchState MatchState) 
{
    if (MatchState == ESTUMatchState::GameOver)
    {
        UpdatePlayersStat();
    }
}

void USTUGameOverWidget::UpdatePlayersStat()
{
    if (GetWorld() && PlayerStatBox)
    {
        PlayerStatBox->ClearChildren();

        for (auto It = GetWorld()->GetControllerIterator(); It; It++)
        {
            const auto Controller = It->Get();
            if (Controller)
            {
                const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
                if (PlayerState)
                {
                    const auto PlayerStatRowWidget = CreateWidget<USTUPlayerStatRowWidget>(GetWorld(), PlayerStatRowWidgetClass);
                    if (PlayerStatRowWidget)
                    {
                        PlayerStatRowWidget->SetPlayerName(FText::FromString(PlayerState->GetPlayerName()));
                        PlayerStatRowWidget->SetKills(FText::FromString(FString::FromInt(PlayerState->GetKillsNum())));
                        PlayerStatRowWidget->SetDeaths(FText::FromString(FString::FromInt(PlayerState->GetDeathsNum())));
                        PlayerStatRowWidget->SetTeam(FText::FromString(FString::FromInt(PlayerState->GetTeamID())));
                        PlayerStatRowWidget->SetPlayerIndicatorVisibility(Controller->IsPlayerController());

                        PlayerStatBox->AddChild(PlayerStatRowWidget);
                    }
                }
            }
        }
    }
}

void USTUGameOverWidget::OnResetLevel() 
{
    const FString CurrentLevelname = UGameplayStatics::GetCurrentLevelName(this);
    UGameplayStatics::OpenLevel(this, FName(CurrentLevelname));
}