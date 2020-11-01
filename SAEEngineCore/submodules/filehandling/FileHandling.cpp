#include "FileHandling.h"

#include <fstream>
#include <unordered_map>

#include <cassert>

namespace sae::engine::core
{
	namespace
	{
		// The map for string to FileType::FILE_TYPE_E conversions
		const static inline std::unordered_map<std::string, FILE_TYPE::FILE_TYPE_E> FileType_STRING_TO_ENUM
		{
			{ ".png", FILE_TYPE::FILE_TYPE_E::F_PNG },
			{ ".txt", FILE_TYPE::FILE_TYPE_E::F_TXT }
		};

		// The map for FileType::FILE_TYPE_E to string conversions
		const static inline std::unordered_map<FILE_TYPE::FILE_TYPE_E, std::string> FileType_ENUM_TO_STRING
		{
			{ FILE_TYPE::FILE_TYPE_E::F_PNG, ".png" },
			{ FILE_TYPE::FILE_TYPE_E::F_TXT, ".txt" }

		};
	};

	FILE_TYPE::FILE_TYPE_E FILE_TYPE::str_to_enum(const std::string& _str)
	{
		// Call find to return an iterator to the pair. If the string isn't in the map, this should be set to map::end();
		auto _at = FileType_STRING_TO_ENUM.find(_str);
		if (_at != FileType_STRING_TO_ENUM.end())
		{
			// Return the associated enum
			return _at->second;
		}
		else
		{
			// Handle bad string value
#ifdef SAE_ENGINE_CORE_HARD_ERRORS
			abort();	// abort if using hard errors
#else
			return FILE_TYPE::FILE_TYPE_E::BAD_VALUE; // return BAD_VALUE otherwise
#endif
		};
	};
	std::optional<const char*> FILE_TYPE::enum_to_str(FILE_TYPE_E _e) // if the filetype is BAD_VALUE, this is a hard error.
	{
		// Check that the FILE_TYPE_E value is valid
		if (_e != FILE_TYPE_E::BAD_VALUE)
		{
			assert(FileType_ENUM_TO_STRING.count(_e) != 0);	// sanity check asserting that the enum conversion exists
			return FileType_ENUM_TO_STRING.at(_e).c_str();	// return the associated string conversion
		}
		else
		{
#ifdef SAE_ENGINE_CORE_HARD_ERRORS
			abort();	// abort if using hard errors
#elif defined(SAE_ENGINE_CORE_USE_EXCEPTIONS)
			throw std::invalid_argument{ "Cannot converted FILE_TYPE::BAD_VALUE to a file extension string" };	// throw exception if they are enabled
#endif
			return std::nullopt; // return nullopt otherwise
		};
	};


	std::optional<const char*> FILE_TYPE::to_cstring() const
	{
		return this->enum_to_str(this->type_);
	};

	FILE_TYPE& FILE_TYPE::operator=(FILE_TYPE_E _ft) noexcept
	{
		this->type_ = _ft;
		return *this;
	};

	FILE_TYPE::FILE_TYPE(const std::string& _str) : 
		type_{ FILE_TYPE::str_to_enum(_str) }
	{};
	FILE_TYPE& FILE_TYPE::operator=(const std::string& _str)
	{
		this->type_ = FILE_TYPE::str_to_enum(_str);
		return *this;
	};

}


namespace sae::engine::core
{
	

	std::optional<std::string> OpenFile(std::string filename) 
	{

		std::ifstream file(filename);		// good job using an ifstream as thats all that is needed

		if (file)
		{

			std::string data;		// consider using a std::vector<char> for the data storage as strings are not ment to store information
			file >> data;

			return data;		// Consider creating a type for storing a loaded file's data
			file.close();

		}
			// Id recommend against returning an error as a string as string comparisons are slow
			// Consider returning a std::optional<T> where T is your data storage type ✔
		return std::nullopt;

		// Go look at the documentation for fstreams and see how error checking and handling is preformed.
		//		https://en.cppreference.com/w/cpp/io/basic_fstream 
		// ✔
	}

}