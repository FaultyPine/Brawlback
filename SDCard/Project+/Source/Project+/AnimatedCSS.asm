########################################################
Allow full CSS background animations [DukeItOut, mawwwk]
########################################################
HOOK @ $8068EAE8
{
    lfs f1, 0x0AE8(r25)     # Original op. Background speed of 0
    cmpwi r21, 0            # Narrow down different models
    bgt %END%
    cmpwi r22, 0                    
    bgt %END%
    cmpwi r23, 1            
    bne %END%
    lfs f1, 0xE4(r13)       # Load 1.0 animation speed
}