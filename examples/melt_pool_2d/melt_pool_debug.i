[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 0.01
    ymin = 0
    ymax = 0.01
    nx = 50
    ny = 50
    elem_type = QUAD4
  []
  [corner_node]
    type = ExtraNodesetGenerator
    new_boundary = 'pinned_node'
    coord = '0.0 0.01'
    input = gen
  []
[]

# [Mesh]
#   #Serial number should match corresponding Executioner parameter
#   file = melt_pool_debug_out_cp/0001_mesh.cpr
#   #This method of restart is only supported on serial meshes
# []

# [Problem]
#   #Note that the suffix is left off in the parameter below.
#   restart_file_base = melt_pool_debug_out_cp/LATEST # You may also use a specific number here
# []

# [Adaptivity]
#   steps = 3
#   marker = box
#   max_h_level = 3
#   initial_steps = 3
#   stop_time = 1.0e-10
#   [Markers]
#     [box]
#       bottom_left = '0.000 0.004 0'
#       inside = refine
#       top_right = '0.01 0.006 0'
#       outside = do_nothing
#       type = BoxMarker
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
  epsilon = 0.00004 #0.00004
  point = '0.005 0.005 0'
  normal = '0 1 0'
[]

[BCs]
  [no_slip]
    type = ADVectorFunctionDirichletBC
    variable = velocity
    boundary = 'bottom left right'
  []
  [no_bc]
    type = INSADMomentumNoBCBC
    variable = velocity
    #viscous_form = 'traction'
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
    varepsilon = 2e-4
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

  [level_set_time_supg]
    type = LevelSetTimeDerivativeSUPG
    velocity = velocity
    variable = ls
  []

  # [level_set_advection_supg]
  #   type = LevelSetAdvectionSUPG
  #   velocity = velocity
  #   variable = ls
  # []

  # [level_set_advection]
  #   type = LevelSetAdvection
  #   velocity = velocity
  #   variable = ls
  # []

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
    laser_power = 100 #'if(t<=1e-3,0.01,100)'
    effective_beam_radius = 0.25e-3
    absorption_coefficient = 0.27
    heat_transfer_coefficient = 100
    StefanBoltzmann_constant = 5.67e-8
    material_emissivity = 0.59
    ambient_temperature = 300
    laser_location_x = '0.005 + 6e-3*t'
    laser_location_y = '0.005'
    rho_l = 8000
    rho_g = 1.184
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
    #viscous_form = 'traction'
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
    k_g = 0.017
    k_s = 31.8724
    k_l = 209.3
    solidus_temperature = 1648
    latent_heat = 2.5e5
    outputs = all
  []
  [mushy]
    type = MushyZoneMaterial
    temperature = temp
    liquidus_temperature = 1673
    solidus_temperature = 1648
    rho_s = 8000
    rho_l = 8000
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
    surface_tension = 1.169
    thermal_capillary = -4.3e-4
    #thermal_capillary = 0
    rho_l = 8000
    rho_g = 1.184
    outputs = all
    output_properties = melt_pool_mass_rate
    cp_name = specific_heat
    k_name = thermal_conductivity
  []
  [mass_transfer]
    type = INSMeltPoolMassTransferMaterial
    temperature = temp
    Boltzmann_constant = 1.38064852e-23
    vaporization_latent_heat = 6.1e6
    atomic_weight = 97.43e-27
    vaporization_temperature = 3134
    reference_pressure = 1.01e5
    outputs = all
  []
  [fluid]
    type = LevelSetFluidMaterial
    rho_g = 1.184
    rho_s = 8000
    rho_l = 8000
    mu_g = 1.81e-5
    mu_l = 0.1
    permeability_constant = 1e-8
    outputs = all
  []
[]

# [MultiApps]
#   [reinit]
#     type = LevelSetReinitializationMultiApp
#     input_files = 'reinit.i'
#     execute_on = TIMESTEP_END
#   []
# []

# [Transfers]
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
  # nl_abs_tol = 1e-7
  nl_rel_tol = 1e-6
  num_steps = 1000
  line_search = 'none'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -pc_factor_mat_solver_package -ksp_type'
  petsc_options_value = 'lu NONZERO 1e-10 superlu_dist preonly'

  # petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -pc_factor_mat_solver_package'
  # petsc_options_value = 'lu NONZERO 1e-5 superlu_dist'

  # petsc_options_iname = '-pc_type -pc_hypre_type'
  # petsc_options_value = 'hypre boomeramg'
  # petsc_options = '-snes_converged_reason -ksp_converged_reason -options_left -ksp_monitor_singular_value'

  # petsc_options = '-snes_converged_reason -ksp_converged_reason -options_left'
  # petsc_options_iname = '-ksp_gmres_restart -pc_factor_shift_type -pc_factor_shift_amount -pc_type'
  # petsc_options_value = '100                NONZERO               1e-10                   asm'

  nl_forced_its = 3
  nl_max_its = 12
  l_max_its = 100

  nl_div_tol = 1e20
  automatic_scaling = true
[]

[Debug]
  show_var_residual = 'ls temp curvature p'
  show_var_residual_norms = true
[]

[Outputs]
  checkpoint = false
  exodus = true
[]
