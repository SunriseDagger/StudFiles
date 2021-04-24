#if __GFORTRAN__ == 1 && __GNUC__ == 4 && __GNUC_MINOR__ < 5
#define class type
#endif
    module fem_module
        use cgm_module
        use param_module
        implicit none

        type :: local_area ! конечные элементы
            integer :: mas_(8) ! номера узлов
            double precision :: lambda_ ! lambda в области
        end type

        type :: cross ! координаты узлов
            double precision :: mas_(3) ! x, y, z соотв.
        end type

        type :: limits ! границы, для вторых-третьих краевых
            double precision :: min_(3), max_(3)
        end type

        type :: local_matrix ! локльные матрицы
            double precision :: m_1(8,8)=reshape(source=(/&
                8d0,4d0,4d0,2d0,4d0,2d0,2d0,1d0,&
                4d0,8d0,2d0,4d0,2d0,4d0,1d0,2d0,&
                4d0,2d0,8d0,4d0,2d0,1d0,4d0,2d0,&
                2d0,4d0,4d0,8d0,1d0,2d0,2d0,4d0,&
                4d0,2d0,2d0,1d0,8d0,4d0,4d0,2d0,&
                2d0,4d0,1d0,2d0,4d0,8d0,2d0,4d0,&
                2d0,1d0,4d0,2d0,4d0,2d0,8d0,4d0,&
                1d0,2d0,2d0,4d0,2d0,4d0,4d0,8d0 &
                /),shape=(/8,8/)) ! матрица массы
            double precision :: g_x(8,8)=reshape(source=(/&
                4d0,-4d0,2d0,-2d0,2d0,-2d0,1d0,-1d0,&
                -4d0,4d0,-2d0,2d0,-2d0,2d0,-1d0,1d0,&
                2d0,-2d0,4d0,-4d0,1d0,-1d0,2d0,-2d0,&
                -2d0,2d0,-4d0,4d0,-1d0,1d0,-2d0,2d0,&
                2d0,-2d0,1d0,-1d0,4d0,-4d0,2d0,-2d0,&
                -2d0,2d0,-1d0,1d0,-4d0,4d0,-2d0,2d0,&
                1d0,-1d0,2d0,-2d0,2d0,-2d0,4d0,-4d0,&
                -1d0,1d0,-2d0,2d0,-2d0,2d0,-4d0,4d0 &
                /),shape=(/8,8/)) ! матрица жесткости (x)
            double precision :: g_y(8,8)=reshape(source=(/&
                4d0,2d0,-4d0,-2d0,2d0,1d0,-2d0,-1d0,&
                2d0,4d0,-2d0,-4d0,1d0,2d0,-1d0,-2d0,&
                -4d0,-2d0,4d0,2d0,-2d0,-1d0,2d0,1d0,&
                -2d0,-4d0,2d0,4d0,-1d0,-2d0,1d0,2d0,&
                2d0,1d0,-2d0,-1d0,4d0,2d0,-4d0,-2d0,&
                1d0,2d0,-1d0,-2d0,2d0,4d0,-2d0,-4d0,&
                -2d0,-1d0,2d0,1d0,-4d0,-2d0,4d0,2d0,&
                -1d0,-2d0,1d0,2d0,-2d0,-4d0,2d0,4d0 &
                /),shape=(/8,8/)) ! матрица жесткости (y)
            double precision :: g_z(8,8)=reshape(source=(/&
                4d0,2d0,2d0,1d0,-4d0,-2d0,-2d0,-1d0,&
                2d0,4d0,1d0,2d0,-2d0,-4d0,-1d0,-2d0,&
                2d0,1d0,4d0,2d0,-2d0,-1d0,-4d0,-2d0,&
                1d0,2d0,2d0,4d0,-1d0,-2d0,-2d0,-4d0,&
                -4d0,-2d0,-2d0,-1d0,4d0,2d0,2d0,1d0,&
                -2d0,-4d0,-1d0,-2d0,2d0,4d0,1d0,2d0,&
                -2d0,-1d0,-4d0,-2d0,2d0,1d0,4d0,2d0,&
                -1d0,-2d0,-2d0,-4d0,1d0,2d0,2d0,4d0 &
                /),shape=(/8,8/)) ! матрица жесткости (z)
            double precision :: c_1(4,4)=reshape(source=(/&
                4d0,2d0,2d0,1d0,&
                2d0,4d0,1d0,2d0,&
                2d0,1d0,4d0,2d0,&
                1d0,2d0,2d0,4d0 &
                /),shape=(/4,4/)) ! матрица c_1
        end type

        type :: fem
            type(slae) :: gl_matr ! СЛАУ
            type(local_area), private, allocatable :: matr(:) ! конечные элементы
            type(cross), private, allocatable :: set(:) ! координаты узлов
            type(local_matrix), private :: lc_matr ! локальные матрицы
            double precision :: t_curr = 0d0, t_old = 0d0 ! значение времени
            double precision, allocatable :: q_curr(:), q_old(:) ! вектора весов
            type(limits), private :: lim ! границы, для вторых-третьих краевых
        contains
            procedure :: calc_ ! функция запуска вычислений
            procedure :: read_ ! функция считывания исходных данных (кроме краевых)
            procedure, private :: do_make_matrix ! формирование матрицы
            procedure, private :: do_make_bound ! учет краевых условий
            procedure, private :: add ! добавлениие эл -та x к (i, j) эл -ту матрицы
            procedure :: dealloc_ ! освобождение памяти
            procedure :: grid_gen ! генератор сеток
        end type

    contains

        subroutine grid_gen(this)
            implicit none
            class(fem) :: this
            integer, allocatable :: grid(:,:,:)
            integer :: num_x, num_y, num_z, i, j, k, number_loc
            num_x = ceiling((x_end - x_start) / x_delta)
            num_y = ceiling((y_end - y_start) / y_delta)
            num_z = ceiling((z_end - y_start) / z_delta)
            allocate(grid(num_x+1, num_y+1, num_z+1))
            ! Координаты
            open(100, file = 'cross.txt', status = 'unknown')
            write(100,*) (num_x+1) * (num_y+1) * (num_z+1)
            number_loc = 1
            do i = 0, num_z, 1
                do j = 0, num_y, 1
                    do k = 0, num_x, 1
                        write(100,*) x_start + dble(k) * x_delta, y_start + dble(j) * y_delta, z_start + dble(i) * z_delta
                        grid(k+1, j+1, i+1) = number_loc
                        number_loc = number_loc + 1
                    end do
                end do
            end do
            write(100,*)
            close(100)
            ! КЭ
            open(100, file = 'local.txt', status = 'unknown')
            write(100,*) (num_x) * (num_y) * (num_z)
            do i = 1, num_z, 1
                do j = 1, num_y, 1
                    do k = 1, num_x, 1
                        write(100,*) grid(k, j, i), grid(k+1, j, i), grid(k, j+1, i), grid(k+1, j+1, i), &
                                   grid(k, j, i+1), grid(k+1, j, i+1), grid(k, j+1, i+1), grid(k+1, j+1, i+1)
                        write(100,*) lambda_grid
                    end do
                end do
            end do
            write(100,*)
            close(100)
            ! Краевые
            open(100, file = 'bound1.txt', status = 'unknown')
            do j = 1, num_y+1, 1
                do k = 1, num_x+1, 1
                    write(100,*) grid(k, j, 1)
                    write(100,*) grid(k, j, num_z+1)
                end do
            end do
            do i = 2, num_z, 1
                do k = 1, num_x+1, 1
                    write(100,*) grid(k, 1, i)
                    write(100,*) grid(k, num_y+1, i)
                end do
            end do
            do i = 2, num_z, 1
                do j = 2, num_y, 1
                    write(100,*) grid(1, j, i)
                    write(100,*) grid(num_x+1, j, i)
                end do
            end do
            write(100,*)
            close(100)

            deallocate(grid)
        end subroutine

        subroutine read_(this) ! функция считывания исходных данных (кроме краевых)
            use list_module
            implicit none
            common / counters / c_local, c_cross
            class(fem) :: this
            integer :: i, j, c_local, c_cross, gg_size, i1, j1, k, tmp
            double precision :: q_l, x_l, y_l, z_l
            class(list), allocatable :: profile(:)
            ! Считывание координат узлов
            open(10, file = 'cross.txt', status = 'old')
            rewind 10
            read(10, * ) c_cross
            this%gl_matr%n = c_cross
            allocate(this%set(c_cross))
            do i = 1, c_cross
                read(10, *) (this%set(i)%mas_(j),  j = 1, 3)
                do j = 1, 3
                    this%lim%max_(j) = max(this%set(i)%mas_(j), this%lim%max_(j))
                    this%lim%min_(j) = min(this%set(i)%mas_(j), this%lim%min_(j))
                end do
            end do
            close(10)
            ! Считывание нумерации
            open(20, file = 'local.txt', status = 'old')
            rewind 20
            read(20, * ) c_local
            allocate(this%matr(c_local))
            do i = 1, c_local
                read(20, *) (this%matr(i)%mas_(j),  j = 1, 8)
                read(20, *) this%matr(i)%lambda_
            end do
            close(20)

            ! Формирование профиля
            gg_size = 0
            allocate(profile(this%gl_matr%n))
            do k = 1, c_local
                do i = 1, 8
                    do j = i + 1, 8, 1
                        i1 = this%matr(k)%mas_(i)
                        j1 = this%matr(k)%mas_(j)
                        if(i1 .lt. j1)then
                            tmp = i1
                            i1 = j1
                            j1 = tmp
                        end if
                        if(profile(i1 -1)%elem_count(j1) .eq. 0) then
                            call profile(i1 -1)%add(j1)
                            gg_size = gg_size + 1
                        end if
                    end do
                end do
            end do
            do i = 1, this%gl_matr%n, 1
                call profile(i)%sort()
            end do
            ! Выделение памяти
            call this%gl_matr%alloc_all(gg_size)
            ! Формирование ig и jg
            this%gl_matr%ig(1) = 1
            this%gl_matr%ig(2) = 1
            tmp = 1
            do i = 1, this%gl_matr%n - 1
                k = 0
                do j = 1, i
                    if(profile(i)%elem_count(j) .ne. 0) then
                        this%gl_matr%jg(tmp) = j
                        tmp = tmp + 1
                        k = k + 1
                    end if
                end do
                this%gl_matr%ig(i + 2) = this%gl_matr%ig(i + 1) + k
            end do
            deallocate(profile)

            ! И еще немного памяти
            allocate(this%q_curr(this%gl_matr%n))
            allocate(this%q_old(this%gl_matr%n))
            this%t_curr = t_start + t_delta
            this%t_old = t_start

            ! Начальные условия
            do i = 1, c_cross
                x_l = this%set(i)%mas_(1)
                y_l = this%set(i)%mas_(2)
                z_l = this%set(i)%mas_(3)
                q_l = u_(x_l, y_l, z_l, t_start)
                this%q_curr(i) = q_l
            end do
        end subroutine

        subroutine calc_(this) ! функция запуска вычислений
            implicit none
            class(fem) :: this
            integer i, j, num_t
            double precision :: diff, x_l, y_l, z_l, u_l, norm
            open(90, file = 'solution.txt', status = 'unknown')
            open(95, file = 'diff.txt', status = 'unknown')
            num_t = ceiling((t_end - t_start) / t_delta)

            write(90, *) '======= t = ', t_start, '========='
            do j = 1, this%gl_matr%n
                write(90, *) this%q_curr(j)
            end do

            do i = 1, num_t, 1
                this%q_old = this%q_curr
                this%gl_matr%di = 0d0
                this%gl_matr%f = 0d0
                this%gl_matr%gg = 0d0
                this%gl_matr%x = 0d0

                call this%do_make_matrix()
                call this%do_make_bound()
                call this%gl_matr%calc_cgm()

                this%q_curr = this%gl_matr%x
                diff = 0d0
                norm = 0d0
                do j = 1, this%gl_matr%n
                    x_l = this%set(j)%mas_(1)
                    y_l = this%set(j)%mas_(2)
                    z_l = this%set(j)%mas_(3)
                    u_l = u_(x_l, y_l, z_l, this%t_curr)
                    diff = diff + (this%q_curr(j) - u_l) ** 2
                    norm = norm + u_l ** 2
                end do
                diff = dsqrt(diff) / dsqrt(norm)
                write(*, '(a7,e14.7,a9,e10.3)') 'Time = ', this%t_curr, '  Diff = ', diff
                write(95, '(a7,e14.7,a9,e10.3)') 'Time = ', this%t_curr, '  Diff = ', diff

                write(90, *) '======= t = ', this%t_curr, '========='
                do j = 1, this%gl_matr%n
                    x_l = this%set(j)%mas_(1)
                    y_l = this%set(j)%mas_(2)
                    z_l = this%set(j)%mas_(3)
                    u_l = u_(x_l, y_l, z_l, this%t_curr)
                    write(90, *) this%q_curr(j), u_l
                end do

                this%t_old = this%t_curr
                this%t_curr = t_start + t_delta * dble(i+1)
            end do
            close(90)
            close(95)
        end subroutine

        subroutine add(this, i, j, x) ! добавлениие эл -та x к (i, j) эл -ту матрицы
            implicit none
            class(fem) :: this
            double precision :: x
            integer :: i, j, k
            do k = this%gl_matr%ig(i), this%gl_matr%ig(i + 1) -1
                if(this%gl_matr%jg(k) .eq. j) goto 100
            end do
