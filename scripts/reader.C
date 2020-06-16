
void reader(const char *fname)
{
  auto fin = new TFile(fname);
  dynamic_cast<EicToyModel *>(fin->Get("EicToyModel"));
} // reader()

// then: 
//  auto eic = EicToyModel::Instance();
//  eic->hdraw();
//  ...
