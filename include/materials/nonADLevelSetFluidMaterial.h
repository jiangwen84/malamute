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
 *This class computes fluid properties in melt pool heat equations
 */
class nonADLevelSetFluidMaterial : public Material
{
public:
  static InputParameters validParams();

  nonADLevelSetFluidMaterial(const InputParameters & parameters);

protected:
  void computeQpProperties() override;

  /// Level set Heaviside function
  const MaterialProperty<Real> & _heaviside_function;
  const MaterialProperty<Real> & _heaviside_function_dc;

  /// Density
  MaterialProperty<Real> & _rho;
  MaterialProperty<Real> & _rho_dc;
  MaterialProperty<Real> & _rho_dT;

  /// Viscosity
  MaterialProperty<Real> & _mu;
  MaterialProperty<Real> & _mu_dc;
  MaterialProperty<Real> & _mu_dT;

  /// Gas density
  const Real & _rho_g;

  /// Liquid density
  const Real & _rho_l;

  /// Solid density
  const Real & _rho_s;

  /// Gas viscosity
  const Real & _mu_g;

  /// Liquid viscosity
  const Real & _mu_l;

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

  /// Permeability in Darcy term
  MaterialProperty<Real> & _permeability;
  MaterialProperty<Real> & _permeability_dT;

  /// Constant in Darcy term
  const Real & _K0;
};
