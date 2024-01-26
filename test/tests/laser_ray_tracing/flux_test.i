[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 40
  ny = 40
  xmax = 40
  ymax = 40
[]

[Variables]
  [u]
    initial_condition = 0
  []
[]

[Kernels]
  [heat_cond]
    type = ADDiffusion
    variable = u
  []
  [heat_time]
    type = ADTimeDerivative
    variable = u
  []
  # [body]
  #   type = ADBodyForce
  #   variable = u
  #   function = source
  # []
[]

[Functions]
  # [source]
  #   type = ParsedFunction
  #   expression = 'if((x - 20) * (x - 20) + (y - 20) * (y - 20) > 25, 0, 2.0*1000000000/(25*3.14159)*exp(-2.0/25*((x - 20) * (x - 20) + (y - 20) * (y - 20))))'
  # []
  [source]
    type = ParsedFunction
    expression = 'if(y>39 & x<25 & x>15, 4, 0)'
  []
[]

[BCs]
  [top]
    type = FunctionNeumannBC
    function = source
    boundary = top
    variable = u
  []
[]

[Postprocessors]
  [total_u]
    type = ElementAverageValue
    variable = u
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  dt = 100
  nl_abs_tol = 1e-12
  num_steps = 2
  nl_max_its = 12
  l_max_its = 100

  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -sub_ksp_type'
  petsc_options_value = ' asm      lu           2               100                 preonly'

[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
    solve_type = 'NEWTON'
  []
[]

[Outputs]
  exodus = true
  csv = true
[]
