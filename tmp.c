// generated by /mnt/c/Users/elliot/Documents/GitHub/cthulhu-private/tests/pl0/import-0/main.pl0

extern int printf(const char *, ...);
extern int printf(const char *, ...);
int lhs[1];
int rhs[1];
int result[1];
void entry(void);
void add(void);
void main(void);
int lhs[1] = { 0 };
int rhs[1] = { 0 };
int result[1] = { 0 };
void entry(void) {
	lhs[0] = 25;
	rhs[0] = 50;
	add();
	printf("%d\x0a", (result[0]));
}
void add(void) {
	int tmp[1];
	tmp[0] = ((lhs[0]) + (rhs[0]));
	result[0] = (tmp[0]);
}
void main(void) {
	entry();
}
