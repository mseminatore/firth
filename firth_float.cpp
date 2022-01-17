#include "firth.h"

//
static int fadd(Firth *pFirth)
{
	auto a = pFirth->popf();
	auto b = pFirth->popf();
	pFirth->pushf(a + b);

	return F_TRUE;
}

static int fsub(Firth *pFirth)
{
	auto a = pFirth->popf();
	auto b = pFirth->popf();
	pFirth->pushf(a - b);

	return F_TRUE;
}

//
static int fmul(Firth *pFirth)
{
	auto a = pFirth->popf();
	auto b = pFirth->popf();
	pFirth->pushf(a * b);

	return F_TRUE;
}

static int fdiv(Firth *pFirth)
{
	auto a = pFirth->popf();
	auto b = pFirth->popf();
	pFirth->pushf(a / b);

	return F_TRUE;
}

static int fdot(Firth *pFirth)
{
	auto a = pFirth->popf();
	pFirth->firth_printf("%f", a);

	return F_TRUE;
}

// register our collection of custom words
static const struct FirthRegister float_lib[] =
{
	{ "F+", fadd },
	{ "F-", fsub },
	{ "F*", fmul },
	{ "F/", fdiv },
	{ "F.", fdot },
	{ nullptr, nullptr }
};

//
int firth_register_float(Firth *pFirth)
{
	return pFirth->register_words(float_lib);
}