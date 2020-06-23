
void vc_viewer(const char *fname)
{
  TEveManager::Create();

  gGeoManager = gEve->GetGeometry(fname);

  TEveGeoTopNode *world = new TEveGeoTopNode(gGeoManager, gGeoManager->GetTopNode());
  // This is essential: want to see the top level objects only;
  world->SetVisLevel(1);
  gEve->AddGlobalElement(world);
  
  gEve->FullRedraw3D(kTRUE);
} // vc_viewer()
