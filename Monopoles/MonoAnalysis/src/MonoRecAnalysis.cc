// system include files                                                                     
#include <vector>
#include <string>
#include <set>
#include <memory>
#include <algorithm>
#include <sstream>

// user include files                                                                       
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

// data formats                                                                             
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/Common/interface/SortedCollection.h"

// Ecal includes                                                                            
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
// For trigger                                                                              
//#include "L1Trigger/GlobalTriggerAnalyzer/interface/L1GtUtils.h"                          
//#include "L1Trigger/L1TGlobal/interface/L1TGlobalUtil.h"                                  
//#include "L1Trigger/GlobalTriggerAnalyzer/interface/L1GtUtils.h"                          
#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"

// Monopole analysis includes                                                               
#include "Monopoles/MonoAlgorithms/interface/EcalMapper.h"
#include "Monopoles/MonoAlgorithms/interface/MonoTruthSnooper.h"
#include "Monopoles/MonoAlgorithms/interface/MonoGenTrackExtrapolator.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
// ROOT includes                                                                            
//#include "TH2D.h"                                                                         
//#include "TTree.h"                                                                        
//#include "TBranch.h"                                                                      
#include "TDirectory.h"
#include "TLatex.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF1.h"
#include "TF2.h"
#include "TMath.h"
#include "TGraphErrors.h"
#include "TFitResult.h"
#include "TVirtualFitter.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace edm;

class MonoRecAnalysis : public edm::EDAnalyzer {

public:

  explicit MonoRecAnalysis(const edm::ParameterSet&);
  ~MonoRecAnalysis();
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);



private:

  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void beginRun(const edm::Run&, const edm::EventSetup&);
  virtual void beginJob();
  virtual void endJob();
  virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
  virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);



  //  void clear();

  std::string m_output;
  TFile *m_outputFile;




  HLTPrescaleProvider hltPSProv_;
  std::string hltProcess_;


  void clear();

  edm::EDGetTokenT< EBRecHitCollection > m_TagEcalEB_RecHits;
  edm::EDGetTokenT< edm::HepMCProduct > m_mcproduct;
  static const int MONO_PID = MONOID;

  edm::Service<TFileService> m_fs;
  TH2D * m_h_2_tVsE;
  TTree * m_tree;
  unsigned m_run;
  unsigned m_lumi;
  unsigned m_event;
  std::vector<double> m_EcalTime;
  std::vector<double> m_EcalE;
  std::vector<double> m_EcalEta;
  std::vector<double> m_EcalPhi;
  double m_mono_eta;
  double m_mono_phi;
  double m_amon_eta;
  double m_amon_phi;
  std::vector<double> m_d1_eta;
  std::vector<double> m_d1_phi;
  std::vector<int> m_d1_pdgId;
  std::vector<double> m_d2_eta;
  std::vector<double> m_d2_phi;
  std::vector<int> m_d2_pdgId;
  double m_monoExp_eta;
  double m_monoExp_phi;
  double m_mono_p;
  double m_mono_m;
  double m_mono_z;
  double m_amonExp_eta;
  double m_amonExp_phi;
  double m_amon_p;
  double m_amon_m;
  double m_amon_z;
  double m_mono_x;
  double m_mono_y;
  double m_monoExpEE_eta;
  double m_monoExpEE_phi;
  double m_amon_px;
  double m_amon_py;
  double m_amon_pz;
  double m_amonExpEE_eta;
  double m_amonExpEE_phi;

  double m_mono_px;
  double m_mono_py;
double m_mono_pz;
  double m_amon_x;
  double m_amon_y;


};

/*
namespace chow {

  double mag ( double x, double y) {
    return sqrt( x*x + y*y );
  }

  double mag ( double x, double y, double z){
    return sqrt( x*x + y*y + z*z );
  }


}
*/

MonoRecAnalysis::MonoRecAnalysis(const edm::ParameterSet& iConfig):

  m_output(iConfig.getParameter<std::string>("Output")),
  hltPSProv_(iConfig,consumesCollector(),*this),
  hltProcess_(iConfig.getParameter<std::string>("hltProcess")),
  m_TagEcalEB_RecHits(consumes<EBRecHitCollection>(iConfig.getParameter<edm::InputTag>("EcalEBRecHits")))
  ,m_mcproduct( consumes< edm::HepMCProduct >( iConfig.getParameter< edm::InputTag >( "GeneratorTag" ) ))
{

}



