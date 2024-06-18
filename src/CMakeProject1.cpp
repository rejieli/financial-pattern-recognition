#include "CMakeProject1.h"
#include "io/csvreader.h"
#include "numerics/stock.h"
#include "math/math.h"
#include "visuals/chart.h"

using namespace std;

int main()
{
    // Load CSV file
    stock::StockMinuteDaily daily_chart = csvreader::read_bars("{CSV FILE HERE}");
	   
    // Identify local peak/troughs
    std::vector<algomath::LineSegment> trends = algomath::IdentifyTrends(daily_chart);

    // Make chart
    chart::DrawBarChart(daily_chart, trends);

    return 0;
}
