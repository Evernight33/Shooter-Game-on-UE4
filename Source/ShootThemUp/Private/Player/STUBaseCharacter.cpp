// Shoot Them up Game, All rights reserved.
 

#include "Player/STUBaseCharacter.h"
#include "Components/STUCharacterMovementComponent.h"
#include "Components/STUHealthComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/STUWeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseCharacter, All, All)

ASTUBaseCharacter::ASTUBaseCharacter(const FObjectInitializer& ObjInit) 
    : Super(ObjInit.SetDefaultSubobjectClass<USTUCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

    HealthComponent = CreateDefaultSubobject<USTUHealthComponent>("HealthComponent");
    WeaponComponent = CreateDefaultSubobject<USTUWeaponComponent>("WeaponComponent");
}

void ASTUBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

    check(HealthComponent);
    check(GetCharacterMovement());
    check(GetMesh());

    OnHealthChanged(HealthComponent->GetHealth(), 0.0f);
    HealthComponent->OnDeath.AddUObject(this, &ASTUBaseCharacter::OnDeath);
    HealthComponent->OnHealthChanged.AddUObject(this, &ASTUBaseCharacter::OnHealthChanged);

    LandedDelegate.AddDynamic(this, &ASTUBaseCharacter::OnGroundLanded);
}

void ASTUBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASTUBaseCharacter::SetPlayerColor(const FLinearColor& Color)
{
    const auto MaterialInstance = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);

    if (MaterialInstance)
    {
        MaterialInstance->SetVectorParameterValue(MaterialColorName, Color);
    }
}

bool ASTUBaseCharacter::IsRunning() const
{
    return false;
}

float ASTUBaseCharacter::GetMovementDirection() const
{
    if (GetVelocity().IsZero())
    {
        return 0.0f;
    }

    const auto VelocityNormal = GetVelocity().GetSafeNormal();
    const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), VelocityNormal));
    const auto CrossProduct = FVector::CrossProduct(GetActorForwardVector(), VelocityNormal);
    const auto Degrees = FMath::RadiansToDegrees(AngleBetween);
    return CrossProduct.IsZero() ? Degrees : FMath::RadiansToDegrees(AngleBetween) * FMath::Sign(CrossProduct.Z);
}

void ASTUBaseCharacter::OnDeath() 
{
    UE_LOG(LogBaseCharacter, Display, TEXT("Player %s is dead"), *GetName());

    //PlayAnimMontage(DeathAnimMontage);

    GetCharacterMovement()->DisableMovement();

    SetLifeSpan(LifeSpanOnDeath);

    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    WeaponComponent->StopFire();

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);

    UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
}

void ASTUBaseCharacter::OnHealthChanged(float Health, float HealthDelta)
{
    
}

void ASTUBaseCharacter::OnGroundLanded(const FHitResult& Hit) 
{
    const auto FallVelocityZ = -GetVelocity().Z;
    UE_LOG(LogBaseCharacter, Display, TEXT("On landed: %f"), FallVelocityZ);

    if (FallVelocityZ < LandedDamageVelocity.X) 
    {
        return;
    }

    const auto FinalDamage = FMath::GetMappedRangeValueClamped(LandedDamageVelocity, LandedDamage, FallVelocityZ);
    UE_LOG(LogBaseCharacter, Display, TEXT("Final Damage: %f"), FinalDamage);
    TakeDamage(FinalDamage, FDamageEvent{}, nullptr, nullptr);
}

void ASTUBaseCharacter::OnStartFire() 
{
    if (!IsRunning())
    {
        WeaponComponent->StartFire();
    }
}