100         continue
            this%gl_matr%gg(k) = this%gl_matr%gg(k) + x
        end subroutine

        subroutine do_make_matrix(this) ! формирование матрицы
            implicit none
            common / counters / c_local, c_cross
            class(fem) :: this
            integer :: i, j, c_local, c_cross, k, i1, j1, l, flag
            double precision :: h(3), b_k, c_k, fr(8), tmp_vector_1(8), tmp_vector_2(8)
            double precision :: tmpmatr(8,8), sigma_l

            ! Вычисление шага h
            do i = 1, this%gl_matr%n
                this%gl_matr%di(i) = 0d0
                this%gl_matr%f(i) = 0d0
            end do
            do i = 1, this%gl_matr%ig(this%gl_matr%n)
                this%gl_matr%gg(i) = 0d0
            end do
            do k = 1, c_local
                do i = 1, 3
                    flag = 1
                    do j = 2, 8
                        if(flag .eq. 1)then
                            flag = 0
                            do l = 1, 3 ! проверяем, лежат ли точки на нужном ребре
                                if(i .ne. l .and. this%set(this%matr(k)%mas_(1))%mas_(l) .ne. this%set(this%matr(k)%mas_(j))%mas_(l))then
                                    flag = 1
                                end if
                            end do
                        end if
                    end do
                    if(flag .eq. 0)then
                        h(i) = dabs(this%set(this%matr(k)%mas_(1))%mas_(i) - this%set(this%matr(k)%mas_(j -1))%mas_(i))
                    end if
                end do

                ! заполнение массива gg
                b_k = this%matr(k)%lambda_ * h(1) * h(2) * h(3) / 36d0
                c_k = h(1) * h(2) * h(3) / 216d0

                ! вектор правой части для локал. матрицы
                do i = 1, 8
                    tmp_vector_1(i) = f_(this%set(this%matr(k)%mas_(i))%mas_(1), &
                        this%set(this%matr(k)%mas_(i))%mas_(2), &
                        this%set(this%matr(k)%mas_(i))%mas_(3), this%t_curr)
                    tmp_vector_2(i) = f_(this%set(this%matr(k)%mas_(i))%mas_(1), &
                        this%set(this%matr(k)%mas_(i))%mas_(2), &
                        this%set(this%matr(k)%mas_(i))%mas_(3), this%t_old)
                end do
                fr = c_k * (matmul(this%lc_matr%m_1, tmp_vector_1) + matmul(this%lc_matr%m_1, tmp_vector_2)) / 2d0

                ! Сигма на текущем КЭ
                sigma_l = 0d0
                do i = 1, 8, 1
                    sigma_l = sigma_l + sigma_(this%set(this%matr(k)%mas_(i))%mas_(1), &
                                                    this%set(this%matr(k)%mas_(i))%mas_(2), &
                                                    this%set(this%matr(k)%mas_(i))%mas_(3), &
                                                    this%t_curr)
                end do
                sigma_l = sigma_l / 8d0
                ! Матрица для добавки в правую часть
                tmpmatr = 1d0 / (this%t_curr - this%t_old) * c_k * sigma_l * this%lc_matr%m_1 - &
                          1d0 / 2d0 * b_k * (1d0 / (h(1) ** 2) * this%lc_matr%g_x + &
                                             1d0 / (h(2) ** 2) * this%lc_matr%g_y + &
                                             1d0 / (h(3) ** 2) * this%lc_matr%g_z)
                ! (1/dt*M-1/2*G)*q^(j-1)
                do i = 1, 8, 1
                    tmp_vector_1(i) = this%q_old(this%matr(k)%mas_(i))
                end do
                fr = fr + matmul(tmpmatr, tmp_vector_1)

                ! Локальная матрица
                tmpmatr = 1d0 / (this%t_curr - this%t_old) * c_k * sigma_l * this%lc_matr%m_1 + &
                          1d0 / 2d0 * b_k * (1d0 / (h(1) ** 2) * this%lc_matr%g_x + &
                                             1d0 / (h(2) ** 2) * this%lc_matr%g_y + &
                                             1d0 / (h(3) ** 2) * this%lc_matr%g_z)
                do i = 1, 8
                    do j = 1, i -1, 1
                        i1 = this%matr(k)%mas_(i)
                        j1 = this%matr(k)%mas_(j)
                        ! добавка в gg
                        if(i1 .lt. j1)then
                            call this%add(j1, i1, tmpmatr(i, j))
                        else
                            call this%add(i1, j1, tmpmatr(i, j))
                        end if
                    end do
                    ! добавка в диагональ
                    this%gl_matr%di(this%matr(k)%mas_(i)) = this%gl_matr%di(this%matr(k)%mas_(i)) + tmpmatr(i, i)
                    ! добавка в правую часть
                    this%gl_matr%f(this%matr(k)%mas_(i)) = this%gl_matr%f(this%matr(k)%mas_(i)) + fr(i)
                end do
            end do
        end subroutine

        subroutine do_make_bound(this) ! учет краевых условий
            implicit none
            common / counters / c_local, c_cross
            class(fem) :: this
            integer :: c_local, c_cross, i, j, cr(4)
            double precision :: teta(4), beta, vec_tet(4), hplane, h1, h2, tmp, grad(4), teta_old(4), grad_old(4)
            integer :: i_s, i_e, cur_row, p
            ! учет вторых краевых условий
            open(30, file = 'bound2.txt', status = 'old', err = 100)
            rewind 30
            !print *, '2 bound detected!'
            do while(.true.)
                read(30, *, err = 100, end = 100) (cr(i), i = 1, 4)

                ! 2-4-6-8
                if(this%set(cr(1))%mas_(1) .eq. this%lim%max_(1) .and. &
                   this%set(cr(2))%mas_(1) .eq. this%lim%max_(1) .and. &
                   this%set(cr(3))%mas_(1) .eq. this%lim%max_(1) .and. &
                   this%set(cr(4))%mas_(1) .eq. this%lim%max_(1)) then
                    do i = 1, 4
                        grad = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        grad_old = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                        do j = 1, c_local
                            if(this%matr(j)%mas_(2) .eq. cr(1) .and. &
                               this%matr(j)%mas_(4) .eq. cr(2) .and. &
                               this%matr(j)%mas_(6) .eq. cr(3) .and. &
                               this%matr(j)%mas_(8) .eq. cr(4)) goto 40
                        end do
