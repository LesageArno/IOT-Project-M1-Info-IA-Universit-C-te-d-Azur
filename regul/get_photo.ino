#define photoPin 33

// Luminosity getter
int getPhoto() {
  return analogRead(photoPin);//(4095-analogRead(photoPin))/4095;
}