##########################################################
Special modes rules menu change cosmetics REDUX [Kapedani]
##########################################################
HOOK @ $806a66b0  # muProcRule1::init
{  
  lis r26, 0x806a 
  lis r29, 0x800b       # \ store function pointer for MuObject::changeTexPatAnimN
  ori r29, r29, 0x4c14  # /
  lwz r28, 0x64c(r27)   # this->muObjects
  lis r25, 0x805A       # \
  lwz	r25, 0x00E0(r25)  # | g_GameGlobal->setRule->gameRule->spMeleeOption1
  lwz r25, 0x1C(r25)    # |
  lbz r12, 0x18(r25)    # /
  cmpwi r12, 0x2        # \ Check if stamina mode
  bne+ checkForZTD      # /
  lwz r3, 0x10(r28)     # \
  ori r4, r26, 0xD610   # | muObjects[0x4]->changeTexPatAnimN("126_TopN")
  mtctr r29             # |
  bctrl                 # /
checkForZTD:
  lbz r12, 0x1A(r25)    # \
  cmpwi r12, 0x2        # | Check if ZTD/regen mode
  bne+ end              # /
  lwz r3, 0x8(r28)      # \
  ori r4, r26, 0xD5E0   # | muObjects[0x2]->changeTexPatAnimN("124_TopN")
  mtctr r29             # |
  bctrl                 # /
end:
  li r3, 0x1   # Original operation
}
HOOK @ $806a8290 # muProcRule1::setMessage
{
  addi r4, r4, 10       # Original operation
  lis r12, 0x805A       # \
  lwz	r12, 0x00E0(r12)  # | g_GameGlobal->setRule->gameRule->spMeleeOption1
  lwz r12, 0x1C(r12)    # |
  lbz r12, 0x18(r12)    # /
  cmpwi r12, 0x2        # \ Check if stamina mode
  bne+ %end%            # /
  addi r4, r4, 137      # Add to get to stamina description msg indices
}
HOOK @ $806a827c # muProcRule1::setMessage
{
  addi r4, r4, 6       # Original operation
  lis r12, 0x805A       # \
  lwz	r12, 0x00E0(r12)  # | g_GameGlobal->setRule->gameRule->spMeleeOption1
  lwz r12, 0x1C(r12)    # |
  lbz r12, 0x1A(r12)    # /
  cmpwi r12, 0x2        # \ Check if stamina mode
  bne+ %end%            # /
  addi r4, r4, 146      # Add to get to frame control for ztd/regen description msg indices
}
HOOK @ $806A792C
{
  cmplwi r4, 2		# Original operation
  lis r12, 0x805A       # \
  lwz	r12, 0x00E0(r12)  # | g_GameGlobal->setRule->gameRule->spMeleeOption1
  lwz r12, 0x1C(r12)    # |
  lbz r12, 0x1A(r12)    # /  
  cmpwi cr1, r12, 0x2;  bne- cr1, %end%
  cmplwi r4, 1		# Try to make it only have two settings in Regen
}