40                      continue
                        teta(i) = grad(1) * this%matr(j)%lambda_
                        teta_old(i) = grad_old(1) * this%matr(j)%lambda_
                    end do
                end if
                ! 1-3-5-7
                if(this%set(cr(1))%mas_(1) .eq. this%lim%min_(1) .and. &
                   this%set(cr(2))%mas_(1) .eq. this%lim%min_(1) .and. &
                   this%set(cr(3))%mas_(1) .eq. this%lim%min_(1) .and. &
                   this%set(cr(4))%mas_(1) .eq. this%lim%min_(1)) then
                    do i = 1, 4
                        grad = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        grad_old = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                        do j = 1, c_local
                            if(this%matr(j)%mas_(1) .eq. cr(1) .and. &
                               this%matr(j)%mas_(3) .eq. cr(2) .and. &
                               this%matr(j)%mas_(5) .eq. cr(3) .and. &
                               this%matr(j)%mas_(7) .eq. cr(4)) goto 41
                        end do
41                      continue
                        teta(i) = - grad(1) * this%matr(j)%lambda_
                        teta_old(i) = - grad_old(1) * this%matr(j)%lambda_
                    end do
                end if
                ! 3-4-7-8
                if(this%set(cr(1))%mas_(2) .eq. this%lim%max_(2) .and. &
                   this%set(cr(2))%mas_(2) .eq. this%lim%max_(2) .and. &
                   this%set(cr(3))%mas_(2) .eq. this%lim%max_(2) .and. &
                   this%set(cr(4))%mas_(2) .eq. this%lim%max_(2)) then
                    do i = 1, 4
                        grad = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        grad_old = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                        do j = 1, c_local
                            if(this%matr(j)%mas_(3) .eq. cr(1) .and. &
                               this%matr(j)%mas_(4) .eq. cr(2) .and. &
                               this%matr(j)%mas_(7) .eq. cr(3) .and. &
                               this%matr(j)%mas_(8) .eq. cr(4)) goto 42
                        end do
