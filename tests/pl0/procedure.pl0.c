/**
 * Autogenerated by the Cthulhu Compiler Collection C99 backend
 * Generated from C:::Users::ehb56::OneDrive::Documents::GitHub::ctulang::tests::pl0
 */
#include <stddef.h>

// String literals
// Imported symbols
extern signed int printf(const char *arg0, ...);

// Global forwarding
signed long x;

// Function forwarding
void set();
void main();

// Global initialization
signed long x = (signed long)0;

// Function definitions
void set() {
  *&x = (signed long)25;
  return;
}
void main() {
  (*&set)();
  return;
}
