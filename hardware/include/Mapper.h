#ifndef MAPPER_H
#define MAPPER_H

#include <Arduino.h>
#include "SensorMatrix.h"

// Print mapping as JSON so the host can map sensor ids to labels.
// Example output:
// {"mapping":[{"id":0,"label":"A1","row":0,"col":0}, {"id":1,"label":"A2","row":0,"col":1}, ...]}
void printMappingJson(const SensorMatrix* sensorMatrix);

#endif