
void reader(const char *fname)
{
  auto fin = new TFile(fname);
  //auto eic = dynamic_cast<EicToyModel *>(fin->Get("EicToyModel")); 
  dynamic_cast<EicToyModel *>(fin->Get("EicToyModel"));
} // reader()

// then: 
//  auto eic = EicToyModel::Instance();
//  eic->hdraw();
//  ...
