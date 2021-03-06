//STL HEADERS
#include <iostream>
#include <vector>
#include <fstream>
#include <memory>
#include <map>
#include <fstream>
#include <sys/stat.h>

//ROOT HEADERS
#include <TH1D.h>
#include <TH2D.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TFile.h>

//RAL PARTICLE HEADERS
#include "TimeStamp.h"
#include "QcdSidebandCorr.h"


// RE-FORMAT HISTOGRAMS TO USE WITH COMBINED


void GetHistograms(std::map<std::string,TH1D*>&); // NEED TO CHANGE THE FILE PATH IN THIS FUNCTION WHEN USING NEW HISTOGRAMS
void CombineHistogram(const std::string&, const unsigned int&, const unsigned int&, std::map<std::string,TH1D*>&, const std::string&, const bool& = false);

int main(){
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////


    // ONE: save info
    const std::string outputDirGeneral = "/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/histos_2017_09_28_CMSSW_8_0_29_dbtV4/MassCutsV04/histosForCombined/codeV3testing_noht_newemptybinerror_new/"; // where we are going to save the output plots (should include the samples name, and any important features)
  

    // TWO: total number of bins in the histograms and the number of ht divisions within
    const unsigned int numberOfBinsTotal = 30;
    const unsigned int numberOfBinsHT = 3;


    // THREE: Samples
    const std::string dataSample = "Data_JetHt2016_goldenJson"; // use dummy data until we can unblind true data
    // std::vector<std::string> signalSampleVec = {"mH30_mSusy1600", "mH50_mSusy1600", "mH70_mSusy1600", "mH90_mSusy1600", "mH30_mSusy2000", "mH50_mSusy2000", "mH70_mSusy2000", "mH90_mSusy2000"}; // the different signal samples you wish to use
    std::vector<std::string> signalSampleVec = {"mH70_mSusy1600", "mH90_mSusy1600"}; // the different signal samples you wish to use
    // const std::vector<std::string> monteCarloBackgrounds = {"TTJets", "ZJets", "WJets"};
    const std::vector<std::string> monteCarloBackgrounds = {"TTJets", "ZJets", "WJets"};


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    
    // loop through the different signal sample references
    for (size_t iSig = 0; iSig < signalSampleVec.size(); ++iSig){

        std::cout << "***** SIGNAL SAMPLE = " << signalSampleVec[iSig] << " *****" << std::endl;

        const std::string outputDir = outputDirGeneral + "/" + signalSampleVec[iSig] + "/";
        const std::string dirExistCommand = "test -e " + outputDir;
        const std::string makeDirCommand = "mkdir -p " + outputDir;
        if (std::system(dirExistCommand.c_str()) != 0) std::system(makeDirCommand.c_str());
        std::system(Form("cp $CMSSW_BASE/src/NTupler/PATNTupler/macros/histos_forCombineV3.cc %s/%s__histos_forCombineV3.cc", outputDir.c_str(), TimeStamp::GetTimeStamp().c_str()));

        std::map<std::string, TH1D*> hOriginal_;
        GetHistograms(hOriginal_);

        // loop through the different mass types
        const std::vector<std::string> massLabelVec = {"S", "UnD"};
        for (auto massLabel : massLabelVec){

            const std::string outputFileName = outputDir + "combineTH1D_" + massLabel + ".root";
            TFile * outputFile = new TFile(outputFileName.c_str(), "RECREATE");

            if (massLabel == "S") CombineHistogram(dataSample, numberOfBinsTotal, numberOfBinsHT, hOriginal_, massLabel, true);
            else CombineHistogram(dataSample, numberOfBinsTotal, numberOfBinsHT, hOriginal_, massLabel);
            CombineHistogram(signalSampleVec[iSig], numberOfBinsTotal, numberOfBinsHT, hOriginal_, massLabel);
            for (size_t iMC = 0; iMC < monteCarloBackgrounds.size(); ++iMC) CombineHistogram(monteCarloBackgrounds[iMC], numberOfBinsTotal, numberOfBinsHT, hOriginal_, massLabel);

            outputFile->Close();
            std::cout << "Created the ROOT file: " << outputFileName << std::endl;
            std::cout << std::endl;
            delete outputFile;

        } // closes loop through differet mass types
        std::cout << std::endl;
        std::cout << std::endl;
    } // closes loop through the different signal samples
    return 0; 
}


