// CMakeProject1.cpp : Defines the entry point for the application.
//

#include "CMakeProject1.h"
#include "io/csvreader.h"
#include "numerics/stock.h"
#include "math/math.h"
#include "visuals/chart.h"

using namespace std;

int main()
{
    stock::StockMinuteDaily daily_chart = csvreader::read_bars("E:\\Auorawave Algo\\Software\\trader\\Client0\\JarOutput\\data\\US\\realtimedata\\2022\\2022-02\\2022-02-03\\U\\U_realtime1minutedata.csv");
	
    algomath::LineSegment segment = algomath::CalcLineSegment(daily_chart.bars.at(2), daily_chart.bars.at(222));

    std::array<bool, stock::NUM_PERIODS> periods = algomath::SlopeIntersectBars<stock::NUM_PERIODS>(daily_chart, segment);

    for(int i = stock::NUM_PERIODS - 1; i >= 0 ; i--){
        cout << "Period: " << daily_chart.bars.at(i).period;
        cout << " Open: " << daily_chart.bars.at(i).open;
        cout << " Low: " << daily_chart.bars.at(i).low;
        cout << " High: " << daily_chart.bars.at(i).high;
        cout << " Close: " << daily_chart.bars.at(i).close;
        cout << " Intersect Line: " << periods.at(i);
        cout << endl;
    }

    std::vector<algomath::LineSegment> trends = algomath::IdentifyTrends(daily_chart);

    for (int i = 0; i < trends.size(); i++) {
        cout << "Start Period: " << trends.at(i).start_period;
        cout << " Start Price: " << trends.at(i).start_val;
        cout << " End Period: " << trends.at(i).end_period;
        cout << " End Price: " << trends.at(i).end_val;
        cout << " Slope: " << trends.at(i).slope;
        cout << endl;
    }
    
    // Make chart
    chart::DrawBarChart(daily_chart, trends);
    cout << "CHART MADE" << endl;
    return 0;
}
