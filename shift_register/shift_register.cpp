#include "shift_register.hpp"
namespace gpiodutils {
	using namespace gpiod;
	using namespace std::chrono;
	using namespace std::this_thread;
	const int ONE = 1;
	shift_register::shift_register(const std::string& chip_name, line_offset_int params, int num_outputs) : shift_register(chip{chip_name}, params, num_outputs){
	}
	shift_register::shift_register(const chip& chip_obj, line_offset_int params, int num_outputs)
	: num_outputs {num_outputs}, delay {nanoseconds(0)} {
		init_lines(chip_obj, params);
		num_output_bitmask = ((1 << num_outputs) - 1); 
	}
	shift_register::shift_register(line_offset_line_4 params, int num_outputs)
	: shift_register({params.data, params.mem_clk, params.r_clk}, num_outputs) {
		mem_clr_used = true;
		MCLR = params.mem_clr;
		MCLR.request({"shift register", line_request::DIRECTION_OUTPUT, 0},0); 	
	}
	shift_register::shift_register(line_offset_line_3 params, int num_outputs)
	: DATA {params.data}, MCLK {params.mem_clk}, RCLK {params.r_clk}, num_outputs {num_outputs}, delay {nanoseconds(0)} {
		num_output_bitmask = ((1 << num_outputs) - 1);
		mem_clr_used = false;
		DATA.request({"shift register", line_request::DIRECTION_OUTPUT, 0},0); 
		MCLK.request({"shift register", line_request::DIRECTION_OUTPUT, 0},0); 
		RCLK.request({"shift register", line_request::DIRECTION_OUTPUT, 0},0); 

	}
	void shift_register::init_lines(const chip& chip_obj, line_offset_int params) {
		DATA = chip_obj.get_line(params.data_offset);
		MCLK = chip_obj.get_line(params.mem_clk_offset);
		RCLK = chip_obj.get_line(params.r_clk_offset);
		if (params.mem_clr_offset >= 0) {
			MCLR = chip_obj.get_line(params.mem_clr_offset);
			MCLR.request({"shift register", line_request::DIRECTION_OUTPUT, 0},0); 	
			mem_clr_used = true;
		} else {
			mem_clr_used = false;
		}
		DATA.request({"shift register", line_request::DIRECTION_OUTPUT, 0},0); 
		MCLK.request({"shift register", line_request::DIRECTION_OUTPUT, 0},0); 
		RCLK.request({"shift register", line_request::DIRECTION_OUTPUT, 0},0); 
	}
	void shift_register::set_val(int n) {
		n = num_output_bitmask & n;
		cur_val = n;
		if (mem_clr_used) {
			MCLR.set_value(0);
			sleep_for(delay);
			MCLR.set_value(1);
		}
		for (int i = 0; i < num_outputs; i++) {
			if ((n & ONE) == ONE) {
				DATA.set_value(1);
			} else {
				DATA.set_value(0);
			}
			MCLK.set_value(1);
			sleep_for(delay);
			MCLK.set_value(0);
			sleep_for(delay);
			n = n >> 1;
		}
		RCLK.set_value(1);
		sleep_for(delay);
		RCLK.set_value(0);
	}
	int shift_register::get_val() {
		return cur_val;
	}
	template <typename Rep, typename Period>
	void shift_register::set_delay(std::chrono::duration<Rep, Period> d) {
		delay = duration_cast<nanoseconds>(d);
	}
	shift_register::~shift_register() {
		DATA.set_value(0);
		MCLK.set_value(0);
		RCLK.set_value(0);
		DATA.release();
		MCLK.release();
		RCLK.release();
		if (mem_clr_used) {
			MCLR.set_value(0);
			MCLR.release();
		}
	}
	shift_register& operator<<(shift_register& sr, int n) {
		sr.set_val(n);
		return sr;
	}
	shift_register& operator>>(shift_register& sr, int& n) {
		n = sr.get_val();
		return sr;
	}
}
