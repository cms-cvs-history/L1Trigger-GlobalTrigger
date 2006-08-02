{

   gROOT->Reset();

   // Load CMS-specific dictionaries (WorkBookSetComputerNode#Set_up_ROOT)
   //
   gSystem->Load("libFWCoreFWLite.so") ;
   AutoLibraryLoader::enable() ;
   
   // Read in ROOT file 
   //
   TFile* f = new TFile("testGlobalTrigger.root") ;
   TTree* tEvt = f->Get("Events") ;
   
   // Check number of events in the tree
   
   int nEvt = tEvt->GetEntries() ;
   std::cout << "Number of events = " << nEvt << std::endl ;
   
   // print the tree structure
   tEvt->Print();
   
}