42                      continue
                        teta(i) = grad(2) * this%matr(j)%lambda_
                        teta_old(i) = grad_old(2) * this%matr(j)%lambda_
                    end do
                end if
                ! 1-2-5-6
                if(this%set(cr(1))%mas_(2) .eq. this%lim%min_(2) .and. &
                   this%set(cr(2))%mas_(2) .eq. this%lim%min_(2) .and. &
                   this%set(cr(3))%mas_(2) .eq. this%lim%min_(2) .and. &
                   this%set(cr(4))%mas_(2) .eq. this%lim%min_(2)) then
                    do i = 1, 4
                        grad = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        grad_old = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                        do j = 1, c_local
                            if(this%matr(j)%mas_(1) .eq. cr(1) .and. &
                               this%matr(j)%mas_(2) .eq. cr(2) .and. &
                               this%matr(j)%mas_(5) .eq. cr(3) .and. &
                               this%matr(j)%mas_(6) .eq. cr(4)) goto 43
                        end do
43                      continue
                        teta(i) = - grad(2) * this%matr(j)%lambda_
                        teta_old(i) = - grad_old(2) * this%matr(j)%lambda_
                    end do
                end if
                ! 5-6-7-8
                if(this%set(cr(1))%mas_(3) .eq. this%lim%max_(3) .and. &
                   this%set(cr(2))%mas_(3) .eq. this%lim%max_(3) .and. &
                   this%set(cr(3))%mas_(3) .eq. this%lim%max_(3) .and. &
                   this%set(cr(4))%mas_(3) .eq. this%lim%max_(3)) then
                    do i = 1, 4
                        grad = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        grad_old = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                        do j = 1, c_local
                            if(this%matr(j)%mas_(5) .eq. cr(1) .and. &
                               this%matr(j)%mas_(6) .eq. cr(2) .and. &
                               this%matr(j)%mas_(7) .eq. cr(3) .and. &
                               this%matr(j)%mas_(8) .eq. cr(4)) goto 44
                        end do
