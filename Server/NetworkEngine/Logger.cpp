#include "pch.h"
#include "Logger.h"
#include "PathManager.h"

static const char* GetTag(ELogLevel level)
{
	switch (level)
	{
	case ELogLevel::Info:
		return "Info";
	case ELogLevel::Debug:
		return "Debug";
	case ELogLevel::Warn:
		return "Warn";
	case ELogLevel::Error:
		return "Error";
	case ELogLevel::Fatal:
		return "Fatal";
	default:
		return "Unknown";
	}
}

Logger* GLogger = Logger::GetInstance();

Logger::Logger()
	:
	mExitFlag(false),
	mConsoleLog(true),
	mFlushDurationMilliSec(500),
	mWorker([this]() { this->flush(); }),
	mLogLevel(ELogLevel::Info)
{
	char buffer[MAX_PATH];
	::GetModuleFileName(NULL, buffer, MAX_PATH);
	basePath = buffer;

	size_t pos = basePath.find_last_of('\\');
	basePath = basePath.substr(0, pos);
}

Logger::~Logger()
{
	{
		std::lock_guard<std::mutex> lk(mSync);
		mExitFlag = true;
		mCV.notify_one();
	}

	if (mWorker.joinable())
	{
		mWorker.join();
	}

	if (mOutFile.is_open())
	{
		mOutFile.close();
	}
}

void Logger::SetFlushDuration(int durationMs)
{
	mFlushDurationMilliSec = durationMs;
}

void Logger::Out(ELogLevel level, std::thread::id thread_id, int line, const char* function, const char* fmt, ...)
{
	if (level < mLogLevel)
		return;

	DateTime now = DateTime::Now();

	const char* threadIdStr = StdThreadIdStr::Get();

	std::string logstr;
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	int size = vsnprintf(nullptr, 0, fmt, arg_ptr) + 1;
	if (size > 1)
	{
		logstr.resize(size, '\0');
		vsnprintf(&logstr[0], size, fmt, arg_ptr);
		logstr.pop_back();
	}
	va_end(arg_ptr);

	std::string message = Format::format("[%s] [%s] %s [%d] : %s\n", now.toString().c_str(), threadIdStr, function, line, logstr.c_str());

	{
		std::lock_guard<std::mutex> lk(mSync);
		mBuffer << message;
	}
}

void Logger::write(const std::string& logs)
{
	try 
	{
		if (mOutFile.is_open() == false)
		{
			DateTime now = DateTime::Now();
			std::string log_file_name = Format::format("%s%d.%d.%d.log", programName.c_str(), now.Year(), now.Month(), now.Day());
			std::string log_file_path = Format::format("%s\\%s", basePath.c_str(), log_file_name.c_str());

			mOutFile.open(log_file_path, std::ios_base::out | std::ios_base::app);
		}

		if (mOutFile.is_open() == false)
		{
			return;
		}

		mOutFile << logs;
		mOutFile.flush();

		if (mConsoleLog)
		{
			std::cout << logs;
		}
	}
	catch (std::exception e)
	{
		std::cerr << "log write failed : " << e.what() << std::endl;
	}
}

void Logger::flush()
{
	std::string logs;
	const auto duration = std::chrono::milliseconds(mFlushDurationMilliSec);
	while (!mExitFlag)
	{
		std::unique_lock<std::mutex> lk(mSync);

		mCV.wait_for(lk, duration, [this, &logs](){ return mExitFlag; });

		if (logs.empty())
		{
			logs = mBuffer.str();
		}

		mBuffer.str("");

		lk.unlock();
	
		if (!logs.empty())
		{
			write(logs);
			logs.clear();
		}
	}
}