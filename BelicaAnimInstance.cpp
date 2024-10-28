#include "BelicaAnimInstance.h"
#include "MyCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBelicaAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if(Belica==nullptr)
	{
		Belica = Cast<AMyCharacter>(TryGetPawnOwner());
	}
	if(Belica)
	{
		FVector Velocity = Belica->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		bIsInAir = Belica->GetCharacterMovement()->IsFalling();
		if (Belica->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.4f)
			bIsAccelerating = true;
		else
			bIsAccelerating = false;

		FRotator BaseAimRotation = Belica->GetBaseAimRotation();
		FRotator VelocityBasedRotation = UKismetMathLibrary::MakeRotFromX(Belica->GetVelocity());
		StrafingValue = UKismetMathLibrary::NormalizedDeltaRotator(BaseAimRotation, VelocityBasedRotation).Yaw;
	}
}

void UBelicaAnimInstance::NativeInitializeAnimation()
{
	Belica = Cast<AMyCharacter>(TryGetPawnOwner());
}
