# Check if Verilator is available
verilator = find_program('verilator', required : false)

if verilator.found()
    verilog_source = files('simple_module.v')
    testbench_source = files('tb_simple_module.cpp')

    verilator_target = custom_target('verilator_target',
        input : verilog_source,
        output : ['Vsimple_module.cpp', 'Vsimple_module.h'],
        command : [verilator, '--cc', '@INPUT@', '--exe', testbench_source],
        build_by_default : true
    )

    verilator_include_dir = include_directories('/usr/share/verilator/include', 'obj_dir')

    verilator_lib = static_library('verilator_lib',
        verilator_target,
        include_directories : verilator_include_dir,
        cpp_args : ['-std=c++17']  # Enable C++17
    )

    executable('Vsimple_module',
        testbench_source,
        link_with : verilator_lib,
        include_directories : verilator_include_dir,
        build_by_default : true,
        cpp_args : ['-std=c++17']  # Enable C++17
    )
else
    message('Verilator not found, skipping Verilator project build')
endif
