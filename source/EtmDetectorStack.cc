
#include <math.h>

#include <EtmDetectorStack.h>
#include <EicToyModel.h>

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

EtmDetectorStack::EtmDetectorStack(const char *label, double etamin, double etamax):
  mStartingDistance(0.0), mFullLength(0.0)
{
  if (label) mLabel = label;

  for(unsigned bf=0; bf<2; bf++)
    mEtaBoundaries[bf] = new EtmEtaBoundary(); 
   
  SetEtaBoundaries(etamin, etamax);

  mDummyDetector = new EtmDetector(this, 0, 0, kBlack, 0.0);
} // EtmDetectorStack::EtmDetectorStack()

// ---------------------------------------------------------------------------------------

void EtmDetectorStack::SetEtaBoundaries(double min, double max) 
{
  mEtaBoundaries[0]->SetEta(min); mEtaBoundaries[1]->SetEta(max); 

  if (min && max) CalculateAlignmentAxis();
} // EtmDetectorStack::SetEtaBoundaries()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

void EtmDetectorStack::offset(double dstart)
{
  if (dstart >= 0.0) mStartingDistance = dstart;
} // EtmDetectorStack::offset()

// ---------------------------------------------------------------------------------------

EtmDetector *EtmDetectorStack::CreateDetector(const char *tag, double length)
{
  auto eic = EicToyModel::Instance();
  int color = eic->Palette().GetColor(tag);
  if (color == -1) {
    printf("Tag '%s' does not exist!\n", tag);
    return 0;
  } //if

  //auto det = new EtmDetector(this, tag, color, kBlack, length);
  auto det = new EtmDetector(this, tag, color, kWhite, length);
  det->SetLineWidth(3);

  return det;
} // EtmDetectorStack::CreateDetector()

// ---------------------------------------------------------------------------------------

EtmDetector *EtmDetectorStack::add(const char *tag, double length)
{
  auto det = CreateDetector(tag, length);

  if (!det) return 0;

  mDetectors.push_back(det); 
  
  EicToyModel::Instance()->home();
  
  return det;
} // EtmDetectorStack::add()

// ---------------------------------------------------------------------------------------

EtmDetector *EtmDetectorStack::add(const char *tag, int color, double length)
{
  // Not the most efficient call, but no cut'n'paste;
  return (EicToyModel::Instance()->Palette().AddEntry(tag, color) ? add(tag, length) : 0);
} // EtmDetectorStack::add()

// ---------------------------------------------------------------------------------------

EtmDetector *EtmDetectorStack::marker( void ) 
{ 
  auto marker = get(_MARKER_);

  if (marker != mDummyDetector) {
    printf("'%s' detector must be unique (one per stack)!\n", _MARKER_);
    return marker;
  } //if
  
  return add(_MARKER_, 0.0); 
} // EtmDetectorStack::marker()

// ---------------------------------------------------------------------------------------

EtmDetector *EtmDetectorStack::insert(const char *tag, double length, 
				      const char *before, unsigned order)
{
  auto det = CreateDetector(tag, length);
  if (!det) return 0;

  auto eic = EicToyModel::Instance();
  unsigned counter = 0;

  for (auto it = mDetectors.begin(); it != mDetectors.end(); it++)
    if (*(*it)->GetLabel() == before && order == counter++) {
      mDetectors.insert(it, det); 
      
      eic->home();

      return det;
    } //for it .. if

  // FIXME: memory leak;
  return 0;
} // EtmDetectorStack::insert()

// ---------------------------------------------------------------------------------------

void EtmDetectorStack::CalculateAlignmentAxis( void )
{
  // FIXME: this is not clean, but should suffice;
  double alignment_theta = mEtaBoundaries[0]->GetEta()*mEtaBoundaries[1]->GetEta() >= 0.0 ?
    (mEtaBoundaries[0]->GetEta() > 0.0 ? 0.0 : M_PI) : M_PI/2;

  mAlignmentAxis = TVector2(cos(alignment_theta), sin(alignment_theta));
} // EtmDetectorStack::CalculateAlignmentAxis()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

//
// FIXME: unify all these later;
//

void EtmDetectorStack::rm(const char *dname)
{
  if (!dname) return;

  auto eic = EicToyModel::Instance();
  unsigned counter = 0;

  for(auto det: mDetectors) 
    if (*det->GetLabel() == dname) 
      counter++;

  if (counter == 1)
    for (auto it = mDetectors.begin(); it != mDetectors.end(); )
      (*(*it)->GetLabel() == dname) ? it = mDetectors.erase(it) : it++;

  eic->home();
} // EtmDetectorStack::rm()

// ---------------------------------------------------------------------------------------

void EtmDetectorStack::rm(const char *dname, unsigned order)
{
  if (!dname) return;

  auto eic = EicToyModel::Instance();
  unsigned counter = 0;

  for (auto it = mDetectors.begin(); it != mDetectors.end(); )
    (*(*it)->GetLabel() == dname && order == counter++) ? it = mDetectors.erase(it) : it++;

  eic->home();
} // EtmDetectorStack::rm()

// ---------------------------------------------------------------------------------------

EtmDetector *EtmDetectorStack::get(const char *dname, unsigned order) const
{
  if (!dname) return mDummyDetector;//0;

  unsigned counter = 0;
  
  for(auto det: mDetectors) 
    if (*det->GetLabel() == dname) 
      if (order == counter++) 
	return det;

  // FIXME: printf a message and return dummy pointer rather than crash;
  //return 0;
  return mDummyDetector;
} // EtmDetectorStack::get()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

void EtmDetectorStack::CalculateActualDistances( void )
{
  auto eic = EicToyModel::Instance();
  mFullLength = mStartingDistance;
  
  // Kind of regularization;
  if (this == eic->mid() && mStartingDistance < eic->vtx()->FullLength()) 
    mFullLength = eic->vtx()->FullLength();

  for(auto det: mDetectors) {
    det->mActualDistance = mFullLength + det->length()/2;
    mFullLength += det->length();
  } //for det
} // EtmDetectorStack::CalculateActualDistances()

// ---------------------------------------------------------------------------------------

void EtmDetectorStack::Build( void )
{  
  for(auto det: mDetectors) 
    det->Build();
} // EtmDetectorStack::Build()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

ClassImp(EtmDetectorStack)
