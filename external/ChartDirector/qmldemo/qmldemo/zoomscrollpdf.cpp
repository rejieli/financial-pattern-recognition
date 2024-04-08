#include "zoomscrollpdf.h"
#include <sstream>
#include <math.h>


ZoomScrollPdf::ZoomScrollPdf(QObject *parent) : QObject(parent)
{
    m_currentChart = 0;

    // Load the data
    loadData();
}

ZoomScrollPdf::~ZoomScrollPdf()
{
    delete m_ranSeries;
    delete m_currentChart;
}

//
// Load the data
//
void ZoomScrollPdf::loadData()
{
    // In this example, we just use random numbers as data.
    m_ranSeries = new RanSeries(127);
    m_timeStamps = m_ranSeries->getDateSeries(1827, Chart::chartTime(2015, 1, 1), 86400);
    m_dataSeriesA = m_ranSeries->getSeries(1827, 150, -10, 10);
    m_dataSeriesB = m_ranSeries->getSeries(1827, 200, -10, 10);
    m_dataSeriesC = m_ranSeries->getSeries(1827, 250, -8, 8);
}

//
// Initialize the QChartViewer
//
void ZoomScrollPdf::initChartViewer(QmlChartViewer *viewer)
{
    // Set the full x range to be the duration of the data
    viewer->setFullRange("x", m_timeStamps[0], m_timeStamps[m_timeStamps.len - 1]);

    // Initialize the view port to show the latest 20% of the time range
    viewer->setViewPortWidth(0.2);
    viewer->setViewPortLeft(1 - viewer->getViewPortWidth());

    // Enable mouse wheel zooming by setting the zoom ratio to 1.1 per wheel event
    viewer->setMouseWheelZoomRatio(1.1);

    // Set the maximum zoom to 10 points
    viewer->setZoomInWidthLimit(10.0 / m_timeStamps.len);
}

//
// Draw the chart and display it in the given viewer
//
void ZoomScrollPdf::drawChart(QmlChartViewer *viewer)
{
    // Get the start date and end date that are visible on the chart.
    double viewPortStartDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft());
    double viewPortEndDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft() +
        viewer->getViewPortWidth());

    // Draw the XYChart
    XYChart* c = drawXYChart(viewPortStartDate, viewPortEndDate);

    // Add a title to the chart using 18 pts Times New Roman Bold Italic font
    c->addTitle("   PDF Report Demonstration", "Times New Roman Bold Italic", 18);

    // We need to update the track line too. If the mouse is moving on the chart (eg. if
    // the user drags the mouse on the chart to scroll it), the track line will be updated
    // in the MouseMovePlotArea event. Otherwise, we need to update the track line here.
    if ((!viewer->isInMouseMoveEvent()) && viewer->isMouseOnPlotArea())
        trackLineLabel(c, viewer->getPlotAreaMouseX());

    delete viewer->getChart();
    viewer->setChart(m_currentChart = c);
}

