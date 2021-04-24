! Fix for gfortran < 4.5.0
#define class type
    module simple_iter_module
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

        type :: simple_iter_solver
            double precision :: omega_ = 1.0d0
            integer :: maxiter_ = 100000
            double precision :: epsilon_ = 1d-10
            type(area), private :: area
            type(slae), private :: slae
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
            procedure, private :: psi1
            procedure, private :: psi2
            procedure, private :: residual
            procedure, private, nopass :: norm_2
        end type

    contains

        function f_(this, q, x, num_fe)
            implicit none
            class(simple_iter_solver) :: this
            double precision :: q(*), u, f_, x
            integer :: num_fe
            u = q(num_fe) * psi1(this, x, num_fe) + &
                q(num_fe+1) * psi2(this, x, num_fe)
            ! # Test 4.1.
            !f_ = u
            ! # Test 4.2.
            !f_ = -4d0 + u
            ! # Test 4.3.
            !f_ = -6d0 * sign(abs(u)**(1d0/3d0), u) + u
            ! # Test 4.4.
            !f_ = 2d0 * u
            ! # Test 4.5.
            !f_ = 0d0
            ! # Test 4.6.
            !f_ = 2d0 * u
            ! # Test 4.7.
            f_ = -4d0 + u
            ! # Test 4.8.
            !f_ = 2d0 * u
            !f_ = - 4d0 * this%area%fe(num_fe)%lambda_ + &
            !    this%area%fe(num_fe)%gamma_ * u
        end function

        function psi1(this, x, num_fe)
            implicit none
            class(simple_iter_solver) :: this
            double precision :: x, psi1, x1, x2, hx
            integer :: num_fe
            x1 = this%area%fe(num_fe)%begin_
            x2 = this%area%fe(num_fe)%end_
            hx = x2 - x1
            psi1 = (x2 - x) / hx
        end function

        function psi2(this, x, num_fe)
            implicit none
            class(simple_iter_solver) :: this
            double precision :: x, psi2, x1, x2, hx
            integer :: num_fe
            x1 = this%area%fe(num_fe)%begin_
            x2 = this%area%fe(num_fe)%end_
            hx = x2 - x1
            psi2 = (x - x1) / hx
        end function

        subroutine read_(this)
            implicit none
            class(simple_iter_solver) :: this
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

        function residual(this)
            implicit none
            class(simple_iter_solver) :: this
            double precision :: residual
            integer :: i
            double precision, allocatable :: dq(:)
            allocate(dq(this%slae%n))
            dq(1) = this%slae%di(1) * this%slae%q_old(1) + &
                this%slae%du(1) * this%slae%q_old(2)
            do i = 2, this%slae%n - 1
                dq(i) = this%slae%di(i) * this%slae%q_old(i) + &
                    this%slae%du(i) * this%slae%q_old(i+1) + &
                    this%slae%dl(i-1) * this%slae%q_old(i-1)
            end do
            dq(this%slae%n) = this%slae%di(this%slae%n) * this%slae%q_old(this%slae%n) + &
                this%slae%dl(this%slae%n-1) * this%slae%q_old(this%slae%n-1)
            dq = dq - this%slae%f
            residual = dsqrt(norm_2(dq, this%slae%n) / &
                norm_2(this%slae%f, this%slae%n))
            deallocate(dq)
            print*, residual
        end function

        subroutine get_matrix(this)
            implicit none
            class(simple_iter_solver) :: this
            integer :: i, j, ind
            double precision :: h, g_mul, m_mul, f1, f2, f_mul
            this%slae%di = 0d0
            this%slae%dl = 0d0
            this%slae%du = 0d0
            this%slae%f = 0d0
            ! Цикл по КЭ
            do i = 1, this%area%fe_num
                h = this%area%fe(i)%end_ - this%area%fe(i)%begin_
                ! Добавки в матрицу
                g_mul = this%area%fe(i)%lambda_ / h
                m_mul = this%area%fe(i)%gamma_ * h / 6d0
                do j = 0, 1
                    ind = i + j
                    this%slae%di(ind) = this%slae%di(ind) + &
                        g_mul * this%g_x(j+1, j+1) + &
                        m_mul * this%m_x(j+1, j+1)
                end do
                this%slae%dl(i) = this%slae%dl(i) + &
                    g_mul * this%g_x(2, 1) + &
                    m_mul * this%m_x(2, 1)
                this%slae%du(i) = this%slae%du(i) + &
                    g_mul * this%g_x(1, 2) + &
                    m_mul * this%m_x(1, 2)
                ! Добавки в правую часть
                f_mul = h / 6d0
                f1 = f_(this, this%slae%q, this%area%fe(i)%begin_, i)
                f2 = f_(this, this%slae%q, this%area%fe(i)%end_, i)
                this%slae%f(i) = this%slae%f(i) + f_mul * (2d0 * f1 + f2)
                this%slae%f(i+1) = this%slae%f(i+1) + f_mul * (f1 + 2d0 * f2)
            end do
            ! Краевые слева
            select case(this%area%bound_type_l)
            case(1)
                this%slae%di(1) = 1d0
                this%slae%du(1) = 0d0
                this%slae%f(1) = this%area%bound_val1_l
            case(2)
                this%slae%f(1) = this%slae%f(1) + this%area%bound_val1_l
            case(3)
                this%slae%di(1) = this%slae%di(1) + this%area%bound_val1_l
                this%slae%f(1) = this%slae%f(1) + this%area%bound_val1_l * &
                    this%area%bound_val2_l
            end select
            ! Краевые справа
            ind = this%slae%n
            select case(this%area%bound_type_r)
            case(1)
                this%slae%di(ind) = 1d0
                this%slae%dl(ind-1) = 0d0
                this%slae%f(ind) = this%area%bound_val1_r
            case(2)
                this%slae%f(ind) = this%slae%f(ind) + this%area%bound_val1_r
            case(3)
                this%slae%di(ind) = this%slae%di(ind) + this%area%bound_val1_r
                this%slae%f(ind) = this%slae%f(ind) + this%area%bound_val1_r * &
                    this%area%bound_val2_r
            end select
        end subroutine

        subroutine solve_(this)
            implicit none
            class(simple_iter_solver) :: this
            integer :: i, iter = 0
            this%slae%n = this%area%fe_num + 1
            allocate(this%slae%di(this%slae%n))
            allocate(this%slae%du(this%slae%n - 1))
            allocate(this%slae%dl(this%slae%n - 1))
            allocate(this%slae%f(this%slae%n))
            allocate(this%slae%q(this%slae%n))
            allocate(this%slae%q_old(this%slae%n))
            this%slae%q = 0d0
            do
                this%slae%q_old = this%slae%q
                call get_matrix(this)
                call this%slae%solve()
                this%slae%q = this%omega_ * this%slae%q + &
                    (1d0 - this%omega_) * this%slae%q_old
                iter = iter + 1
                print*, 'Iteration:', iter
                do i = 1, this%slae%n
                    print*, this%slae%q(i)
                end do
                if(residual(this).lt.this%epsilon_.or.iter.gt.this%maxiter_) exit
            end do
        end subroutine

        subroutine solve(this)
            implicit none
            class(slae) :: this
            integer :: i
            double precision, allocatable :: dl(:), du(:), di(:), f(:)
            allocate(di(this%n))
            allocate(du(this%n - 1))
            allocate(dl(this%n - 1))
            allocate(f(this%n))
            di = this%di
            dl = this%dl
            du = this%du
            f = this%f
            do i = 2, this%n
                dl(i-1) = dl(i-1) / di(i-1)
                di(i) = di(i) - dl(i-1) * du(i-1)
                f(i) = f(i) - f(i-1) * dl(i-1)
            end do
            do i = this%n, 2, -1
                this%q(i) = f(i) / di(i)
                f(i-1) = f(i-1) - this%q(i) * du(i-1)
            end do
            this%q(1) = f(1) / di(1)
            deallocate(di)
            deallocate(du)
            deallocate(dl)
            deallocate(f)
        end subroutine

        subroutine clean_(this)
            implicit none
            class(simple_iter_solver) :: this
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
            class(simple_iter_solver) :: this
            integer :: i
            open(10, file='../simple_iter.txt', status='unknown')
            do i = 1, this%slae%n
                write(10, fmt='( e27.16 )') this%slae%q(i)
            end do
            close(10)
        end subroutine
    end module

    program main
        use simple_iter_module
        implicit none
        type(simple_iter_solver) :: si
        call si%read_()
        call si%solve_()
        call si%write_()
        call si%clean_()
        print*,'Hellop'
    end program
