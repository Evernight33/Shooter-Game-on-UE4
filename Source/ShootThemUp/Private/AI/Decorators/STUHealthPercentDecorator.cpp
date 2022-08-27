// Shoot Them up Game, All rights reserved.


#include "AI/Decorators/STUHealthPercentDecorator.h"
#include "AIController.h"
#include "UI/STUPlayerHUDWidget.h"
#include "Components/STUHealthComponent.h"

USTUHealthPercentDecorator::USTUHealthPercentDecorator() 
{
    NodeName = "Health Percent";
}

bool USTUHealthPercentDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    bool ReturnVal = false;

    const auto Controller = OwnerComp.GetAIOwner();
    if (Controller != nullptr)
    {
        const auto HealthComponent = USTUPlayerHUDWidget::GetComponent<USTUHealthComponent>(Controller->GetPawn());
        if (HealthComponent && !HealthComponent->IsDead())
        {
            ReturnVal = HealthComponent->GetHealthPercent() <= HealthPercent;
        }
    }

    return ReturnVal;
}