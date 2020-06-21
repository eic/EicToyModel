
void extract_cd_step(const char *fin, const char *fout)
{
  TFile froot(fin);
  auto ostr = dynamic_cast<TObjString *>(froot.Get("CD.STEP"));
  auto &str = ostr->GetString();

  std::ofstream fstep(fout);
  fstep << str;
  fstep.close();

  exit(0);
} // extract_cd_step()
