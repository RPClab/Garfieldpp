#ifndef G_MEDIUM_MAGBOLTZ_9
#define G_MEDIUM_MAGBOLTZ_9

#include <array>

#include "MediumGas.hh"

namespace Garfield {

/// Interface to %Magboltz (version 9).
///  - http://magboltz.web.cern.ch/magboltz/

class MediumMagboltz : public MediumGas {

 public:
  /// Constructor
  MediumMagboltz();
  /// Destructor
  virtual ~MediumMagboltz() {}

  /// Set the highest electron energy to be included
  //// in the scattering rates table.
  bool SetMaxElectronEnergy(const double e);
  /// Get the highest electron energy in the scattering rates table.
  double GetMaxElectronEnergy() const { return m_eFinal; }

  // Set/get the highest photon energy to be included
  // in the scattering rates table
  bool SetMaxPhotonEnergy(const double e);
  double GetMaxPhotonEnergy() const { return m_eFinalGamma; }

  // Switch on/off automatic adjustment of max. energy when an
  // energy exceeding the present range is requested
  void EnableEnergyRangeAdjustment() { m_useAutoAdjust = true; }
  void DisableEnergyRangeAdjustment() { m_useAutoAdjust = false; }

  // Switch on/off anisotropic scattering (enabled by default)
  void EnableAnisotropicScattering() {
    m_useAnisotropic = true;
    m_isChanged = true;
  }
  void DisableAnisotropicScattering() {
    m_useAnisotropic = false;
    m_isChanged = true;
  }

  // Select secondary electron energy distribution parameterization
  void SetSplittingFunctionOpalBeaty();
  void SetSplittingFunctionGreenSawada();
  void SetSplittingFunctionFlat();

  // Switch on/off de-excitation handling
  void EnableDeexcitation();
  void DisableDeexcitation() { m_useDeexcitation = false; }
  // Switch on/off discrete photoabsorption levels
  void EnableRadiationTrapping();
  void DisableRadiationTrapping() { m_useRadTrap = false; }

  // Switch on/off simplified simulation of Penning transfers by means of
  // transfer probabilities (not compatible with de-excitation handling)
  void EnablePenningTransfer(const double r, const double lambda);
  void EnablePenningTransfer(const double r, const double lambda,
                             std::string gasname);
  void DisablePenningTransfer();
  void DisablePenningTransfer(std::string gasname);

  // When enabled, the gas cross-section table is written to file
  // when loaded into memory.
  void EnableCrossSectionOutput(const bool on = true) { m_useCsOutput = on; }

  // Multiply excitation cross-sections by a uniform scaling factor
  void SetExcitationScalingFactor(const double r, std::string gasname);

  bool Initialise(const bool verbose = false);
  void PrintGas();

  // Get the overall null-collision rate [ns-1]
  double GetElectronNullCollisionRate(const int band) override;
  // Get the (real) collision rate [ns-1] at a given electron energy e [eV]
  double GetElectronCollisionRate(const double e, const int band) override;
  // Get the collision rate [ns-1] for a specific level
  double GetElectronCollisionRate(const double e, const unsigned int level,
                                  const int band);
  // Sample the collision type
  bool GetElectronCollision(const double e, int& type, int& level, double& e1,
                            double& dx, double& dy, double& dz, 
                            std::vector<std::pair<int, double> >& secondaries,
                            int& ndxc, int& band) override;
  void ComputeDeexcitation(int iLevel, int& fLevel);
  unsigned int GetNumberOfDeexcitationProducts() const override { 
    return m_dxcProducts.size();
  }
  bool GetDeexcitationProduct(const unsigned int i, double& t, double& s, 
                              int& type, double& energy) const override;

  double GetPhotonCollisionRate(const double e) override;
  bool GetPhotonCollision(const double e, int& type, int& level, double& e1,
                          double& ctheta, int& nsec, double& esec) override;

