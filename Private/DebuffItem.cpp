
#include "DebuffItem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

ADebuffItem::ADebuffItem()
{
    SpeedMultiplier = 0.5f;
    DebuffDuration = 3.0f; 
    ItemType = "Debuff";
}

void ADebuffItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    if (Activator && Activator->ActorHasTag("Player"))
    {
        ACharacter* PlayerCharacter = Cast<ACharacter>(Activator);
        if (PlayerCharacter)
        {
            UCharacterMovementComponent* MovementComp = PlayerCharacter->GetCharacterMovement();
            if (MovementComp)
            {
                MovementComp->MaxWalkSpeed *= SpeedMultiplier;
            }

            GetWorld()->GetTimerManager().SetTimer(
                DebuffTimerHandle,
                FTimerDelegate::CreateUObject(this, &ADebuffItem::ResetSpeed, PlayerCharacter),
                DebuffDuration,
                false
            );

            DestroyItem();
        }
    }
}

void ADebuffItem::ResetSpeed(ACharacter* AffectedActor)
{
    if (AffectedActor && AffectedActor->GetCharacterMovement())
    {
        AffectedActor->GetCharacterMovement()->MaxWalkSpeed /= SpeedMultiplier;
    }
}