#ifndef __FUN4ALL_MBD_PRDF_H__
#define __FUN4ALL_MBD_PRDF_H__

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4allraw/Fun4AllEventOutputManager.h>
#include <phool/recoConsts.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <globalvertex/GlobalVertexReco.h>
#include <mbd/MbdReco.h>

#if defined(__CLING__)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)
R__LOAD_LIBRARY(libmbd.so)
R__LOAD_LIBRARY(libglobalvertex.so)
#endif

void Fun4All_MBD_Prdf(const std::string input_file = "beam/beam_seb18-000020868-0000.prdf", const int nEvents = 0, const int nskip = 0, const string& cdbtag = "")
{
  
  pair<int, int> runseg = Fun4AllUtils::GetRunSegment(fname);
  int run_number = runseg.first;
  int seg = runseg.second;
  cout << "RUN\t" << run_number << "\t" << seg << endl;

  recoConsts *rc = recoConsts::instance();
  rc->set_uint64Flag("TIMESTAMP", run_number);

  if ( cdbtag.size() != 0 )
  {
    rc->set_StringFlag("CDB_GLOBALTAG",cdbtag); 
    //rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2023"); 
    //rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2024"); 

    cout << "Using CDB_GLOBALTAG " << cdbtag << endl;
  }
  else
  {
    // For local calibrations
    TString bdir = "./results/";
    bdir += run_number;
    cout << bdir << endl;
    rc->set_StringFlag("MBD_CALDIR",bdir.Data()); 

    cout << "Using MBD_CALDIR " << bdir << endl;
  }

  Fun4AllServer *se = Fun4AllServer::instance();
  //se->Verbosity(1);

  // Sync Headers and Flags
  SyncReco *sync = new SyncReco();
  se->registerSubsystem(sync);

  HeadReco *head = new HeadReco();
  se->registerSubsystem(head);

  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  // MBD/BBC Reconstruction
  MbdReco *mbdreco = new MbdReco();
  se->registerSubsystem(mbdreco);

  // Official vertex storage
  GlobalVertexReco *gvertex = new GlobalVertexReco();
  se->registerSubsystem(gvertex);

  Fun4AllInputManager *in = new Fun4AllPrdfInputManager("PRDFin");
  in->fileopen(input_file);
  //in->Verbosity(1);
  se->registerInputManager(in);

  std::filesystem::path filePath(input_file);
  TString outfile = filePath.filename().string();
  outfile.ReplaceAll(".prdf",".root");
  cout << outfile << endl;
  Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", outfile.Data());
  se->registerOutputManager(out);

  se->skip(nskip);
  se->run(nEvents);

  se->End();
  delete se;
  gSystem->Exit(0);

  cout << "Finished" << endl;
}

#endif

