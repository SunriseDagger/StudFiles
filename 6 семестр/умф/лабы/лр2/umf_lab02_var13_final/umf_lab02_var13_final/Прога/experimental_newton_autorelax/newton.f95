! Fix for gfortran < 4.5.0
#define class type
    module newton_module
        implicit none

        type, private :: finite_element
            double precision :: begin_, end_, lambda_, gamma_
        end type

        type, private :: area
            type(finite_element), allocatable :: fe(:)
            integer :: fe_num
            ! Для первых bound_val1_x = ug, bound_val2_x = undefined
            ! Для вторых bound_val1_x = theta, bound_val2_x = undefined
            ! Для третьих bound_val1_x = beta, bound_val2_x = ub
            double precision :: bound_val1_l, bound_val2_l, bound_val1_r, bound_val2_r
            integer :: bound_type_l, bound_type_r
        end type

        type, private :: slae
            double precision, allocatable :: di(:), dl(:), du(:), f(:), q(:), q_old(:)
            integer :: n
        contains
            procedure :: solve
        end type

        type, private :: temp_matrix
            double precision, allocatable :: dl(:), du(:), di(:), f(:)
        contains
            procedure :: alloc
            procedure :: dealloc
        end type

        type :: newton_solver
            integer :: maxiter_ = 1000
            double precision :: epsilon_ = 1d-14
            type(area), private :: area
            type(slae), private :: slae
            type(temp_matrix), private :: tmpmtr
            double precision, private :: & ! матрица массы
                m_x(2,2)=reshape(source=(/2d0,1d0,1d0,2d0/),shape=(/2,2/))
            double precision, private :: & ! матрица жесткости
                g_x(2,2)=reshape(source=(/1d0,-1d0,-1d0,1d0/),shape=(/2,2/))
        contains
            procedure :: read_
            procedure :: solve_
            procedure :: clean_
            procedure :: write_
            procedure, private :: get_matrix
            procedure, private :: f_
            procedure, private :: df_dq
            procedure, private :: dbi_dqr
            procedure, private, nopass :: f_u
            procedure, private :: psi1
            procedure, private :: psi2
            procedure, private :: residual
            procedure, private :: get_omega
            procedure, private, nopass :: norm_2
        end type

    contains

        function f_u(u_, lambda_, gamma_)
            implicit none
            double precision :: u_, lambda_, gamma_, f_u
            f_u = - 4d0 * lambda_ + gamma_ * u_
        end function

        function f_(this, q, x, num_fe)
            implicit none
            class(newton_solver) :: this
            double precision :: q(*), u, f_, x
            integer :: num_fe
            u = q(num_fe) * psi1(this, x, num_fe) + &
                q(num_fe+1) * psi2(this, x, num_fe)
            f_ = f_u(u, this%area%fe(num_fe)%lambda_, &
                this%area%fe(num_fe)%gamma_)
        end function

        function df_dq(this, q, l, r, num_fe)
            implicit none
            class(newton_solver) :: this
            double precision :: q(*), df_dq, df_du, u1, u2, u, h, x, x1, x2
            integer :: num_fe, l, r
            if(l.eq.1) then
                x = this%area%fe(num_fe)%begin_
            else
                x = this%area%fe(num_fe)%end_
            end if
            h = (this%area%fe(num_fe)%end_ - this%area%fe(num_fe)%begin_) / 1d6
            x1 = x - h
            x2 = x + h
            u1 = q(num_fe) * psi1(this, x1, num_fe) + &
                q(num_fe+1) * psi2(this, x1, num_fe)
            u2 = q(num_fe) * psi1(this, x2, num_fe) + &
                q(num_fe+1) * psi2(this, x2, num_fe)
            if(dabs(u2 - u1) .gt. this%epsilon_) then
                h = dabs(u2 - u1)
                df_du = (f_u(u2, this%area%fe(num_fe)%lambda_, &
                    this%area%fe(num_fe)%gamma_) &
                    - f_u(u1, this%area%fe(num_fe)%lambda_, &
                    this%area%fe(num_fe)%gamma_)) / h
            else
                df_du = 0d0
            end if
            if(r.eq.1) then
                df_dq = df_du * psi1(this, x, num_fe)
            else
                df_dq = df_du * psi2(this, x, num_fe)
            end if
        end function

        function dbi_dqr(this, q, i, r, num_fe)
            implicit none
            class(newton_solver) :: this
            integer :: i, r, num_fe
            double precision :: q(*), dbi_dqr, h, df1, df2
            h = this%area%fe(num_fe)%end_ - this%area%fe(num_fe)%begin_
            df1 = df_dq(this, q, 1, r, num_fe)
            df2 = df_dq(this, q, 2, r, num_fe)
            if(dabs(df1).le.this%epsilon_.and.dabs(df1).le.this%epsilon_) then
                dbi_dqr = 0d0
            else
                if(i.eq.1) then
                    dbi_dqr = h / 6d0 * (2d0 * df1 + df2)
                else
                    dbi_dqr = h / 6d0 * (df1 + 2d0 * df2)
                end if
            end if
        end function

        function psi1(this, x, num_fe)
            implicit none
            class(newton_solver) :: this
            double precision :: x, psi1, x1, x2, hx
            integer :: num_fe
            x1 = this%area%fe(num_fe)%begin_
            x2 = this%area%fe(num_fe)%end_
            hx = x2 - x1
            psi1 = (x2 - x) / hx
        end function

        function psi2(this, x, num_fe)
            implicit none
            class(newton_solver) :: this
            double precision :: x, psi2, x1, x2, hx
            integer :: num_fe
            x1 = this%area%fe(num_fe)%begin_
            x2 = this%area%fe(num_fe)%end_
            hx = x2 - x1
            psi2 = (x - x1) / hx
        end function

        subroutine read_(this)
            implicit none
            class(newton_solver) :: this
            integer :: i
            open(10,file='../area.txt',status='old')
            read(10,*) this%area%fe_num
            allocate(this%area%fe(this%area%fe_num))
            do i=1,this%area%fe_num
                read(10,*) this%area%fe(i)%begin_, &
                    this%area%fe(i)%end_, &
                    this%area%fe(i)%lambda_, &
                    this%area%fe(i)%gamma_
            end do
            close(10)
            open(10,file='../bound.txt',status='old')
            read(10,*) this%area%bound_type_l
            if(this%area%bound_type_l.eq.3) then
                read(10,*) this%area%bound_val1_l,this%area%bound_val2_l
            else
                read(10,*) this%area%bound_val1_l
                this%area%bound_val2_l = 0d0
            end if
            read(10,*) this%area%bound_type_r
            if(this%area%bound_type_r.eq.3) then
                read(10,*) this%area%bound_val1_r,this%area%bound_val2_r
            else
                read(10,*) this%area%bound_val1_r
                this%area%bound_val2_r = 0d0
            end if
            close(10)
        end subroutine

        function norm_2(x, n)
            implicit none
            double precision :: x(*), norm_2
            integer :: n, i
            norm_2 = 0d0
            do i = 1, n
                norm_2 = norm_2 + x(i)**2
            end do
        end function

        function residual(this, q)
            implicit none
            class(newton_solver) :: this
            double precision :: residual, q(*)
            integer :: i
            double precision, allocatable :: dq(:)
            allocate(dq(this%slae%n))
            dq(1) = this%tmpmtr%di(1) * q(1) + &
                this%tmpmtr%du(1) * q(2)
            do i = 2, this%slae%n - 1
                dq(i) = this%tmpmtr%di(i) * q(i) + &
                    this%tmpmtr%du(i) * q(i+1) + &
                    this%tmpmtr%dl(i-1) * q(i-1)
            end do
            dq(this%slae%n) = this%tmpmtr%di(this%slae%n) * q(this%slae%n) + &
                this%tmpmtr%dl(this%slae%n-1) * q(this%slae%n-1)
            dq = dq - this%tmpmtr%f
            residual = dsqrt(norm_2(dq, this%slae%n) / &
                norm_2(this%tmpmtr%f, this%slae%n))
            deallocate(dq)
        end function

        function get_omega(this)
            implicit none
            class(newton_solver) :: this
            double precision :: get_omega, a = 0d0, b = 3d0, eps = 0.1
            double precision :: delta
            double precision :: x1, x2, f1, f2
            integer :: i, iter
            double precision, allocatable :: dq(:)
            allocate(dq(this%slae%n))
            delta = eps / 2d0
            do iter = 1, 10000
                x1 = (a + b - delta) / 2d0
                x2 = (a + b + delta) / 2d0
                dq = x1 * this%slae%q + (x1 - 1) * this%slae%q_old
                !call get_matrix(this, dq)
                f1 = residual(this, dq)
                dq = x2 * this%slae%q + (x2 - 1) * this%slae%q_old
                !call get_matrix(this, dq)
                f2 = residual(this, dq)
                !print*, f1, f2
                if(f1.le.f2) then
                    b = x2
                else
                    a = x1
                end if
                if(dabs(a - b).lt.eps) exit
            end do
            deallocate(dq)
            get_omega = (a + b) / 2d0
        end function

        subroutine get_matrix(this, q)
            implicit none
            class(newton_solver) :: this
            integer :: i, j, ind
            double precision :: h, g_mul, m_mul, f1, f2, f_mul, add, q(*)
            this%slae%di = 0d0
            this%slae%dl = 0d0
            this%slae%du = 0d0
            this%slae%f = 0d0
            this%tmpmtr%di = 0d0
            this%tmpmtr%dl = 0d0
            this%tmpmtr%du = 0d0
            this%tmpmtr%f = 0d0
            ! Цикл по КЭ
            do i = 1, this%area%fe_num
                h = this%area%fe(i)%end_ - this%area%fe(i)%begin_
                ! Добавки в матрицу
                g_mul = this%area%fe(i)%lambda_ / h
                m_mul = this%area%fe(i)%gamma_ * h / 6d0
                do j = 0, 1
                    ind = i + j
                    add = g_mul * this%g_x(j+1, j+1) + &
                        m_mul * this%m_x(j+1, j+1)
                    this%slae%di(ind) = this%slae%di(ind) + add
                    this%tmpmtr%di(ind) = this%tmpmtr%di(ind) + add
                end do
                add = g_mul * this%g_x(2, 1) + &
                    m_mul * this%m_x(2, 1)
                this%slae%dl(i) = this%slae%dl(i) + add
                this%tmpmtr%dl(i) = this%tmpmtr%dl(i) + add
                add = g_mul * this%g_x(1, 2) + &
                    m_mul * this%m_x(1, 2)
                this%slae%du(i) = this%slae%du(i) + add
                this%tmpmtr%du(i) = this%tmpmtr%du(i) + add
                ! Добавки в правую часть
                f_mul = h / 6d0
                f1 = f_(this, q, this%area%fe(i)%begin_, i)
                f2 = f_(this, q, this%area%fe(i)%end_, i)
                add = f_mul * (2d0 * f1 + f2)
                this%slae%f(i) = this%slae%f(i) + add
                this%tmpmtr%f(i) = this%tmpmtr%f(i) + add
                add = f_mul * (f1 + 2d0 * f2)
                this%slae%f(i+1) = this%slae%f(i+1) + add
                this%tmpmtr%f(i+1) = this%tmpmtr%f(i+1) + add
            end do
            ! Добавки от линеаризации
            do i = 1, this%area%fe_num
                this%slae%di(i) = this%slae%di(i) - dbi_dqr(this, q, 1, 1, i)
                this%slae%di(i+1) = this%slae%di(i+1) - dbi_dqr(this, q, 2, 2, i)
                this%slae%dl(i) = this%slae%dl(i) - dbi_dqr(this, q, 2, 1, i)
                this%slae%du(i) = this%slae%du(i) - dbi_dqr(this, q, 1, 2, i)
                add = dbi_dqr(this, q, 1, 1, i) * q(i) + &
                    dbi_dqr(this, q, 1, 2, i) * q(i+1)
                this%slae%f(i) = this%slae%f(i) - add
                add = dbi_dqr(this, q, 2, 1, i) * q(i) + &
                    dbi_dqr(this, q, 2, 2, i) * q(i+1)
                this%slae%f(i+1) = this%slae%f(i+1) - add
            end do
            ! Краевые слева
            select case(this%area%bound_type_l)
            case(1)
                this%slae%di(1) = 1d0
                this%slae%du(1) = 0d0
                this%slae%f(1) = this%area%bound_val1_l
                this%tmpmtr%di(1) = 1d0
                this%tmpmtr%du(1) = 0d0
                this%tmpmtr%f(1) = this%area%bound_val1_l
            case(2)
                this%slae%f(1) = this%slae%f(1) + this%area%bound_val1_l
                this%tmpmtr%f(1) = this%tmpmtr%f(1) + this%area%bound_val1_l
            case(3)
                this%slae%di(1) = this%slae%di(1) + this%area%bound_val1_l
                this%slae%f(1) = this%slae%f(1) + this%area%bound_val1_l * &
                    this%area%bound_val2_l
                this%tmpmtr%di(1) = this%tmpmtr%di(1) + this%area%bound_val1_l
                this%tmpmtr%f(1) = this%tmpmtr%f(1) + this%area%bound_val1_l * &
                    this%area%bound_val2_l
            end select
            ! Краевые справа
            ind = this%slae%n
            select case(this%area%bound_type_r)
            case(1)
                this%slae%di(ind) = 1d0
                this%slae%dl(ind-1) = 0d0
                this%slae%f(ind) = this%area%bound_val1_r
                this%tmpmtr%di(ind) = 1d0
                this%tmpmtr%dl(ind-1) = 0d0
                this%tmpmtr%f(ind) = this%area%bound_val1_r
            case(2)
                this%slae%f(ind) = this%slae%f(ind) + this%area%bound_val1_r
                this%tmpmtr%f(ind) = this%tmpmtr%f(ind) + this%area%bound_val1_r
            case(3)
                this%slae%di(ind) = this%slae%di(ind) + this%area%bound_val1_r
                this%slae%f(ind) = this%slae%f(ind) + this%area%bound_val1_r * &
                    this%area%bound_val2_r
                this%tmpmtr%di(ind) = this%tmpmtr%di(ind) + this%area%bound_val1_r
                this%tmpmtr%f(ind) = this%tmpmtr%f(ind) + this%area%bound_val1_r * &
                    this%area%bound_val2_r
            end select
        end subroutine

        subroutine alloc(this, n)
            implicit none
            class(temp_matrix) :: this
            integer :: n
            allocate(this%di(n))
            allocate(this%du(n - 1))
            allocate(this%dl(n - 1))
            allocate(this%f(n))
        end subroutine

        subroutine dealloc(this)
            implicit none
            class(temp_matrix) :: this
            deallocate(this%di)
            deallocate(this%du)
            deallocate(this%dl)
            deallocate(this%f)
        end subroutine

        subroutine solve_(this)
            implicit none
            class(newton_solver) :: this
            integer :: i, iter = 0
            double precision :: res, omega
            this%slae%n = this%area%fe_num + 1
            allocate(this%slae%di(this%slae%n))
            allocate(this%slae%du(this%slae%n - 1))
            allocate(this%slae%dl(this%slae%n - 1))
            allocate(this%slae%f(this%slae%n))
            allocate(this%slae%q(this%slae%n))
            allocate(this%slae%q_old(this%slae%n))
            call this%tmpmtr%alloc(this%slae%n)
            this%slae%q = 0d0
            do
                this%slae%q_old = this%slae%q
                call get_matrix(this, this%slae%q)
                call this%slae%solve()
                res = residual(this, this%slae%q_old)
                omega = get_omega(this)
                this%slae%q = omega * this%slae%q + &
                    (1d0 - omega) * this%slae%q_old
                iter = iter + 1
                do i = 1, this%slae%n
                    print*, this%slae%q(i)
                end do
                print*, 'Iteration:', iter
                print*, 'Residual= ', res
                print*, 'Omega   = ', omega
                if(res.lt.this%epsilon_ &
                    .or.iter.gt.this%maxiter_) exit
            end do
            call this%tmpmtr%dealloc()
        end subroutine

        subroutine solve(this)
            implicit none
            class(slae) :: this
            integer :: i
            do i = 2, this%n
                this%dl(i-1) = this%dl(i-1) / this%di(i-1)
                this%di(i) = this%di(i) - this%dl(i-1) * this%du(i-1)
                this%f(i) = this%f(i) - this%f(i-1) * this%dl(i-1)
            end do
            do i = this%n, 2, -1
                this%q(i) = this%f(i) / this%di(i)
                this%f(i-1) = this%f(i-1) - this%q(i) * this%du(i-1)
            end do
            this%q(1) = this%f(1) / this%di(1)
        end subroutine

        subroutine clean_(this)
            implicit none
            class(newton_solver) :: this
            deallocate(this%slae%di)
            deallocate(this%slae%du)
            deallocate(this%slae%dl)
            deallocate(this%slae%f)
            deallocate(this%slae%q)
            deallocate(this%slae%q_old)
            deallocate(this%area%fe)
        end subroutine

        subroutine write_(this)
            implicit none
            class(newton_solver) :: this
            integer :: i
            open(10, file='../newton.txt', status='unknown')
            do i = 1, this%slae%n
                write(10, fmt='( e27.16 )') this%slae%q(i)
            end do
            close(10)
        end subroutine
    end module

    program main
        use newton_module
        implicit none
        type(newton_solver) :: n
        call n%read_()
        call n%solve_()
        call n%write_()
        call n%clean_()
        print*,'Hellop'
    end program
