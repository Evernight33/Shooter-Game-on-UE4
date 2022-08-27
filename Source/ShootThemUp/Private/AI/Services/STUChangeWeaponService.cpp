// Shoot Them up Game, All rights reserved.


#include "AI/Services/STUChangeWeaponService.h"
#include "Components/STUWeaponComponent.h"
#include "UI/STUPlayerHUDWidget.h"
#include "AIController.h"

USTUChangeWeaponService::USTUChangeWeaponService() 
{
    NodeName = "Change Weapon";
}

void USTUChangeWeaponService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
    const auto Controller = OwnerComp.GetAIOwner();

    if (Controller != nullptr)
    {
        const auto WeaponComponent = USTUPlayerHUDWidget::GetComponent<USTUWeaponComponent>(Controller->GetPawn());

        if (WeaponComponent != nullptr && Probability > 0 && FMath::FRand() <= Probability)
        {
            if (!WeaponComponent->GetIsReloadAnimInProgress())
            {
                WeaponComponent->NextWeapon();
            }          
        }
    }

    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}