HOOK @ $8002d4f8 {
  stw r0, r1, 0xFFFC /* 65532 */
  mfspr r0, 256
  b unk_br1 /* 75 */
  addis r31, r0, 0x8140 /* 33088 */
  addi r31, r31, 0x0 /* 0 */
  b 0x1 /* 1 */
  mfspr r3, 256
  addi r3, r3, 0x16C /* 364 */
  or r4, r31, r31
  b 0x12 /* 18 */
  lwz r30, r31, 0x8 /* 8 */
  addi r29, r31, 0xC /* 12 */
  addi r3, r29, 0x4 /* 4 */
  lwz r4, r29, 0x0 /* 0 */
  b 0xD /* 13 */
  addi r29, r29, 0x24 /* 36 */
  addi r30, r30, 0xFFFF /* 65535 */
  cmpi crf0, 0, r30, 0x0 /* 0 */
  bc 0b100, 0x2 /* 2 */, 0x3FFA /* 16378 */
  lwz r3, r31, 0x4 /* 4 */
  lwz r7, r31, 0x0 /* 0 */
  mtspr 288, r7
  bcctr 0b10100, 0x0 /* 0 */
  b 0x3C /* 60 */
  mtspr 256, r0
  lwz r0, r1, 0xFFFC /* 65532 */
  b 0x4B /* 75 */
  stw r0, r1, 0xFFFC /* 65532 */
  mfspr r0, 256
  b 0x30 /* 48 */
  or r31, r3, r3
  or r30, r4, r4
  b 0x1 /* 1 */
  mfspr r3, 256
  addi r3, r3, 0xE0 /* 224 */
  addis r4, r0, 0x8040 /* 32832 */
  addi r4, r4, 0x6920 /* 26912 */
  addis r7, r0, 0x8040 /* 32832 */
  addi r7, r7, 0xA280 /* 41600 */
  mtspr 288, r7
  bcctr 0b10100, 0x0 /* 0 */
  b 0x1 /* 1 */
  mfspr r3, 256
  addi r3, r3, 0xBC /* 188 */
  or r4, r31, r31
  addis r7, r0, 0x8040 /* 32832 */
  addi r7, r7, 0xA384 /* 41860 */
  mtspr 288, r7
  bcctr 0b10100, 0x0 /* 0 */
  b 0x1 /* 1 */
  mfspr r3, 256
  addi r3, r3, 0x9C /* 156 */
  b 0x1 /* 1 */
  mfspr r4, 256
  addi r4, r4, 0xBC /* 188 */
  addis r7, r0, 0x803F /* 32831 */
  addi r7, r7, 0xBEB8 /* 48824 */
  mtspr 288, r7
  bcctr 0b10100, 0x0 /* 0 */
  or r29, r3, r3
  or r3, r30, r30
  addi r4, r0, 0x1 /* 1 */
  addis r5, r0, 0x2 /* 2 */
  or r6, r29, r29
  addis r7, r0, 0x803F /* 32831 */
  addi r7, r7, 0xBEE4 /* 48868 */
  mtspr 288, r7
  bcctr 0b10100, 0x0 /* 0 */
  or r3, r29, r29
unk_br1:
  addis r7, r0, 0x803F /* 32831 */
  addi r7, r7, 0xBE8C /* 48780 */
  mtspr 288, r7
  bcctr 0b10100, 0x0 /* 0 */
  b 0xA /* 10 */
  mtspr 256, r0
  lwz r0, r1, 0xFFFC /* 65532 */
  bclr 0b10100, 0x0 /* 0 */
  stw r0, r1, 0x4 /* 4 */
  mfspr r0, 288
  stw r0, r1, 0xFFF8 /* 65528 */
  stwu r1, r1, 0xFF08 /* 65288 */
  stmw r2, r1, 0x8 /* 8 */
  bclr 0b10100, 0x0 /* 0 */
  lmw r2, r1, 0x8 /* 8 */
  addi r1, r1, 0xF8 /* 248 */
  lwz r0, r1, 0xFFF8 /* 65528 */
  mtspr 288, r0
  lwz r0, r1, 0x4 /* 4 */
  bclr 0b10100, 0x0 /* 0 */
}

writeFill8(BASE, 0x0, 0x0)
writeFill8(BASE, 0x0, 0x0)
writeFill8(BASE, 0x0, 0x0)
writeFill8(BASE, 0x0, 0x65)
execRepeat(BL(0))
execRepeat(BL(0))
setRepeat(0x0, BL(0))