44                      continue
                        teta(i) = grad(3) * this%matr(j)%lambda_
                        teta_old(i) = grad_old(3) * this%matr(j)%lambda_
                    end do
                end if
                ! 1-2-3-4
                if(this%set(cr(1))%mas_(3) .eq. this%lim%min_(3) .and. &
                   this%set(cr(2))%mas_(3) .eq. this%lim%min_(3) .and. &
                   this%set(cr(3))%mas_(3) .eq. this%lim%min_(3) .and. &
                   this%set(cr(4))%mas_(3) .eq. this%lim%min_(3)) then
                    do i = 1, 4
                        grad = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        grad_old = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                        do j = 1, c_local
                            if(this%matr(j)%mas_(1) .eq. cr(1) .and. &
                               this%matr(j)%mas_(2) .eq. cr(2) .and. &
                               this%matr(j)%mas_(3) .eq. cr(3) .and. &
                               this%matr(j)%mas_(4) .eq. cr(4)) goto 45
                        end do
45                      continue
                        teta(i) = - grad(3) * this%matr(j)%lambda_
                        teta_old(i) = - grad_old(3) * this%matr(j)%lambda_
                    end do
                end if

                h1 = 0d0
                h2 = 0d0
                do i = 1, 3
                    tmp = dabs(this%set(cr(1))%mas_(i) - this%set(cr(2))%mas_(i))
                    if(tmp .gt. eps)then
                        h1 = h1 + tmp
                    end if
                    tmp = dabs(this%set(cr(1))%mas_(i) - this%set(cr(3))%mas_(i))
                    if(tmp .gt. eps)then
                        h2 = h2 + tmp
                    end if
                end do
                ! добавка в правую часть
                hplane = h1 * h2 / 36d0
                vec_tet = (matmul(this%lc_matr%c_1, teta) + matmul(this%lc_matr%c_1, teta_old)) / 2d0
                do i = 1, 4
                    this%gl_matr%f(cr(i)) = this%gl_matr%f(cr(i)) + hplane * vec_tet(i)
                end do
            end do
            close(30)
