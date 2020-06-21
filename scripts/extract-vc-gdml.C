
void extract_vc_gdml(const char *fin, const char *fout)
{
  TFile froot(fin);
  auto ostr = dynamic_cast<TObjString *>(froot.Get("VC.GDML"));
  if (ostr) {
    std::ofstream fgdml(fout);
    fgdml << ostr->GetString();
    fgdml.close();
  } //if

  exit(0);
} // extract_vc_gdml()
