/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#pragma once

#include "ADMaterial.h"

#define usingLevelSetTwoPhaseMateriallMembers usingMaterialMembers;

class LevelSetTwoPhaseMaterial : public ADMaterial
{
public:
  static InputParameters validParams();
  LevelSetTwoPhaseMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  const ADVariableValue & _phi;
  const Real _prop_value_1;
  const Real _prop_value_2;
  ADMaterialProperty<Real> & _prop;

};