void GetHistograms(std::map<std::string,TH1D*>& h_)
{
    // histos locations
    std::string preamble = "/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/histos_2017_09_28_CMSSW_8_0_29_dbtV4/MassCutsV04/";
    
    std::string postamble = "MassCutsV04_ak8pt300_ht1500x2500x3500x_ak4pt250n250_lumi37.root";
    std::vector<std::string> histoNameVec;
    histoNameVec.push_back("Data_JetHt2016_goldenJson"); // comment out when working on MC
    histoNameVec.push_back("QCD");
    histoNameVec.push_back("TTJets");
    histoNameVec.push_back("ZJets");
    histoNameVec.push_back("WJets");
    histoNameVec.push_back("mH30_mSusy800");
    histoNameVec.push_back("mH50_mSusy800");
    histoNameVec.push_back("mH70_mSusy800");
    histoNameVec.push_back("mH90_mSusy800");
    histoNameVec.push_back("mH30_mSusy1200");
    histoNameVec.push_back("mH50_mSusy1200");
    histoNameVec.push_back("mH70_mSusy1200");
    histoNameVec.push_back("mH90_mSusy1200");
    histoNameVec.push_back("mH30_mSusy1600");
    histoNameVec.push_back("mH50_mSusy1600");
    histoNameVec.push_back("mH70_mSusy1600");
    histoNameVec.push_back("mH90_mSusy1600");
    histoNameVec.push_back("mH30_mSusy2000");
    histoNameVec.push_back("mH50_mSusy2000");
    histoNameVec.push_back("mH70_mSusy2000");
    histoNameVec.push_back("mH90_mSusy2000");

    std::string postamble_noAk4 = "MassCutsV04_ak8pt300_ht1500x2500x3500x_ak4pt-1n-1_lumi37.root";
    std::vector<std::string> histoNameVec_noAk4;
    histoNameVec_noAk4.push_back("Data_JetHt2016_goldenJson_NOAK4"); // comment out when working on MC
    histoNameVec_noAk4.push_back("QCD_NOAK4");
    histoNameVec_noAk4.push_back("TTJets_NOAK4");
    histoNameVec_noAk4.push_back("ZJets_NOAK4");
    histoNameVec_noAk4.push_back("WJets_NOAK4");
    histoNameVec_noAk4.push_back("mH70_mSusy800_NOAK4");
    histoNameVec_noAk4.push_back("mH70_mSusy1200_NOAK4");
    histoNameVec_noAk4.push_back("mH70_mSusy1600_NOAK4");
    histoNameVec_noAk4.push_back("mH70_mSusy2000_NOAK4");

    for (size_t iH = 0; iH < (histoNameVec.size() + histoNameVec_noAk4.size()); ++iH){

        std::string postambleToUse;
        std::string histoToUse;
        if (iH < histoNameVec.size()){
            postambleToUse = postamble;
            histoToUse = histoNameVec[iH];
        }
        else {
            postambleToUse = postamble_noAk4;
            histoToUse = histoNameVec_noAk4[iH - histoNameVec.size()];
        }

        TFile * f = new TFile(Form("%s/%s/%s", preamble.c_str(), histoToUse.c_str(), postambleToUse.c_str()));
        // explanation of terminology
        // 1. S, U, D --> refers to mass space. pred is the prediction of S. UnD is the sum U+D.
        // 2. tag, anti, control --> refers to 2*DBT space
        // 3. sample name on the end
        h_[Form("S_tag_%s", histoToUse.c_str())] = (TH1D*)f->Get("S_dbtMed2MaxAndMed2Max");
        h_[Form("UnD_tag_%s", histoToUse.c_str())] = (TH1D*)f->Get("U_dbtMed2MaxAndMed2Max");
        h_[Form("UnD_tag_%s", histoToUse.c_str())]->Add((TH1D*)f->Get("D_dbtMed2MaxAndMed2Max"));

    } // closes loop through histoNameVec
} // closes function GetHistograms


