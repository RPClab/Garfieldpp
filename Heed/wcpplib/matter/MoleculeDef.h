#ifndef MOLECULE_DEF_H
#define MOLECULE_DEF_H

#include "wcpplib/matter/AtomDef.h"

namespace Heed {

class VanDerWaals {
 private:
  double ah;
  double bh;
  double Vkh;
  double Pkh;
  double Tkh;

 public:
  VanDerWaals(double fPk, double fTk);
  virtual ~VanDerWaals() {}
  double a(void) const { return ah; }
  double b(void) const { return bh; }
  double Vk(void) const { return Vkh; }
  double Pk(void) const { return Pkh; }
  double Tk(void) const { return Tkh; }
  /*
  double pressure(double M, // the number of moles
                  double volume,
                  double T);
  double volume(double T,  // relative to T_k
                double p,  // relative to p_k
                int &s_not_single);
  */
  // Return number of moles in the unit volume
  double volume_of_mole(double T, double p, int& s_not_single);

  macro_copy_header(VanDerWaals);
};
std::ostream& operator<<(std::ostream& file, const VanDerWaals& f);

/// Definition of molecule as a mixture of atoms.
/// Only the basic information: the name, the notation,
/// the mean charge and atomic weight and the parameters of mixture class.
///
/// The principle of definitions of matters is the same as for atoms:
/// a dictionary or a database. See details there. But the logbook is different,
/// of course.
///
/// 1998-2004 I. Smirnov

class MoleculeDef : public AtomMixDef {
  std::string nameh;
  std::string notationh;
  /// Number of atoms of particular sort in the molecule.
  /// Obviously it is not normalized to one, but instead
  /// the sum is equal to tqatomh
  std::vector<long> qatom_psh;
  long Z_totalh;
  double A_totalh;
  /// Total number of atoms in molecule
  /// Attention: this is not the number of different sorts of atoms
  /// The latter is qatom() from AtomMixDef
  long tqatomh;
  ActivePtr<VanDerWaals> awlsh;

 public:
  inline const std::string& name(void) const { return nameh; }
  inline const std::string& notation(void) const { return notationh; }
  inline const std::vector<long>& qatom_ps(void) const { return qatom_psh; }
  inline long qatom_ps(long n) const { return qatom_psh[n]; }
  inline long Z_total(void) const { return Z_totalh; }
  inline double A_total(void) const { return A_totalh; }
  inline long tqatom(void) const { return tqatomh; }
  inline const ActivePtr<VanDerWaals>& awls(void) const { return awlsh; }
  MoleculeDef(void);
  MoleculeDef(const std::string& fname, const std::string& fnotation,
              long fqatom, const std::vector<std::string>& fatom_not,
              const std::vector<long>& fqatom_ps,
              ActivePtr<VanDerWaals> fawls = ActivePtr<VanDerWaals>());
  MoleculeDef(const std::string& fname, const std::string& fnotation,
              const std::string& fatom_not, long fqatom_ps,
              ActivePtr<VanDerWaals> fawls = ActivePtr<VanDerWaals>());
  MoleculeDef(const std::string& fname, const std::string& fnotation,
              const std::string& fatom_not1, long fqatom_ps1,
              const std::string& fatom_not2, long fqatom_ps2,
              ActivePtr<VanDerWaals> fawls = ActivePtr<VanDerWaals>());
  MoleculeDef(const std::string& fname, const std::string& fnotation,
              const std::string& fatom_not1, long fqatom_ps1,
              const std::string& fatom_not2, long fqatom_ps2,
              const std::string& fatom_not3, long fqatom_ps3,
              ActivePtr<VanDerWaals> fawls = ActivePtr<VanDerWaals>());
  ~MoleculeDef();

  void print(std::ostream& file, int l) const;
  static void printall(std::ostream& file);
  /// Check that there is no molecule with the same name in the container
  void verify(void);
  static std::list<MoleculeDef*>& get_logbook(void);
  /// Initialize the logbook at the first request
  /// and keep it as internal static variable.
  static const std::list<MoleculeDef*>& get_const_logbook(void);
  /// Return the address of the molecule with this name.
  /// If there is no molecule with this notation, the function returns NULL
  /// but does not terminate the program as that for AtomDef. Be careful.
  static MoleculeDef* get_MoleculeDef(const std::string& fnotation);

  macro_copy_total(MoleculeDef);
};
std::ostream& operator<<(std::ostream& file, const MoleculeDef& f);
}

#endif