//
// Draw an XYChart using data from startX to endX
//
XYChart* ZoomScrollPdf::drawXYChart(double startX, double endX)
{
    // Get the array indexes that corresponds to the visible start and end dates
    int startIndex = (int)floor(Chart::bSearch(m_timeStamps, startX));
    int endIndex = (int)ceil(Chart::bSearch(m_timeStamps, endX));
    int noOfPoints = endIndex - startIndex + 1;

    // Extract the part of the data array that are visible.
    DoubleArray viewPortTimeStamps = DoubleArray(m_timeStamps.data + startIndex, noOfPoints);
    DoubleArray viewPortDataSeriesA = DoubleArray(m_dataSeriesA.data + startIndex, noOfPoints);
    DoubleArray viewPortDataSeriesB = DoubleArray(m_dataSeriesB.data + startIndex, noOfPoints);
    DoubleArray viewPortDataSeriesC = DoubleArray(m_dataSeriesC.data + startIndex, noOfPoints);

    //
    // At this stage, we have extracted the visible data. We can use those data to plot the chart.
    //

    ///////////////////////////////////////////////////////////////////////////////////////
    // Configure overall chart appearance.
    ///////////////////////////////////////////////////////////////////////////////////////

    // Create an XYChart object of size 650 x 350 pixels, with a white (ffffff) background and grey
    // (aaaaaa) border
    XYChart* c = new XYChart(650, 350, 0xffffff, 0xaaaaaa);

    // Set the plotarea at (55, 55) with width 90 pixels less than chart width, and height 90 pixels
    // less than chart height. Use a vertical gradient from light blue (f0f6ff) to sky blue (a0c0ff)
    // as background. Set border to transparent and grid lines to white (ffffff).
    c->setPlotArea(55, 55, c->getWidth() - 90, c->getHeight() - 90, c->linearGradientColor(0, 55, 0,
        c->getHeight() - 35, 0xf0f6ff, 0xa0c0ff), -1, Chart::Transparent, 0xffffff, 0xffffff);

    // As the data can lie outside the plotarea in a zoomed chart, we need enable clipping.
    c->setClipping();

    // Add a legend box at (55, 30) using horizontal layout. Use 8pts Arial Bold as font. Set the
    // background and border color to Transparent and use line style legend key.
    LegendBox* b = c->addLegend(55, 30, false, "Arial Bold", 8);
    b->setBackground(Chart::Transparent);
    b->setLineStyleKey();

    // Set legend icon style to use line style icon, sized for 8pt font
    c->getLegend()->setLineStyleKey();
    c->getLegend()->setFontSize(8);

    // Set the axis stem to transparent
    c->xAxis()->setColors(Chart::Transparent);
    c->yAxis()->setColors(Chart::Transparent);

    // Add axis title using 10pts Arial Bold Italic font
    c->yAxis()->setTitle("Ionic Temperature (C)", "Arial Bold Italic", 10);

    ///////////////////////////////////////////////////////////////////////////////////////
    // Add data to chart
    ///////////////////////////////////////////////////////////////////////////////////////

    //
    // In this example, we represent the data by lines. You may modify the code below to use other
    // representations (areas, scatter plot, etc).
    //

    // Add a line layer for the lines, using a line width of 2 pixels
    LineLayer* layer = c->addLineLayer();
    layer->setLineWidth(2);

    // In this demo, we do not have too many data points. In real code, the chart may contain a lot
    // of data points when fully zoomed out - much more than the number of horizontal pixels in this
    // plot area. So it is a good idea to use fast line mode.
    layer->setFastLineMode();

    // Now we add the 3 data series to a line layer, using the color red (ff0000), green
    // (00cc00) and blue (0000ff)
    layer->setXData(viewPortTimeStamps);
    layer->addDataSet(viewPortDataSeriesA, 0xff3333, "Alpha");
    layer->addDataSet(viewPortDataSeriesB, 0x008800, "Beta");
    layer->addDataSet(viewPortDataSeriesC, 0x3333CC, "Gamma");

    ///////////////////////////////////////////////////////////////////////////////////////
    // Configure axis scale and labelling
    ///////////////////////////////////////////////////////////////////////////////////////

    // Set the x-axis as a date/time axis with the scale according to the view port x range.
    //viewer->syncDateAxisWithViewPort("x", c->xAxis());
    c->xAxis()->setDateScale(startX, endX);

    //
    // In this demo, the time range can be from a few years to a few days. We demonstrate
    // how to set up different date/time format based on the time range.
    //

    // If all ticks are yearly aligned, then we use "yyyy" as the label format.
    c->xAxis()->setFormatCondition("align", 360 * 86400);
    c->xAxis()->setLabelFormat("{value|yyyy}");

    // If all ticks are monthly aligned, then we use "mmm yyyy" in bold font as the first
    // label of a year, and "mmm" for other labels.
    c->xAxis()->setFormatCondition("align", 30 * 86400);
    c->xAxis()->setMultiFormat(Chart::StartOfYearFilter(), "<*font=bold*>{value|mmm yyyy}",
        Chart::AllPassFilter(), "{value|mmm}");

    // If all ticks are daily algined, then we use "mmm dd<*br*>yyyy" in bold font as the
    // first label of a year, and "mmm dd" in bold font as the first label of a month, and
    // "dd" for other labels.
    c->xAxis()->setFormatCondition("align", 86400);
    c->xAxis()->setMultiFormat(Chart::StartOfYearFilter(),
        "<*block,halign=left*><*font=bold*>{value|mmm dd<*br*>yyyy}",
        Chart::StartOfMonthFilter(), "<*font=bold*>{value|mmm dd}");
    c->xAxis()->setMultiFormat(Chart::AllPassFilter(), "{value|dd}");

    // For all other cases (sub-daily ticks), use "hh:nn<*br*>mmm dd" for the first label of
    // a day, and "hh:nn" for other labels.
    c->xAxis()->setFormatCondition("else");
    c->xAxis()->setMultiFormat(Chart::StartOfDayFilter(),
        "<*font=bold*>{value|hh:nn<*br*>mmm dd}", Chart::AllPassFilter(), "{value|hh:nn}");

    return c;
}

