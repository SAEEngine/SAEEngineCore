#include "SAEEngineCore_Logging.h"

#include <mutex>
#include <iostream>

namespace sae::engine::core
{

	namespace
	{
		std::mutex LOG_MTX{};
	}

	void LogStream::_Pre_Write() const noexcept
	{
		LOG_MTX.lock();
	};
	void LogStream::_Post_Write() const noexcept
	{
#pragma warning( suppress: 26110 )
		LOG_MTX.unlock();
	};


	void LogStream::flush()
	{
		this->_Pre_Write();
		this->ostr_->flush();
		this->_Post_Write();
	}; 
	
	
	LogStream::LogStream(std::ostream* _ostr) noexcept :
		ostr_{ _ostr }
	{};


	log_t lout{ &std::cout };

}