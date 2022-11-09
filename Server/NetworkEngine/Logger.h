#pragma once
#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "Singleton.h"
#include "DateTime.h"
#include "Format.h"

namespace ns_logger
{
	enum class ELogLevel
	{
		Info,
		Debug,
		Warn,
		Error,
		Fatal,
	};

	class Logger : public ISingleton<Logger>
	{
		friend class ISingleton<Logger>;
	private:
		bool mExitFlag;
		bool mConsoleLog;
		int mFlushDurationMilliSec;
		std::string basePath;
		std::string programName;

		std::ofstream mOutFile;
		std::stringstream mBuffer;

		std::condition_variable mCV;
		std::thread mWorker;
		std::mutex  mSync;

		ELogLevel mLogLevel;
	private:
		Logger();

	public:
		~Logger();

		std::chrono::steady_clock::time_point st;

		static Logger* getCurrentloggerger() { return getInstance(); }

		void setConsolelogger(bool bConsolelogger) { mConsoleLog = bConsolelogger; }

		void setloggerLevel(ELogLevel eloggerlevel) { mLogLevel = eloggerlevel; }

		void setProgramName(const char* name) { programName = name; }

		template<typename ...Args>
		void out(ELogLevel level, std::thread::id thread_id, int line, const char* function, const char* fmt, Args&&... args)
		{
			if (level < mLogLevel)
				return;

			DateTime now = DateTime::now();
			std::stringstream ss;
			ss << thread_id;

			std::string logger_str = Format::format(fmt, std::forward<Args>(args)...);
			std::string message = Format::format("[%s] [%s] %s [%d] : %s\n", now.toString().c_str(), ss.str().c_str(), function, line, logger_str.c_str());

			{
				std::lock_guard<std::mutex> lk(mSync);
				mBuffer << message;
			}
		}
	private:
		void write(const std::string& log);

		void flush();
	};

	extern Logger* GLogger;
}

#define LOG_FATAL(fmt, ...) ns_logger::GLogger->out(ns_logger::ELogLevel::Fatal, std::this_thread::get_id(), __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) ns_logger::GLogger->out(ns_logger::ELogLevel::Error, std::this_thread::get_id(), __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOG_WARN(fmt, ...) ns_logger::GLogger->out(ns_logger::ELogLevel::Warn, std::this_thread::get_id(), __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOG_DEBUG(fmt, ...) ns_logger::GLogger->out(ns_logger::ELogLevel::Debug, std::this_thread::get_id(), __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOG_INFO(fmt, ...) ns_logger::GLogger->out(ns_logger::ELogLevel::Info, std::this_thread::get_id(), __LINE__, __FUNCTION__, fmt, __VA_ARGS__)

#endif