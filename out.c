/**
 * Autogenerated by the Cthulhu Compiler Collection C99 backend
 * Generated from tests/ctu/pass/call.ct
 */

// String literals
// Imported symbols

// Global forwarding

// Global initialization

// Function forwarding
_Bool main(signed int arg0);

// Function definitions
_Bool main(signed int arg0) {
  _Bool vreg0 = arg0 == (signed int)100;
  if (vreg0) { goto block2; } else { goto block5; }
block2: /* empty */;
  return 1;
  goto block5;
block5: /* empty */;
  _Bool vreg6 = (*&main)((signed int)4521);
  _Bool vreg7 = (*&main)(arg0);
  _Bool vreg8 = vreg6 == vreg7;
  return vreg8;
  return;
}