100         continue
            ! учет третьих краевых условий
            open(40, file = 'bound3.txt', status = 'old', err = 200)
            rewind 40
            !print * , '3 bound detected!'
            do while(.true.)
                read(40, *, err = 200, end = 200) (cr(i), i = 1, 4)
                read(40, *, err = 200, end = 200) beta

                ! 2-4-6-8
                if(this%set(cr(1))%mas_(1) .eq. this%lim%max_(1) .and. &
                   this%set(cr(2))%mas_(1) .eq. this%lim%max_(1) .and. &
                   this%set(cr(3))%mas_(1) .eq. this%lim%max_(1) .and. &
                   this%set(cr(4))%mas_(1) .eq. this%lim%max_(1)) then
                    do i = 1, 4
                        grad = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        grad_old = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                        do j = 1, c_local
                            if(this%matr(j)%mas_(2) .eq. cr(1) .and. &
                               this%matr(j)%mas_(4) .eq. cr(2) .and. &
                               this%matr(j)%mas_(6) .eq. cr(3) .and. &
                               this%matr(j)%mas_(8) .eq. cr(4)) goto 60
                        end do
60                      continue
                        teta(i) = grad(1) * this%matr(j)%lambda_ + beta * &
                                  u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        teta_old(i) = grad_old(1) * this%matr(j)%lambda_ + beta * &
                                  u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                    end do
                end if
                ! 1-3-5-7
                if(this%set(cr(1))%mas_(1) .eq. this%lim%min_(1) .and. &
                   this%set(cr(2))%mas_(1) .eq. this%lim%min_(1) .and. &
                   this%set(cr(3))%mas_(1) .eq. this%lim%min_(1) .and. &
                   this%set(cr(4))%mas_(1) .eq. this%lim%min_(1)) then
                    do i = 1, 4
                        grad = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        grad_old = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                        do j = 1, c_local
                            if(this%matr(j)%mas_(1) .eq. cr(1) .and. &
                               this%matr(j)%mas_(3) .eq. cr(2) .and. &
                               this%matr(j)%mas_(5) .eq. cr(3) .and. &
                               this%matr(j)%mas_(7) .eq. cr(4)) goto 61
                        end do