//
// Draw track cursor when mouse is moving over plotarea
//
void ZoomScrollPdf::drawTrackCursor(QmlChartViewer *viewer, int mouseX)
{
    trackLineLabel((XYChart *)viewer->getChart(), mouseX);
    viewer->updateDisplay();

    // Hide the track cursor when the mouse leaves the plot area
    viewer->removeDynamicLayer("mouseLeavePlotArea");
}

//
// Draw track line with data labels
//
void ZoomScrollPdf::trackLineLabel(XYChart* c, int mouseX)
{
    // Clear the current dynamic layer and get the DrawArea object to draw on it.
    DrawArea* d = c->initDynamicLayer();

    // The plot area object
    PlotArea* plotArea = c->getPlotArea();

    // Get the data x-value that is nearest to the mouse, and find its pixel coordinate.
    double xValue = c->getNearestXValue(mouseX);
    int xCoor = c->getXCoor(xValue);

    // Draw a vertical track line at the x-position
    d->vline(plotArea->getTopY(), plotArea->getBottomY(), xCoor,
        d->dashLineColor(0x000000, 0x0101));

    // Draw a label on the x-axis to show the track line position.
    std::ostringstream xlabel;
    xlabel << "<*font,bgColor=000000*> " << c->xAxis()->getFormattedLabel(xValue, "mmm dd, yyyy")
        << " <*/font*>";
    TTFText* t = d->text(xlabel.str().c_str(), "Arial Bold", 8);

    // Restrict the x-pixel position of the label to make sure it stays inside the chart image.
    int xLabelPos = (std::max)(0, (std::min)(xCoor - t->getWidth() / 2, c->getWidth() - t->getWidth()));
    t->draw(xLabelPos, plotArea->getBottomY() + 6, 0xffffff);
    t->destroy();

    // Iterate through all layers to draw the data labels
    for (int i = 0; i < c->getLayerCount(); ++i) {
        Layer* layer = c->getLayerByZ(i);

        // The data array index of the x-value
        int xIndex = layer->getXIndexOf(xValue);

        // Iterate through all the data sets in the layer
        for (int j = 0; j < layer->getDataSetCount(); ++j)
        {
            DataSet* dataSet = layer->getDataSetByZ(j);
            const char* dataSetName = dataSet->getDataName();

            // Get the color, name and position of the data label
            int color = dataSet->getDataColor();
            int yCoor = c->getYCoor(dataSet->getPosition(xIndex), dataSet->getUseYAxis());

            // Draw a track dot with a label next to it for visible data points in the plot area
            if ((yCoor >= plotArea->getTopY()) && (yCoor <= plotArea->getBottomY()) && (color !=
                Chart::Transparent) && dataSetName && *dataSetName)
            {
                d->circle(xCoor, yCoor, 4, 4, color, color);

                std::ostringstream label;
                label << "<*font,bgColor=" << std::hex << color << "*> "
                    << c->formatValue(dataSet->getValue(xIndex), "{value|P4}") << " <*font*>";
                t = d->text(label.str().c_str(), "Arial Bold", 8);

                // Draw the label on the right side of the dot if the mouse is on the left side the
                // chart, and vice versa. This ensures the label will not go outside the chart image.
                if (xCoor <= (plotArea->getLeftX() + plotArea->getRightX()) / 2)
                    t->draw(xCoor + 5, yCoor, 0xffffff, Chart::Left);
                else
                    t->draw(xCoor - 5, yCoor, 0xffffff, Chart::Right);

                t->destroy();
            }
        }
    }
}

