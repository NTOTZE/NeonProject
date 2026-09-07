
#include "DataType/NPCombatTypes.h"


const FString GetStateFlagsName(ENPCharacterState flag)
{
    FString str;
    uint8 count = 0;
    for (uint8 i = 0; i < 8; ++i)
    {
        if ((uint8)flag & (1 << i))
        {
            if (count > 0)
            {
                str += " | ";
            }
            count++;
            str += ENPCharacterStateString[i];
        }
    }

    return str;
}