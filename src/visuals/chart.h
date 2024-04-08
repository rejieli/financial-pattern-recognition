#pragma once

#include "../numerics/stock.h"
#include <chartdir.h>
#include <vector>
#include <FinanceChart.h>
#include "../math/math.h"

namespace chart{
	
	void DrawBarChart(stock::StockMinuteDaily const& daily_chart, std::vector<algomath::LineSegment> const& trends);

	void DrawTrendLines(FinanceChart& c, std::vector<algomath::LineSegment> const& trends);

	void DrawPeakAndTrough(XYChart* mainChart, stock::StockMinuteDaily const& daily_chart);
	
	void DrawAscendingChannel(XYChart* mainChart, stock::StockMinuteDaily const& daily_chart, std::vector<stock::PeakAndTrough> const& peak_and_trough);

}