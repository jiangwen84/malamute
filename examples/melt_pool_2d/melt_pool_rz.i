[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 2e-3
    ymin = 0
    ymax = 4e-3
    nx = 80
    ny = 160
    elem_type = QUAD4
  []
  uniform_refine = 1
  [corner_node]
    type = ExtraNodesetGenerator
    new_boundary = 'pinned_node'
    coord = '2e-3 4e-3'
    input = gen
  []
  coord_type = RZ
[]

[Problem]
  type = LevelSetProblem
[]

[ICs]
  [ls_ic]
    type = FunctionIC
    function = ls_exact
    variable = ls
  []
  [velocity]
    type = VectorConstantIC
    x_value = 1e-10
    y_value = 1e-10
    variable = velocity
  []
[]

[AuxVariables]
  [vel_x]
  []
  [vel_y]
  []
[]

[AuxKernels]
  [vel_x]
    type = VectorVariableComponentAux
    component = x
    vector_variable = velocity
    variable = vel_x
  []
  [vel_y]
    type = VectorVariableComponentAux
    component = y
    vector_variable = velocity
    variable = vel_y
  []
[]

[Variables]
  [ls]
  []
  [temp]
    initial_condition = 300
  []
  # [grad_ls]
  #   family = LAGRANGE_VEC
  # []
  [velocity]
    family = LAGRANGE_VEC
  []
  [p]
  []
  [curvature]
  []
[]

# [AuxVariables]
#   [curvature]
#     initial_condition = 0.0
#   []
# []

[Functions/ls_exact]
  type = LevelSetOlssonPlane
  epsilon = 0.04e-3
  point = '2e-3 2e-3 0'
  normal = '0 1 0'
[]

[BCs]
  [no_slip]
    type = ADVectorFunctionDirichletBC
    variable = velocity
    boundary = 'bottom right'
  []
  [no_bc]
    type = INSADMomentumNoBCBC
    variable = velocity
    viscous_form = 'traction'
    boundary = 'top'
    pressure = p
  []
  [pressure_pin]
    type = DirichletBC
    variable = p
    boundary = 'pinned_node'
    value = 0
  []
[]

[Kernels]
  [curvature]
    type = LevelSetCurvatureRegularization
    #level_set_regularized_gradient = grad_ls
    level_set = ls
    variable = curvature
    varepsilon = 2e-4
  []

  # [grad_ls]
  #   type = VariableGradientRegularization
  #   regularized_var = ls
  #   variable = grad_ls
  # []

  [level_set_time]
    type = ADTimeDerivative
    variable = ls
  []

  [level_set_advection_supg]
    type = LevelSetAdvectionSUPG
    velocity = velocity
    variable = ls
  []

  [level_set_time_supg]
    type = LevelSetTimeDerivativeSUPG
    velocity = velocity
    variable = ls
  []

  [level_set_advection]
    type = LevelSetAdvection
    velocity = velocity
    variable = ls
  []

  # [level_set_phase_change]
  #   type = LevelSetPhaseChange
  #   variable = ls
  #   rho_l = 7000
  #   rho_g = 10
  # []

  # [level_set_phase_change_supg]
  #   type = LevelSetPhaseChangeSUPG
  #   variable = ls
  #   velocity = velocity
  #   rho_l = 7000
  #   rho_g = 10
  # []

  [heat_time]
    type = ADHeatConductionTimeDerivative
    specific_heat = specific_heat
    density_name = rho
    variable = temp
  []

  [heat_cond]
    type = ADHeatConduction
    thermal_conductivity = thermal_conductivity
    variable = temp
  []

  [heat_conv]
    type = INSADEnergyAdvection
    variable = temp
  []

  [heat_source]
    type = MeltPoolHeatSource
    variable = temp
    laser_power = 400
    effective_beam_radius = 0.5e-3
    absorption_coefficient = 0.27
    heat_transfer_coefficient = 100
    StefanBoltzmann_constant = 5.67e-8
    material_emissivity = 0.59
    ambient_temperature = 300
    laser_location_x = '0.00'
    laser_location_y = '2e-3' #'0.005-1e-3*t'
    rho_l = 7000
    rho_g = 1
    vaporization_latent_heat = 6.1e6
  []

  [mass]
    type = INSADMass
    variable = p
  []

  [mass_pspg]
    type = INSADMassPSPG
    variable = p
  []

  [momentum_time]
    type = INSADMomentumTimeDerivative
    variable = velocity
  []

  [momentum_convection]
    type = INSADMomentumAdvection
    variable = velocity
  []

  [momentum_viscous]
    type = INSADMomentumViscous
    variable = velocity
    viscous_form = 'traction'
  []

  [momentum_pressure]
    type = INSADMomentumPressure
    variable = velocity
    pressure = p
    integrate_p_by_parts = true
  []

  [momentum_supg]
    type = INSADMomentumSUPG
    variable = velocity
    velocity = velocity
  []

  [melt_pool_momentum_source]
    type = INSMeltPoolMomentumSource
    variable = velocity
  []

  [gravity]
    type = INSADGravityForce
    variable = velocity
    gravity = '0 -9.81 0'
  []

  # [buoyancy]
  #   type = INSADBoussinesqBodyForce
  #   variable = velocity
  #   temperature = temp
  #   gravity = '0 -9.81 0'
  # []
[]

[Materials]
  [thermal]
    type = LevelSetThermalMaterial
    temperature = temp
    c_g = 600
    c_s = 400
    c_l = 400
    # k_g = 0.017
    k_g = 10
    k_s = 40
    k_l = 40
    solidus_temperature = 1350
    latent_heat = 2.5e5
    outputs = all
  []
  [mushy]
    type = MushyZoneMaterial
    temperature = temp
    liquidus_temperature = 1400 #1673
    solidus_temperature = 1350 #1648
    rho_s = 7000
    rho_l = 7000
    outputs = all
  []
  [delta]
    type = LevelSetDeltaFunction
    # level_set_gradient = grad_ls
    level_set = ls
    outputs = all
  []
  [heaviside]
    type = LevelSetHeavisideFunction
    level_set = ls
    outputs = all
  []
  [ins_melt_pool_mat]
    type = INSMeltPoolMaterial
    #level_set_gradient = grad_ls
    level_set = ls
    velocity = velocity
    pressure = p
    alpha = .1
    temperature = temp
    curvature = curvature
    surface_tension = 2
    thermal_capillary = -5e-6 #-4.3e-4
    rho_l = 7000
    rho_g = 1
    outputs = all
    output_properties = 'melt_pool_mass_rate saturated_vapor_pressure'
    cp_name = specific_heat
    k_name = thermal_conductivity
  []
  [mass_transfer]
    type = INSMeltPoolMassTransferMaterial
    temperature = temp
    Boltzmann_constant = 1.38064852e-23
    vaporization_latent_heat = 6.1e6
    atomic_weight = 97.43e-27
    vaporization_temperature = 3134 #3134
    reference_pressure = 1.01e5 #1.01e5
    outputs = all
  []

  [fluid]
    type = LevelSetFluidMaterial
    rho_g = 1
    rho_s = 7000
    rho_l = 7000
    mu_g = 1e-6
    mu_l = 0.1
    permeability_constant = 1e-8
    outputs = all
  []
[]

[MultiApps]
  [reinit]
    type = LevelSetReinitializationMultiApp
    input_files = 'reinit_rz.i'
    execute_on = TIMESTEP_END
  []
[]

[Transfers]
  # [marker_to_sub]
  #   type = LevelSetMeshRefinementTransfer
  #   to_multi_app = reinit
  #   source_variable = combo
  #   variable = combo
  #   check_multiapp_execute_on = false
  # []
  [to_sub]
    type = MultiAppCopyTransfer
    source_variable = ls
    variable = ls
    to_multi_app = reinit
    execute_on = 'timestep_end'
  []

  [to_sub_init]
    type = MultiAppCopyTransfer
    source_variable = ls
    variable = ls_0
    to_multi_app = reinit
    execute_on = 'timestep_end'
  []

  [from_sub]
    type = MultiAppCopyTransfer
    source_variable = ls
    variable = ls
    from_multi_app = reinit
    execute_on = 'timestep_end'
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
  dt = 1e-3
  nl_abs_tol = 1e-6
  num_steps = 10000
  nl_max_its = 12
  l_max_its = 100
  line_search = 'none'
  # petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -pc_factor_mat_solver_package -ksp_type'
  # petsc_options_value = 'lu NONZERO 1e-5 superlu_dist preonly'

  #petsc_options = '-snes_converged_reason -ksp_converged_reason -options_left -ksp_monitor_singular_value'

  petsc_options_iname = '-ksp_type -pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -sub_pc_factor_levels -sub_pc_factor_shift_type -pc_factor_shift_amount'
  petsc_options_value = 'gmres asm      lu           2               100 5 NONZERO 1e-8'

  # petsc_options_iname = '-ksp_type -pc_type -sub_pc_type -snes_max_it -sub_pc_factor_shift_type -pc_asm_overlap -pc_factor_shift_amount -sub_pc_factor_levels -sub_ksp_type'
  # petsc_options_value = 'gmres asm lu 100 NONZERO 3 1e-10 1 preonly'

  # petsc_options_iname = '-ksp_max_it -ksp_gmres_restart -pc_type -snes_max_funcs -sub_pc_factor_levels'
  # petsc_options_value = '100       100          asm      1000000         1'

  # petsc_options_iname = '-pc_type -pc_hypre_type'
  # petsc_options_value = 'hypre boomeramg'
  # #petsc_options = '-snes_converged_reason -ksp_converged_reason -options_left -ksp_monitor_singular_value'

  #nl_div_tol = 1e20
  automatic_scaling = true
[]

[Outputs]
  #file_base = 'test'
  exodus = true
[]
