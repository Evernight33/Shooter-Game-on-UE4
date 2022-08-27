// Shoot Them up Game, All rights reserved.


#include "AI/Tasks/STUNextLocationTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"

USTUNextLocationTask::USTUNextLocationTask() 
{
    NodeName = "Next Location";
}

EBTNodeResult::Type USTUNextLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
    EBTNodeResult::Type ResultVal = EBTNodeResult::Failed;

    const auto Controller = OwnerComp.GetAIOwner();
    const auto Blackboard = OwnerComp.GetBlackboardComponent();

    if (Controller != nullptr && Blackboard != nullptr)
    {
        const auto Pawn = Controller->GetPawn();
        if (Pawn != nullptr)
        {
            const auto NavSys = UNavigationSystemV1::GetCurrent(Pawn);
            if (NavSys != nullptr)
            {
                FNavLocation NavLocation;

                auto Location = Pawn->GetActorLocation();
                if (!SelfCenter)
                {
                    auto CenterActor = Cast<AActor>(Blackboard->GetValueAsObject(CenterActorKey.SelectedKeyName));
                    if (!CenterActor)
                    {
                        return EBTNodeResult::Failed;
                    }
                    else
                    {
                        Location = CenterActor->GetActorLocation();
                    }
                }

                const auto Found = NavSys->GetRandomReachablePointInRadius(Location, Radius, NavLocation);
                if (Found == true)
                {
                    Blackboard->SetValueAsVector(AimLocationKey.SelectedKeyName, NavLocation.Location);
                    ResultVal = EBTNodeResult::Succeeded;
                }
            }
        }
    }

    return ResultVal;
}