  // Reset the collision counters
  void ResetCollisionCounters();
  // Get total number of electron collisions
  unsigned int GetNumberOfElectronCollisions() const;
  // Get number of collisions broken down by cross-section type
  unsigned int GetNumberOfElectronCollisions(unsigned int& nElastic, 
    unsigned int& nIonising, unsigned int& nAttachment, 
    unsigned int& nInelastic, unsigned int& nExcitation,
    unsigned int& nSuperelastic) const;
  // Get number of cross-section terms
  int GetNumberOfLevels();
  // Get detailed information about a given cross-section term i
  bool GetLevel(const unsigned int i, int& ngas, int& type, std::string& descr,
                double& e);
  // Get number of collisions for a specific cross-section term
  unsigned int GetNumberOfElectronCollisions(const unsigned int level) const;

  int GetNumberOfPenningTransfers() const { return m_nPenning; }

  // Get total number of photon collisions
  unsigned int GetNumberOfPhotonCollisions() const;
  // Get number of photon collisions by collision type
  unsigned int GetNumberOfPhotonCollisions(unsigned int& nElastic, 
    unsigned int& nIonising, unsigned int& nInelastic) const;

  void RunMagboltz(const double e, const double b, const double btheta,
                   const int ncoll, bool verbose, double& vx, double& vy,
                   double& vz, double& dl, double& dt, 
                   double& alpha, double& eta, double& lor,
                   double& vxerr, double& vyerr, double& vzerr,
                   double& dlerr, double& dterr, 
                   double& alphaerr, double& etaerr, double& lorerr,
                   double& alphatof);

  // Generate a new gas table (can later be saved to file)
  void GenerateGasTable(const int numCollisions = 10,
                        const bool verbose = true);

 private:
  static const int nEnergySteps = 20000;
  static const int nEnergyStepsLog = 200;
  static const int nEnergyStepsGamma = 5000;
  static const int nMaxInelasticTerms = 250;
  static const int nMaxLevels = 512;
  static const int nCsTypes = 6;
  static const int nCsTypesGamma = 4;

  static const int DxcTypeRad;
  static const int DxcTypeCollIon;
  static const int DxcTypeCollNonIon;

  // Energy spacing of collision rate tables
  double m_eFinal, m_eStep;
  double m_eHigh, m_eHighLog;
  double m_lnStep;
  bool m_useAutoAdjust = true;

  // Flag enabling/disabling output of cross-section table to file
  bool m_useCsOutput = false;
  // Number of different cross-section types in the current gas mixture
  unsigned int m_nTerms = 0;
  // Recoil energy parameter
  std::array<double, m_nMaxGases> m_rgas;
  // Opal-Beaty-Peterson splitting parameter [eV]
  std::array<double, nMaxLevels> m_wOpalBeaty;
  /// Green-Sawada splitting parameters [eV] 
  /// (&Gamma;s, &Gamma;b, Ts, Ta, Tb).
  std::array<std::array<double, 5>, m_nMaxGases> m_parGreenSawada;
  std::array<bool, m_nMaxGases> m_hasGreenSawada;

  // Energy loss
  std::array<double, nMaxLevels> m_energyLoss;
  // Cross-section type
  std::array<int, nMaxLevels> m_csType;

  // Parameters for calculation of scattering angles
  bool m_useAnisotropic = true;
  std::vector<std::vector<double> > m_scatPar;
  std::vector<std::vector<double> > m_scatCut;
  std::vector<std::vector<double> > m_scatParLog;
  std::vector<std::vector<double> > m_scatCutLog;
  std::array<int, nMaxLevels> m_scatModel;

  // Level description
  std::vector<std::string> m_description;

  // Total collision frequency
  std::vector<double> m_cfTot;
  std::vector<double> m_cfTotLog;
  // Null-collision frequency
  double m_cfNull = 0.;
  // Collision frequencies
  std::vector<std::vector<double> > m_cf;
  std::vector<std::vector<double> > m_cfLog;

