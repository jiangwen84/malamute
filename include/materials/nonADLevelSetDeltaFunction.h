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
 * This class computes delta function (derivative of the Heaviside function) given by a level set
 */
class nonADLevelSetDeltaFunction : public Material
{
public:
  static InputParameters validParams();

  nonADLevelSetDeltaFunction(const InputParameters & parameters);

protected:
  void computeQpProperties() override;

  /// Gradient of the level set variable
  const VariableGradient & _grad_c;

  /// Delta function
  MaterialProperty<Real> & _delta_function;

  MaterialProperty<RealVectorValue> & _delta_function_dc;
};