MonoRecAnalysis::~MonoRecAnalysis()

{

}


void MonoRecAnalysis::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)

{
  int psColumn = hltPSProv_.prescaleSet(iEvent,iSetup);
  std::cout <<"PS column "<<psColumn<<std::endl;
  if(psColumn==0 && iEvent.isRealData()){
    std::cout <<"PS column zero detected for data, this is unlikely (almost all triggers are disabled in normal menus here) and its more likely that you've not loaded the correct global tag in "<<std::endl;

}


  l1t::L1TGlobalUtil& l1GtUtils = const_cast<l1t::L1TGlobalUtil&> (hltPSProv_.l1tGlobalUtil());

  std::cout <<"l1 menu: name decisions prescale "<<std::endl;
  for(size_t bitNr=0;bitNr<l1GtUtils.decisionsFinal().size();bitNr++){
    const std::string& bitName = l1GtUtils.decisionsFinal()[bitNr].first;
    bool passInitial = l1GtUtils.decisionsInitial()[bitNr].second;
    bool passInterm = l1GtUtils.decisionsInterm()[bitNr].second;
    bool passFinal = l1GtUtils.decisionsFinal()[bitNr].second; 
    int prescale = l1GtUtils.prescales()[bitNr].second;
    std::cout <<"   "<<bitNr<<" "<<bitName<<" "<<passInitial<<" "<<passInterm<<" "<<passFinal<<" "<<prescale<<std::endl;
  }

  clear();

  m_run = iEvent.id().run();
  m_lumi = iEvent.id().luminosityBlock();
  m_event = iEvent.id().event();






  Handle<EBRecHitCollection > EcalRecHits;
  iEvent.getByToken(m_TagEcalEB_RecHits,EcalRecHits);
  assert( EcalRecHits->size() > 0 );


  /*
  Mono::MonoTruthSnoop snoopy(iEvent,iSetup);
  const HepMC::GenParticle *mono = snoopy.mono(Mono::monopole);
  const HepMC::GenParticle *anti = snoopy.mono(Mono::anti_monopole);


  m_m1_eta = mono ? mono->momentum().eta() : -999.;
  m_m1_phi = mono ? mono->momentum().phi() : -999.;
  m_m2_eta = anti ? anti->momentum().eta() : -999.;
  m_m2_phi = anti ? anti->momentum().phi() : -999.;


  std::vector<const HepMC::GenParticle *> d1 = snoopy.monoDaughter(Mono::monopole);
  std::vector<const HepMC::GenParticle *> d2 = snoopy.monoDaughter(Mono::anti_monopole);
  for ( unsigned i=0; i < d1.size(); i++ ) {
    std::cout << "Found monopole daughter!" << std::endl;
    m_d1_eta.push_back( d1[i]->momentum().eta() );
    m_d1_phi.push_back( d1[i]->momentum().phi() );
    m_d1_pdgId.push_back( d1[i]->pdg_id() );
  }
  for ( unsigned i=0; i < d2.size(); i++ ) {
    std::cout << "Found monopole daughter!" << std::endl;
    m_d2_eta.push_back( d2[i]->momentum().eta() );
    m_d2_phi.push_back( d2[i]->momentum().phi() );
    m_d2_pdgId.push_back( d2[i]->pdg_id() );
  }
  Mono::MonoGenTrackExtrapolator extrap;

  if ( mono ) {
    extrap.setMonopole(*mono);
    m_monoExp_eta = extrap.etaVr(1.29);
    m_monoExp_phi = extrap.phi();
    m_monoExp_g = extrap.charge();
    m_monoExp_m = extrap.mass();
    m_monoExp_z = extrap.zVr(1.29);
  }


  if ( anti ) {
    extrap.setMonopole(*anti);
    m_amonExp_eta = extrap.etaVr(1.29);
    m_amonExp_phi = extrap.phi();
    m_amonExp_g = extrap.charge();
    m_amonExp_m = extrap.mass();
    m_amonExp_z = extrap.zVr(1.29);
  }




  */




  /*
  Mono::MonoTruthSnoop snoopy(iEvent,iSetup);
  const HepMC::GenParticle *mono = snoopy.mono(Mono::monopole);
  const HepMC::GenParticle *amon = snoopy.mono(Mono::anti_monopole);
  */

  Mono::MonoGenTrackExtrapolator extrap;  

  Handle<edm::HepMCProduct> mcproduct;
  iEvent.getByToken(m_mcproduct, mcproduct);
  //                                                                                                    
  const HepMC::GenEvent* mc = mcproduct->GetEvent();
  assert(mc);
  const HepMC::GenEvent::particle_const_iterator end = mc->particles_end();
  for (HepMC::GenEvent::particle_const_iterator p = mc->particles_begin();
       p != end; ++p)
    {
      const HepMC::GenParticle* particle = *p;
      const reco::Candidate::LorentzVector p4( particle->momentum());


      // I only care about monopoles                                                                    
      if (abs(particle->pdg_id()) != MONO_PID || particle->status() != 1) continue;

      if(particle->pdg_id() > 0){
	//m_mono_p   = chow::mag(particle->momentum().px()
	//		       ,particle->momentum().py()
	//		       ,particle->momentum().pz());
	m_mono_eta = particle->momentum().eta();
	m_mono_phi = particle->momentum().phi();
	m_mono_m   = particle->momentum().m();
	m_mono_px  = particle->momentum().px();
	m_mono_py  = particle->momentum().py();
	m_mono_pz  = particle->momentum().pz();
	m_mono_x   = particle->momentum().x();
	m_mono_y   = particle->momentum().y();
	m_mono_z   = particle->momentum().z();
	                                                                                              
		extrap.setMonopole(*particle);
		m_monoExp_eta = extrap.etaVr(1.29);
		m_monoExp_phi = extrap.phi();
		m_monoExpEE_eta = extrap.etaVz(3.144);
		m_monoExpEE_phi = extrap.phi();
      }

      else{
	//m_amon_p   = chow::mag(particle->momentum().px()
	//     	       ,particle->momentum().py()
	//		       ,particle->momentum().pz());
	m_amon_eta = particle->momentum().eta();
	m_amon_phi = particle->momentum().phi();
	m_amon_m   = particle->momentum().m();
	m_amon_px  = particle->momentum().px();
	m_amon_py  = particle->momentum().py();
	m_amon_pz  = particle->momentum().pz();
	m_amon_x   = particle->momentum().x();
	m_amon_y   = particle->momentum().y();
	m_amon_z   = particle->momentum().z();
	                                                                                              
		extrap.setMonopole(*particle);
		m_amonExp_eta = extrap.etaVr(1.29);
		m_amonExp_phi = extrap.phi();
      }
    }








/*



  std::vector<const HepMC::GenParticle *> d1 = snoopy.monoDaughter(Mono::monopole);                                                            
  std::vector<const HepMC::GenParticle *> d2 = snoopy.monoDaughter(Mono::anti_monopole);                                                       
  for ( unsigned i=0; i < d1.size(); i++ ) {                                                                                                   
    std::cout << "Found monopole daughter!" << std::endl;                                                                                      
    m_d1_eta.push_back( d1[i]->momentum().eta() );                                                                                             
    m_d1_phi.push_back( d1[i]->momentum().phi() );                                                                                             
    m_d1_pdgId.push_back( d1[i]->pdg_id() );                                                                                                   
  }                                                                                                                                            
  for ( unsigned i=0; i < d2.size(); i++ ) {                                                                                                   
    std::cout << "Found monopole daughter!" << std::endl;                                                                                      
    m_d2_eta.push_back( d2[i]->momentum().eta() );                                                                                             
    m_d2_phi.push_back( d2[i]->momentum().phi() );                                                                                             
    m_d2_pdgId.push_back( d2[i]->pdg_id() );                                                                                                   
  }                                                   

  //  Mono::MonoGenTrackExtrapolator extrap;

  if ( mono ) {
    m_mono_p = chow::mag( mono->momentum().px(), mono->momentum().py(), mono->momentum().pz() );
    m_mono_eta = mono->momentum().eta();
    m_mono_phi = mono->momentum().phi();
    m_mono_m = mono->momentum().m();
    m_mono_px = mono->momentum().px();
    m_mono_py = mono->momentum().py();
    m_mono_pz = mono->momentum().pz();
    m_mono_x = mono->momentum().x();
    m_mono_y = mono->momentum().y();
    m_mono_z = mono->momentum().z();


    //    extrap.setMonopole(*mono);
    // m_monoExp_eta = extrap.etaVr(1.29);
    // m_monoExp_phi = extrap.phi();
    // m_monoExpEE_eta = extrap.etaVz(3.144);
    // m_monoExpEE_phi = extrap.phi();
  }


  if ( amon ) { 
    m_amon_p = chow::mag( amon->momentum().px(), amon->momentum().py(), amon->momentum().pz() );
    m_amon_eta = amon->momentum().eta();
    m_amon_phi = amon->momentum().phi();
    m_amon_m = amon->momentum().m(); 
    m_amon_px = amon->momentum().px();
    m_amon_py = amon->momentum().py();
    m_amon_pz = amon->momentum().pz();
    m_amon_x = amon->momentum().x();
    m_amon_y = amon->momentum().y();
    m_amon_z = amon->momentum().z();

    //    extrap.setMonopole(*amon);
    // m_amonExp_eta = extrap.etaVr(1.29);
    // m_amonExp_phi = extrap.phi();
  }


*/


  Mono::EcalMapper ecalMapper(iSetup);
  ecalMapper.fillMap(iEvent);

  edm::ESHandle<CaloGeometry> calo;
  iSetup.get<CaloGeometryRecord>().get(calo);
  const CaloGeometry *m_caloGeo = (const CaloGeometry*)calo.product();
  const CaloSubdetectorGeometry *geom = m_caloGeo->getSubdetectorGeometry(DetId::Ecal,EcalBarrel);
  // loop over Hits                                                                                                                                              
  EBRecHitCollection::const_iterator itHit = EcalRecHits->begin();
  for ( ; itHit != EcalRecHits->end(); itHit++ ) {

    EBDetId detId( (*itHit).id() );
    const CaloCellGeometry *cell = geom->getGeometry( detId );
    m_h_2_tVsE->Fill((*itHit).energy(),(*itHit).time());
    m_EcalTime.push_back( (*itHit).time() );
    m_EcalE.push_back( (*itHit).energy() );
    m_EcalEta.push_back( cell->getPosition().eta() );
    m_EcalPhi.push_back( cell->getPosition().phi() );
  }
  m_tree->Fill();

}



