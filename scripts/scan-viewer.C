
void scan_viewer(const char *fname)
{
  TFile *ff = new TFile(fname);
  auto RL  = (TH2D*)ff->Get("RL");
  auto ZL  = (TH2D*)ff->Get("ZL");
  auto Bdl = (TH2D*)ff->Get("Bdl");

  gROOT->Reset();  

  gStyle->SetOptStat(0);
  gStyle->SetLabelFont(  52, "xy");
  gStyle->SetLabelSize(0.04, "xy");

  double hmargin = 0.12, vmargin = 0.12;
  gStyle->SetPadBottomMargin(vmargin);
  gStyle->SetPadTopMargin   (vmargin);
  gStyle->SetPadRightMargin (hmargin);
  gStyle->SetPadLeftMargin  (hmargin);

  auto c1 = new TCanvas("ETM", "ETM", 0, 0, 1500, 500);
  c1->Divide(3, 1);
  c1->UseCurrentStyle();
  c1->SetBorderMode(0);
  c1->SetFrameBorderMode(0);
  c1->SetFrameLineColor(kWhite);
  c1->SetFillColor(kWhite);

  double eta = 4.0, theta = 2*atan(exp(-eta))*1000;

  // NB: this does not work as a stack variable;
  auto ellipse = new TEllipse(0.0, 0.0, theta, theta);
  ellipse->SetLineColor(kWhite);//Green);
  ellipse->SetLineWidth(3);
  ellipse->SetLineStyle(9);
  ellipse->SetFillStyle(0);
  {
    gStyle->SetPalette(105);
    c1->cd(1);
    Double_t max = RL->GetYaxis()->GetXmax();
    gPad->DrawFrame(-max, -max, max, max, "Material budget in the acceptance, [%]");
    RL->SetMinimum(  1.0);
    RL->SetMaximum(100.0);
    RL->Draw("same colz pol");
    gPad->SetLogz();
    ellipse->Draw();
  }
  {
    c1->cd(2);
    Double_t max = ZL->GetYaxis()->GetXmax();
    gPad->DrawFrame(-max, -max, max, max, "Maximum lever arm for a silicon tracker, [cm]");
    TColor::InvertPalette();
    ZL->SetMinimum( -0.01);
    ZL->SetMaximum(200.0);
    ZL->Draw("same colz pol");
    ellipse->Draw();
  }
  {
    c1->cd(3);
    Double_t max = Bdl->GetYaxis()->GetXmax();
    gPad->DrawFrame(-max, -max, max, max, "Effective |Bt|*dl integral for a silicon tracker, [T*m]");
    Bdl->SetMinimum( -0.01);
    Bdl->SetMaximum(  0.50);
    Bdl->Draw("same colz pol");
    ellipse->Draw();
  }
} // scan_viewer()
