/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "LaserDepositionRayKernel.h"

registerMooseObject("MalamuteApp", LaserDepositionRayKernel);

InputParameters
LaserDepositionRayKernel::validParams()
{
  auto params = AuxRayKernel::validParams();
  params.addRequiredCoupledVar("phase", "The field variable that contains the phase");

  return params;
}

LaserDepositionRayKernel::LaserDepositionRayKernel(const InputParameters & params)
  : AuxRayKernel(params), _phase(coupledValue("phase")), _grad_phase(coupledGradient("phase"))
{
}

void
LaserDepositionRayKernel::onSegment()
{
  // 'start' and 'end' are the true traced start and end points before
  // refraction (if any). If a refraction kernel just changed a Ray, we now have
  // two segments:
  // [start -> _ray->start()] and [_ray->start() -> new end]
  //     ^ tracing now ^             ^ tracing next ^

  if (currentRay()->trajectoryChanged())
  {
    const auto phase_normal = _grad_phase[0].unit();
    auto dot_prod = std::abs(currentRay()->direction() * phase_normal);
    // addValue(100.0 / (currentRay()->intersections() + 1));
    addValue(dot_prod);
  }
}
