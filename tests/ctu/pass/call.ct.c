/**
 * Autogenerated by the Cthulhu Compiler Collection C99 backend
 * Generated from C:::Users::ehb56::OneDrive::Documents::GitHub::ctulang::tests::ctu::pass
 */
#include <stddef.h>

// String literals
// Imported symbols

// Global forwarding

// Function forwarding
_Bool main(signed int arg0);

// Global initialization

// Function definitions
_Bool main(signed int arg0) {
  _Bool vreg0 = arg0 == (signed int)100;
  if (vreg0) { goto block2; } else { goto block5; }
block2: /* empty */;
  return 1;
block5: /* empty */;
  _Bool vreg6 = (*&main)((signed int)4521);
  _Bool vreg7 = (*&main)(arg0);
  _Bool vreg8 = vreg6 == vreg7;
  return vreg8;
}
