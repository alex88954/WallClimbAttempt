// Fill out your copyright notice in the Description page of Project Settings.


#include "WallJumpCharacterMovement.h"
#include <GameFramework/Character.h>
#include <Components/CapsuleComponent.h>
#include "WallClimbAttemptCharacter.h"

void UWallJumpCharacterMovement::BeginPlay()
{
	Super::BeginPlay();

	CharacterOwner->GetCapsuleComponent()->OnComponentHit.AddUniqueDynamic(
		this, &UWallJumpCharacterMovement::HandleCapsuleHit // Subscribe to OnComponentHit
	);

    OnWallRunStartLocal.BindUObject(this, &UWallJumpCharacterMovement::FindWallDirection);
}

void UWallJumpCharacterMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	RunAlongWall();
	SearchWallDistance();
    RotateCharacterToWall(DeltaTime);

    /*if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Black, bIsWallSliding ? (TEXT("True")) : (TEXT("False")));
    }*/

    /*if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Black, Cast<AWallClimbAttemptCharacter>(CharacterOwner)->bIsWallRunning ? (TEXT("True")) : (TEXT("False")));
    }*/
}

void UWallJumpCharacterMovement::SetWallSlidingTrue()
{
    if (bIsWallSliding) return;

    bIsWallSliding = true;

    bCanPrintM = true;
    OnWallRunStartLocal.ExecuteIfBound();

    if (auto* character = Cast<AWallClimbAttemptCharacter>(CharacterOwner))
    {
        character->OnWallRunStart();
    }
}

void UWallJumpCharacterMovement::SetWallSlidingFalse()
{
    if (!bIsWallSliding) return;

    bIsWallSliding = false;

    bCanPrintM = false;

    if (auto* character = Cast<AWallClimbAttemptCharacter>(CharacterOwner))
    {
        character->OnWallRunStop();
    }
}

void UWallJumpCharacterMovement::HandleCapsuleHit(UPrimitiveComponent* HitComponent,AActor* OtherActor,
	UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit)
{
	if(IsFalling())
	{
		WallHitResult = Hit;
            if (IsWallVertical())
            {
                SetWallSlidingTrue();
            }
	}
	else
	{
        SetWallSlidingFalse();
        ResetWallJumpCooldownTimer();
	}

	if(CanWallJump())
	{
        SetWallSlidingFalse();
        GetWorld()->GetTimerManager().SetTimer(WallJumpCooldownTimer, this,
            &UWallJumpCharacterMovement::ResetWallJumpCooldownTimer, 
            WallJumpCooldown, false);
		ExecWallJump();
	}
}

bool UWallJumpCharacterMovement::CanWallJump()
{
	return IsFalling() && Cast<AWallClimbAttemptCharacter>(CharacterOwner)->bJumpPressed && !IsWallJumpCooldownActive();
}

void UWallJumpCharacterMovement::ExecWallJump()
{
	auto* character = Cast<AWallClimbAttemptCharacter>(CharacterOwner);

	float Forward = character->GetForwardInput();
	float Right = character->GetRightInput();

    FVector LocalLaunchDirection = {};

    if (Forward == 1)
    {
        LocalLaunchDirection.X += WallJumpPower;
    }
    else if (Forward == -1)
    {
        LocalLaunchDirection.X -= WallJumpPower;
    }
    if (Right == 1)
    {
        LocalLaunchDirection.Y += WallJumpPower;

    }
    else if (Right == -1)
    {
        LocalLaunchDirection.Y -= WallJumpPower;
    }

    // Transform local direction to world space based on camera rotation
    FRotator CameraRotation = character->GetController()->GetControlRotation();
    FVector WorldLaunchVelocity = CameraRotation.RotateVector(LocalLaunchDirection);

    WorldLaunchVelocity.Z = WallJumpPower;

    CharacterOwner->LaunchCharacter(WorldLaunchVelocity, true, true);
}

void UWallJumpCharacterMovement::FindWallDirection()
{
    if (!bIsWallSliding) return;

    FVector WallNormal = WallHitResult.ImpactNormal;

    auto* Character = Cast<AWallClimbAttemptCharacter>(CharacterOwner);

        /*float ForwardDot = FVector::DotProduct(WallNormal, Character->GetActorForwardVector());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("%.2f"), ForwardDot));

        }*/

        float RightDot = FVector::DotProduct(WallNormal, Character->GetActorRightVector());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("%.2f"), RightDot));
        }

        /*if (FMath::IsWithin(ForwardDot, 0.5f, 1.f) || FMath::IsWithin(ForwardDot, -0.5f, -1.f))
        {
            WallSide = EWallSide::WallFront;
            return;
        }*/
        if (RightDot > 0.f)
        {
            WallSide = EWallSide::WallLeft;
            return;
        }
        else if (RightDot < 0.f)
        {
            WallSide = EWallSide::WallRight;
            return;
        }
        /*else 
        {
            WallSide = EWallSide::WallFront;
        }*/
        
        

}

