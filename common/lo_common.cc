#include "lo_common.h"
#include "scene.h"

extern void MaterialStart();
extern void MaterialFinish();

Scene *world;

Scene *GetWorld() {
  return world;
}

void GlobalStart() {
  world = new Scene();
  MaterialStart();
}

void GlobalFinish() {
  MaterialFinish();
  delete world;
}

