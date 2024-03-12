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
 * This class computes Heaviside function given by a level set
 */
class nonADLevelSetHeavisideFunction : public Material
{
public:
  static InputParameters validParams();

  nonADLevelSetHeavisideFunction(const InputParameters & parameters);

protected:
  void computeQpProperties() override;

  /// Level set variable
  const VariableValue & _c;

  /// Heaviside function
  MaterialProperty<Real> & _heaviside_function;

  /// Heaviside function
  MaterialProperty<Real> & _heaviside_function_dc;
};
