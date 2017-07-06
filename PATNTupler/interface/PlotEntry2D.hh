#ifndef PLOTENTRY2D_HH
#define PLOTENTRY2D_HH

class PlotEntry2D{

public:
    // constructor
    PlotEntry2D(const std::string&, const TH2F&, const std::string&); // name of entry (eg for legends), histogram template, variableY:variableX that we wish to plot. (this is for data)
	PlotEntry2D(const std::string&, const TH2F&, const std::string&, const double&); // name of entry (eg for legends), histogram template, variableY:variableX that we wish to plot, luminosity. (this is for MC)
	
	void AddInput(const std::string&, const std::string&); // flatTree address, selection cut (this is for data)
	void AddInput(const std::string&, const std::string&, const double&, const bool& = true); // flatTree address, selection cut, sample cross-section (this is for MC), bool to prevent adding to numberOfEventsBeforeCuts
	std::string GetPlotEntryName() const;
	TH2F* GetHistogram() const;
	double GetNumberOfEventsBeforeCuts() const;
	double GetNumberOfEventsAfterCuts() const;
	double GetNumberOfEventsAfterCutsStatError() const;

private:
	std::string plotEntryName;
	double luminosity;
	std::string variablesToPlot;
	TH2F hNull;
	TH2F* hTotal;
	double numberOfEventsBeforeCuts;
	double numberOfEventsAfterCuts_StatErrorSquared;
};

#endif