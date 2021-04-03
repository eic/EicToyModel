// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef ETMTRACKFASTSIMEVAL_H
#define ETMTRACKFASTSIMEVAL_H

#include <fun4all/SubsysReco.h>

#include <string>

class PHCompositeNode;
class TH1D;

class EtmTrackFastSimEval : public SubsysReco {
 public:
  // Default constructor;
  EtmTrackFastSimEval(const std::string& name = "EtmTrackFastSimEval",
                   const std::string& filename = "g4eval.root",
                   const std::string& trackmapname = "SvtxTrackMap");

  // Initialization;
  int Init(PHCompositeNode*);

  // Process Event;
  int process_event(PHCompositeNode*);

  // End, write and close files;
  int End(PHCompositeNode*);

  // Change output filename;
  void set_filename(const std::string &file) { _outfile_name = file; };

 private:
  // Output filename;
  std::string _outfile_name;

  // Name of SvtxTrackMap collection;
  std::string _trackmapname;

  // Event counter;
  int _event;

  // 1D dp/p histogram;
  TH1D *_h1d_Delta_mom;
};

#endif
