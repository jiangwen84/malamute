[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 1.5
    ymin = 0
    ymax = 3
    nx = 80
    ny = 160
    elem_type = QUAD4
  []
  #uniform_refine = 2
  [corner_node]
    type = ExtraNodesetGenerator
    new_boundary = 'pinned_node'
    coord = '0.0 3'
    input = gen
  []
  coord_type = RZ
[]

[Problem]
  type = LevelSetProblem
[]

# [Adaptivity]
#   steps = 3
#   marker = combo
#   max_h_level = 3
#   #initial_steps = 3
#   #stop_time = 1.0e-10
#   #[Markers]
#   # [box]
#   #   bottom_left = '0.000 0.001 0'
#   #   inside = refine
#   #   top_right = '0.005 0.007 0'
#   #   outside = do_nothing
#   #   type = BoxMarker
#   # []
#   #[]
#   [Indicators]
#     [ind_ls]
#       type = GradientJumpIndicator
#       variable = ls
#     []
#     [ind_vel_x]
#       type = GradientJumpIndicator
#       variable = vel_x
#     []
#     [ind_vel_y]
#       type = GradientJumpIndicator
#       variable = vel_y
#     []
#   []
#   [Markers]
#     [error_ls]
#       type = ErrorFractionMarker
#       refine = 0.95
#       coarsen = 0.3
#       indicator = ind_ls
#     []
#     [error_vel_x]
#       type = ErrorFractionMarker
#       refine = 0.95
#       coarsen = 0.3
#       indicator = ind_vel_x
#     []
#     [error_vel_y]
#       type = ErrorFractionMarker
#       refine = 0.95
#       coarsen = 0.3
#       indicator = ind_vel_y
#     []
#     [error_temp]
#       type = ValueRangeMarker
#       lower_bound = 1000
#       upper_bound = 2000
#       buffer_size = 0.1
#       variable = temp
#       third_state = DO_NOTHING
#     []
#     [combo]
#       type = ComboMarker
#       # markers = 'error_vel_y error_vel_x  error_ls error_temp'
#       markers = 'error_ls error_temp'
#     []
#   []
# []

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
  [grad_ls]
    family = LAGRANGE_VEC
  []
  [velocity]
    family = LAGRANGE_VEC
  []
  [p]
  []
  [curvature]
  []
[]

[Functions/ls_exact]
  type = LevelSetOlssonPlane
  epsilon = 0.04
  point = '1.5 1.5 0'
  normal = '0 1 0'
[]

[RayBCs]
  [kill]
    type = KillRayBC
    boundary = 'top right bottom left'
  []
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
    level_set_regularized_gradient = grad_ls
    variable = curvature
    varepsilon = 2e-1
  []

  [grad_ls]
    type = VariableGradientRegularization
    regularized_var = ls
    variable = grad_ls
  []

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
  #   rho_l = 8000
  #   rho_g = 1.184
  # []

  # [level_set_phase_change_supg]
  #   type = LevelSetPhaseChangeSUPG
  #   variable = ls
  #   velocity = velocity
  #   rho_l = 8000
  #   rho_g = 1.184
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
    laser_power = 100
    effective_beam_radius = 0.25
    absorption_coefficient = 0.27
    heat_transfer_coefficient = 100e-6
    StefanBoltzmann_constant = 5.67e-14
    material_emissivity = 0.59
    ambient_temperature = 300
    laser_location_x = '0.00'
    laser_location_y = '1.5-1e-3*t' #'0.005-1e-3*t'
    rho_l = 8000e-9
    rho_g = 1.184e-9
    vaporization_latent_heat = 6.1e6
    laser_deposition = deposition
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
[]

[Materials]
  [thermal]
    type = LevelSetThermalMaterial
    temperature = temp
    c_g = 300
    c_s = 500
    c_l = 500
    # k_g = 0.017
    k_g = 1.7e-3
    k_s = 31.8724e-3
    k_l = 209.3e-3
    solidus_temperature = 1648
    latent_heat = 2.5e5
    outputs = all
  []
  [mushy]
    type = MushyZoneMaterial
    temperature = temp
    liquidus_temperature = 1400 #1673
    solidus_temperature = 1350 #1648
    rho_s = 8000e-9
    rho_l = 8000e-9
    outputs = all
  []
  [delta]
    type = LevelSetDeltaFunction
    level_set_gradient = grad_ls
    outputs = all
  []
  [heaviside]
    type = LevelSetHeavisideFunction
    level_set = ls
    outputs = all
  []
  [ins_melt_pool_mat]
    type = INSMeltPoolMaterial
    level_set_gradient = grad_ls
    velocity = velocity
    pressure = p
    alpha = .1
    temperature = temp
    curvature = curvature
    surface_tension = 1.169e-6
    thermal_capillary = -4.3e-10
    rho_l = 8000e-9
    rho_g = 1.184e-9
    outputs = all
    output_properties = 'melt_pool_mass_rate saturated_vapor_pressure'
    cp_name = specific_heat
    k_name = thermal_conductivity
  []
  [mass_transfer]
    type = INSMeltPoolMassTransferMaterial
    temperature = temp
    Boltzmann_constant = 1.38064852e-17
    vaporization_latent_heat = 6.1e6
    atomic_weight = 97.43e-27
    vaporization_temperature = 3134 #3134
    reference_pressure = 1.01e-1 #1.01e5
    outputs = all
  []
  [fluid]
    type = LevelSetFluidMaterial
    rho_g = 1.184e-9
    rho_s = 8000e-9
    rho_l = 8000e-9
    mu_g = 1.81e-8
    mu_l = 0.1e-3
    permeability_constant = 1e-8
    outputs = all
  []
[]

[AuxVariables]
  [refractive_index]
    [InitialCondition]
      type = FunctionIC
      function = '1.0 + (x > 2.5) * 0.3'
    []
  []
  [deposition]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[RayKernels]
  [refraction]
    type = LaserReflectionRayKernel
    phase = ls
    refractive_index = refractive_index
  []
  [deposition]
    type = LaserDepositionRayKernel
    variable = deposition
    depends_on = refraction
    phase = ls
  []
[]

# [UserObjects/study]
#   type = ConeRayStudy

#   start_points = '0.001 1.6 0'
#   directions = '0 -1 0'
#   half_cone_angles = 20
#   ray_data_name = weight

#   azimuthal_quad_orders = 50

#   # Must be set with RayKernels that
#   # contribute to the residual
#   execute_on = TIMESTEP_BEGIN

#   # For outputting Rays
#   always_cache_traces = true
#   data_on_cache_traces = true
# []

[UserObjects/study]
  type = RepeatableRayStudy
  names = 'ray_1
  ray_2
  ray_3
  ray_4
  ray_5
  ray_6
  ray_7
  ray_8
  ray_9
  ray_10'
  start_points = '1e-3 2 0
  1e-2 2 0
  2e-2 2 0
  3e-2 2 0
  4e-2 2 0
  5e-2 2 0
  6e-2 2 0
  7e-2 2 0
  8e-2 2 0
  9e-2 2 0'
  #end_points = '5 5 0'
  directions = '0 -1 0
  0 -1 0
  0 -1 0
  0 -1 0
  0 -1 0
  0 -1 0
  0 -1 0
  0 -1 0
  0 -1 0
  0 -1 0'
  execute_on = TIMESTEP_BEGIN

  # Needed to cache trace information for RayTracingMeshOutput
  always_cache_traces = true
  # Needed to cache Ray data for RayTracingMeshOutput
  data_on_cache_traces = true
[]

# [MultiApps]
#   [reinit]
#     type = LevelSetReinitializationMultiApp
#     input_files = 'reinit_rz.i'
#     execute_on = TIMESTEP_END
#   []
# []

# [Transfers]
#   # [marker_to_sub]
#   #   type = LevelSetMeshRefinementTransfer
#   #   to_multi_app = reinit
#   #   source_variable = combo
#   #   variable = combo
#   #   check_multiapp_execute_on = false
#   # []
#   [to_sub]
#     type = MultiAppCopyTransfer
#     source_variable = ls
#     variable = ls
#     to_multi_app = reinit
#     execute_on = 'timestep_end'
#   []

#   [to_sub_init]
#     type = MultiAppCopyTransfer
#     source_variable = ls
#     variable = ls_0
#     to_multi_app = reinit
#     execute_on = 'timestep_end'
#   []

#   [from_sub]
#     type = MultiAppCopyTransfer
#     source_variable = ls
#     variable = ls
#     from_multi_app = reinit
#     execute_on = 'timestep_end'
#   []
# []

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
  nl_abs_tol = 1e-7
  num_steps = 10000
  nl_max_its = 12
  l_max_its = 100
  line_search = 'none'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -pc_factor_mat_solver_package -ksp_type'
  petsc_options_value = 'lu NONZERO 1e-5 superlu_dist preonly'
  #petsc_options = '-snes_converged_reason -ksp_converged_reason -options_left -ksp_monitor_singular_value'

  #petsc_options_iname = '-ksp_type -pc_type -sub_pc_type -snes_max_it -sub_pc_factor_shift_type -pc_asm_overlap -pc_factor_shift_amount -sub_pc_factor_levels -sub_ksp_type'
  #petsc_options_value = 'gmres asm lu 100 NONZERO 3 1e-10 1 preonly'

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
  [rays]
    type = RayTracingExodus
    study = study
    execute_on = TIMESTEP_END
  []
[]