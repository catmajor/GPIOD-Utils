#ifndef SHIFT_REG_HPP
#define SHIFT_REG_HPP
#include <gpiod.hpp>
#include <chrono>
#include <thread>
#include <string>
#include <optional>
namespace gpiodutils {
	using namespace gpiod;
	using namespace std::chrono;
	using namespace std::this_thread;
	using std::optional;
	class shift_register {
		public:
			struct offsets {
				const int& data_offset;
				const int& mem_clk_offset;
				const int& r_clk_offset;
				optional<int> mem_clr_offset;
				optional<int> disp_offset;	
			};
			struct lines {
				const line& data;
				const line& mem_clk;
				const line& r_clk;
				optional<line> mem_clr;
				optional<line> disp;
			};
			shift_register(const std::string&, offsets, int num_outputs = 8);
			shift_register(const chip&, offsets, int num_outputs = 8);
			shift_register(lines, int num_outputs = 8);
			shift_register& set_val(int);
			int get_val();
			shift_register& enable_display(bool);
			~shift_register();
			void request();
			void release();
			template <typename Rep, typename Period>
			void set_delay(std::chrono::duration<Rep, Period> d);
		private:
			void init_lines(const chip& chip, offsets);
			line DATA;
			line MCLK;
			line RCLK;
			line MCLR;
			line DISP;
			int cur_val = 0;
			int num_outputs = 8;
			int num_output_bitmask = 0;
			bool mem_clr_used = false;
			bool disp_used = false;
			bool released = true;
			nanoseconds delay = nanoseconds(0);
	};
	shift_register& operator<<(shift_register&, int);
	shift_register& operator>>(shift_register&, int&);
	template <typename Rep, typename Period>
	void shift_register::set_delay(std::chrono::duration<Rep, Period> d) {
		delay = duration_cast<nanoseconds>(d);
	}
}

#endif
