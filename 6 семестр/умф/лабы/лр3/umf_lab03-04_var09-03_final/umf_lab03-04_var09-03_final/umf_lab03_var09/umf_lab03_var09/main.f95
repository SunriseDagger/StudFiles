#if __GFORTRAN__ == 1 && __GNUC__ == 4 && __GNUC_MINOR__ < 5
#define class type
#endif

    subroutine trans_face(file_cord, file_face)
        use addition_classes
        use module_list
        use harm_fem_
        implicit none
        character*255 :: file_cord, file_face
        integer :: n, i, m, k, j
        class(node), allocatable :: nodes(:)
        class(list) :: face_node
        class(harm_fem) :: hf
        double precision :: x, y, z
        open(50, file=file_cord, status='old')
        read(50, *) n
        allocate(nodes(n))
        do i = 1, n, 1
            read(50, *) nodes(i)%x, nodes(i)%y, nodes(i)%z
        end do
        close(50)
        open(50, file=file_face, status='old')
        read(50, *) m
        do i = 1, m, 1
            do j = 1, 8, 1
                read(50, *) k
                call face_node%add(k)
            end do
        end do
        close(50)
        call face_node%sort()
        open(50, file=file_face(1:index(file_face, ' ') - 1) // '1', status='unknown')
        m = face_node%list_size()
        write(50, fmt='( i10 )') m
        do i = 0, m - 1, 1
            k = face_node%get(i) + 1
            x = nodes(k)%x
            y = nodes(k)%y
            z = nodes(k)%z
            if(mod(k, 2) .eq. 0) then
                write(50, fmt='( i10e27.16 )') k, hf%u_sin(x, y, z)
            else
                write(50, fmt='( i10e27.16 )') k, hf%u_cos(x, y, z)
            end if
        end do
        write(50, fmt='( i10 )') 0
        write(50, fmt='( i10 )') 0
        close(50)
        call face_node%del()
        deallocate(nodes)
    end subroutine

    program main
        use harm_fem_
        use grid_gen_
        character*255 :: file_cords, file_els, file_face
        class(harm_fem) :: our_meth
        class(gen_point) :: st, en
        class(grid_gen_cube) :: gc
        double precision :: hx, hy, hz
        file_cords = 'cords'
        file_els = 'els'
        file_face = 'face'
        call st%set_point(0d0, 0d0, 0d0)
        call en%set_point(4d0, 4d0, 4d0)
        hx = 0.2d0
        hy = 0.2d0
        hz = 0.2d0
        print*, 'grid gen'
        ! генерция сетки
        call gc%generate_reg_grid_fem_c(st, en, hx, hy, hz, file_cords, file_els, file_face)
        print*, 'grid init'
        ! инициализация сетки
        call our_meth%transf_grid(file_cords, file_els, file_face)
        call trans_face(file_cords(1:index(file_cords, ' ') - 1) // 'tr', &
                        file_face(1:index(file_face, ' ') - 1) // 'tr')
        print*, 'solve'
        ! Запуск
        call our_meth%init_hf(file_cords(1:index(file_cords, ' ') - 1) // 'tr', &
                              file_els(1:index(file_els, ' ') - 1) // 'tr', &
                              file_face(1:index(file_face, ' ') - 1) // 'tr')
        call our_meth%form_matrix()
        call our_meth%solve_()
        call our_meth%out_rez('sol.txt')
        call our_meth%out_diff('diff.txt')
    end program