  // Collision counters
  // 0: elastic
  // 1: ionisation
  // 2: attachment
  // 3: inelastic
  // 4: excitation
  // 5: super-elastic
  std::array<unsigned int, nCsTypes> m_nCollisions;
  // Number of collisions for each cross-section term
  std::vector<unsigned int> m_nCollisionsDetailed;

  // Penning transfer
  // Penning transfer probability (by level)
  std::array<double, nMaxLevels> m_rPenning;
  // Mean distance of Penning ionisation (by level)
  std::array<double, nMaxLevels> m_lambdaPenning;
  // Number of Penning ionisations
  unsigned int m_nPenning = 0;

  // Deexcitation
  // Flag enabling/disabling detailed simulation of de-excitation process
  bool m_useDeexcitation = false;
  // Flag enabling/disable radiation trapping
  // (absorption of photons discrete excitation lines)
  bool m_useRadTrap = true;

  struct Deexcitation {
    // Gas component
    int gas;
    // Associated cross-section term
    int level;
    // Level description
    std::string label;
    // Energy
    double energy;
    // Branching ratios
    std::vector<double> p;
    // Final levels
    std::vector<int> final;
    // Type of transition
    std::vector<int> type;
    // Oscillator strength
    double osc;
    // Total decay rate
    double rate;
    // Doppler broadening
    double sDoppler;
    // Pressure broadening
    double gPressure;
    // Effective width
    double width;
    // Integrated absorption collision rate
    double cf;
  };
  std::vector<Deexcitation> m_deexcitations;
  // Mapping between deexcitations and cross-section terms.
  std::array<int, nMaxLevels> m_iDeexcitation;

  // List of de-excitation products
  struct dxcProd {
    // Radial spread
    double s;
    // Time delay
    double t;
    // Type of deexcitation product
    int type;
    // Energy of the electron or photon
    double energy;
  };
  std::vector<dxcProd> m_dxcProducts;

  // Ionisation potentials
  std::array<double, m_nMaxGases> m_ionPot;
  // Minimum ionisation potential
  double m_minIonPot = -1.;

  // Scaling factor for excitation cross-sections
  std::array<double, m_nMaxGases> m_scaleExc;
  // Flag selecting secondary electron energy distribution model
  bool m_useOpalBeaty = true;
  bool m_useGreenSawada = false;

  // Energy spacing of photon collision rates table
  double m_eFinalGamma, m_eStepGamma;
  // Number of photon collision cross-section terms
  unsigned int m_nPhotonTerms = 0;
  // Total photon collision frequencies
  std::vector<double> m_cfTotGamma;
  // Photon collision frequencies
  std::vector<std::vector<double> > m_cfGamma;
  std::vector<int> csTypeGamma;
  // Photon collision counters
  // 0: elastic
  // 1: ionisation
  // 2: inelastic
  // 3: excitation
  std::array<unsigned int, nCsTypesGamma> m_nPhotonCollisions;

  bool GetGasNumberMagboltz(const std::string& input, int& number) const;
  bool Mixer(const bool verbose = false);
  void SetupGreenSawada();
  void ComputeAngularCut(const double parIn, double& cut, double& parOut) const;
  void ComputeDeexcitationTable(const bool verbose);
  void AddPenningDeexcitation(Deexcitation& dxc, const double rate, 
                              const double pPenning) {
    dxc.p.push_back(rate * pPenning);
    dxc.p.push_back(rate * (1. - pPenning));
    dxc.type.push_back(DxcTypeCollIon);
    dxc.type.push_back(DxcTypeCollNonIon);
  }
  double RateConstantWK(const double energy, const double osc, 
    const double pacs, const int igas1, const int igas2) const;
  double RateConstantHardSphere(const double r1, const double r2,
    const int igas1, const int igas2) const;
  void ComputeDeexcitationInternal(int iLevel, int& fLevel);
  bool ComputePhotonCollisionTable(const bool verbose);
};
}
#endif
