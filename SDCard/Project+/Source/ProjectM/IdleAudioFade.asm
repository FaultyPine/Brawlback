###################################
Idle Audio Fade v2.02 [InternetExplorer, mawwwk, DukeItOut]
###################################
.macro Word(<reg>, <arg1>)
{
.alias temp_Hi = <arg1> / 0x10000
.alias temp_Lo = <arg1> & 0xFFFF
    lis <reg>, temp_Hi
    ori <reg>, <reg>, temp_Lo
}

.alias InactiveFrames = 7200

float 1.0   @ $805A7404 # Max volume
float 0.25  @ $805A7408 # Min volume
float 0.975 @ $805A740C # Mult. per frame

HOOK @ $801BCE60
{
    lfs f0, 0x80(r29)                       # Original op
    
    lis r12, 0x805B; lwz r12, 0x50AC(r12)   # Check if in replay
    lwz r12, 0x10(r12); lwz r12, 0(r12)     # Skip volume change if so
    lis r11, 0x8070; ori r11, r11, 0x39D8   # compare with "sqReplay"
    cmpw r11, r12; beq %END%
    
    %Word(r5, 0x805BAD00)       # Addr for reading controller inputs
    %Word(r4, 0x805A7400)       # Addr storing frames since last input
    lfs f3, 0x0C(r4)            # Volume change per frame
    li r12, 0

checkController_Buttons:
    addi r12, r12, 1            # Increment port check count
    lwz r0, 0x210(r5)           # Button press check
    cmpwi r0, 0
    beq checkController_X       # If any button pressed, fail check
    b inputSent

checkController_X:
    lbz r0, 0x30(r5)            # Check control stick X
    cmpwi r0, 0x20              # If in (32, 224), fail check
    blt checkController_Y
    cmpwi r0, 0xE0
    bgt checkController_Y
    b inputSent

checkController_Y:
    lbz r0, 0x31(r5)            # Check control stick Y
    cmpwi r0, 0x20              # If in (32, 224), fail check
    blt loopPorts
    cmpwi r0, 0xE0
    bgt loopPorts
    b inputSent

loopPorts:
    addi r5, r5, 0x40           # Loop to next port
    cmpwi r12, 8                # 8 = 4 GCC + 4 Wii remote
    blt checkController_Buttons
    lwz r5, 0(r4)               # If no inputs given from any port, store inactive frame count in r5
    b initFadeMultiplier

inputSent:
    li r5, 0

initFadeMultiplier:        
    lwz r11, 0x10(r4)           # i.e. slider muted from save file load
    cmpwi r11, 0				# Check if volume multiplier is 0.0
    bne frameCountCheck
    
    lis r12, 0x3F80             # \ If 0.0, initialize to 1.0
    stw r12, 0x10(r4)           # /

frameCountCheck:
    lfs f4, 0x10(r4)            # Current fade multiplier, in range [min, max]
    addi r5, r5, 1              # Increment frame count and store 
    stw r5, 0(r4)
    
    cmpwi r5, InactiveFrames    # \ If no inputs sent in x frames, fade out volume
    bge calcLowerVolume         # /

# r4: address of frame count and multipliers @ $805A7400
# f1: Fade multiplier max       $805A7404
# f2: Fade multiplier min       $805A7408
# f3: Vol change per frame      $805A740C
# f4: Current frame fade mult   $805A7410
# f0: New volume (dynamic loc)
# f0 replaces value used by original op

calcRestoredVolume:
    lfs f1, 0x04(r4)        # Max volume multiplier (1.0)
    fdivs f4, f4, f3        # Divide twice to fade in faster
    fdivs f4, f4, f3
    fcmpu cr0, f4, f1       # Store min(vMax, vNew) in f4
    ble storeVolume
    fmr f4, f1              
    b storeVolume

calcLowerVolume:
    lfs f2, 0x08(r4)        # Min volume multiplier
    fmuls f4, f4, f3
    fcmpu cr0, f4, f2       # Store max(vMin, vNew) in f4
    bge storeVolume
    fmr f4, f2

storeVolume:
    fmuls f0, f0, f4
    stfs f4, 0x10(r4)
}