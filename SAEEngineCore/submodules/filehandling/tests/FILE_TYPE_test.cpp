#include "FileHandling.h"

#include <cassert>

namespace eng = sae::engine::core;

struct FileType_Enumerator_Check : eng::FILE_TYPE
{
public:
	static void test()
	{
		for (enum_integral_type n = 0; n < (enum_integral_type)FILE_TYPE_E::FILE_TYPE_E_MAX_VALUE; ++n)
		{
			auto _res = eng::FILE_TYPE::enum_to_str((FILE_TYPE_E)n);
			assert(_res);
			assert(eng::FILE_TYPE::str_to_enum(*_res) == (FILE_TYPE_E)n);
		};
	};
};

int main()
{
	FileType_Enumerator_Check::test();

	assert(eng::FILE_TYPE{ ".png" } == eng::FILE_TYPE::F_PNG);
	assert(eng::FILE_TYPE{ ".txt" } == eng::FILE_TYPE::F_TXT);

#ifndef SAE_ENGINE_CORE_HARD_ERRORS
	assert(eng::FILE_TYPE{ ".NOT_A_REAL_FILE_EXTENSION_HAHA" } == eng::FILE_TYPE::BAD_VALUE);
#endif

	return 0;
};
