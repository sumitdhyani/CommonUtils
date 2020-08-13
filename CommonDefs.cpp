#include "CommonDefs.h"

namespace ULCommonUtils
{
	time_point now()
	{
		return std::chrono::system_clock::now();
	}
}