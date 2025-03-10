#include "swap_chain.h"

bool SwapchainSupportDetails::IsAdequate()
{
	return !formats.empty() && !presentModes.empty();
}
