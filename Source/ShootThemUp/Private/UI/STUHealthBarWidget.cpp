// Shoot Them up Game, All rights reserved.

#include "UI/STUHealthBarWidget.h"
#include "Components/ProgressBar.h"

void USTUHealthBarWidget::SetHealthPercent(float Percent)
{
    if (HealthProgressBar)
    {
        ESlateVisibility HealthBarVisibility = (Percent > PercentVisibilityThreshold || FMath::IsNearlyZero(Percent) 
            ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
        HealthProgressBar->SetVisibility(HealthBarVisibility);

        FLinearColor HealthBarColor = Percent > PercentColorThreshold ? GoodColor : BadColor;
        HealthProgressBar->SetFillColorAndOpacity(HealthBarColor);

        HealthProgressBar->SetPercent(Percent);
    }
}