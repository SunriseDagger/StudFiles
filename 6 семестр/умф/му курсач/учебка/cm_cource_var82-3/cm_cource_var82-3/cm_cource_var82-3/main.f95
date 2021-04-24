    program main
        use fem_module
        type(fem) :: a
        call a%grid_gen()
        call a%read_()
        call a%calc_()
        call a%dealloc_()
    end
