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
 * This class computes thermal properties in melt pool heat equations
 */
class nonADLevelSetThermalMaterial : public Material
{
public:
  static InputParameters validParams();

  nonADLevelSetThermalMaterial(const InputParameters & parameters);

protected:
  void computeQpProperties() override;

  /// Temperature variable
  const VariableValue & _temp;

  /// Heavisde function
  const MaterialProperty<Real> & _heaviside_function;

  /// Heavisde function dc
  const MaterialProperty<Real> & _heaviside_function_dc;

  /// Enthalpy
  MaterialProperty<Real> & _h;

  MaterialProperty<Real> & _h_dc;
  MaterialProperty<Real> & _h_dT;

  /// Thermal conductivity
  MaterialProperty<Real> & _k;
  MaterialProperty<Real> & _k_dc;
  MaterialProperty<Real> & _k_dT;

  /// Specifc heat
  MaterialProperty<Real> & _cp;
  MaterialProperty<Real> & _cp_dc;
  MaterialProperty<Real> & _cp_dT;

  /// Gas specific heat
  const Real & _c_g;

  /// Solid specific heat
  const Real & _c_s;

  /// Liquid specific heat
  const Real & _c_l;

  /// Gas thermal conductivity
  const Real & _k_g;

  /// Solid thermal conductivity
  const Real & _k_s;

  /// Liquid thermal conductivity
  const Real & _k_l;

  /// Latent heat
  const Real & _latent_heat;

  /// Solidus temperature
  const Real & _solidus_temperature;

  /// Liquid mass fraction
  const MaterialProperty<Real> & _f_l;
  const MaterialProperty<Real> & _f_l_dT;

  /// Solid mass fraction
  const MaterialProperty<Real> & _f_s;
  const MaterialProperty<Real> & _f_s_dT;

  /// Liquid volume fraction
  const MaterialProperty<Real> & _g_l;
  const MaterialProperty<Real> & _g_l_dT;

  /// Solid volume fraction
  const MaterialProperty<Real> & _g_s;
  const MaterialProperty<Real> & _g_s_dT;
};