61                      continue
                        teta(i) = - grad(1) * this%matr(j)%lambda_ + beta * &
                                  u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        teta_old(i) = - grad_old(1) * this%matr(j)%lambda_ + beta * &
                                  u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                    end do
                end if
                ! 3-4-7-8
                if(this%set(cr(1))%mas_(2) .eq. this%lim%max_(2) .and. &
                   this%set(cr(2))%mas_(2) .eq. this%lim%max_(2) .and. &
                   this%set(cr(3))%mas_(2) .eq. this%lim%max_(2) .and. &
                   this%set(cr(4))%mas_(2) .eq. this%lim%max_(2)) then
                    do i = 1, 4
                        grad = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        grad_old = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                        do j = 1, c_local
                            if(this%matr(j)%mas_(3) .eq. cr(1) .and. &
                               this%matr(j)%mas_(4) .eq. cr(2) .and. &
                               this%matr(j)%mas_(7) .eq. cr(3) .and. &
                               this%matr(j)%mas_(8) .eq. cr(4)) goto 62
                        end do
62                      continue
                        teta(i) = grad(2) * this%matr(j)%lambda_ + beta * &
                                  u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        teta_old(i) = grad_old(2) * this%matr(j)%lambda_ + beta * &
                                  u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                    end do
                end if
                ! 1-2-5-6
                if(this%set(cr(1))%mas_(2) .eq. this%lim%min_(2) .and. &
                   this%set(cr(2))%mas_(2) .eq. this%lim%min_(2) .and. &
                   this%set(cr(3))%mas_(2) .eq. this%lim%min_(2) .and. &
                   this%set(cr(4))%mas_(2) .eq. this%lim%min_(2)) then
                    do i = 1, 4
                        grad = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        grad_old = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                        do j = 1, c_local
                            if(this%matr(j)%mas_(1) .eq. cr(1) .and. &
                               this%matr(j)%mas_(2) .eq. cr(2) .and. &
                               this%matr(j)%mas_(5) .eq. cr(3) .and. &
                               this%matr(j)%mas_(6) .eq. cr(4)) goto 63
                        end do
63                      continue
                        teta(i) = - grad(2) * this%matr(j)%lambda_ + beta * &
                                  u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        teta_old(i) = - grad_old(2) * this%matr(j)%lambda_ + beta * &
                                  u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                    end do
                end if
                ! 5-6-7-8
                if(this%set(cr(1))%mas_(3) .eq. this%lim%max_(3) .and. &
                   this%set(cr(2))%mas_(3) .eq. this%lim%max_(3) .and. &
                   this%set(cr(3))%mas_(3) .eq. this%lim%max_(3) .and. &
                   this%set(cr(4))%mas_(3) .eq. this%lim%max_(3)) then
                    do i = 1, 4
                        grad = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        grad_old = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                        do j = 1, c_local
                            if(this%matr(j)%mas_(5) .eq. cr(1) .and. &
                               this%matr(j)%mas_(6) .eq. cr(2) .and. &
                               this%matr(j)%mas_(7) .eq. cr(3) .and. &
                               this%matr(j)%mas_(8) .eq. cr(4)) goto 64
                        end do
