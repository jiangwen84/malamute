[Mesh/gen]
  type = GeneratedMeshGenerator
  dim = 2
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 1
  nx = 40
  ny = 40
  elem_type = QUAD4
[]

[ICs]
  [ls_ic]
    type = FunctionIC
    function = ls_exact
    variable = ls
  []
[]

[Variables]
  [temp]
    initial_condition = 300
  []
  # [grad_ls]
  #   family = LAGRANGE_VEC
  # []
[]

[AuxVariables]
  [ls]
  []
[]

[Functions/ls_exact]
   type = LevelSetOlssonPlane
   epsilon = 0.04
   point = '0.5 0.5 0'
   normal = '0 1 0'
[]

[Kernels]
  # [./grad_ls]
  #   type = VariableGradientRegularization
  #   regularized_var = ls
  #   variable = grad_ls
  # [../]

  [heat_time]
    type = HeatConductionTimeDerivative
    specific_heat = specific_heat
    density_name = rho
    variable = temp
  []

  [heat_cond]
    type = HeatConduction
    diffusion_coefficient = thermal_conductivity
    variable = temp
  []

  [heat_source]
    type = nonADMeltPoolHeatSource
    variable = temp
    laser_power = 250
    effective_beam_radius = 0.1
    absorption_coefficient = 0.27
    heat_transfer_coefficient = 100
    StefanBoltzmann_constant = 5.67e-8
    material_emissivity = 0.59
    ambient_temperature = 300
    laser_location_x = '0.5'
    laser_location_y = '0.5'
    rho_l = 8000
    rho_g = 1.184
    vaporization_latent_heat = 6.1e6
    level_set = ls
  []
[]

[Materials]
  [thermal]
    type = nonADLevelSetThermalMaterial
    temperature = temp
    c_g = 300
    c_s = 500
    c_l = 500
    k_g = 0.017
    k_s = 31.8724
    k_l = 209.3
    solidus_temperature = 1648
    latent_heat = 2.5e5
  []
  [mushy]
    type = nonADMushyZoneMaterial
    temperature = temp
    liquidus_temperature = 1673
    solidus_temperature = 1648
    rho_s = 8000
    rho_l = 8000
  []
  [rho]
    type = GenericConstantMaterial
    prop_names  = 'rho melt_pool_mass_rate rho_dc rho_dT melt_pool_mass_rate_dT'
    prop_values = '8000 0 0 0 0'
  []
  [heaviside]
    type = nonADLevelSetHeavisideFunction
    level_set = ls
  []
  [delta]
    type = nonADLevelSetDeltaFunction
    level_set = ls
  []
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
    solve_type = 'NEWTON'
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  dt = 1
  nl_abs_tol = 1e-7
  num_steps = 2
[]

[Outputs]
  exodus = true
[]
