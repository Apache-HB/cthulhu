/**
 * Autogenerated by the Cthulhu Compiler Collection C99 backend
 * Generated from out.c
 */

// String literals
const char *strtab0 = "%d\x0a";
// Imported symbols
extern signed int printf(const char *, ...);

// Global forwarding
signed long ret;
signed long arg;
signed long max;

// Global initialization
signed long ret = (null);
signed long arg = (null);
signed long max = (null);

// Function forwarding
void _Z7isprimev();
void _Z6primesv();
void main();

// Function definitions
void _Z7isprimev() {
  signed long i;
  *&ret = (null);
  *&i = (null);
block2: /* empty */;
  signed long vreg3 = *&i;
  signed long vreg4 = *&arg;
  _Bool vreg5 = vreg3 < vreg4;
  if (vreg5) { goto block7; } else { goto block26; }
block7: /* empty */;
  signed long vreg8 = *&arg;
  signed long vreg9 = *&i;
  signed long vreg10 = vreg8 / vreg9;
  signed long vreg11 = *&i;
  signed long vreg12 = vreg10 * vreg11;
  signed long vreg13 = *&arg;
  _Bool vreg14 = vreg12 == vreg13;
  if (vreg14) { goto block16; } else { goto block21; }
block16: /* empty */;
  *&ret = (null);
  signed long vreg18 = *&arg;
  *&i = vreg18;
  goto block21;
block21: /* empty */;
  signed long vreg22 = *&i;
  signed long vreg23 = vreg22 + (null);
  *&i = vreg23;
  goto block2;
block26: /* empty */;
  return;
}
void _Z6primesv() {
  *&arg = (null);
block1: /* empty */;
  signed long vreg2 = *&arg;
  signed long vreg3 = *&max;
  _Bool vreg4 = vreg2 < vreg3;
  if (vreg4) { goto block6; } else { goto block20; }
block6: /* empty */;
  (*&_Z7isprimev)();
  signed long vreg8 = *&ret;
  _Bool vreg9 = vreg8 == (null);
  if (vreg9) { goto block11; } else { goto block15; }
block11: /* empty */;
  signed long vreg12 = *&arg;
  signed int vreg13 = (*&printf)(strtab0, vreg12);
  goto block15;
block15: /* empty */;
  signed long vreg16 = *&arg;
  signed long vreg17 = vreg16 + (null);
  *&arg = vreg17;
  goto block1;
block20: /* empty */;
  return;
}
void main() {
  (*&_Z6primesv)();
  return;
}