void MonoRecAnalysis::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{

  bool changed=false;
  hltPSProv_.init(run,setup,hltProcess_,changed);
  const l1t::L1TGlobalUtil& l1GtUtils = hltPSProv_.l1tGlobalUtil();
  std::cout <<"l1 menu "<<l1GtUtils.gtTriggerMenuName()<<" version "<<l1GtUtils.gtTriggerMenuVersion()<<" comment "<<std::endl;
  std::cout <<"hlt name "<<hltPSProv_.hltConfigProvider().tableName()<<std::endl;
}



void MonoRecAnalysis::beginJob()
{

  m_outputFile = new TFile(m_output.c_str(), "recreate");
  m_h_2_tVsE = m_fs->make<TH2D>("tVsE","tVsE",50,0.,20.,50,-15.,30.);
  m_tree = m_fs->make<TTree>("tree","tree");
  m_tree->Branch("EcalTime",&m_EcalTime);
  m_tree->Branch("EcalEnergy",&m_EcalE);
  m_tree->Branch("EcalEta",&m_EcalEta);
  m_tree->Branch("EcalPhi",&m_EcalPhi);

  //  m_tree->Branch("mono_eta",&m_mono_eta);
  //  m_tree->Branch("mono_eta",&m_m1_eta);
 
  m_tree->Branch("mono_p", &m_mono_p, "mono_p/D");
  m_tree->Branch("mono_eta", &m_mono_eta, "mono_eta/D");
  m_tree->Branch("mono_phi", &m_mono_phi, "mono_phi/D");
  m_tree->Branch("mono_m", &m_mono_m, "mono_m/D");
  m_tree->Branch("mono_px",&m_mono_px, "mono_px/D");
  m_tree->Branch("mono_py",&m_mono_py, "mono_py/D");
  m_tree->Branch("mono_pz",&m_mono_pz, "mono_pz/D");
  m_tree->Branch("mono_x",&m_mono_x, "mono_x/D");
  m_tree->Branch("mono_y",&m_mono_y, "mono_y/D");
  m_tree->Branch("mono_z",&m_mono_z, "mono_z/D");
  
  m_tree->Branch("monoExp_eta",&m_monoExp_eta, "monoExp_eta/D");
  m_tree->Branch("monoExp_phi",&m_monoExp_phi, "monoExp_phi/D");

  m_tree->Branch("amon_p", &m_amon_p, "amon_p/D");
  m_tree->Branch("amon_eta", &m_amon_eta, "amon_eta/D");
  m_tree->Branch("amon_phi", &m_amon_phi, "amon_phi/D");
  m_tree->Branch("amon_m", &m_amon_m, "amon_m/D");
  m_tree->Branch("amon_px",&m_amon_px, "amon_px/D");
  m_tree->Branch("amon_py",&m_amon_py, "amon_py/D");
  m_tree->Branch("amon_pz",&m_amon_pz, "amon_pz/D");
  m_tree->Branch("amon_x",&m_amon_x, "amon_x/D");
  m_tree->Branch("amon_y",&m_amon_y, "amon_y/D");
  m_tree->Branch("amon_z",&m_amon_z, "amon_z/D");

  m_tree->Branch("amonExp_eta",&m_amonExp_eta, "amonExp_eta/D");
  m_tree->Branch("amonExp_phi",&m_amonExp_phi, "amonExp_phi/D");







  //m_tree->Branch("mono_phi",&m_mono_phi);
  //m_tree->Branch("monoExp_eta",&m_monoExp_eta);
  //m_tree->Branch("mono_z",&m_mono_z);
  //m_tree->Branch("monoExp_phi",&m_monoExp_phi);
  //m_tree->Branch("mono_p",&m_mono_p);
  // m_tree->Branch("mono_m",&m_mono_m);
  // m_tree->Branch("anti_eta",&m_amon_eta);
  // m_tree->Branch("anti_phi",&m_amon_phi);
  // m_tree->Branch("antiExp_eta",&m_amonExp_eta);
  // m_tree->Branch("anti_z",&m_amon_z);
  // m_tree->Branch("antiExp_phi",&m_amonExp_phi);
  // m_tree->Branch("anti_p",&m_amon_p);
  // m_tree->Branch("anti_m",&m_amon_m);
 


 m_tree->Branch("md_eta",&m_d1_eta);
  m_tree->Branch("md_phi",&m_d1_phi);
  m_tree->Branch("md_pdgId",&m_d1_pdgId);
  m_tree->Branch("ad_eta",&m_d2_eta);
  m_tree->Branch("ad_phi",&m_d2_phi);
  m_tree->Branch("ad_pdgId",&m_d2_pdgId);
  m_tree->Branch("run",&m_run);
  m_tree->Branch("lumi",&m_lumi);
  m_tree->Branch("event",&m_event);


}


