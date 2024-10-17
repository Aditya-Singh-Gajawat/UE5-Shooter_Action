#include "BelicaAnimInstance.h"
#include "MyCharacter.h"
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
	}
}

void UBelicaAnimInstance::NativeInitializeAnimation()
{
	Belica = Cast<AMyCharacter>(TryGetPawnOwner());
}
