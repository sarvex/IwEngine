#pragma once

#include "iwlog.h"
#include "sink/sink.h"
#include "sink/std_sink.h"
#include "sink/file_sink.h"
#include <vector>
#include <sstream>

namespace iwlog {
	class IWLOG_API logger {
	private:
		struct {
			std::vector<sink*> m_sinks;
			std::string preamble = "[Pream] Start of log...";
		};

	public:
		template<
			typename _sink_t,
			typename... _args_t>
		void make_sink(
			loglevel level,
			_args_t&& ... args)
		{
			_sink_t* sink = new _sink_t(level, std::forward<_args_t>(args)...);
			m_sinks.push_back(sink);

			sink->log(preamble);
		}

		template<
			typename _t>
		void log(
			loglevel level,
			const _t& obj)
		{
			std::stringstream string;
			string << obj;

			sink_msg(level, string.str());
		}

		void flush();
		static logger& instance();
	private:
		logger() = default;
		logger(logger const& other) = delete;
		logger(logger&& other) = delete;

		void sink_msg(
			loglevel level,
			std::string string);
	};

	class IWLOG_API log_view {
	private:
		loglevel m_level;

	public:
		log_view(loglevel level);

		template<
			typename _t>
		log_view& operator<<(
			const _t& msg)
		{
			logger::instance().log(m_level, msg);
			return *this;
		}
	};
}

#define LOG_SINK(_type_, _level_, ...)                               \
	iwlog::logger::instance().make_sink<_type_>(_level_, __VA_ARGS__)

#define LOG_FLUSH   iwlog::logger::instance().flush

#define LOG_INFO    iwlog::log_view(iwlog::INFO)
#define LOG_DEBUG   iwlog::log_view(iwlog::DEBUG)
#define LOW_WARNING iwlog::log_view(iwlog::WARNING)
#define LOG_ERROR   iwlog::log_view(iwlog::ERR)
#define LOG_TRACE   iwlog::log_view(iwlog::TRACE)
