#ifndef SHIFT_REG_HPP
#define SHIFT_REG_HPP
#include <gpiod.hpp>
#include <chrono>
#include <thread>
#include <string>
namespace gpiodutils {
	using namespace gpiod;
	using namespace std::chrono;
	using namespace std::this_thread;
	struct line_offset_int {
		const int& data_offset;
		const int& mem_clk_offset;
	      	const int& r_clk_offset;
		int mem_clr_offset = -1;	
	};
	struct line_offset_line_4 {
		const line& data;
		const line& mem_clk;
		const line& r_clk;
		const line& mem_clr;
	};
	struct line_offset_line_3 {
		const line& data;
		const line& mem_clk;
		const line& r_clk;
	};
	class shift_register {
		public:
			shift_register(const std::string&, line_offset_int, int num_outputs = 8);
			shift_register(const chip&, line_offset_int, int num_outputs = 8);
			shift_register(line_offset_line_4, int num_outputs = 8);
			shift_register(line_offset_line_3, int num_outputs = 8);
			void set_val(int);
			int get_val();
			~shift_register();
			template <typename Rep, typename Period>
			void set_delay(std::chrono::duration<Rep, Period>);
		private:
			void init_lines(const chip& chip, line_offset_int);
			line DATA;
			line MCLK;
			line RCLK;
			line MCLR;
			int cur_val = 0;
			int num_outputs = 8;
			int num_output_bitmask = 0;
			bool mem_clr_used = false;
			nanoseconds delay = nanoseconds(0);
	};
	shift_register& operator<<(shift_register&, int);
	shift_register& operator>>(shift_register&, int&);
}
#endif
