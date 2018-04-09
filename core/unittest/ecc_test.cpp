#include "../ecc_native.h"
#include <iostream>

void TestFailed(const char* szExpr, uint32_t nLine)
{
	printf("Test failed! Line=%u, Expression: %s", nLine, szExpr);
	abort();
}

#define verify(x) \
	do { \
		if (!(x)) \
			TestFailed(#x, __LINE__); \
	} while (false);

namespace ECC {

void GenerateRandom(void* p, uint32_t n)
{
	for (uint32_t i = 0; i < n; i++)
		((uint8_t*) p)[i] = (uint8_t) rand();
}

void SetRandom(uintBig& x)
{
	GenerateRandom(x.m_pData, sizeof(x.m_pData));
}

void SetRandom(Scalar::Native& x)
{
	Scalar s;
	while (true)
	{
		SetRandom(s.m_Value);
		if (!x.Import(s))
			break;
	}
}

Context g_Ctx;
const Context& Context::get() { return g_Ctx; }

void TestScalars()
{
	Scalar::Native s0, s1, s2;
	s0 = 17U;

	// neg
	s1 = -s0;
	verify(!(s1 == Zero));
	s1 += s0;
	verify(s1 == Zero);

	// inv, mul
	s1.SetInv(s0);

	s2 = -s1;
	s2 += s0;
	verify(!(s2 == Zero));

	s1 *= s0;
	s2 = 1U;
	s2 = -s2;
	s2 += s1;
	verify(s2 == Zero);

	// import,export

	for (int i = 0; i < 1000; i++)
	{
		SetRandom(s0);

		Scalar s_;
		s0.Export(s_);
		s1.Import(s_);

		s1 = -s1;
		s1 += s0;
		verify(s1 == Zero);
	}
}

void TestPoints()
{
	// generate, import, export
	Point::Native p0, p1;

	for (int i = 0; i < 1000; i++)
	{
		Point p_;
		SetRandom(p_.m_X);
		p_.m_bQuadraticResidue = 0 != (1 & i);

		while (!p0.Import(p_))
		{
			verify(p0 == Zero);
			p_.m_X.Inc();
		}
		verify(!(p0 == Zero));

		p1 = -p0;
		verify(!(p1 == Zero));

		p1 += p0;
		verify(p1 == Zero);
	}

	// multiplication
	Scalar::Native s0, s1;
	Scalar s_;

	s0 = 1U;

	Point::Native g;
	g = Context::get().G * s0;
	verify(!(g == Zero));

	s0 = Zero;
	p0 = Context::get().G * s0;
	verify(p0 == Zero);

	s0.Export(s_);
	p0 += g * s_;
	verify(p0 == Zero);

	for (int i = 0; i < 300; i++)
	{
		SetRandom(s0);

		p0 = Context::get().G * s0; // via generator

		s1 = -s0;
		p1 = p0;
		p1 += Context::get().G * s1; // inverse, also testing +=
		verify(p1 == Zero);

		s1.Export(s_);
		p1 = p0;
		p1 += g * s_; // simple multiplication

		verify(p1 == Zero);
	}

	// H-gen
	Point::Native h;
	h = Context::get().H * 1U;
	verify(!(h == Zero));

	p0 = Context::get().H * 0U;
	verify(p0 == Zero);

	for (int i = 0; i < 300; i++)
	{
		Amount val;
		GenerateRandom(&val, sizeof(val));

		p0 = Context::get().H * val; // via generator

		s0 = val;
		s0.Export(s_);

		p1 = Zero;
		p1 += h * s_;
		p1 = -p1;
		p1 += p0;

		verify(p1 == Zero);
	}

	// doubling, all bits test
	s0 = 1U;
	s1 = 2U;
	p0 = g;

	for (int nBit = 1; nBit < 256; nBit++)
	{
		s0 *= s1;
		p1 = Context::get().G * s0;
		verify(!(p1 == Zero));

		p0 = p0 * Two;
		p0 = -p0;
		p0 += p1;
		verify(p0 == Zero);

		p0 = p1;
	}

}

} // namespace ECC

//int main()
//{
//    return 0;
//}