# Build pyalpm extension module
python.extension_module(
        'pyalpm',
        ['pyalpm.c',
         'util.c',
         'package.c',
         'db.c',
         'options.c',
         'handle.c',
         'transaction.c',
        ],
        dependencies: [libalpm_dep],
        c_args: common_c_args,
        install: true,
        subdir: '',
)
