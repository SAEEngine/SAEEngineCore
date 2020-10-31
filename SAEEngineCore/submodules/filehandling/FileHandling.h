#pragma once

#include "Logging.h"
#include <string>
#include <fstream>
#include <optional>
namespace sae::engine::core
{

	std::optional<std::string> OpenFile(std::string filename);

}
