#include "seisGenPara.h"

SeisGenPara::SeisGenPara(double idt, double ierr, double itDur, double itRise, double itDrop, PSDFlag iflag)
:dt(idt),err(ierr),tDuration(itDur),tRise(itRise),tDrop(itDrop),flag(iflag)
{}