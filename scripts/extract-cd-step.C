
void extract_cd_step(const char *fin, const char *fout)
{
  TFile froot(fin);
  auto ostr = dynamic_cast<TObjString *>(froot.Get("CD.STEP"));
  if (ostr) {
    std::ofstream fstep(fout);
    fstep << ostr->GetString();
    fstep.close();
  } //if

  exit(0);
} // extract_cd_step()
