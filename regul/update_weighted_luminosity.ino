// update weigthed luminosity using exponential decay, this avoid enabling alarm with 
void updateWeightedLuminosity(int luminosity, float avgLightDecay, float weightedLuminosity) {
  weightedLuminosity = avgLightDecay * weightedLuminosity + (1-avgLightDecay) * luminosity;
}