64                      continue
                        teta(i) = grad(3) * this%matr(j)%lambda_ + beta * &
                                  u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        teta_old(i) = grad_old(3) * this%matr(j)%lambda_ + beta * &
                                  u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                    end do
                end if
                ! 1-2-3-4
                if(this%set(cr(1))%mas_(3) .eq. this%lim%min_(3) .and. &
                   this%set(cr(2))%mas_(3) .eq. this%lim%min_(3) .and. &
                   this%set(cr(3))%mas_(3) .eq. this%lim%min_(3) .and. &
                   this%set(cr(4))%mas_(3) .eq. this%lim%min_(3)) then
                    do i = 1, 4
                        grad = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        grad_old = grad_u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                        do j = 1, c_local
                            if(this%matr(j)%mas_(1) .eq. cr(1) .and. &
                               this%matr(j)%mas_(2) .eq. cr(2) .and. &
                               this%matr(j)%mas_(3) .eq. cr(3) .and. &
                               this%matr(j)%mas_(4) .eq. cr(4)) goto 65
                        end do
65                      continue
                        teta(i) = - grad(3) * this%matr(j)%lambda_ + beta * &
                                  u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_curr)
                        teta_old(i) = - grad_old(3) * this%matr(j)%lambda_ + beta * &
                                  u_(this%set(cr(i))%mas_(1), this%set(cr(i))%mas_(2), this%set(cr(i))%mas_(3), this%t_old)
                    end do
                end if

                h1 = 0d0
                h2 = 0d0
                do i = 1, 3
                    tmp = dabs(this%set(cr(1))%mas_(i) - this%set(cr(2))%mas_(i))
                    if(tmp .gt. eps)then
                        h1 = h1 + tmp
                    end if
                    tmp = dabs(this%set(cr(1))%mas_(i) - this%set(cr(3))%mas_(i))
                    if(tmp .gt. eps)then
                        h2 = h2 + tmp
                    end if
                end do
                ! добавка в правую часть
                hplane = h1 * h2 / 36d0
                vec_tet = (matmul(this%lc_matr%c_1, teta) + matmul(this%lc_matr%c_1, teta_old)) / 2d0
                do i = 1, 4
                    teta_old(i) = this%q_old(cr(i)) ! Это у нас такой q_old
                end do
                teta = (matmul(this%lc_matr%c_1, teta_old)) / 2d0 ! А это у нас такой С1*q_old
                do i = 1, 4
                    this%gl_matr%f(cr(i)) = this%gl_matr%f(cr(i)) + hplane * vec_tet(i) - hplane * teta(i)
                end do
                ! добавка в глобальную матрицу
                hplane = hplane * beta / 2d0
                do i = 1, 4
                    this%gl_matr%di(cr(i)) = this%gl_matr%di(cr(i)) + hplane * this%lc_matr%c_1(i, i)
                    do j = 1, i -1, 1
                        call this%add(cr(i), cr(j), hplane * this%lc_matr%c_1(i, j))
                    end do
                end do
            end do
            close(40)
200         continue
            ! учет первых краевых условий
            open(50, file = 'bound1.txt', status = 'old', err = 300)
            rewind 50
            !print*, '1 bound detected!'
            do while(.true.)
                read(50, *, err = 300, end = 300) cur_row
                tmp = u_(this%set(cur_row)%mas_(1), this%set(cur_row)%mas_(2), this%set(cur_row)%mas_(3), this%t_curr)
                this%gl_matr%di(cur_row) = 1d0
                this%gl_matr%f(cur_row) = tmp
                i_s = this%gl_matr%ig(cur_row)
                i_e = this%gl_matr%ig(cur_row + 1) - 1
                do i = i_s, i_e, 1
                    this%gl_matr%f(this%gl_matr%jg(i)) = this%gl_matr%f(this%gl_matr%jg(i)) - this%gl_matr%gg(i) * tmp
                    this%gl_matr%gg(i) = 0d0
                end do
                do p = cur_row + 1, this%gl_matr%n, 1
                    i_s = this%gl_matr%ig(p)
                    i_e = this%gl_matr%ig(p + 1) - 1
                    do i = i_s, i_e, 1
                        if(this%gl_matr%jg(i) .eq. cur_row) then
                            this%gl_matr%f(p) = this%gl_matr%f(p) - this%gl_matr%gg(i) * tmp
                            this%gl_matr%gg(i) = 0d0
                        end if
                    end do
                end do
            end do
            close(50)
300         continue
        end subroutine

        subroutine dealloc_(this) ! освобождение памяти
            implicit none
            class(fem) :: this
            call this%gl_matr%dealloc_all()
            deallocate(this%set)
            deallocate(this%matr)
            deallocate(this%q_curr)
            deallocate(this%q_old)
        end subroutine

    end module
