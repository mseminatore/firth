#include "firth.h"

//
static int fadd(Firth *pFirth)
{
	auto a = pFirth->popf();
	auto b = pFirth->popf();
	pFirth->pushf(a + b);

	return FTH_TRUE;
}

static int fsub(Firth *pFirth)
{
	auto a = pFirth->popf();
	auto b = pFirth->popf();
	pFirth->pushf(a - b);

	return FTH_TRUE;
}

//
static int fmul(Firth *pFirth)
{
	auto a = pFirth->popf();
	auto b = pFirth->popf();
	pFirth->pushf(a * b);

	return FTH_TRUE;
}

static int fdiv(Firth *pFirth)
{
	auto a = pFirth->popf();
	auto b = pFirth->popf();
	pFirth->pushf(a / b);

	return FTH_TRUE;
}

static int fdot(Firth *pFirth)
{
	auto a = pFirth->popf();
	pFirth->firth_printf("%f", a);

	return FTH_TRUE;
}

static int fsin(Firth *pFirth)
{
	auto a = pFirth->popf();
	pFirth->pushf(sin(a));

	return FTH_TRUE;
}

static int fcos(Firth *pFirth)
{
	auto a = pFirth->popf();
	pFirth->pushf(cos(a));

	return FTH_TRUE;
}

static int ftan(Firth *pFirth)
{
	auto a = pFirth->popf();
	pFirth->pushf(tan(a));

	return FTH_TRUE;
}

static int fln(Firth *pFirth)
{
	auto a = pFirth->popf();
	pFirth->pushf(log(a));

	return FTH_TRUE;
}

static int fexp(Firth *pFirth)
{
	auto a = pFirth->popf();
	pFirth->pushf(exp(a));

	return FTH_TRUE;
}

static int fabsolute(Firth *pFirth)
{
	auto a = pFirth->popf();
	pFirth->pushf(fabs(a));

	return FTH_TRUE;
}

static int fsqrt(Firth *pFirth)
{
	auto a = pFirth->popf();
	pFirth->pushf(sqrtf(a));

	return FTH_TRUE;
}

static int ffetch(Firth *pFirth)
{
	FirthFloat *pNum = (FirthFloat*)pFirth->pop();
	auto val = *pNum;
	pFirth->pushf(val);

	return FTH_TRUE;
}

static int fstore(Firth *pFirth)
{
	FirthFloat *pNum = (FirthFloat*)pFirth->pop();
	auto val = pFirth->popf();
	*pNum = val;

	return FTH_TRUE;
}

static int fdepth(Firth *pFirth)
{
	pFirth->push(pFirth->fdepth());

	return FTH_TRUE;
}

static int fdup(Firth *pFirth)
{
	auto a = pFirth->topf();
	pFirth->pushf(a);

	return FTH_TRUE;
}

static int fdrop(Firth *pFirth)
{
	pFirth->popf();

	return FTH_TRUE;
}

static int fswap(Firth *pFirth)
{
	auto a = pFirth->popf();
	auto b = pFirth->popf();
	
	pFirth->pushf(a);
	pFirth->pushf(b);

	return FTH_TRUE;
}

static int dotf(Firth *pFirth)
{
	std::stack<FirthFloat> fs;

	pFirth->firth_printf("Top -> [ ");

	while (pFirth->fdepth())
	{
		auto f = pFirth->popf();
		pFirth->firth_printf("%f ", f);
		fs.push(f);
	}

	pFirth->firth_printf("]\n");

	while (fs.size())
	{
		pFirth->pushf(fs.top());
		fs.pop();
	}

	return FTH_TRUE;
}

// register our collection of custom words
static const struct FirthWordSet float_lib[] =
{
	{ "F+", fadd },
	{ "F-", fsub },
	{ "F*", fmul },
	{ "F/", fdiv },
	{ "F.", fdot },
	{ "Fsin", fsin},
	{ "Fcos", fcos },
	{ "Ftan", ftan },
	{ "Fln", fln },
	{ "Fexp", fexp },
	{ "Fabs", fabsolute },
	{ "Fsqrt", fsqrt },
	{ "F@", ffetch },
	{ "F!", fstore },
	{ "FDEPTH", fdepth },
	{ "FDUP", fdup },
	{ "FDROP", fdrop },
	{ "FSWAP", fswap },
	{ ".F", dotf },

	{ nullptr, nullptr }
};

//
int firth_register_float(Firth *pFirth)
{
	return pFirth->register_wordset(float_lib);
}