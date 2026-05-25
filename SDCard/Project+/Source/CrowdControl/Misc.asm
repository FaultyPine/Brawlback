##############################################
Final Smash Unload Remover (For multiple final smashes at the same time) [MarioDox]
##############################################
# Note: Might overflow memory? Not recommended for normal use especially if characters have big final smash's
op nop @ $80820430

###############################################
Assist Trophy Stands Use Normal Item Limit [Kapedani]
###############################################
op li r3, 1 @ $809b0b3c

###############################################
Explodey Items Use Normal Item Limit [Kapedani]
###############################################
op li r3, 1 @ $809b0ec8

###############################################
Party Balls Use Normal Item Limit [Kapedani]
###############################################
op li r3, 1 @ $809b0e5c

###############################################
Bunny Hoods Use Normal Item Limit [Kapedani]
###############################################
op li r3, 1 @ $809b0e2c

###############################################
Smash Balls use Normal Item Limit [Kapedani]
###############################################
# Not recommended for normal use since vanilla behvaviour makes other players Final Smash unload upon a character receiving their Final Smash
op li r3, 1 @ $809b0c20
op li r3, 1 @ $809b0c40

######################################################
Increase Pokemon Queue Size to Its Max (12) [Kapedani]
######################################################
op cmpwi r3, 20 @ $809afd74
op cmpwi r3, 20 @ $809ad73c

###############################################
Force Assist Preload [Kapedani]
###############################################
op nop @ $809afc40