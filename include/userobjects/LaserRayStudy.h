//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "RepeatableRayStudyBase.h"

// Local includes
#include "BoundingBoxIntersectionHelper.h"
#include "Ray.h"

/**
 * A RayTracingStudy used for generating a lot of rays for testing purposes
 */
class LaserRayStudy : public RepeatableRayStudyBase
{
public:
  LaserRayStudy(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual void defineRays() override;

  const RayDataIndex _energy_density_index;

  const RayDataIndex _num_reflection_index;

  const RayDataIndex _num_deposition_index;

  // Laser power
  const Real _power;

  // Laser radius
  const Real _Rb;

  const Real _cut_off_r;

  /// Number of rays in x, y direction
  const unsigned int &_nx, &_ny;

  /// The min/max values for rays domain
  const Real &_xmin, &_xmax, &_ymin, &_ymax;

  const Real _starting_height;

  const Real _max_distance;

private:
};
