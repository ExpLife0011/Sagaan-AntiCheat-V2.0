#include "Formulas.h"
#include <cmath>

double nearest_ten(double n)
{
	return std::round(n / 10.0) * 10.0;
}

namespace HeartbeatFormula
{
	ULONG Formula1(ULONG x)
	{

		ULONG y = x / 2;
		ULONG sum = ((x*(x - 1)) / 2) - ((y*(y + 1)) / 2);
		sum += x * y ^ (x + y) - x;
		return sum;
	}
	ULONG Formula2(ULONG x)
	{

		ULONG y = x * 2;
		ULONG sum = ((x*(x - 1)) / 2) + ((y*(y + 1)) / 2);
		sum += x * y ^ (x + y) - y;
		return sum;
	}
	ULONG Formula3(ULONG x)
	{
		ULONG y = x + x^x - x;
		ULONG sum = ((x*(x - 1)) / 2) - ((y*(y + 1)) / 2);
		sum += x * y ^ (x + y) - x;
		return sum;
	}
	ULONG Formula4(ULONG x)
	{

		ULONG y = x + 2;
		ULONG sum = ((x*(x - 1)) / 2) * ((y*(y + 1)) / 2);
		sum += x * y ^ (x + y) - y;
		return sum;
	}
	ULONG Formula5(ULONG x)
	{
		ULONG y = x * 2 + x;
		ULONG sum = ((x*(x - 1)) / 2) / ((y*(y + 1)) / 2);
		sum += x * y ^ (x + y) - x;
		return sum;
	}

}

