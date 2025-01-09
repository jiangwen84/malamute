[Mesh]
  [./gmg]
    type = ConcentricCircleMeshGenerator
    num_sectors = 8
    radii = '0.0015'
    rings = '4'
    has_outer_square = no
    pitch = 1.42063
    #portion = left_half
    preserve_volumes = off
  []

  [./extrude]
    type = MeshExtruderGenerator
    input = gmg
    num_layers = 25
    extrusion_vector = '0 0 0.006'
    bottom_sideset = 'new_front'
    top_sideset = 'new_back'
  []
  [corner_node]
    type = ExtraNodesetGenerator
    new_boundary = 'pinned_node'
    coord = '0.0 0 0'
    input = extrude
  []
[]

[Problem]
  type = LevelSetProblem
[]

[Adaptivity]
  marker = marker
  max_h_level = 2
  #cycles_per_step = 2
  #initial_steps = 1
  [Indicators]
    [error]
      type = GradientJumpIndicator
      variable = ls
    []
    [error2]
      type = GradientJumpIndicator
      variable = temp
    []
  []
  [Markers]
    [./marker]
      type = ComboMarker
      markers = 'marker1 marker2'
    [../]
    [marker1]
      type = ValueRangeMarker
      lower_bound = 0.05
      upper_bound = 0.95
      variable = ls
    []
    [./marker2]
      type = ValueThresholdMarker
      coarsen = 400
      variable = temp
      refine = 600
    [../]
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

[Functions/ls_exact]
  type = LevelSetOlssonPlane
  epsilon = 0.00006
  point = '0.00 0.00 0.003'
  normal = '0 0 1'
[]

[BCs]
  [no_slip]
    type = ADVectorFunctionDirichletBC
    variable = velocity
    boundary = 'new_front new_back outer'
  []
  # [no_bc]
  #   type = INSADMomentumNoBCBC
  #   variable = velocity
  #   viscous_form = 'traction'
  #   boundary = 'front left right top back'
  #   pressure = p
  # []
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
    level_set = ls
    variable = curvature
    varepsilon = 2e-3
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

  # [level_set_reinit]
  #   type = LevelSetOlssonOneStepReinitialization
  #   variable = ls
  #   reinit_speed = 1e-6
  #   epsilon = 0.00006
  # []

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
    laser_power = 2000
    effective_beam_radius = 1e-3
    absorption_coefficient = 0.27
    heat_transfer_coefficient = 100
    StefanBoltzmann_constant = 5.67e-8
    material_emissivity = 0.59
    ambient_temperature = 300
    laser_location_x = '0.0025'
    laser_location_y = '0.0025'
    laser_location_z = '0.0025'
    rho_l = 8000
    rho_g = 1.184
    vaporization_latent_heat = 6.1e6
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

  [gravity]
    type = INSADGravityForce
    variable = velocity
    gravity = '0 -9.81 0'
  []
[]

[AuxVariables]
  [refractive_index]
    [InitialCondition]
      type = FunctionIC
      function = 0
    []
  []
  [deposition]
    order = CONSTANT
    family = MONOMIAL
  []
  [deposition_number]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[RayBCs]
  [kill]
    type = KillRayBC
    boundary = 'new_back new_front outer'
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

[UserObjects/study]
  type = LaserRayStudy
  execute_on = TIMESTEP_BEGIN

  vertex_to_vertex = false
  centroid_to_vertex = false
  centroid_to_centroid = false

  always_cache_traces = true
  data_on_cache_traces = true
[]

[Materials]
  [thermal]
    type = LevelSetThermalMaterial
    temperature = temp
    c_g = 600
    c_s = 400
    c_l = 400
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
    liquidus_temperature = 1400
    solidus_temperature = 1350
    rho_s = 7000
    rho_l = 7000
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
    surface_tension = 1.169 #1.169
    thermal_capillary = -4.3e-5
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
    reference_pressure = 1.01e5 #1.01e5
    outputs = all
  []
  [fluid]
    type = LevelSetFluidMaterial
    rho_g = 1.184
    rho_s = 7000
    rho_l = 7000
    mu_g = 1e-5
    mu_l = 0.01
    permeability_constant = 1e-8
    outputs = all
  []
[]

[MultiApps]
  [reinit]
    type = LevelSetReinitializationMultiApp
    input_files = 'reinit.i'
    execute_on = TIMESTEP_END
  []
[]

