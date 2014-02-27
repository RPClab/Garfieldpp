#ifndef PAIRPROD_H
#define PAIRPROD_H
#include "wcpplib/random/PointsRan.h"
#include "wcpplib/stream/prstream.h"
#include "wcpplib/util/String.h"

/*
Class defining the pair production occured at passing the
delta-electroon through the matter.

All energies are assumed in eV.
But it seems that it is not strict requirement.
If to change this in file "file_name" and in interface,
the energies could be in any units.

2003, I. Smirnov
*/

class PairProd : public RegPassivePtr {
 public:
  PairProd(void) { ; }
  PairProd(const String& file_name, double fwa, double ffactorFano = 0.19);
  double get_eloss(void) const;         // in eV
  double get_eloss(double ecur) const;  // in eV assuming V = wa / 2.0
                                        // ecur is also in eV
  // No it can be different, see PairProd.c

  virtual void print(std::ostream& file, int l) const;

 private:
  double wa;
  double factorFano;
  double wa_table;
  double factorFano_table;
  double I_table;
  double J_table;
  /* Coefficients for conversion of the table parameter W=30 and F=0.16
     to the requested parameters by the formula
     e = e_table * k + s, where e_table is the random energy generated by the
     table.
     The calculations use the features found at MC calculations:
     If w is encreased by n times (with the same form of distribution,
     that is equivalent to the shift of distribution,
     the dispersion is changed as 1/n^2 D_old.
     If the resulting energy is multiplied by k, that is the proportional
     extension of the distribution, the relative dispersion is not changed.
     The relative dispersion is (<N^2> - <N>^2)/N.
     If to add the onbvious change of the mean energy occured with
     the change of these parameters,
     it is possible to obtain the formulae for conversion.
  */
  double k;
  double s;
  //DynLinArr< double > xx;
  //DynLinArr< double > yy;
  PointsRan pran;

};

#endif
