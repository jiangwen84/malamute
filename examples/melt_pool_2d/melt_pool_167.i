[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 0.001
    ymin = 0
    ymax = 0.002
    nx = 15
    ny = 30
    elem_type = QUAD4
  []
  [corner_node]
    type = ExtraNodesetGenerator
    new_boundary = 'pinned_node'
    coord = '0.0 0.002'
    input = gen
  []
  uniform_refine = 3
[]
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
[Adaptivity]
  marker = marker
  max_h_level = 3
  # cycles_per_step = 1
  [Indicators]
    [error1]
      type = GradientJumpIndicator
      variable = vel_x
    []
    [error2]
      type = GradientJumpIndicator
      variable = vel_y
    []
  []
  [Markers]
    [./marker]
      type = ComboMarker
      markers = 'marker1 marker2 marker3 marker4'
    [../]
      [marker1]
        type = ValueRangeMarker
        lower_bound = 0.01
        upper_bound = 0.99
        variable = ls
      []
      [marker2]
      type = ErrorFractionMarker
      coarsen = 0.2
      refine = 0.8
      indicator = error1
    []
    [marker3]
      type = ErrorFractionMarker
      coarsen = 0.2
      refine = 0.8
      indicator = error2
    []
    [./marker4]
      type = ValueThresholdMarker
      coarsen = 300
      variable = temp
      refine = 1000
    [../]
      # [./marker2]
      #   type = BoxMarker
      #   bottom_left = '0.0005 0.001 0'
      #   top_right = '0.001 0.002 0'
      #   inside = refine
      #   outside = coarsen
      # [../]
  []
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

[Problem]
  type = LevelSetProblem
[]

[Functions]
  [ls_exact]
    type = LevelSetOlssonPlane
    epsilon = 0.00002
    point = '0.001 0.001 0'
    normal = '0 -1 0'
  []
[]

[BCs]
  [no_slip]
    type = ADVectorFunctionDirichletBC
    variable = velocity
    boundary = 'bottom left right top'
  []

  # [no_bc]
  #   type = INSADMomentumNoBCBC
  #   variable = velocity
  #   viscous_form = 'traction'
  #   boundary = 'top'
  #   pressure = p
  #   mu_name = mu
  #   integrate_p_by_parts = true
  # []

  [pressure_pin]
    type = DirichletBC
    variable = p
    boundary = 'pinned_node'
    value = 0
  []
#  [temp]
#   type = DirichletBC
#   value = 300
#   boundary = bottom
#   variable = temp
#  []
[]
[Kernels]
  [curvature]
    type = LevelSetCurvatureRegularization
    #level_set_regularized_gradient = grad_ls
    level_set = ls
    variable = curvature
    varepsilon = 4e-6
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
  # [level_set_reinit]
  #   type = LevelSetOlssonOneStepReinitialization
  #   variable = ls
  #   reinit_speed = 1e-4
  #   epsilon = 0.0001
  # []
#   [level_set_advection_supg]
#     type = LevelSetAdvectionSUPG
#     velocity = velocity
#     variable = ls
# []
#   [level_set_time_supg]
#     type = LevelSetTimeDerivativeSUPG
#     velocity = velocity
#     variable = ls
#   []
  [level_set_advection]
    type = LevelSetAdvection
    # rho_l = 8000
    # rho_g = 1.78
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
    laser_power = 500
    effective_beam_radius = 0.14e-3
    absorption_coefficient = 0.27
    heat_transfer_coefficient = 100
    StefanBoltzmann_constant = 5.67e-8
    material_emissivity = 0.59
    ambient_temperature = 300
    laser_location_x = '0.00075'
    laser_location_y = '0.0015'
    rho_l = 8000
    rho_g = 1.78
    vaporization_latent_heat = 9.6e6
    laser_deposition = deposition
    laser_deposition_number = deposition_number
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
    c_g = 520
    c_s = 700
    c_l = 700
    k_g = 0.028
    k_s = 35
    k_l = 35
    solidus_temperature = 1723
    latent_heat = 2.7e5
    outputs = all
  []
  [mushy]
    type = MushyZoneMaterial
    temperature = temp
    liquidus_temperature = 1658
    solidus_temperature = 1723
    rho_s = 8000
    rho_l = 8000
    outputs = all
  []
  [delta]
    type = LevelSetDeltaFunction
    #level_set_gradient = grad_ls
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
    surface_tension = 1.68 #1.169
    thermal_capillary = -1e-4 #-1e-4 #-1e-4
    rho_l = 8000
    rho_g = 1.78
    outputs = all
    output_properties = melt_pool_mass_rate
    cp_name = specific_heat
    k_name = thermal_conductivity
    fusion_latent_heat = 290e3
    fluid_mass_fraction = fluid_frac
  []
  [mass_transfer]
    type = INSMeltPoolMassTransferMaterial
    temperature = temp
    Boltzmann_constant = 1.38064852e-23
    vaporization_latent_heat = 9.6e6
    atomic_weight = 97.43e-27
    mole_mass = 415e-3
    vaporization_temperature = 3068
    reference_pressure = 1.01e5 #1.01e5
    R_constant = 8.314
    outputs = all
  []
  [fluid]
    type = LevelSetFluidMaterial
    rho_g = 1.78
    rho_s = 8000
    rho_l = 8000
    mu_g = 1.5e-5
    mu_l = 5e-3
    mu_s = 1e3
    permeability_constant = 1e6
    outputs = all
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
  [deposition_number]
    type = LaserDepositionNumberRayKernel
    variable = deposition_number
    depends_on = refraction
    phase = ls
  []
[]
[AuxVariables]
  [refractive_index]
  []
  [deposition]
    order = CONSTANT
    family = MONOMIAL
  []
  [deposition_number]
    order = CONSTANT
    family = MONOMIAL
  []
  [fluid_frac]
    order = CONSTANT
    family = MONOMIAL
  []
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

[AuxKernels]
  [fluid_vol_frac]
    type = ADMaterialRealAux
    variable = fluid_frac
    property = liquid_mass_fraction
  []
[]

[UserObjects/study]
  type = LaserRayStudy
  execute_on = TIMESTEP_BEGIN

  vertex_to_vertex = false
  centroid_to_vertex = false
  centroid_to_centroid = false

  always_cache_traces = true
  data_on_cache_traces = true
  power = 250
[]

[MultiApps]
  [reinit]
    type = LevelSetReinitializationMultiApp
    input_files = 'reinit_7.i'
    execute_on = TIMESTEP_END
  []
[]
[Transfers]
  [./marker_to_sub]
    type = LevelSetMeshRefinementTransfer
    to_multi_app = reinit
    source_variable = marker
    variable = marker
  [../]
  [to_sub]
    type = MultiAppCopyTransfer
    source_variable = ls
    variable = ls
    to_multi_app = reinit
    execute_on = 'timestep_end'
  []
  # [to_sub_temp]
  #   type = MultiAppCopyTransfer
  #   source_variable = temp
  #   variable = temp
  #   to_multi_app = reinit
  #   execute_on = 'timestep_end'
  # []
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

[RayBCs]
  [kill]
    type = KillRayBC
    boundary = 'top right bottom left'
  []
[]

[Preconditioning]
  active = 'FSP'
  [SMP]
    type = SMP
    full = false
    coupled_groups = 'velocity p'
    solve_type = 'NEWTON'
  []
  [FSP]
    type = FSP
    topsplit = 'by_var'
    full = false
    [by_var]
      splitting = 'up temp curvature'
      splitting_type = multiplicative
      petsc_options_iname = '-ksp_type'
      petsc_options_value = 'fgmres'
    []
    [up]
      vars = 'velocity p'
      petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -pc_factor_shift_type -sub_pc_factor_mat_solver_type -sub_pc_factor_shift_amount'
      petsc_options_value = ' asm      lu           2               31 NONZERO superlu_dist 1e-12'
      #   petsc_options_iname = '-pc_type -pc_hypre_type -ksp_type -ksp_rtol -ksp_gmres_restart -ksp_pc_side'
      #  petsc_options_value = 'hypre    boomeramg      gmres    5e-1      300                 right'
    []
    [temp]
      vars = 'temp'
      petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -pc_hypre_type  -ksp_pc_side'
      petsc_options_value = 'gmres    300                5e-2      hypre  boomeramg  right'
    []
    [curvature]
      vars = 'curvature ls'
      petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -pc_hypre_type  -ksp_pc_side'
      petsc_options_value = 'gmres    300                5e-2      hypre  boomeramg  right'
      # petsc_options_iname = '-pc_type -ksp_type'
      # petsc_options_value = '     hypre  preonly'
    []
    # [ls]
    #   vars = 'ls'
    #   petsc_options_iname = '-pc_type -ksp_type'
    #   petsc_options_value = 'hypre  preonly'
    #   # petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -pc_hypre_type  -ksp_pc_side'
    #   # petsc_options_value = 'gmres    300                5e-2      hypre  boomeramg  right'
    #   # petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -pc_factor_shift_type -sub_pc_factor_mat_solver_type -sub_pc_factor_shift_amount'
    #   # petsc_options_value = ' asm      lu           2               31 NONZERO superlu_dist 1e-12'
    # []
  []
  # [FSP]
  #   type = FSP
  #   topsplit = 'by_var'
  #   [by_var]
  #     splitting = 'up temp curvature ls'
  #     splitting_type = additive
  #     petsc_options_iname = '-ksp_type'
  #     petsc_options_value = 'fgmres'
  #   []
  #   [up]
  #      splitting = 'u p' # 'u' and 'p' are the names of subsolvers
  #      vars = 'velocity p'
  #      splitting_type = schur
  #      petsc_options_iname = '-pc_fieldsplit_schur_fact_type -pc_fieldsplit_schur_precondition -ksp_gmres_restart -ksp_rtol -ksp_type'
  #      petsc_options_value = 'full selfp 300 1e-4 fgmres'
  #   []
  #   [u]
  #     vars = 'velocity'
  #     # petsc_options_iname = '-pc_type -pc_hypre_type -ksp_type -ksp_rtol -ksp_gmres_restart -ksp_pc_side'
  #     # petsc_options_value = 'hypre    boomeramg      gmres    5e-1      300                 right'
  #     petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -pc_factor_shift_type -sub_pc_factor_mat_solver_type -sub_pc_factor_shift_amount'
  #     petsc_options_value = ' asm      lu           2               31 NONZERO superlu_dist 1e-12'
  #   []
  #   [p]
  #      vars = 'p'
  #     #  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -pc_factor_shift_type -sub_pc_factor_mat_solver_type -sub_pc_factor_shift_amount'
  #     #  petsc_options_value = ' asm      lu           2               31 NONZERO superlu_dist 1e-12'
  #     petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -ksp_pc_side'
  #     petsc_options_value = 'gmres    300                5e-1      jacobi    right'
  #   []
  #   [temp]
  #     vars = 'temp'
  #     petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -pc_hypre_type  -ksp_pc_side'
  #     petsc_options_value = 'gmres    300                5e-2      hypre  boomeramg  right'
  #   []
  #   [curvature]
  #     vars = 'curvature'
  #     petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -pc_hypre_type  -ksp_pc_side'
  #     petsc_options_value = 'gmres    300                5e-2      hypre  boomeramg  right'
  #   []
  #   [ls]
  #     vars = 'ls'
  #     petsc_options_iname = '-pc_type -ksp_type'
  #     petsc_options_value = 'hypre  preonly'
  #   []
  # []
[]
[Executioner]
  type = Transient
  solve_type = NEWTON
  dt = 1e-5
  nl_abs_tol = 1e-7
  num_steps = 10000
  nl_forced_its = 2
  line_search = 'none'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_mat_solver_package -ksp_type'
  petsc_options_value = 'lu NONZERO superlu_dist preonly'
  # petsc_options_iname = '-pc_type  -sub_pc_type -pc_factor_shift_type -sub_pc_factor_shift_amount'
  # petsc_options_value = 'asm             lu NONZERO 1e-10'
  l_max_its = 50
  nl_max_its = 20
  nl_div_tol = 1e20
  automatic_scaling = true
  off_diagonals_in_auto_scaling = true
[]
[Outputs]
  exodus = true
  [rays]
    type = RayTracingExodus
    study = study
    execute_on = TIMESTEP_END
  []
[]