[Transfers]

  [marker]
    type = LevelSetMeshRefinementTransfer
    source_variable = marker
    variable = marker
    to_multi_app = reinit
  []

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
  # [SMP]
  #   type = SMP
  #   full = false
  #   solve_type = 'NEWTON'
  # []
  [FSP]
    type = FSP
    topsplit = 'by_var'
    full = true
    [by_var]
      splitting = 'up temp curvature'
      splitting_type = multiplicative
      petsc_options_iname = '-ksp_type'
      petsc_options_value = 'fgmres'
    []
    [up]
      vars = 'velocity p'
      petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -pc_factor_shift_type -sub_pc_factor_mat_solver_type -sub_pc_factor_shift_amount'
      petsc_options_value = ' asm      lu           2               31 NONZERO superlu_dist 1e-8'
      #   petsc_options_iname = '-pc_type -pc_hypre_type -ksp_type -ksp_rtol -ksp_gmres_restart -ksp_pc_side'
      #  petsc_options_value = 'hypre    boomeramg      gmres    5e-1      300                 right'
    []
    [temp]
      vars = 'temp'
      petsc_options_iname = '-ksp_type -ksp_gmres_restart -pc_type -pc_hypre_type  -ksp_pc_side'
      petsc_options_value = 'gmres    300  hypre  boomeramg  right'
      # petsc_options_iname = '-pc_type -ksp_type'
      # petsc_options_value = '     hypre  preonly'
    []
    [curvature]
      vars = 'curvature ls'
      # petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -pc_hypre_type  -ksp_pc_side'
      # petsc_options_value = 'gmres    300                5e-2      hypre  boomeramg  right'
      petsc_options_iname = '-pc_type -ksp_type'
      petsc_options_value = '     hypre  preonly'
    []
    # [ls]
    #   vars = 'ls'
    #   # petsc_options_iname = '-pc_type -ksp_type'
    #   # petsc_options_value = 'hypre  preonly'
    #   petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -pc_hypre_type  -ksp_pc_side'
    #   petsc_options_value = 'gmres    300                5e-2      hypre  boomeramg  right'
    #   # petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -pc_factor_shift_type -sub_pc_factor_mat_solver_type -sub_pc_factor_shift_amount'
    #   # petsc_options_value = ' asm      lu           2               31 NONZERO superlu_dist 1e-12'
    # []
  []
# [FSP]
#   type = FSP
#   topsplit = 'by_var'
#   full = true
#   [by_var]
#     splitting = 'up temp ls'
#     splitting_type = additive
#     petsc_options_iname = '-ksp_type'
#     petsc_options_value = 'fgmres'
#   []
#   [up]
#     # vars = 'velocity p'
#     # petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -pc_factor_shift_type -sub_pc_factor_mat_solver_type -sub_pc_factor_shift_amount'
#     # petsc_options_value = ' asm      lu           2               31 NONZERO superlu_dist 1e-12'
#     #   petsc_options_iname = '-pc_type -pc_hypre_type -ksp_type -ksp_rtol -ksp_gmres_restart -ksp_pc_side'
#     #  petsc_options_value = 'hypre    boomeramg      gmres    5e-1      300                 right'

#     #splitting = 'u p' # 'u' and 'p' are the names of subsolvers
#     splitting_type = schur
#     vars = 'velocity p'
#     petsc_options_iname = '-pc_fieldsplit_schur_fact_type  -pc_fieldsplit_schur_precondition -ksp_rtol -ksp_gmres_restart -ksp_type'
#     petsc_options_value = 'full                            selfp          5e-1                   300                    fgmres'
#   []

#   [u]
#     vars = 'velocity'
#     petsc_options_iname = '-pc_type -pc_hypre_type -ksp_type -ksp_rtol -ksp_gmres_restart -ksp_pc_side'
#     petsc_options_value = 'hypre    boomeramg      gmres    5e-1      300                 right'
#   []
#   [p]
#     vars = 'p'
#     petsc_options_iname = '-pc_type -pc_hypre_type -ksp_type -ksp_rtol -ksp_gmres_restart -ksp_pc_side'
#     petsc_options_value = 'hypre    boomeramg      gmres    5e-1      300                 right'
#   []

#   [temp]
#     vars = 'temp'
#     petsc_options_iname = '-ksp_type -ksp_gmres_restart -pc_type -pc_hypre_type  -ksp_pc_side'
#     petsc_options_value = 'gmres    50                     hypre  boomeramg  right'
#     #       petsc_options_iname = '-pc_type -ksp_type'
#     # petsc_options_value = '     hypre  preonly'
#   []
#   [ls]
#     vars = 'curvature ls'
#     # petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -pc_hypre_type  -ksp_pc_side'
#     # petsc_options_value = 'gmres    300                5e-2      hypre  boomeramg  right'
#     petsc_options_iname = '-pc_type -ksp_type'
#     petsc_options_value = '     hypre  preonly'
#   []
# []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  dt = 0.00025
  nl_abs_tol = 1e-6
  num_steps = 1000
  nl_max_its = 10
  l_max_its = 50
  line_search = 'none'
  # petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_mat_solver_package -ksp_type'
  # petsc_options_value = 'lu NONZERO superlu_dist preonly'
  # petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -sub_ksp_type'
  # petsc_options_value = ' asm      lu           2               100                 preonly'
  nl_div_tol = 1e20
  automatic_scaling = true
  off_diagonals_in_auto_scaling = true
[]

[Postprocessors]
  [memory_average]
    type = MemoryUsage
    value_type = average
    mem_units = gibibytes
  []
  [memory_max]
    type = MemoryUsage
    value_type = max_process
    mem_units = gibibytes
  []
  [memory_peak]
    type = MemoryUsage
    value_type = max_process
    report_peak_value = true
    mem_units = gibibytes
  []
[]



[Outputs]
  checkpoint = true
  exodus = true
  [rays]
    type = RayTracingExodus
    study = study
    execute_on = TIMESTEP_END
  []
[]
