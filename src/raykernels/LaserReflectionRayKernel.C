/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "LaserReflectionRayKernel.h"

// Local includes
#include "BasicLineSearch.h"

// MOOSE includes
#include "Assembly.h"
#include "MooseUtils.h"
#include "NonlinearSystemBase.h"

registerMooseObject("MalamuteApp", LaserReflectionRayKernel);

InputParameters
LaserReflectionRayKernel::validParams()
{
  auto params = RayKernelBase::validParams();

  params.addRequiredCoupledVar("phase", "The field variable that contains the phase");
  params.addCoupledVar("refractive_index", "The field variable that contains the refractive index");

  return params;
}

LaserReflectionRayKernel::LaserReflectionRayKernel(const InputParameters & params)
  : RayKernelBase(params),
    _phase(coupledValue("phase")),
    _grad_phase(coupledGradient("phase")),
    _otf_phase(_mesh.dimension(),
               getVar("phase", 0)->feType(),
               coupledDofValues("phase"),
               _fe_problem.assembly(_tid, _nl.number()).elem()),
    _refractive_index(coupledValue("refractive_index")),
    _q_point(_fe_problem.assembly(_tid, _nl.number()).qPoints()),
    _just_refracted(false),
    _threshold(0.5),
    _has_refracted_data_index(_study.registerRayData("has_refracted"))
{
}

void
LaserReflectionRayKernel::onSegment()
{
  // mooseAssert(start.absolute_fuzzy_equals(_q_point[0]), "Quadrature point 0 must be segment
  // start"); mooseAssert(end.absolute_fuzzy_equals(_q_point[1]), "Quadrature point 1 must be
  // segment end");

  // std::cerr << std::endl
  //           << "[" << name() << "_" << currentRay()->id() << "]: " << _current_segment_start
  //           << " -> " << _current_segment_end << std::endl;
  // std::cerr << "  phase start = " << _phase[0] << std::endl;
  // std::cerr << "  phase end = " << _phase[1] << std::endl;

  // If we just refracted, it means the start point is in the middle of an
  // element so there's nothing to do here except remove the flag

  auto & has_refracted = currentRay()->data(_has_refracted_data_index);

  if (has_refracted)
  {
    has_refracted = 0;
    return;
  }

  // Phase at the start and end points on the segment
  // const auto start_phase = _phase[0];
  // const auto end_phase = _phase[1];
  const auto start_phase = _otf_phase(_current_segment_start);
  const auto end_phase = _otf_phase(_current_segment_end);

  // std::cerr << "  start phase " << start_phase << std::endl;
  // std::cerr << "  end phase " << end_phase << std::endl;

  // If we refract at the end point, let the next segment handle it. Without
  // doing this, the refraction will happen both in this elem and the next elem
  if (MooseUtils::absoluteFuzzyEqual(end_phase, _threshold))
  {
    // std::cerr << "  refracting on next segment" << std::endl;
    return;
  }

  // Phase changes happens in this element
  if ((MooseUtils::absoluteFuzzyGreaterEqual(start_phase, _threshold) &&
       MooseUtils::absoluteFuzzyLessEqual(end_phase, _threshold)) ||
      (MooseUtils::absoluteFuzzyGreaterEqual(end_phase, _threshold) &&
       MooseUtils::absoluteFuzzyLessEqual(start_phase, _threshold)))
  {
    // Normal to the phase change surface
    const auto phase_normal = _grad_phase[0].unit();
    // Indices of refraction at the start and end points on the segment
    // const auto start_r = _refractive_index[0];
    // const auto end_r = _refractive_index[1];

    // Point in this element at which we refract
    const auto refracted_point = phaseChange(_current_segment_start,
                                             _current_segment_end,
                                             currentRay()->direction(),
                                             start_phase,
                                             end_phase);

    // Direction we refract to
    // const auto refracted_direction = snell(currentRay()->direction(), phase_normal, start_r,
    // end_r);
    const auto reflected_direction = reflectedDirection(currentRay()->direction(), phase_normal);

    // Refract!
    // std::cerr << "  start phase " << start_phase << std::endl;
    // std::cerr << "  end phase " << end_phase << std::endl;
    // std::cerr << "  refracted at point " << refracted_point << std::endl;
    // std::cerr << "  refracted to direction " << reflected_direction << std::endl;

    changeRayStartDirection(refracted_point, reflected_direction);

    has_refracted = 1;
  }
}

Point
LaserReflectionRayKernel::reflectedDirection(const Point & direction, const Point & normal)
{
  mooseAssert(MooseUtils::absoluteFuzzyEqual(direction.norm(), 1.), "Direction not normalized");
  mooseAssert(MooseUtils::absoluteFuzzyEqual(normal.norm(), 1.), "Normal not normalized");

  Point reflected_direction = direction;
  reflected_direction -= 2.0 * (reflected_direction * normal) * normal;
  return reflected_direction / reflected_direction.norm();
}

void
LaserReflectionRayKernel::preTrace()
{
  RayKernelBase::preTrace();

  // std::cerr << "RESET flag = " << currentRay()->data(_has_refracted_data_index) << std::endl;
  // // On a new Ray, reset the refraction flag
  // auto & has_refracted = currentRay()->data(_has_refracted_data_index);

  // has_refracted = 0;
}

Point
LaserReflectionRayKernel::snell(const Point & direction,
                                const Point & normal,
                                const Real r1,
                                const Real r2) const
{
  mooseAssert(r1 > 0, "Invalid index of refraction");
  mooseAssert(r2 > 0, "Invalid index of refraction");

  // Dot product between normal and direction
  const Real c = std::abs(normal * direction);

  // Direction and normal are parallel
  if (c > 1.0 - TOLERANCE)
    return direction;

  const Real r = r1 / r2;
  return (r * direction + (r * c - std::sqrt(1 - r * r * (1 - c * c))) * normal).unit();
}

Point
LaserReflectionRayKernel::phaseChange(const Point & start,
                                      const Point & end,
                                      const Point & direction,
                                      const Real & start_phase,
                                      const Real & end_phase)
{
  // No need to line search if it happens at the beginning
  if (MooseUtils::absoluteFuzzyEqual(start_phase, _threshold))
    return start;

  // Segment distance used for parameterization
  const Real distance = (end - start).norm();

  // Parameterized function for the line search
  const auto f = [this, &start, &distance, &direction](const Real t)
  { return _otf_phase(start + t * distance * direction) - _threshold; };

  // Location on segment (start is t = 0, end is t = 1) where phase = _threshold
  const auto t = BasicLineSearch::brents(0, 1, start_phase - _threshold, end_phase - _threshold, f);

  // Change back into physical space
  return start + t * distance * direction;
}
