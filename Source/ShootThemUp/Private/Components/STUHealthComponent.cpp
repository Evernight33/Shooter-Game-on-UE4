// Shoot Them up Game, All rights reserved.


#include "Components/STUHealthComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Camera/CameraShakeBase.h"
#include "UI/STUPlayerHUDWidget.h"
#include "Player/STUBaseCharacter.h"
#include "STUGameModeBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All)

USTUHealthComponent::USTUHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void USTUHealthComponent::BeginPlay()
{
	Super::BeginPlay();

    check(MaxHealth); // check if MaxHealth is greater than zero

    Health = MaxHealth;
    OnHealthChanged.Broadcast(MaxHealth, 0.0f);

    AActor* ComponentOwner = GetOwner();
    if (ComponentOwner != nullptr)
    {
        ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &USTUHealthComponent::OnTakeAnyDamage);
        ComponentOwner->OnTakePointDamage.AddDynamic(this, &USTUHealthComponent::OnTakePointDamage);
        ComponentOwner->OnTakeRadialDamage.AddDynamic(this, &USTUHealthComponent::OnTakeRadialDamage);
    }
}

void USTUHealthComponent::OnTakeAnyDamage(
    AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    UE_LOG(LogHealthComponent, Display, TEXT("On any damage: %f"), Damage);
}

void USTUHealthComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
    class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType,
    AActor* DamageCauser)
{
    float FinalDamage = Damage * GetPointDamageModifier(DamagedActor, BoneName);

    UE_LOG(LogHealthComponent, Display, TEXT("On point damage: %f, final damage: %f, bone: %s"), Damage, FinalDamage, *BoneName.ToString());
    ApplyDamage(FinalDamage, InstigatedBy);
}

void USTUHealthComponent::OnTakeRadialDamage(AActor* DamagedActor, float Damage,
    const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy,
    AActor* DamageCauser)
{
    UE_LOG(LogHealthComponent, Display, TEXT("On radial damage: %f"), Damage);
    ApplyDamage(Damage, InstigatedBy);
}

void USTUHealthComponent::HealUpdate() 
{
    SetHealth(Health + HealModifier);

    if (FMath::IsNearlyEqual(Health, MaxHealth) && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
    }
}

void USTUHealthComponent::SetHealth(float NewHealth)
{
    const auto NextHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
    const auto HealthDelta = NextHealth - Health;

    Health = NextHealth;
    OnHealthChanged.Broadcast(Health, HealthDelta);
}

void USTUHealthComponent::PlayCameraShake() 
{
    if (!IsDead())
    {
        const auto Player = Cast<APawn>(GetOwner());

        if (Player != nullptr)
        {
            const auto Controller = Player->GetController<APlayerController>();

            if (Controller != nullptr)
            {
                Controller->PlayerCameraManager->StartCameraShake(CameraShake);
            }
        }
    }
}

void USTUHealthComponent::Killed(AController* KillerController) 
{
    if (GetWorld())
    {
        const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            const auto Player = Cast<APawn>(GetOwner());
            const auto VictimController = Player ? Player->Controller : nullptr;

            GameMode->Killed(KillerController, VictimController);
        }
    }    
}

void USTUHealthComponent::ApplyDamage(float Damage, AController* InstigatedBy) 
{
    const auto ComponentOwnerPawn = Cast<APawn>(this->GetOwner());
    bool DoAttack = USTUPlayerHUDWidget::AreEnemies(InstigatedBy, ComponentOwnerPawn->GetController()) ||
                    !InstigatedBy;  // If InstigatedBy is nullptr this is DamageActor

    if (Damage <= 0 || IsDead() || !DoAttack)
    {
        return;
    }

    SetHealth(Health - Damage);
    GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);

    if (IsDead())
    {
        Killed(InstigatedBy);
        OnDeath.Broadcast();
    }

    else if (AutoHeal && GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(HealTimerHandle, this, &USTUHealthComponent::HealUpdate, HealUpdateTime, true, HealDelay);
    }

    PlayCameraShake();
}

float USTUHealthComponent::GetPointDamageModifier(AActor* DamagedActor, const FName& BoneName) 
{
    const auto Character = Cast<ACharacter>(DamagedActor);
    float ReturnValue = 1;
    if (Character)
    {
        const auto PhysMaterial = Character->GetMesh()->GetBodyInstance(BoneName)->GetSimplePhysicalMaterial();

        if (PhysMaterial)
        {
            if (DamageModifiers.Contains(PhysMaterial))
            {
                ReturnValue = DamageModifiers[PhysMaterial];
            }
        }      
    }
    return ReturnValue;
}