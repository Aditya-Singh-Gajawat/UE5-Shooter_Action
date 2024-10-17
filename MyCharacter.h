#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()

class SHOOTER__PROJECT_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AMyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return Camera; }
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon-Sounds", meta = (AllowPrivateAccess = "true"))
	class USoundCue* AR_GunFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon-FX", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon-FX", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamFX;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon-FX", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* HitFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon-Animation", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* Fire_SMG_Montage;



	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookupAtRate(float Rate);
	void PrimaryFire();
};
