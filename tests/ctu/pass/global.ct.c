/**
 * Autogenerated by the Cthulhu Compiler Collection C99 backend
 * Generated from C:::Users::ehb56::OneDrive::Documents::GitHub::ctulang::tests::ctu::pass
 */
#include <stddef.h>

// String literals
// Imported symbols

// Global forwarding
signed int x;
_Bool z;

// Function forwarding
signed int main();
signed int f(signed int arg0);

// Global initialization

// Function definitions
signed int main() {
  signed int vreg0 = *&x;
  *vreg0 = (signed int)100;
  signed int vreg2 = (*&f)((signed int)200);
  return vreg2;
}
signed int f(signed int arg0) {
  signed int vreg0 = *&x;
  signed int vreg1 = vreg0 + arg0;
  return vreg1;
}