//
// User clicks on the Save Chart pushbutton
//
void ZoomScrollPdf::saveChartImage(QmlChartViewer *viewer, QUrl url)
{
    QString path = url.toLocalFile();
    if (!path.isEmpty())
    {
        // Save the chart
        BaseChart *c = viewer->getChart();
        if (0 != c)
            c->makeChart(path.toUtf8().constData());
    }
}

//
// User clicks on the Create PDF Report pushbutton
//
void ZoomScrollPdf::createPdfReport(QUrl url)
{
    QString path = url.toLocalFile();

    // The MultiPagePDF object can create PDF from multiple pages, each with one chart
    // object. Since a chart object can contain text (eg. using BaseChart.addText) and
    // other charts (eg. using MultiChart), that means each page can contain text and
    // multiple charts.
    MultiPagePDF doc;

    // Page configuration - A4 = 210 x 297mm. The PDF default is 96 dpi (dot per inch),
    // so the A4 size is equal to 794 x 1123 dots.
    const char* pageConfig = "pagewidth = 794; pageHeight = 1123";

    // In this example, we include a cover page with only text. This is by creating an
    // empty pie chart with text only.
    PieChart firstPage(720, 960);
    firstPage.addText(360, 320, "<*size=50*>ChartDirector<*br*><*size=30*>PDF Report Demonstration<*/*>",
        "Arial Bold", 30, 0x000000, Chart::Center);
    firstPage.setOutputOptions(pageConfig);
    doc.addPage(&firstPage);

    // We include 2 charts per page, with each chart showing one year of data. Each page
    // will also have a header and page number

    int startYear = Chart::getChartYMD(m_timeStamps[0]) / 10000;
    int endYear = Chart::getChartYMD(m_timeStamps[m_timeStamps.len - 1] - 1) / 10000;
    int pageNumber = 0;

    for (int yyyy = startYear; yyyy <= endYear; yyyy += 2)
    {
        // This chart is the page.
        MultiChart m(760, 920);

        // Use addTitle to add a header
        m.addTitle("ChartDirector PDF Report Demonstration", "Arial Bold", 20);

        // Create the first chart
        XYChart* c = drawXYChart(Chart::chartTime(yyyy, 1, 1), Chart::chartTime(yyyy + 1, 1, 1));
        m.addChart((m.getWidth() - c->getWidth()) / 2, 100, c);
        c->addTitle(c->formatValue(yyyy, "Year {value}"));

        XYChart* c2 = 0;
        if (yyyy < endYear)
        {
            // Create the second chart
            c2 = drawXYChart(Chart::chartTime(yyyy + 1, 1, 1), Chart::chartTime(yyyy + 2, 1, 1));
            c2->addTitle(c->formatValue(yyyy + 1, "Year {value}"));
            m.addChart((m.getWidth() - c2->getWidth()) / 2, 500, c2);
        }

        // Add the page number
        ++pageNumber;
        m.addTitle(Chart::BottomCenter, c->formatValue(pageNumber, "{value}"), "Arial Bold", 8);

        m.setOutputOptions(pageConfig);
        doc.addPage(&m);

        delete c;
        delete c2;
    }

    // Output the PDF report
    doc.outPDF(path.toUtf8().constData());
}
