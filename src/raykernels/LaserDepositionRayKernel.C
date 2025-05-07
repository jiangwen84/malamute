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
  params.addRequiredParam<Real>("epsilon", "The material constant associated the material's electrical conductance");
  return params;
}

LaserDepositionRayKernel::LaserDepositionRayKernel(const InputParameters & params)
  : AuxRayKernel(params), _phase(coupledValue("phase")), _grad_phase(coupledGradient("phase")),_epsilon(getParam<Real>("epsilon"))
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
    
    // Compute α (alpha), an angular modulation factor influenced by anisotropy ε and angle θ.
    // Formula:
    // α = 1 - 0.5 * [ (1 + (1 - ε·cosθ)²) / (1 + (1 + ε·cosθ)²) + 
    //                 (ε² - 2ε·cosθ + 2cos²θ) / (ε² + 2ε·cosθ + 2cos²θ) ]

    const auto phase_normal = _grad_phase[0].unit();
    const auto original_direction = currentRay()->direction();

    auto dot_prod = phase_normal * original_direction;
    auto phase_normal_norm = phase_normal.norm();
    auto original_direction_norm = original_direction.norm();
    auto _theta = std::acos(dot_prod / (phase_normal_norm * original_direction_norm));
    auto _theta_d = _theta*(180/3.141592);

    auto epsilonTimesCosTheta = _epsilon * std::cos(_theta);

    auto numerator1 = 1 + std::pow(1 - epsilonTimesCosTheta, 2);
    auto denominator1 = 1 + std::pow(1 + epsilonTimesCosTheta, 2);
    auto numerator2 = std::pow(_epsilon, 2) - 2 * epsilonTimesCosTheta + 2 * std::pow(std::cos(_theta), 2);
    auto denominator2 = std::pow(_epsilon, 2) + 2 * epsilonTimesCosTheta + 2 * std::pow(std::cos(_theta), 2);
    auto _alpha = 1 - 0.5 * ((numerator1 / denominator1) + (numerator2 / denominator2));

    // Energy carried from previous reflection
    auto energy = currentRay()->data(currentRay()->study().getRayDataIndex("energy_density"));
    // Absorbed energy
    auto absorbed_energy = _alpha*energy;
    // Reflected energy
    auto reflected_energy = (1 - _alpha)*energy;

    addValue(absorbed_energy);

    currentRay()->data(currentRay()->study().getRayDataIndex("energy_density")) = reflected_energy;
    currentRay()->data(currentRay()->study().getRayDataIndex("num_reflection")) += 1;
    currentRay()->data(currentRay()->study().getRayDataIndex("num_deposition")) += 1;
  }
}
