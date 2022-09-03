// Shoot Them up Game, All rights reserved.


#include "UI/STUGameHUD.h"
#include "Engine/Canvas.h"
#include "Blueprint/UserWidget.h"
#include "STUGameModeBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogSTUGameHUD, All, All);

void ASTUGameHUD::DrawHUD() 
{
    Super::DrawHUD();

    //DrawCrossHair();
}

void ASTUGameHUD::BeginPlay()
{
    Super::BeginPlay();

    GameWidgets.Add(ESTUMatchState::InProgress, CreateWidget<UUserWidget>(GetWorld(), PlayerHUDWidgetClass));
    GameWidgets.Add(ESTUMatchState::Pause, CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass));
    GameWidgets.Add(ESTUMatchState::GameOver, CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass));

    for (auto GameWidgetPair : GameWidgets)
    {
        const auto GameWidget = GameWidgetPair.Value;
        if (GameWidget)
        {
            GameWidget->AddToViewport();
            GameWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    if (GetWorld())
    {
        const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->OnMatchStateChange.AddUObject(this, &ASTUGameHUD::OnMatchStateChange);
        }
    }
}

void ASTUGameHUD::DrawCrossHair() // draws green aim
{
    int32 SizeX = Canvas->SizeX * 0.5f;
    int32 SizeY = Canvas->SizeY * 0.5f;

    const TInterval<float> Center(SizeX, SizeY);

    const float HalfLineSize = 10.0f;
    const float LineThickness = 2.0f;
    const FLinearColor LineColor = FLinearColor::Green;

    DrawLine(Center.Min - HalfLineSize, Center.Max, Center.Min + HalfLineSize, Center.Max, LineColor, LineThickness);
    DrawLine(Center.Min, Center.Max - HalfLineSize, Center.Min, Center.Max + HalfLineSize, LineColor, LineThickness);
}

void ASTUGameHUD::OnMatchStateChange(ESTUMatchState MatchState)
{
    if (CurrentWidget)
    {
        CurrentWidget->SetVisibility(ESlateVisibility::Hidden);
    }
    
    if (GameWidgets.Contains(MatchState))
    {
        CurrentWidget = GameWidgets[MatchState];

        if (CurrentWidget)
        {
            CurrentWidget->SetVisibility(ESlateVisibility::Visible);
        }
    }

    UE_LOG(LogSTUGameHUD, Display, TEXT("Match state changed to: %s"), *UEnum::GetValueAsString(MatchState));
}
