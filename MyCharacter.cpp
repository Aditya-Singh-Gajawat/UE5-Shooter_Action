#include "MyCharacter.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/SkeletalMeshSocket.h"

AMyCharacter::AMyCharacter()
{
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;


	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Player-SpringArm"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player-Main-Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	//About camera rotation according to player mesh rotation
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

//M O V E M E N T  - C H A R A C T E R

void AMyCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyCharacter::LookupAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMyCharacter::PrimaryFire()
{
	const USkeletalMeshSocket* MuzzleSocket = GetMesh()->GetSocketByName("MuzzleSocket");

	if(AR_GunFire)
	{
		UGameplayStatics::PlaySound2D(this, AR_GunFire);
	}

	if (MuzzleSocket)
	{
		const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetMesh());
		if (MuzzleFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFX, SocketTransform);
		}

		//Setting ViewportSize
		FVector2D ViewportSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
		}
		//Calculating CrosshairLocation
		FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		CrosshairLocation.Y -= 50.f;
		FVector CrossHairWorldPosition;
		FVector CrossHairWorldDirection;

		//Deprojecting Crosshair on Screen to World
		bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
		(
			UGameplayStatics::GetPlayerController(this, 0),
			CrosshairLocation, 
			CrossHairWorldPosition,
			CrossHairWorldDirection
		);

		if (bScreenToWorld)
		{
			FHitResult ScreenTraceHit;
			const FVector Start{ CrossHairWorldPosition };
			const FVector End{ CrossHairWorldPosition + CrossHairWorldDirection * 50'000.f };

			FVector BeamEndPoint = End;
			GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECollisionChannel::ECC_Visibility);
			if (ScreenTraceHit.bBlockingHit)
			{
				BeamEndPoint = ScreenTraceHit.Location;
			}

			//Another check for Hit, this time from Gun Barrel
			FHitResult BarrelTraceHit;
			const FVector BarrelStart{ SocketTransform.GetLocation() };
			const FVector BarrelEnd{ BeamEndPoint };
			GetWorld()->LineTraceSingleByChannel(BarrelTraceHit, BarrelStart, BarrelEnd, ECollisionChannel::ECC_Visibility);
			if(BarrelTraceHit.bBlockingHit)
			{
				BeamEndPoint = BarrelTraceHit.Location;
			}
			//Checking for FXs after deciding over BeamEndPoint
			if (HitFX)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitFX, BeamEndPoint);
			}

			if (BeamFX)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamFX, SocketTransform); // Beam will start from SocketTransform i.e. Socket location
				if (Beam) //Calculating end point or target for Beam
				{
					Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
				}
			}
		}
	}
	//Playing Montage
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && Fire_SMG_Montage)
	{
		AnimInstance -> Montage_Play(Fire_SMG_Montage);
		AnimInstance -> Montage_JumpToSection(FName("SMG_FIRE"));
	}
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AMyCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyCharacter::LookupAtRate);
	PlayerInputComponent->BindAxis("Camera_Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Camera_LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction("Gun_Fire", EInputEvent::IE_Pressed, this, &AMyCharacter::PrimaryFire);
}