void CombineHistogram(const std::string& processName, const unsigned int& numberOfBinsTotal, const unsigned int& numberOfBinsHT, std::map<std::string,TH1D*>& hOriginal_, const std::string& massLabel, const bool& isThisSignalData){
    std::cout << "creating histogram for process " << massLabel << "_" << processName;
    if (isThisSignalData) std::cout << " (this is our signal data sample)" << std::endl;
    else std::cout << std::endl;

    TH1D * h;
    if (isThisSignalData) h = new TH1D("data_obs", ";bin;events / bin", numberOfBinsTotal, 0, numberOfBinsTotal);
    else h = new TH1D(processName.c_str(), ";bin;events / bin", numberOfBinsTotal, 0, numberOfBinsTotal);

    std::vector<unsigned int> numberOfEmptyBinsVec(numberOfBinsHT, 0);
    std::vector<double> eventWeightVec;

    for (unsigned int iBin = 1; iBin < numberOfBinsTotal + 1; ++iBin){
    
        double binContent = hOriginal_[Form("%s_tag_%s", massLabel.c_str(), processName.c_str())]->GetBinContent(iBin);
        double binError = hOriginal_[Form("%s_tag_%s", massLabel.c_str(), processName.c_str())]->GetBinError(iBin);
    
        if (binContent == 0){
            unsigned int iHT = floor( (iBin - 1) * numberOfBinsHT / numberOfBinsTotal );
            numberOfEmptyBinsVec[iHT]++;
        } 
        else eventWeightVec.push_back(binError * binError / binContent);

    } // closes loop through individual bins (for the first time)

    // check that all event weights for a given process are the same
    double eventWeight = -7000.0;
    for (size_t iEW = 0; iEW < eventWeightVec.size(); ++iEW){
        if (iEW == 0) eventWeight = eventWeightVec[0];
        if (iEW != 0 && float(eventWeightVec[iEW]) != float(eventWeightVec[iEW-1]) ){
            std::cout << "WARNING: The event weights are not the same for process^^^! Will not write histogram to file..." << std::endl;
            return;
        }
    }
    for (unsigned int iBin = 1; iBin < numberOfBinsTotal + 1; ++iBin){

        double binContent = hOriginal_[Form("%s_tag_%s", massLabel.c_str(), processName.c_str())]->GetBinContent(iBin);
        
        double binError = -8000.0;
        unsigned int iHT = floor( (iBin - 1) * numberOfBinsHT / numberOfBinsTotal );
        if (binContent > 0) binError = hOriginal_[Form("%s_tag_%s", massLabel.c_str(), processName.c_str())]->GetBinError(iBin);
        else if (binContent == 0.0 && numberOfEmptyBinsVec[iHT] < (numberOfBinsTotal / numberOfBinsHT) ){
            binContent = 0.0;
            binError = eventWeight;
        }
        else{
            binContent = 0.0;
            binError = 0.0;
        }

        if (massLabel == "UnD"){
            double corrRatio = QcdSidebandCorr::GetCorr(iBin);
            binContent = corrRatio * binContent;
            binError = corrRatio * binError;
        }

        h->SetBinContent(iBin, binContent);
        h->SetBinError(iBin, binError);

    } // closes loop through individual bins (for the second time)
    
    h->Write();
    return;
} // closes the function CombineHistogram