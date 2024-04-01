#include "RenderingPassFactory.h"

std::unordered_map<RenderPassType, IRenderPass*> RenderingPassFactory::renderPassMap;