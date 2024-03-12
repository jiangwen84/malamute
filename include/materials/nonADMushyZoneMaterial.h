/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "ADMaterial.h"

/**
 * This class computes material properties in the mushy zone.
 */
class nonADMushyZoneMaterial : public Material
{
public:
  static InputParameters validParams();

  nonADMushyZoneMaterial(const InputParameters & parameters);

protected:
  void computeQpProperties() override;

  /// Temperature variable
  const VariableValue & _temp;

  /// Solidus temperature
  const Real & _solidus_temperature;

  /// Liquidus  temperature
  const Real & _liquidus_temperature;

  /// Liquid mass fraction
  MaterialProperty<Real> & _f_l;
  MaterialProperty<Real> & _f_l_dT;

  /// Solid mass fraction
  MaterialProperty<Real> & _f_s;
  MaterialProperty<Real> & _f_s_dT;

  /// Liquid volume fraction
  MaterialProperty<Real> & _g_l;
  MaterialProperty<Real> & _g_l_dT;

  /// Solid volume fraction
  MaterialProperty<Real> & _g_s;
  MaterialProperty<Real> & _g_s_dT;

  /// Solid density
  const Real & _rho_s;

  /// Liquid density
  const Real & _rho_l;
};
