/**
 * Autogenerated by the Cthulhu Compiler Collection C99 backend
 * Generated from C:::Users::ehb56::OneDrive::Documents::GitHub::ctulang::tests::ctu::pass
 */
#include <stddef.h>

// String literals
const char *strtab0 = "fizz";
const char *strtab1 = "buzz";
// Imported symbols
extern signed int puts(const char *arg0);
extern signed int putd(signed int arg0);

// Global forwarding

// Function forwarding
void fizzbuzz(signed int arg0);
signed int main(signed int arg0, const char ** arg1);

// Global initialization

// Function definitions
void fizzbuzz(signed int arg0) {
  signed int count;
  *&count = (signed int)0;
block1: /* empty */;
  if (1) { goto block3; } else { goto block43; }
block3: /* empty */;
  signed int vreg4 = *&count;
  _Bool vreg5 = vreg4 > arg0;
  if (vreg5) { goto block7; } else { goto block10; }
block7: /* empty */;
  goto block43;
block10: /* empty */;
  signed int vreg11 = *&count;
  signed int vreg12 = vreg11 % (signed int)3;
  _Bool vreg13 = vreg12 == (signed int)0;
  if (vreg13) { goto block15; } else { goto block18; }
block15: /* empty */;
  signed int vreg16 = (*&puts)(strtab0);
  goto block18;
block18: /* empty */;
  signed int vreg19 = *&count;
  signed int vreg20 = vreg19 % (signed int)5;
  _Bool vreg21 = vreg20 == (signed int)0;
  if (vreg21) { goto block23; } else { goto block26; }
block23: /* empty */;
  signed int vreg24 = (*&puts)(strtab1);
  goto block26;
block26: /* empty */;
  signed int vreg27 = *&count;
  signed int vreg28 = vreg27 % (signed int)3;
  _Bool vreg29 = vreg28 != (signed int)0;
  signed int vreg30 = *&count;
  signed int vreg31 = vreg30 % (signed int)5;
  _Bool vreg32 = vreg31 != (signed int)0;
  _Bool vreg33 = vreg29 && vreg32;
  if (vreg33) { goto block35; } else { goto block37; }
block35: /* empty */;
  goto block37;
block37: /* empty */;
  signed int vreg38 = *&count;
  signed int vreg39 = *&count;
  signed int vreg40 = vreg39 + (signed int)1;
  *vreg38 = vreg40;
  goto block1;
block43: /* empty */;
  return;
}
signed int main(signed int arg0, const char ** arg1) {
  (*&fizzbuzz)((signed int)300);
  return (signed int)0;
}
