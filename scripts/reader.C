
void reader(const char *fname)
{
  // Import the EicToyModel model singleton class instance;
  EicToyModel::Import(fname);
} // reader()

// then: 
//
//    auto eic = EicToyModel::Instance();
//    eic->hdraw();
//    ...
