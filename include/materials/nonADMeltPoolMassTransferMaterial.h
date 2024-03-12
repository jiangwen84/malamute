/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "Material.h"

/**
 * This class computes extra residuals from mass transfer for the INS equations.
 */
class nonADMeltPoolMassTransferMaterial : public Material
{
public:
  static InputParameters validParams();

  nonADMeltPoolMassTransferMaterial(const InputParameters & parameters);

protected:
  void computeQpProperties() override;

  /// Temperature variable
  const VariableValue & _temp;

  /// Mass rate
  MaterialProperty<Real> & _melt_pool_mass_rate;
  MaterialProperty<Real> & _melt_pool_mass_rate_dT;

  /// Atomic weight
  const Real & _m;

  /// Boltzmann constant
  const Real & _boltzmann;

  /// Retrodiffusion coefficient
  const Real & _beta_r;

  /// Latent heat of vaporization
  const Real & _Lv;

  /// Vaporization temperature
  const Real & _vaporization_temperature;

  /// Reference pressure for vaporization
  const Real & _p0;

  /// Saturated vapor pressure
  MaterialProperty<Real> & _saturated_vapor_pressure;
  MaterialProperty<Real> & _saturated_vapor_pressure_dT;
};
