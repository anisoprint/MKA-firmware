

#pragma once

struct PrintFileExtruderInfo {
	  char PlasticMaterialName[MAT_NAME_SIZE];
	  float PlasticConsumption;
	  char FiberMaterialName[MAT_NAME_SIZE];
	  float FiberConsumption;
  };

  struct PrintFileInfo {
	  uint32_t PrintDuration;
	  PrintFileExtruderInfo ExtruderInfo[HOTENDS];
  };
