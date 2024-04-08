#include "patterns.h"
#include <cstdlib>

namespace patterns {

	// To classify as an ascending channel, the peak_and_trough must create higher highs and higher lows for at least 3 highs and 3 lows, where linear regression of both bottom and top line are parallel
	std::vector<AscendingChannel>  IdentifyAscendingChannel(stock::StockMinuteDaily const& daily_chart, std::vector<stock::PeakAndTrough> const& peak_and_trough) {
		// Results
		std::vector<AscendingChannel> ascendingChannels{};
		// Identify region of higher highs and high lows
		uint16_t peak_count = 0, trough_count = 0;

		stock::IndexAndPriceVector highs{};
		stock::IndexAndPriceVector lows{};

		stock::IndexAndPrice previous_high { 0, 0 };
		stock::IndexAndPrice current_high  { 0, 0 };

		stock::IndexAndPrice previous_low{ 0, 0 };
		stock::IndexAndPrice current_low{ 0, 0 };

		for (const auto& pt : peak_and_trough) {
			if (pt.is_peak) { 
				// Ensure higher highs
				current_high.index = pt.index;
				current_high.price = daily_chart.bars.at(pt.index).high;

				if (previous_high.price < current_high.price) { // If higher highs
					// Keep track of this peak
					highs.index.push_back(pt.index);
					highs.price.push_back(current_high.price);
					peak_count++;
				}
				else { // If we have a lower high
					// Check if peak_count >= 3 && trough_count >= 3 (see if ascending channel has been formed)
					if (peak_count >= 1 && trough_count >= 1) {
						// Create new ascending channel using linear regression
						algomath::Line top_line = algomath::LinearRegression(highs.index, highs.price);
						algomath::Line bottom_line = algomath::LinearRegression(lows.index, lows.price);
						// Check if the slopes are parallel to a certain extend: within 10% difference
						if (abs(algomath::PercentChange(top_line.slope, bottom_line.slope) < 10)){ 
						
						// adjust bottom line to be parallel to top_line
						algomath::AdjustSlope(top_line, bottom_line, lows.index.at(0));
						
						AscendingChannel channel{ highs, lows, top_line, bottom_line };
						ascendingChannels.push_back(channel);
						}
					}
					highs.clear();
					lows.clear();	

					peak_count = 0;
				}
				previous_high = current_high;
			}
			else {
				// Ensure higher lows
				current_low.index = pt.index;
				current_low.price = daily_chart.bars.at(pt.index).low;

				if (previous_low.price < current_low.price) { // If higher highs
					// Keep track of this peak
					lows.index.push_back(pt.index);
					lows.price.push_back(current_low.price);

					trough_count++;
				}
				else { // If we have a lower low
					// Check if peak_count >= 3 && trough_count >= 3 (see if ascending channel has been formed)
					if (peak_count >= 1 && trough_count >= 1) {
						// Create new ascending channel using linear regression
						algomath::Line top_line = algomath::LinearRegression(highs.index, highs.price);
						algomath::Line bottom_line = algomath::LinearRegression(lows.index, lows.price);
						if (abs(algomath::PercentChange(top_line.slope, bottom_line.slope) < 10)) {
							// adjust bottom line to be parallel to top_line
							algomath::AdjustSlope(top_line, bottom_line, lows.index.at(0));
							AscendingChannel channel{ highs, lows, top_line, bottom_line };
							ascendingChannels.push_back(channel);
						}
					}
					highs.clear();
					lows.clear();

					trough_count = 0;
				}
				previous_low = current_low;
			}
		}
		return ascendingChannels;
	}

}