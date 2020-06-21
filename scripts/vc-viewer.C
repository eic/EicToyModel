
void vc_viewer(const char *fname)
{
  TEveManager::Create();

  gGeoManager = gEve->GetGeometry(fname);
  
  TEveGeoTopNode *world = new TEveGeoTopNode(gGeoManager, gGeoManager->GetTopNode());
  world->SetVisLevel(6);
  gEve->AddGlobalElement(world);
  
  gEve->FullRedraw3D(kTRUE);
} // vc_viewer()
