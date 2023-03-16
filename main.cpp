#include "main.h"

constexpr int STATE_ON = 1;
constexpr int STATE_OFF = 0;

#define CHIP_NAME "gpiochip0"
#define CONSUMER_NAME "modulatedLED"
#define LINE_NAME "GPIO18"

gpiod::line prepare_requested_line(gpiod::chip& chip, gpiod::line_request& line_config)
{
	gpiod::line current_line = chip.find_line(LINE_NAME);
	current_line.request(line_config);
	return current_line;
}

gpiod::line_request line_config_setup(const std::string& consumer_name)
{
	gpiod::line_request line_request = gpiod::line_request();
	line_request.consumer = consumer_name;
	line_request.request_type = gpiod::line_request::DIRECTION_OUTPUT;
	return line_request;
}

int main()
{
	gpiod::chip chip(CHIP_NAME);

	auto line_config = line_config_setup(CONSUMER_NAME);
	auto current_line = prepare_requested_line(chip, line_config);

	while (true) {
		current_line.set_value(current_line.get_value() ? STATE_OFF : STATE_ON);

		int time_offset = (time_offset + 1) % 1000;
		auto sleeptime = current_line.get_value() ? std::chrono::microseconds(1000 - time_offset)
			: std::chrono::microseconds(time_offset);
		std::this_thread::sleep_for(sleeptime);
	}

	chip.get_all_lines().release();
}