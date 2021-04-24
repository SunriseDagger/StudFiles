#if __GFORTRAN__ == 1 && __GNUC__ == 4 && __GNUC_MINOR__ < 5
#define class type
#endif
    module grid_gen_
        implicit none

        type :: gen_point
            double precision :: x = 0d0, y = 0d0, z = 0d0
        contains
            procedure :: set_point
        end type

        type, private :: grid_gen
        contains
            procedure :: generate_reg_grid
            procedure :: nodes_number_gen_reg
            procedure, private :: generate_reg_grid_pr
        end type

        type grid_gen_cube
            type(grid_gen), private :: gg
        contains
            procedure :: generate_reg_grid_c
            procedure :: generate_reg_grid_fem_c
            procedure, private :: generate_reg_grid_pr_c
        end type

    contains

        subroutine set_point(this, x, y, z)
            implicit none
            class(gen_point) :: this
            double precision :: x, y
            double precision, optional :: z
            this%x = x
            this%y = y
            if(present(z))then
                this%z = z
            end if
        end subroutine

        function generate_reg_grid_pr(this, a, b, h, grid_mass)
            implicit none
            class(grid_gen) :: this
            double precision :: a, b, h
            double precision, pointer :: grid_mass(:)
            integer :: generate_reg_grid_pr, n, i
            n = int((b - a) / h) + 1 ! число узлов сетки
            allocate(grid_mass(n))
            grid_mass(1) = a
            do i = 2, n - 1, 1
                grid_mass(i) = a * dble(i - 1) * h
            end do
            grid_mass(n) = b
            generate_reg_grid_pr = n
        end function

        function nodes_number_gen_reg(this, a, b, h)
            implicit none
            class(grid_gen) :: this
            double precision :: a, b, h
            integer :: nodes_number_gen_reg
            nodes_number_gen_reg = int((b - a) / h) + 1
        end function

        subroutine generate_reg_grid(this, a, b, h, n, grid_mass)
            implicit none
            class(grid_gen) :: this
            double precision :: a, b, h
            double precision, pointer :: grid_mass(:)
            integer :: n
            n = this%generate_reg_grid_pr(a, b, h, grid_mass)
        end subroutine

        function generate_reg_grid_pr_c(this, left_bottom, right_top, h_xmin, h_ymin, h_zmin, grid_mass)
            implicit none
            class(grid_gen_cube) :: this
            double precision :: h_xmin, h_ymin, h_zmin
            class(gen_point), pointer :: grid_mass(:)
            class(gen_point) :: left_bottom, right_top
            double precision, pointer :: mass_x(:), mass_y(:), mass_z(:) ! координаты точек
            integer :: n_x, n_y, n_z ! количество узлов
            integer :: n_xy, n, i, j, k, generate_reg_grid_pr_c
            double precision :: a_x, a_y, a_z, b_x, b_y, b_z
            ! Координаты для начал отрезков генерации по перменным
            a_x = left_bottom%x
            a_y = left_bottom%y
            a_z = left_bottom%z
            ! Координаты для концов отрезков генерации по перменным
            b_x = right_top%x
            b_y = right_top%y
            b_z = right_top%z
            ! Генерация одномерных сеток по переменным
            call this%gg%generate_reg_grid(a_x, b_x, h_xmin, n_x, mass_x)
            call this%gg%generate_reg_grid(a_y, b_y, h_ymin, n_y, mass_y)
            call this%gg%generate_reg_grid(a_z, b_z, h_zmin, n_z, mass_z)
            n_xy = n_x * n_y ! количество узлов в одной плоскости xy
            n = n_xy * n_z ! общее число узлов сетки
            allocate(grid_mass(n))
            do i = 1, n_x, 1
                do j = 1, n_y, 1
                    do k = 1, n_z, 1
                        grid_mass((k-1)*n_xy + (j-1)*n_x + i)%x = mass_x(i)
                        grid_mass((k-1)*n_xy + (j-1)*n_x + i)%y = mass_y(j)
                        grid_mass((k-1)*n_xy + (j-1)*n_x + i)%z = mass_z(k)
                    end do
                end do
            end do
            deallocate(mass_x)
            deallocate(mass_y)
            deallocate(mass_z)
            generate_reg_grid_pr_c = n
        end function

        subroutine generate_reg_grid_c(this, left_bottom, right_top, h_xmin, h_ymin, h_zmin, file_name)
            implicit none
            class(grid_gen_cube) :: this
            double precision :: h_xmin, h_ymin, h_zmin
            character*255 :: file_name
            class(gen_point) :: left_bottom, right_top
            class(gen_point), pointer :: grid_mass(:) ! сюда сетку
            integer :: n, i
            n = generate_reg_grid_pr_c(this, left_bottom, right_top, h_xmin, h_ymin, h_zmin, grid_mass)
            open(30, file=file_name, status='unknown')
            write(30, fmt='( i10 )') n
            do i = 1, n, 1
                write(30, fmt='( 3e27.16 )') grid_mass(i)%x, grid_mass(i)%y, grid_mass(i)%z
            end do
            close(30)
            deallocate(grid_mass)
        end subroutine

        subroutine generate_reg_grid_fem_c(this, left_bottom, right_top, h_xmin, h_ymin, h_zmin, &
                                           file_cords, file_elements, file_faces)
            implicit none
            class(grid_gen_cube) :: this
            double precision :: h_xmin, h_ymin, h_zmin
            character*255 :: file_cords, file_elements, file_faces
            class(gen_point) :: left_bottom, right_top
            double precision :: a_x, a_y, a_z, b_x, b_y, b_z
            integer :: n_x, n_y, n_z
            integer :: n_xy, n, i, j, k
            integer :: node_i0, node_i1, node_i2, node_i3, node_i4, node_i5, node_i6, node_i7
            call generate_reg_grid_c(this, left_bottom, right_top, h_xmin, h_ymin, h_zmin, file_cords)
            ! Координаты для начал отрезков генерации по перменным
            a_x = left_bottom%x
            a_y = left_bottom%y
            a_z = left_bottom%z
            ! Координаты для концов отрезков генерации по перменным
            b_x = right_top%x
            b_y = right_top%y
            b_z = right_top%z
            ! Сначало элементы
            open(40, file=file_elements, status='unknown')
            ! Логика такая - берём вершиную, и 7 других прилижащих к ней записываем в КЭ, вроде просто
            ! Для начала нам понадобится коилчество точек по каждой переменной
            n_x = this%gg%nodes_number_gen_reg(a_x, b_x, h_xmin)
            n_y = this%gg%nodes_number_gen_reg(a_y, b_y, h_ymin)
            n_z = this%gg%nodes_number_gen_reg(a_z, b_z, h_zmin)
            n_xy = n_x * n_y
            write(40, fmt='( i10 )') (n_x-1)*(n_y-1)*(n_z-1)
            do i = 0, n_x - 1, 1
                do j = 0, n_y - 1, 1
                    do k = 0, n_z - 1, 1
                        node_i0 = k*n_xy + j*n_x + i + 1! глобальный номер вершины
                        ! Остальные 7 вершин
                        ! Те, который в той же плоскти по xy
                        node_i1 = k*n_xy + j*n_x + i + 2
                        node_i2 = k*n_xy + (j+1)*n_x + i + 1
                        node_i3 = k*n_xy + (j+1)*n_x + i + 2
                        ! Те, которые выше
                        node_i4 = (k+1)*n_xy + j*n_x + i + 1
                        node_i5 = (k+1)*n_xy + j*n_x + i + 2
                        node_i6 = (k+1)*n_xy + (j+1)*n_x + i + 1
                        node_i7 = (k+1)*n_xy + (j+1)*n_x + i + 2
                        write(40, fmt='( 7(i10/) i10 )') node_i0, node_i1, node_i2, node_i3, node_i4, node_i5, node_i6, node_i7
                    end do
                end do
            end do
            close(40)
            ! А затем - грани
            open(40, file=file_faces, status='unknown')
            write(40, fmt='( i10 )') 2*(n_x-1)*(n_y-1) + 2*(n_x-1)*(n_z-1) + 2*(n_y-1)*(n_z-1) ! вывод количества граней
            ! При z = z_min
             do i = 0, n_x - 1, 1
                do j = 0, n_y - 1, 1
                    node_i0 = j*n_x + i + 1
                    node_i1 = j*n_x + i + 2
                    node_i2 = (j+1)*n_x + i + 1
                    node_i3 = (j+1)*n_x + i + 2
                    write(40, fmt='( 3(i10/) i10 )') node_i0, node_i1, node_i2, node_i3
                end do
            end do
            ! При z = z_max
            do i = 0, n_x - 1, 1
                do j = 0, n_y - 1, 1
                    node_i0 = (n_z-1)*n_xy + j*n_x + i + 1
                    node_i1 = (n_z-1)*n_xy + j*n_x + i + 2
                    node_i2 = (n_z-1)*n_xy + (j+1)*n_x + i + 1
                    node_i3 = (n_z-1)*n_xy + (j+1)*n_x + i + 2
                    write(40, fmt='( 3(i10/) i10 )') node_i0, node_i1, node_i2, node_i3
                end do
            end do
            ! При y = y_min
            do i = 0, n_x - 1, 1
                do k = 0, n_z - 1, 1
                    node_i0 = k*n_xy + i + 1
                    node_i1 = k*n_xy + i + 2
                    node_i2 = (k+1)*n_xy + i + 1
                    node_i3 = (k+1)*n_xy + i + 2
                    write(40, fmt='( 3(i10/) i10 )') node_i0, node_i1, node_i2, node_i3
                end do
            end do
            ! При y = y_max
            do i = 0, n_x - 1, 1
                do k = 0, n_z - 1, 1
                    node_i0 = k*n_xy + (n_y-1)*n_x + i + 1
                    node_i1 = k*n_xy + (n_y-1)*n_x + i + 2
                    node_i2 = (k+1)*n_xy + (n_y-1)*n_x + i + 1
                    node_i3 = (k+1)*n_xy + (n_y-1)*n_x + i + 2
                    write(40, fmt='( 3(i10/) i10 )') node_i0, node_i1, node_i2, node_i3
                end do
            end do
            ! При x = x_min
            do j = 0, n_y - 1, 1
                do k = 0, n_z - 1, 1
                    node_i0 = k*n_xy + j*n_x + 1
                    node_i1 = k*n_xy + (j+1)*n_x + 1
                    node_i2 = (k+1)*n_xy + j*n_x + 1
                    node_i3 = (k+1)*n_xy + (j+1)*n_x + 1
                    write(40, fmt='( 3(i10/) i10 )') node_i0, node_i1, node_i2, node_i3
                end do
            end do
            ! При x = x_max
            do j = 0, n_y - 1, 1
                do k = 0, n_z - 1, 1
                    node_i0 = k*n_xy + j*n_x + n_x
                    node_i1 = k*n_xy + (j+1)*n_x + n_x
                    node_i2 = (k+1)*n_xy + j*n_x + n_x
                    node_i3 = (k+1)*n_xy + (j+1)*n_x + n_x
                    write(40, fmt='( 3(i10/) i10 )') node_i0, node_i1, node_i2, node_i3
                end do
            end do
            close(40)
        end subroutine

    end module
