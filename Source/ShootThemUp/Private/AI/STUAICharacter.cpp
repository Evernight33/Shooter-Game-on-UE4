// Shoot Them up Game, All rights reserved.


#include "AI/STUAICharacter.h"
#include "AI/STUAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/STUAIWeaponComponent.h"
#include "BrainComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/STUHealthComponent.h"
#include "UI/STUHealthBarWidget.h"

ASTUAICharacter::ASTUAICharacter(const FObjectInitializer& ObjInit) 
    : Super(ObjInit.SetDefaultSubobjectClass<USTUAIWeaponComponent>("WeaponComponent"))
{
    AutoPossessAI = EAutoPossessAI::Disabled;
    AIControllerClass = ASTUAIController::StaticClass();

    bUseControllerRotationYaw = false;
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->bUseControllerDesiredRotation = true;
        GetCharacterMovement()->RotationRate = FRotator(0.0f, 200.0f, 0.0f);
    }

    HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("HealthWidgetComponent");
    HealthWidgetComponent->SetupAttachment(GetRootComponent());
    HealthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    HealthWidgetComponent->SetDrawAtDesiredSize(true);
}

void ASTUAICharacter::Tick(float DeltaTime) 
{
    Super::Tick(DeltaTime);
    UpdateHealthWidgetVisibility();
}

void ASTUAICharacter::BeginPlay() 
{
    Super::BeginPlay();
    check(HealthWidgetComponent);
}

void ASTUAICharacter::OnDeath()
{
    Super::OnDeath();

    const auto STUController = Cast<AAIController>(Controller);

    if (Controller != nullptr && STUController->BrainComponent) 
    {
        STUController->BrainComponent->Cleanup();
    }
}

void ASTUAICharacter::OnHealthChanged(float Health, float HealthDelta) 
{
    Super::OnHealthChanged(Health, HealthDelta);

    const auto HealthBarWidget = Cast<USTUHealthBarWidget>(HealthWidgetComponent->GetUserWidgetObject());

    if (HealthBarWidget)
    {
        HealthBarWidget->SetHealthPercent(HealthComponent->GetHealthPercent());
    }
}

void ASTUAICharacter::UpdateHealthWidgetVisibility() 
{
    if (GetWorld() && GetWorld()->GetFirstPlayerController() && GetWorld()->GetFirstPlayerController()->GetPawnOrSpectator())
    {
        FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawnOrSpectator()->GetActorLocation();
        float Distance = FVector::Distance(PlayerLocation, GetActorLocation());
        HealthWidgetComponent->SetVisibility(Distance < HealthVisibilityDistance, true);
    }  
}