void MonoRecAnalysis::endJob()
{

  m_outputFile->cd();
  m_tree->Write();
  m_outputFile->Close();




}

void MonoRecAnalysis::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
void  MonoRecAnalysis::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
void MonoRecAnalysis::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation                                                                            
  // Please change this to state exactly what you do use, even if it is no parameters                                                                            
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


void MonoRecAnalysis::clear()
{
  /*  m_mono_eta = 0.;
  m_mono_phi = 0.;
  m_amon_eta = 0.;
  m_amon_phi = 0.;

  m_monoExp_eta = 0.;
  m_monoExp_phi = 0.;
  m_mono_p = 0.;
  m_mono_m = 0.;
  m_amonExp_eta = 0.;
  m_amonExp_phi = 0.;
  m_amon_p = 0.;
  m_amon_p = 0.;
  */


  m_mono_p = 0.;
  m_mono_eta = 0.;
  m_mono_phi = 0.;
  m_mono_m = 0.;
  m_mono_px = 0.;
  m_mono_py = 0.;
  m_mono_pz = 0.;
  m_mono_x = 0.;
  m_mono_y = 0.;
  m_mono_z = 0.;

  m_monoExp_eta = 0.;
  m_monoExp_phi = 0.;

  m_amon_p = 0.;
  m_amon_eta = 0.;
  m_amon_phi = 0.;
  m_amon_m = 0.;
  m_amon_px = 0.;
  m_amon_py = 0.;
  m_amon_pz = 0.;
  m_amon_x = 0.;
  m_amon_y = 0.;
  m_amon_z = 0.;

  m_amonExp_eta = 0.;
  m_amonExp_phi = 0.;
  m_run = 0.;
  m_lumi = 0.;
  m_event = 0.;

  m_EcalTime.clear();
  m_EcalE.clear();
  m_EcalEta.clear();
  m_EcalPhi.clear();

  m_d1_eta.clear();
  m_d1_phi.clear();
  m_d1_pdgId.clear();
  m_d2_eta.clear();
  m_d2_phi.clear();
  m_d2_pdgId.clear();

}






DEFINE_FWK_MODULE(MonoRecAnalysis);






