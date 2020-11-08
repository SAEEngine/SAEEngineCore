#pragma once

#include <SAEEngineCore_Config.h>

#include <ostream>

namespace sae::engine::core
{

	struct LogStream
	{
	private:
		void _Pre_Write() const noexcept;
		void _Post_Write() const noexcept;

	public:

		template <typename T>
		friend inline LogStream& operator<<(LogStream& _lstr, const T& _t)
		{
			_lstr._Pre_Write();
			(*_lstr.ostr_) << _t;
			_lstr._Post_Write();
			return _lstr;
		};
		
		void flush();

		LogStream(std::ostream* _ostr) noexcept;

	private:
		std::ostream* ostr_ = nullptr;

	};

	using log_t = LogStream;
	
	extern log_t lout;

}