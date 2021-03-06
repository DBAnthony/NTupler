//STL HEADERS
#include <iostream>
#include <vector>
#include <fstream>
#include <memory>
#include <fstream>
#include <sys/stat.h>

//ROOT HEADERS
#include <TH1D.h>
#include <TH2D.h>
#include <TStyle.h>
#include <TLegend.h>

//RAL PARTICLE HEADERS
#include "PlotEntry.hh"
#include "DoubleBTagWPs.h"
#include "TimeStamp.h"
#include "MacrosOnCondor.h"

// MAKE CUT FLOW TABLES

int main(int argc, char** argv){
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////



    // ONE: save info
    std::string outputDir = "/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/plots_2019_01_01/2017_94X/cutFlowTables/mH70_mSusyALL/"; // where we are going to save the output plots (should include the samples name, and any important features)



    // TWO: set of cut params (first element is the choice for an individual table, second element is the choice for an individual table, etc.)
    std::vector<std::vector<std::string>> cut2_ak8Dbt = { {"DIAG_UP", "Loose"}, {"DIAG_UP", "Loose"}, {"DIAG_UP", "Loose"} }; // 4 elements in sub-vector: 1st for fatJetA min, 2nd for fatJetA max, 3rd for fatJetB min, 4th for fatJetB max --> "Off", "Loose", "Med1", "Med2", "Tight", "Max"
    // std::vector<std::vector<std::string>> cut2_ak8Dbt = { {"Off","Max","Off","Max"}, {"Off","Max","Off","Max"}, {"Off","Max","Off","Max"} }; // 4 elements in sub-vector: 1st for fatJetA min, 2nd for fatJetA max, 3rd for fatJetB min, 4th for fatJetB max --> "Off", "Loose", "Med1", "Med2", "Tight", "Max"
    std::vector<int> cut3_ak8Pt = {300, 300, 300};
    std::vector<std::vector<int>>cut4_ht = { {1500,2500}, {2500,3500}, {3500,99999} }; // these are HT bins, not just cuts (NB: use 99999 for a maximum)
    std::vector<std::vector<int>> cut5_ak4Pt = { {300,-1}, {300,-1}, {300,-1} }; // (2 elements in sub-vector, 1st for leading pt, 2nd for seconary pt)



    // THREE: lumi settings
    double luminosity = 41.529; // 2016 DATASET



    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    if (argc != 2){
        std::cout << "Haven't provided valid argument, the format should be:" << std::endl;
        std::cout << argv[0] << " <runInstructionString>" << std::endl;
        std::cout << "runInstructionString = 'local' or 'batch'" << std::endl;
        std::cout << "Exiting..." << std::endl;
        return -1;
    }

    if (cut2_ak8Dbt.size() != cut3_ak8Pt.size() || cut3_ak8Pt.size() != cut4_ht.size() || cut4_ht.size() != cut5_ak4Pt.size()){
        std::cout << "stepXlabels not all same size, exiting..." << std::endl;
        return 1;
    }

    std::string runInstructionString(argv[1]);
    
    if (runInstructionString == "local"){
        std::string dirExistCommand = "test -e " + outputDir;
        std::string makeDirCommand = "mkdir -p " + outputDir;
        if (std::system(dirExistCommand.c_str()) != 0) std::system(makeDirCommand.c_str());
        std::system(Form("cp $CMSSW_BASE/src/NTupler/PATNTupler/macros/table_investigateEventSelectionCuts.cc %s/%s__table_investigateEventSelectionCuts.cc", outputDir.c_str(), TimeStamp::GetTimeStamp().c_str()));
    }
    else if (runInstructionString == "batch"){
        std::string dirExistCommand = "test -e " + outputDir;
        std::string makeDirCommand = "mkdir -p " + outputDir;
        if (std::system(dirExistCommand.c_str()) != 0) std::system(makeDirCommand.c_str());
        std::system(Form("cp $CMSSW_BASE/src/NTupler/PATNTupler/macros/table_investigateEventSelectionCuts.cc %s/%s__table_investigateEventSelectionCuts.cc", outputDir.c_str(), TimeStamp::GetTimeStamp().c_str()));
        MacrosOnCondor::SubmitJob(outputDir.c_str(), "table_investigateEventSelectionCuts", "/opt/ppd/scratch/xap79297/jobLogs/macros/");
        return 0;
    }
    else if (runInstructionString == "batchRUN"){
        outputDir = ".";
    }
    else{
        std::cout << "Haven't provided valid argument, the format should be:" << std::endl;
        std::cout << argv[0] << " <runInstructionString>" << std::endl;
        std::cout << "runInstructionString = 'local' or 'batch'" << std::endl;
        std::cout << "Exiting..." << std::endl;
        return -1;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string varToPlot = "fatJetA_softDropMassPuppi";    
    TH1D hTemplate("hTemplate", ";fatJetA_SoftDrop_Mass NOT FOR PLOTTING (GeV);NOT FOR PLOTTING", 1, -99991, 99991);

    for (size_t iTab = 0; iTab < cut3_ak8Pt.size(); ++iTab){ // loop through different table configurations

        std::string dbtCut = "";
        if (cut2_ak8Dbt[iTab].size() == 2 && cut2_ak8Dbt[iTab][0] == "DIAG_UP")
            dbtCut = Form("fatJetA_doubleBtagDiscrim >= (-1.0 * fatJetB_doubleBtagDiscrim + 1.0 + %f) ", DoubleBTagWPs::dbtNameToDouble(cut2_ak8Dbt[iTab][1]) );
        if (cut2_ak8Dbt[iTab].size() == 4)
            dbtCut = Form("fatJetA_doubleBtagDiscrim>=%f && fatJetA_doubleBtagDiscrim<%f && fatJetB_doubleBtagDiscrim>=%f && fatJetB_doubleBtagDiscrim<%f ", DoubleBTagWPs::dbtNameToDouble(cut2_ak8Dbt[iTab][0]), DoubleBTagWPs::dbtNameToDouble(cut2_ak8Dbt[iTab][1]), DoubleBTagWPs::dbtNameToDouble(cut2_ak8Dbt[iTab][2]), DoubleBTagWPs::dbtNameToDouble(cut2_ak8Dbt[iTab][3]) );

        // get the full set of cuts for the table
        std::vector<std::string> cutToApplyVec;
        cutToApplyVec.push_back("weight_isr");
        cutToApplyVec.push_back(Form("weight_isr * (fatJetA_p4.Pt()>%d && fatJetB_p4.Pt()>%d)", cut3_ak8Pt[iTab], cut3_ak8Pt[iTab]));
        cutToApplyVec.push_back(Form("weight_isr * (fatJetA_p4.Pt()>%d && fatJetB_p4.Pt()>%d && slimJetA_p4.Pt()>%d && slimJetB_p4.Pt()>%d)", cut3_ak8Pt[iTab], cut3_ak8Pt[iTab], cut5_ak4Pt[iTab][0], cut5_ak4Pt[iTab][1]));
        cutToApplyVec.push_back(Form("weight_isr * (fatJetA_p4.Pt()>%d && fatJetB_p4.Pt()>%d && ht>=%d && ht<%d && slimJetA_p4.Pt()>%d && slimJetB_p4.Pt()>%d)", cut3_ak8Pt[iTab], cut3_ak8Pt[iTab], cut4_ht[iTab][0], cut4_ht[iTab][1], cut5_ak4Pt[iTab][0], cut5_ak4Pt[iTab][1]));
        cutToApplyVec.push_back(Form("weight_isr * weight_dbtTag * (%s && fatJetA_p4.Pt()>%d && fatJetB_p4.Pt()>%d && ht>=%d && ht<%d && slimJetA_p4.Pt()>%d && slimJetB_p4.Pt()>%d)", dbtCut.c_str(), cut3_ak8Pt[iTab], cut3_ak8Pt[iTab], cut4_ht[iTab][0], cut4_ht[iTab][1], cut5_ak4Pt[iTab][0], cut5_ak4Pt[iTab][1]));
        // get the required count info for the table
        std::vector<std::vector<PlotEntry>> cutFlowObject; // all cut stages, for all samples
        for (int iC = 0; iC != 5; ++iC){

            std::vector<PlotEntry> givenCutObject; // single cut stage, for all samples



            // FOUR: SAMPLE INFO

            givenCutObject.push_back( PlotEntry("mH70_mSusy800", hTemplate, varToPlot.c_str(), luminosity) );
            givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017/mH70p0_mSusy800p0_ratio0p99_splitting0p1/flatTree.root", cutToApplyVec[iC].c_str(), 6.4720000*0.840*0.840);
        
            givenCutObject.push_back( PlotEntry("mH70_mSusy1200", hTemplate, varToPlot.c_str(), luminosity) );
            givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017/mH70p0_mSusy1200p0_ratio0p99_splitting0p1/flatTree.root", cutToApplyVec[iC].c_str(), 0.4951000*0.840*0.840);
        
            givenCutObject.push_back( PlotEntry("mH70_mSusy1600", hTemplate, varToPlot.c_str(), luminosity) );
            givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017/mH70p0_mSusy1600p0_ratio0p99_splitting0p1/flatTree.root", cutToApplyVec[iC].c_str(), 0.0603900*0.840*0.840);
        
            // ********************
            // givenCutObject.push_back( PlotEntry("mH30_mSusy2000", hTemplate, varToPlot.c_str(), luminosity) );
            // givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017/mH30p0_mSusy2000p0_ratio0p99_splitting0p1/flatTree.root", cutToApplyVec[iC].c_str(), 0.0091050*0.868*0.868);
        
            // givenCutObject.push_back( PlotEntry("mH50_mSusy2000", hTemplate, varToPlot.c_str(), luminosity) );
            // givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017/mH50p0_mSusy2000p0_ratio0p99_splitting0p1/flatTree.root", cutToApplyVec[iC].c_str(), 0.0091050*0.858*0.858);

            givenCutObject.push_back( PlotEntry("mH70_mSusy2000", hTemplate, varToPlot.c_str(), luminosity) );
            givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017/mH70p0_mSusy2000p0_ratio0p99_splitting0p1/flatTree.root", cutToApplyVec[iC].c_str(), 0.0091050*0.840*0.840);
        
            // givenCutObject.push_back( PlotEntry("mH90_mSusy2000", hTemplate, varToPlot.c_str(), luminosity) );
            // givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017/mH90p0_mSusy2000p0_ratio0p99_splitting0p1/flatTree.root", cutToApplyVec[iC].c_str(), 0.0091050*0.816*0.816);

            // givenCutObject.push_back( PlotEntry("mH110_mSusy2000", hTemplate, varToPlot.c_str(), luminosity) );
            // givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017/mH110p0_mSusy2000p0_ratio0p99_splitting0p1/flatTree.root", cutToApplyVec[iC].c_str(), 0.0091050*0.749*0.749);

            // givenCutObject.push_back( PlotEntry("mH125_mSusy2000", hTemplate, varToPlot.c_str(), luminosity) );
            // givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017/mH125p0_mSusy2000p0_ratio0p99_splitting0p1/flatTree.root", cutToApplyVec[iC].c_str(), 0.0091050*0.581*0.581);
            // ********************

            givenCutObject.push_back( PlotEntry("mH70_mSusy2400", hTemplate, varToPlot.c_str(), luminosity) );
            givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017/mH70p0_mSusy2400p0_ratio0p99_splitting0p1/flatTree.root", cutToApplyVec[iC].c_str(), 0.0015050*0.840*0.840);

            // ********************
            // ********************
            // ********************

            // givenCutObject.push_back( PlotEntry("QCD HT>1000", hTemplate, varToPlot.c_str(), luminosity) );
            // givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017_forTablesOnly/QCD_HT1000to1500/flatTree.root", cutToApplyVec[iC].c_str(), 1005);
            // givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017_forTablesOnly/QCD_HT1500to2000/flatTree.root", cutToApplyVec[iC].c_str(), 101.8);
            // givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017_forTablesOnly/QCD_HT2000toInf/flatTree.root", cutToApplyVec[iC].c_str(), 20.54);

            // givenCutObject.push_back( PlotEntry("TT+Jets", hTemplate, varToPlot.c_str(), luminosity) );
            // givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017_forTablesOnly/TTToHadronic/flatTree.root", cutToApplyVec[iC].c_str(), 377.96);
            // givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017_forTablesOnly/TTToSemiLeptonic/flatTree.root", cutToApplyVec[iC].c_str(), 365.34);
            // givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017_forTablesOnly/TTTo2L2Nu/flatTree.root", cutToApplyVec[iC].c_str(), 88.29);

            // givenCutObject.push_back( PlotEntry("Z+Jets HT>800", hTemplate, varToPlot.c_str(), luminosity) );
            // givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017_forTablesOnly/ZJets/flatTree.root", cutToApplyVec[iC].c_str(), 18.69);

            // givenCutObject.push_back( PlotEntry("W+Jets HT>800", hTemplate, varToPlot.c_str(), luminosity) );
            // givenCutObject[givenCutObject.size()-1].AddInput("/opt/ppd/scratch/xap79297/Analysis_boostedNmssmHiggs/flatTrees_2019_01_01/2017_forTablesOnly/WJets/flatTree.root", cutToApplyVec[iC].c_str(), 34.00);




            cutFlowObject.push_back(givenCutObject);
        }

        // create the cut flow table
        std::ofstream table;
        table << std::fixed;
        std::string cutName = "";
        if (cut2_ak8Dbt[iTab].size() == 4) cutName += "dbt" + cut2_ak8Dbt[iTab][0] + cut2_ak8Dbt[iTab][1] + "And" + cut2_ak8Dbt[iTab][2] + cut2_ak8Dbt[iTab][3];
        if (cut2_ak8Dbt[iTab].size() == 2 && cut2_ak8Dbt[iTab][0] == "DIAG_UP") cutName += "dbtDiagUp" + cut2_ak8Dbt[iTab][1];
        cutName += Form("_ak8pt%d", cut3_ak8Pt[iTab]);
        if (cut4_ht[iTab][1]==99999) cutName += Form("_ht%dplus",cut4_ht[iTab][0]);
        else cutName += Form("_ht%dto%d",cut4_ht[iTab][0],cut4_ht[iTab][1]);
        cutName += Form("_ak4pt%dn%d", cut5_ak4Pt[iTab][0], cut5_ak4Pt[iTab][1]);
        table.open( Form("%s/cutCount_%.0finvFb__%s.csv", outputDir.c_str(), luminosity, cutName.c_str()) );
        table << "Luminosity = " << luminosity << "fb^-1," << "\n";
        table << "col1: Number of Events\n";
        table << "col2: Fraction from last stage\n";
        table << "bottom: Total fraction\n";
        table << ",";
        for (size_t i = 0; i < cutFlowObject[0].size(); ++i) table << cutFlowObject[0][i].GetPlotEntryName().c_str() << ",,,";
        std::vector<double> holdValueVec(cutFlowObject[0].size(), 0.0);

        table << "\nPreCuts:,";
        for (size_t i = 0; i < cutFlowObject[0].size(); ++i){
            table << cutFlowObject[0][i].GetNumberOfEventsBeforeCuts() << ",,,";
            holdValueVec[i] = cutFlowObject[0][i].GetNumberOfEventsBeforeCuts();
        }
        
        table << "\n2 FatJets (pre-selection):,";
        for (size_t i = 0; i < cutFlowObject[0].size(); ++i){
            table << std::setprecision(6) << cutFlowObject[0][i].GetNumberOfEventsAfterCuts() << ",";
            if (cutFlowObject[0][i].GetNumberOfEventsAfterCuts() < 0.00000001) table << "0,,";
            else table << cutFlowObject[0][i].GetNumberOfEventsAfterCuts() / holdValueVec[i] << ",,";
            holdValueVec[i] = cutFlowObject[0][i].GetNumberOfEventsAfterCuts();
        }

        table << "\nAK8PT " << cut3_ak8Pt[iTab] << "(GeV):,";
        for (size_t i = 0; i < cutFlowObject[0].size(); ++i){
            table << std::setprecision(6) << cutFlowObject[1][i].GetNumberOfEventsAfterCuts() << ",";
            if (cutFlowObject[1][i].GetNumberOfEventsAfterCuts() < 0.00000001) table << "0,,";
            else table << cutFlowObject[1][i].GetNumberOfEventsAfterCuts() / holdValueVec[i] << ",,";
            holdValueVec[i] = cutFlowObject[1][i].GetNumberOfEventsAfterCuts();
        }
        
        table << "\nAK4PT " << cut5_ak4Pt[iTab][0] << "&" << cut5_ak4Pt[iTab][1] << "(GeV):,";
        for (size_t i = 0; i < cutFlowObject[0].size(); ++i){
            table << std::setprecision(6) << cutFlowObject[2][i].GetNumberOfEventsAfterCuts() << ",";
            if (cutFlowObject[2][i].GetNumberOfEventsAfterCuts() < 0.00000001) table << "0,,";
            else table << cutFlowObject[2][i].GetNumberOfEventsAfterCuts() / holdValueVec[i] << ",,";
            holdValueVec[i] = cutFlowObject[2][i].GetNumberOfEventsAfterCuts();
        }

        table << "\nHT " << cut4_ht[iTab][0] << "-" << cut4_ht[iTab][1] << "(GeV):,";
        for (size_t i = 0; i < cutFlowObject[0].size(); ++i){
            table << std::setprecision(6) << cutFlowObject[3][i].GetNumberOfEventsAfterCuts() << ",";
            if (cutFlowObject[3][i].GetNumberOfEventsAfterCuts() < 0.00000001) table << "0,,";
            else table << cutFlowObject[3][i].GetNumberOfEventsAfterCuts() / holdValueVec[i] << ",,";
            holdValueVec[i] = cutFlowObject[3][i].GetNumberOfEventsAfterCuts();
        }        

        if (cut2_ak8Dbt[iTab].size() == 4) table << "\nAK8DBT " << cut2_ak8Dbt[iTab][0].c_str() << cut2_ak8Dbt[iTab][1].c_str() << "&" << cut2_ak8Dbt[iTab][2].c_str() << cut2_ak8Dbt[iTab][3].c_str() << ":,";
        if (cut2_ak8Dbt[iTab].size() == 2 && cut2_ak8Dbt[iTab][0] == "DIAG_UP") table << "\nAK8DBT_diagUp" << cut2_ak8Dbt[iTab][1].c_str() << ":,";
        for (size_t i = 0; i < cutFlowObject[0].size(); ++i){
            table << std::setprecision(6) << cutFlowObject[4][i].GetNumberOfEventsAfterCuts() << ",";
            if (cutFlowObject[4][i].GetNumberOfEventsAfterCuts() < 0.00000001) table << "0,,";
            else table << cutFlowObject[4][i].GetNumberOfEventsAfterCuts() / holdValueVec[i] << ",,";
            holdValueVec[i] = cutFlowObject[4][i].GetNumberOfEventsAfterCuts();
        }
        
        table << "\n,,";
        for (size_t i = 0; i < cutFlowObject[0].size(); ++i) table << holdValueVec[i] / cutFlowObject[0][i].GetNumberOfEventsBeforeCuts() << ",,,";

        table.close();
    } // closes loop through different table configurations

	return 0;
}