// Shoot Them up Game, All rights reserved.


#include "Components/STUAIPerceptionComponent.h"
#include "AIController.h"
#include "Components/STUHealthComponent.h"
#include "Perception/AISense_Sight.h"
#include "UI/STUPlayerHUDWidget.h"

AActor* USTUAIPerceptionComponent::GetClosestEnemy() const 
{
    AActor* ReturnVal = nullptr;
    TArray<AActor*> PercieveActors;
    GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PercieveActors);

    if (PercieveActors.Num() != 0)
    {
        const auto Controller = Cast<AAIController>(GetOwner());
        
        if (Controller != nullptr)
        {
            const auto Pawn = Controller->GetPawn();

            if (Pawn != nullptr)
            {
                float BestDistance = MAX_FLT;
                AActor* BestPawn = nullptr;

                for (const auto PercieveActor : PercieveActors)
                {
                    const auto HealthComponent = USTUPlayerHUDWidget::GetComponent<USTUHealthComponent>(PercieveActor);
                    const auto PercievePawn = Cast<APawn>(PercieveActor);
                    bool AreEnemies = PercievePawn && USTUPlayerHUDWidget::AreEnemies(Controller, PercievePawn->Controller);

                    if (HealthComponent != nullptr && !HealthComponent->IsDead() && AreEnemies) 
                    {
                        const auto CurrentDistance = (PercieveActor->GetActorLocation() - Pawn->GetActorLocation()).Size();
                        if (CurrentDistance < BestDistance)
                        {
                            BestDistance = CurrentDistance;
                            BestPawn = PercieveActor;
                        }
                        ReturnVal = BestPawn;
                    }
                }
            }
        }
    }

    return ReturnVal;
}