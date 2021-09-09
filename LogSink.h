#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>
#include <wx/wx.h>
#include <wx/textctrl.h>

namespace spdlog {
	namespace sinks {
		template<typename mutex>
		class LogSink : public spdlog::sinks::base_sink<mutex>
		{
		public:
			LogSink(std::shared_ptr<wxTextCtrl> control);

		protected:
			void sink_it_(const details::log_msg& msg) override;
			void flush_() override;
		private:
			std::shared_ptr<wxTextCtrl> mLogBook;
			std::array<wxColour, level::n_levels> colours;

		};

		template<typename mutex>
		inline LogSink<mutex>::LogSink(std::shared_ptr<wxTextCtrl> control)
			: mLogBook(control)
		{
			colours[level::trace] = wxColour(186, 85, 211);
			colours[level::debug] = wxColour(0, 255, 255);
			colours[level::info] = wxColour(139, 69, 19);
			colours[level::warn] = wxColour(255, 127, 80);
			colours[level::err] = wxColour(128, 0, 0);
			colours[level::critical] = wxColour(255, 140, 0);
			colours[level::off] = wxColour(244, 164, 96);
		}

		template<typename mutex>
		inline void LogSink<mutex>::sink_it_(const details::log_msg& msg)
		{
			memory_buf_t formatted;
			base_sink<mutex>::formatter_->format(msg, formatted);
			mLogBook->Freeze();
			if (msg.color_range_end > msg.color_range_start)
			{
				const wxColour text_colour = mLogBook->GetDefaultAttributes().colFg;
				std::string_view text1(formatted.data(), msg.color_range_start);
				mLogBook->AppendText(std::string(text1));
				
				std::string_view text2(formatted.data() + msg.color_range_start, (msg.color_range_end - msg.color_range_start));
				mLogBook->SetDefaultStyle(wxTextAttr(colours[msg.level]));
				mLogBook->AppendText(std::string(text2));

				std::string_view text3(formatted.data() + msg.color_range_end, (formatted.size() - msg.color_range_end));
				mLogBook->SetDefaultStyle(wxTextAttr(text_colour));
				mLogBook->AppendText(std::string(text3));
			}
			mLogBook->Thaw();
			//mLogBook->AppendText(formatted.data());
		}

		template<typename mutex>
		inline void LogSink<mutex>::flush_()
		{
			//no flush
		}

		using LogSink_mt = LogSink<std::mutex>;
		using LogSink_st = LogSink<details::null_mutex>;

	}

	//
// factory functions
//
	template<typename Factory = spdlog::synchronous_factory>
	inline std::shared_ptr<logger> app_logger_mt(const std::string & logger_name, std::shared_ptr<wxTextCtrl> logbook)
	{
		return Factory::template create<sinks::LogSink_mt>(logger_name, logbook);
	}

	template<typename Factory = spdlog::synchronous_factory>
	inline std::shared_ptr<logger> app_logger_st(const std::string & logger_name, std::shared_ptr<wxTextCtrl> logbook)
	{
		return Factory::template create<sinks::LogSink_st>(logger_name, logbook);
	}
}
