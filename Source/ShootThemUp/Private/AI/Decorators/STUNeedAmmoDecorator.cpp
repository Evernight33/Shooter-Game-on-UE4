// Shoot Them up Game, All rights reserved.


#include "AI/Decorators/STUNeedAmmoDecorator.h"
#include "AIController.h"
#include "UI/STUPlayerHUDWidget.h"
#include "Components/STUWeaponComponent.h"

USTUNeedAmmoDecorator::USTUNeedAmmoDecorator() 
{
    NodeName = "Need Ammo";
}

bool USTUNeedAmmoDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const 
{
    bool ReturnVal = false;

    const auto Controller = OwnerComp.GetAIOwner();
    if (Controller != nullptr) 
    {
        const auto WeaponComponent = USTUPlayerHUDWidget::GetComponent<USTUWeaponComponent>(Controller->GetPawn());
        if (WeaponComponent != nullptr)
        {
            ReturnVal = WeaponComponent->NeedAmmo(WeaponType);
        }
    }

    return ReturnVal;
}