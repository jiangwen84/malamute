[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 1
    ymin = 0
    ymax = 1
    nx = 10
    ny = 10
    elem_type = QUAD4
  []
[]

[ICs]
  [ls_ic]
    type = FunctionIC
    function = ls_exact
    variable = ls
  []
  [velocity_x]
    type = ConstantIC
    value = 0.01
    variable = velocity_x
  []
  [velocity_y]
    type = ConstantIC
    value = 0.01
    variable = velocity_y
  []
[]

[Variables]
  [temp]
    initial_condition = 300
  []
  [ls]
    family = LAGRANGE
    order = FIRST
  []
[]

[AuxVariables]
  [velocity_x]
  []
  [velocity_y]
  []
[]

[Functions]
  [ls_exact]
    type = LevelSetOlssonPlane
    epsilon = 0.04
    point = '0.5 0.5 0'
    normal = '0 1 0'
  []
[]

[Kernels]
  [level_set_time]
    type = TimeDerivative
    variable = ls
  []
  [level_set_advection]
    type = nonADLevelSetAdvection
    velocity_x = velocity_x
    velocity_y = velocity_y
    variable = ls
  []

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
    prop_names = 'rho melt_pool_mass_rate rho_dc rho_dT melt_pool_mass_rate_dT'
    prop_values = '8000 0 0 0 0'
  []
  [delta]
    type = nonADLevelSetDeltaFunction
    level_set = ls
  []
  [heaviside]
    type = nonADLevelSetHeavisideFunction
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
  dt = 1e-2
  dtmin = 1e-2
  nl_abs_tol = 1e-10
  nl_rel_tol = 1e-7
  num_steps = 2
  line_search = 'none'
[]

[Outputs]
  exodus = true
[]
