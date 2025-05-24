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
class SolidificationRate : public Material
{
public:
  static InputParameters validParams();

  SolidificationRate(const InputParameters & parameters);
  virtual void initQpStatefulProperties() override;

protected:
  void computeQpProperties() override;

  /// Solidus temperature
  const Real & _solidus_temperature;

  /// Liquidus  temperature
  const Real & _liquidus_temperature;

  const ADMaterialProperty<Real> & _thermal_conductivity;

  const ADVectorVariableValue & _temp_grad;

  const ADVariableValue & _temp;
  const VariableValue & _temp_old;

  MaterialProperty<Real> & _temp_gradient;

  MaterialProperty<Real> & _cooling_rate;

  MaterialProperty<Real> & _solidification_rate;

  MaterialProperty<Real> & _liquidus_time;
  const MaterialProperty<Real> & _liquidus_time_old;
  MaterialProperty<Real> & _solidus_time;
  const MaterialProperty<Real> & _solidus_time_old;
};
