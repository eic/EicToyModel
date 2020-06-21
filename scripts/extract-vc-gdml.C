
void extract_vc_gdml(const char *fin, const char *fout)
{
  TFile froot(fin);
  auto ostr = dynamic_cast<TObjString *>(froot.Get("VC.GDML"));
  auto &str = ostr->GetString();

  std::ofstream fgdml(fout);
  fgdml << str;
  fgdml.close();

  exit(0);
} // extract_vc_gdml()