void UWallJumpCharacterMovement::RunAlongWall()
{
    if (!bIsWallSliding) return;

    FVector WallNormal = WallHitResult.ImpactNormal;

    FVector HorizontalVelocity = FVector(Velocity.X, Velocity.Y, 0);
    FVector HorizontalAlongWall = FVector::VectorPlaneProject(HorizontalVelocity, WallNormal);

    FVector VerticalVelocity = FVector(0, 0, Velocity.Z);

    // Force Capsule into wall to maintain contact
    FVector PushIntoWall = -WallNormal * 20.0f;

    Velocity = HorizontalAlongWall + VerticalVelocity + PushIntoWall;
}

void UWallJumpCharacterMovement::SearchWallDistance()
{
    if (!bIsWallSliding) return;

    FVector CharacterLoc = CharacterOwner->GetActorLocation();

    // Use orthogonal projection to find the exact direction toward the physical wall surface
    FVector NearestPointOnPlane = FVector::PointPlaneProject(CharacterLoc, WallHitResult.ImpactPoint, WallHitResult.ImpactNormal);
    FVector DirectionToWall = (NearestPointOnPlane - CharacterLoc).GetSafeNormal();

    // Trace slightly further than the capsule radius to check if the wall is still physically there
    float SweepDistance = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius() + 30.0f;
    FVector TraceStart = CharacterLoc;
    FVector TraceEnd = TraceStart + (DirectionToWall * SweepDistance);

    FHitResult ContinuousHit;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(CharacterOwner);

    // Dynamic Line Trace to confirm the mesh is still there
    if (GetWorld()->LineTraceSingleByChannel(ContinuousHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
    {
        // Update our hit results with the fresh frame-by-frame data
        WallHitResult = ContinuousHit;

        DrawDebugLine(GetWorld(), TraceStart, ContinuousHit.ImpactPoint, FColor::Green, false, -1.0f, 0, 2.0f);
    }
    else
    {
        // No physical mesh found under the player (player slid off the edge)
        SetWallSlidingFalse();
        DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, -1.0f, 0, 2.0f);
    }
}

bool UWallJumpCharacterMovement::IsWallVertical()
{
    FVector WallNormal = WallHitResult.ImpactNormal;
    return FMath::IsNearlyZero(WallNormal.Z);
}

bool UWallJumpCharacterMovement::IsWallJumpCooldownActive()
{
    return GetWorld()->GetTimerManager().IsTimerActive(WallJumpCooldownTimer);
}

void UWallJumpCharacterMovement::ResetWallJumpCooldownTimer()
{
    GetWorld()->GetTimerManager().ClearTimer(WallJumpCooldownTimer);
}

void UWallJumpCharacterMovement::RotateCharacterToWall(float DeltaTime)
{
    if (!bIsWallSliding) return;
    FVector WallNormal = WallHitResult.ImpactNormal;
    FVector WallForward = {};
    if(WallSide == EWallSide::WallLeft)
    {
        WallForward = FVector::CrossProduct(WallNormal, FVector::UpVector).GetSafeNormal();
    }
    if(WallSide == EWallSide::WallRight)
    {
        WallForward = FVector::CrossProduct(FVector::UpVector, WallNormal).GetSafeNormal();
    }
    /*if(WallSide == EWallSide::WallFront)
    {
        WallForward = -WallNormal;
    }*/
    FString WallSideString{};
    switch (WallSide)
    {
    case EWallSide::WallFront:
        WallSideString = TEXT("Front");
        break;
    case EWallSide::WallRight:
        WallSideString = TEXT("Right");
        break;
    case EWallSide::WallLeft:
        WallSideString = TEXT("Left");
        break;
    }
    if (bCanPrintM)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, WallSideString);
            bCanPrintM = false;
        }
    }

    FRotator TargetRotation = WallForward.ToOrientationRotator();

    CharacterOwner->SetActorRotation(FMath::RInterpTo(CharacterOwner->GetActorRotation(), TargetRotation, DeltaTime, 10.f));
}
