// Shoot Them up Game, All rights reserved.

#include "Player/STUPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/STUWeaponComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

ASTUPlayerCharacter::ASTUPlayerCharacter(const FObjectInitializer& ObjInit)
    : Super(ObjInit)
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->bUsePawnControlRotation = true;
    SpringArmComponent->SocketOffset = FVector(0.0f, 100.0f, 80.0f);

    CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    CameraComponent->SetupAttachment(SpringArmComponent);
    CameraComponent->bUsePawnControlRotation = true;

    CameraCollisionComponent = CreateDefaultSubobject<USphereComponent>("CameraCollisionComponent");
    CameraCollisionComponent->SetupAttachment(CameraComponent);
    CameraCollisionComponent->SetSphereRadius(10.0f);
    CameraCollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    CameraCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ASTUPlayerCharacter::BeginPlay() 
{
    Super::BeginPlay();

    check(CameraCollisionComponent);

    CameraCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASTUPlayerCharacter::OnCameraCollisionBeginOverlap);
    CameraCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ASTUPlayerCharacter::OnCameraCollisionEndOverlap);
}

void ASTUPlayerCharacter::OnDeath()
{
    Super::OnDeath();

    if (Controller != nullptr)
    {
        Controller->ChangeState(NAME_Spectating);
    }
}

void ASTUPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    check(PlayerInputComponent);
    check(WeaponComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ASTUPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ASTUPlayerCharacter::MoveRight);
    PlayerInputComponent->BindAxis("LookUp", this, &ASTUPlayerCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("TurnAround", this, &ASTUPlayerCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASTUPlayerCharacter::Jump);
    PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASTUPlayerCharacter::OnStartRunning);
    PlayerInputComponent->BindAction("Run", IE_Released, this, &ASTUPlayerCharacter::OnStopRunning);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &USTUWeaponComponent::StartFire);
    PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &USTUWeaponComponent::StopFire);
    PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, WeaponComponent, &USTUWeaponComponent::NextWeapon);
    PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &USTUWeaponComponent::Reload);

    DECLARE_DELEGATE_OneParam(FZoomInputSignature, bool);
    PlayerInputComponent->BindAction<FZoomInputSignature>("Zoom", IE_Pressed, WeaponComponent, &USTUWeaponComponent::Zoom, true);
    PlayerInputComponent->BindAction<FZoomInputSignature>("Zoom", IE_Released, WeaponComponent, &USTUWeaponComponent::Zoom, false);

    //PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASTUPlayerCharacter::OnStartFire);
}

void ASTUPlayerCharacter::MoveForward(float Amount)
{
    IsMovingForward = Amount > 0.0f;

    if (Amount != 0)
    {
        AddMovementInput(GetActorForwardVector(), Amount);

        if (IsRunning() && WeaponComponent->IsFiring())
        {
            WeaponComponent->StopFire();
        }
    }
    
}

void ASTUPlayerCharacter::MoveRight(float Amount)
{  
    AddMovementInput(GetActorRightVector(), Amount);
}

void ASTUPlayerCharacter::OnStartRunning()
{
    WantsToRun = true;

    if (IsRunning())
    {
        WeaponComponent->StopFire();
    }
}

void ASTUPlayerCharacter::OnStopRunning()
{
    WantsToRun = false;
}

bool ASTUPlayerCharacter::IsRunning() const
{
    return WantsToRun && IsMovingForward && !GetVelocity().IsZero() ? true : false;
}

void ASTUPlayerCharacter::OnCameraCollisionBeginOverlap
(
    UPrimitiveComponent* OverlappedComponent, 
    AActor* OtherActor, 
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, 
    bool bFromSweep, 
    const FHitResult& SweepResult
)
{
    CheckCameraOverlap();
}

void ASTUPlayerCharacter::OnCameraCollisionEndOverlap
(
    UPrimitiveComponent* OverlappedComponent, 
    AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, 
    int32 OtherBodyIndex
)
{
    CheckCameraOverlap();
}

void ASTUPlayerCharacter::CheckCameraOverlap() 
{
    bool HideMesh = CameraCollisionComponent->IsOverlappingComponent(GetCapsuleComponent());
    GetMesh()->SetOwnerNoSee(HideMesh);
}