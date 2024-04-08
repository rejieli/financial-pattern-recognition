#include "csvreader.h"

#include <string>
#include <iostream>

namespace csvreader {
	// The file path must contain exactly stock::NUM_PERIODS worth of time periods.
	stock::StockMinuteDaily read_bars(std::string file_path) {
		stock::StockMinuteDaily daily_chart{};

		//Read file
		io::CSVReader<6, io::trim_chars<>, io::no_quote_escape<';'>> csv_reader(file_path);
		csv_reader.read_header(io::ignore_extra_column, "Time", "Open", "Low", "High", "Close", "Volume");

		int time, volume;
		float open, low, high, close;
		int counter = 0;
		// Loop to read each row of the file
		while (csv_reader.read_row(time, open, low, high, close, volume)) {
			if (counter >= stock::NUM_PERIODS) {
				break;
			}
			daily_chart.bars.at(counter++) = stock::Bar{ time, open, low, high, close, volume };
		}
		return daily_chart;
	}
}