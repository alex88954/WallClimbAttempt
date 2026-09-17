// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WallJumpCharacterMovement.generated.h"

/**
 * 
 */

UENUM(BlueprintType, DisplayName = "Wall Run Side")
enum class EWallSide : uint8
{
	WallFront,
	WallLeft,
	WallRight,

	None
};

DECLARE_DELEGATE(FOnWallRunStartDelegate);

UCLASS()
class WALLCLIMBATTEMPT_API UWallJumpCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EWallSide GetWallSide() const { return WallSide; }

private:
	FOnWallRunStartDelegate OnWallRunStartLocal;
private:
	FHitResult WallHitResult;

	FTimerHandle WallJumpCooldownTimer;

	EWallSide WallSide = EWallSide::None;


	bool bIsWallSliding = false;
	
	bool bCanPrintM = false;

	UPROPERTY(EditAnywhere, Category = Movement, meta = (DisplayName = "Wall Jump Cooldown Duration"))
	float WallJumpCooldown = 1.f;

	UPROPERTY(EditAnywhere, Category = Movement, meta = (DisplayName = "Wall Jump Power"))
	float WallJumpPower = 600;

private:
	void SetWallSlidingTrue();

	void SetWallSlidingFalse();

protected:
	UFUNCTION()
	virtual void HandleCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual bool CanWallJump();

	virtual void ExecWallJump();
	
	virtual void FindWallDirection();

	virtual void RunAlongWall();

	virtual void SearchWallDistance();

	virtual bool IsWallVertical();

	virtual bool IsWallJumpCooldownActive();

	virtual void ResetWallJumpCooldownTimer();

	virtual void RotateCharacterToWall(float DeltaTime);
};
