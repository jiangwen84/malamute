/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "KernelValue.h"

/**
 * This class computes the laser heat source and heat loss in the melt pool heat equation.
 */
class nonADMeltPoolHeatSource : public KernelValue
{
public:
  static InputParameters validParams();

  nonADMeltPoolHeatSource(const InputParameters & parameters);

protected:
  Real precomputeQpResidual() override;

  Real precomputeQpJacobian() override;

  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  /// Delta function
  const MaterialProperty<Real> & _delta_function;

  /// Delta function dc
  const MaterialProperty<RealVectorValue> & _delta_function_dc;

  /// Laser power
  const Function & _power;

  /// Absorption coefficient
  const Real & _alpha;

  /// Laser beam radius
  const Real & _Rb;

  /// Heat transfer coefficient
  const Real & _Ah;

  /// Stefan Boltzmann constant
  const Real & _stefan_boltzmann;

  /// Material emissivity
  const Real & _varepsilon;

  /// Ambient temperature
  const Real & _T0;

  /// Function of laser location in x coordinate
  const Function & _laser_location_x;

  /// Function of laser location in x coordinate
  const Function & _laser_location_y;

  /// Function of laser location in x coordinate
  const Function & _laser_location_z;

  /// Density
  const MaterialProperty<Real> & _rho;

  /// Density dc
  const MaterialProperty<Real> & _rho_dc;

  /// Density dT
  const MaterialProperty<Real> & _rho_dT;

  /// Mass transfer rate
  const MaterialProperty<Real> & _melt_pool_mass_rate;

  /// Mass transfer rate dT
  const MaterialProperty<Real> & _melt_pool_mass_rate_dT;

  /// Specific heat
  const MaterialProperty<Real> & _cp;

  const MaterialProperty<Real> & _cp_dc;

  const MaterialProperty<Real> & _cp_dT;

  /// Latent heat of vaporization
  const Real _Lv;

  /// Liquid density
  const Real _rho_l;

  /// Gas density
  const Real _rho_g;

  const unsigned int _c_id;
};
