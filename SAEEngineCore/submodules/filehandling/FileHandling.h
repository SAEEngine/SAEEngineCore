#pragma once

#include "../error/Error.h"
#include "../logging/Logging.h"

#include <vector>
#include <optional>
#include <filesystem>
#include <string>
#include <istream>
#include <variant>

#ifdef SAE_ENGINE_CORE_USE_EXCEPTIONS
#include <stdexcept>
#endif

namespace sae::engine::core
{

	/**
	 * @brief Enum wrapper for File types adding to/from string conversions.
	*/
	class FILE_TYPE
	{
	public:

		using enum_integral_type = uint32_t;

		enum FILE_TYPE_E : enum_integral_type
		{
			F_TXT = 0,
			F_PNG,
			BAD_VALUE,
			FILE_TYPE_E_MAX_VALUE = BAD_VALUE
		};

	protected:

		// This should be set to whatever FILE_TYPE_E::BAD_VALUE is equal to just not directly. The static asserts below will help prevent
		// desync between this value and the true value of FILE_TYPE_E::BAD_VALUE
		constexpr static inline size_t ENUM_MAX_VALUE = 2;
		static_assert(ENUM_MAX_VALUE == FILE_TYPE_E::FILE_TYPE_E_MAX_VALUE, "FILE_TYPE::FILE_TYPE_E::FILE_TYPE_E_MAX_VALUE doesn't match FILE_TYPE::ENUM_MAX_VALUE");
		static_assert(FILE_TYPE_E::BAD_VALUE == FILE_TYPE_E::FILE_TYPE_E_MAX_VALUE, "FILE_TYPE::FILE_TYPE_E::BAD_VALUE doesn't match FILE_TYPE::FILE_TYPE_E::FILE_TYPE_E_MAX_VALUE");

		// Internal string to enum conversion function
		static FILE_TYPE_E str_to_enum(const std::string& _str);

		// Overload of str_to_enum that disabling abort on bad value
		static FILE_TYPE_E str_to_enum(const std::string& _str, no_abort_t);


		// Internal enum to string conversion function
		static std::optional<const char*> enum_to_str(FILE_TYPE_E _e);

	public:
		explicit operator bool() const = delete;

		/**
		 * @brief Returns true if the set FILE_TYPE_E value is NOT BAD_VALUE.
		*/
		constexpr bool valid() const noexcept
		{
			return this->type_ != FILE_TYPE_E::BAD_VALUE;
		};

		/**
		 * @brief Returns the held FILE_TYPE_E value. This allows FileType to be used in switch statements. 
		*/
		constexpr operator FILE_TYPE_E() const noexcept
		{
			return this->type_;
		};

#if defined(SAE_ENGINE_CORE_USE_EXCEPTIONS)
		/**
		 * @brief Attempts to convert the set FILE_TYPE_E value to a string. Returns nullopt if valid() == false (FILE_TYPE_E == BAD_VALUE).
		 * @exception std::invalid_arguement thrown on valid() == false (FILE_TYPE_E == BAD_VALUE).
		*/
		std::optional<const char*> to_cstring() const;
#else
		/**
		 * @brief Attempts to convert the set FILE_TYPE_E value to a string. Returns nullopt if valid() == false (FILE_TYPE_E == BAD_VALUE).
		*/
		std::optional<const char*> to_cstring() const;
#endif

		// to_cstring() returns a const char* to avoid a string copy from the optional.

		friend inline constexpr bool operator==(FILE_TYPE _lhs, FILE_TYPE_E _rhs) noexcept
		{
			return _lhs.type_ == _rhs;
		};
		friend inline constexpr bool operator!=(FILE_TYPE _lhs, FILE_TYPE_E _rhs) noexcept
		{
			return _lhs.type_ != _rhs;
		};

		friend inline constexpr bool operator==(FILE_TYPE_E _lhs, FILE_TYPE _rhs) noexcept
		{
			return _rhs == _lhs;
		};
		friend inline constexpr bool operator!=(FILE_TYPE_E _lhs, FILE_TYPE _rhs) noexcept
		{
			return _rhs != _lhs;
		};

		// The following comparison operators check for true equality.

		constexpr bool operator==(FILE_TYPE other) const noexcept
		{
			return this->type_ == other.type_;
		};
		constexpr bool operator!=(FILE_TYPE other) const noexcept
		{
			return this->type_ != other.type_;
		};

		/**
		 * @brief Converts the provided string to a FILE_TYPE_E value. Will abort on bad string if SAE_ENGINE_CORE_HARD_ERRORS is defined.
		*/
		FILE_TYPE(const std::string& _str);

		/**
		 * @brief Converts the provided string to a FILE_TYPE_E value. Will set to BAD_VALUE on invalid string.
		*/
		FILE_TYPE(const std::string& _str, no_abort_t);

		/**
		 * @brief Converts the provided string to a FILE_TYPE_E value. Will set to BAD_VALUE on invalid string.
		*/
		FILE_TYPE& operator=(const std::string& _str);

		// istream / ostream operator overloading to support standard IO operations (this can be used with lout)

		friend inline std::ostream& operator<<(std::ostream& _ostr, const FILE_TYPE& _ft)
		{
			auto _out = _ft.to_cstring();
			return (_out)? (_ostr << *_out) : _ostr;
		};
		friend inline std::istream& operator>>(std::istream& _istr, FILE_TYPE& _ft)
		{
			std::string _str{};
			_istr >> _str;
			_ft = _str;
			return _istr;
		};

		constexpr FILE_TYPE() noexcept = default;

		constexpr FILE_TYPE(FILE_TYPE_E _ft) noexcept : 
			type_{ _ft }
		{};
		FILE_TYPE& operator=(FILE_TYPE_E _ft) noexcept;

	private:
		FILE_TYPE_E type_ = FILE_TYPE_E::BAD_VALUE;

	};


	/**
	 * @brief Represents raw file data that has been loaded into memory but not yet sanitized. This is a STL-like container that should be usable
	 * by standard library algorithms.
	*/
	template <typename _AllocT = std::allocator<char>>
	class Basic_UnsanitizedFileData
	{
	private:
		// Because this is a class template, the function definitions MUST go in the header.
		// Same goes for constexpr functions.

	public:
		using value_type = char;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;

		using allocator_type = _AllocT;

		using size_type = size_t;
		using offset_type = int32_t;

	private:
		using ContainerT = std::vector<value_type, _AllocT>;

	public:

		using iterator = typename ContainerT::iterator;
		using const_iterator = typename ContainerT::const_iterator;

		iterator begin() noexcept { return this->raw_data_.begin(); };
		const_iterator begin() const noexcept { return this->raw_data_.cbegin(); };
		const_iterator cbegin() const noexcept { return this->raw_data_.cbegin(); };

		iterator end() noexcept { return this->raw_data_.end(); };
		const_iterator end() const noexcept { return this->raw_data_.cend(); };
		const_iterator cend() const noexcept { return this->raw_data_.cend(); };

		size_type size() const noexcept { return this->raw_data_.size(); };
		size_type capacity() const noexcept { return this->raw_data_.capacity(); };

		void resize(size_type _count) { this->raw_data_.resize(_count); };
		void resize(size_type _count, value_type _value) { this->raw_data_.resize(_count, _value); };

		void reserve(size_type _count) { this->raw_data_.reserve(_count); };









	private:
		FILE_TYPE ftype_{};
		ContainerT raw_data_{};

	};

	using UnsanitizedFileData = Basic_UnsanitizedFileData<>;


	template <FILE_TYPE::FILE_TYPE_E FileType>
	class FileData;






	std::optional<std::string> OpenFile(std::string filename);

}
