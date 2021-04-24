#if __GFORTRAN__ == 1 && __GNUC__ == 4 && __GNUC_MINOR__ < 5
#define class type
#endif
    module solver_gmres
        implicit none
        ! Параметры решателя
        integer, parameter, private :: max_iter = 10000
        double precision, parameter, private :: eps = 1d-14
        integer, parameter, private :: m = 3

        type :: gmres
            integer, private :: n ! Размерность СЛАУ
            ! Основные массивы
            integer, pointer, private :: ig(:), jg(:)
            double precision, pointer, private :: gu(:), gl(:), di(:)
            double precision, pointer, private :: rp(:)
            ! Массивы для преобславливателя
            double precision, allocatable, private :: uu(:), ll(:), ld(:)
            ! Вспомогательное для GMRES
            double precision, allocatable, private :: h(:), h2(:), v(:), w(:), y(:)
        contains
            procedure, private :: precond  ! Вычисление матриц L и U
            procedure, private :: dot_prod ! скалярное произведение
            procedure, private :: mull_a   ! x = Af
            procedure, private :: solve_l  ! Lx = f, прямой ход
            procedure, private :: solve_u  ! Ux = f, обратный ход
            procedure :: init   ! инициализация
            procedure :: set_rp ! Установка правой части
            procedure :: solve  ! Получение решения и количества итераций
            procedure, private :: mult_v_y
            procedure, private :: insert_col
            procedure, private :: calc_h_w
            procedure, private :: calc_hth
            procedure, private :: gauss
        end type

    contains

        subroutine init(this, s_ig, s_jg, s_gu, s_gl, s_di, s_n)
            implicit none
            class(gmres) :: this
            integer, pointer :: s_ig(:), s_jg(:)
            double precision, pointer :: s_gu(:), s_gl(:), s_di(:)
            integer :: s_n
            this%ig => s_ig
            this%jg => s_jg
            this%gu => s_gu
            this%gl => s_gl
            this%di => s_di
            this%n = s_n
            call this%precond()
        end subroutine

        subroutine set_rp(this, s_rp)
            implicit none
            class(gmres) :: this
            double precision, pointer :: s_rp(:)
            this%rp => s_rp
        end subroutine

        subroutine precond(this)
            implicit none
            class(gmres) :: this
            double precision :: sum_l, sum_u, sum_d ! Промежуточные переменные, для вычисления сумм
            integer :: copy_end, i, j, k, k1, i_s, i_e, m, j_s, j_e
            copy_end = this%ig(this%n + 1)
            allocate(this%ll(copy_end))
            allocate(this%uu(copy_end))
            allocate(this%ld(this%n))
            ! Копируем старые в новые
            do i = 1, copy_end, 1
                this%ll(i) = this%gl(i)
                this%uu(i) = this%gu(i)
            end do
            do i = 1, this%n, 1
                this%ld(i) = this%di(i)
            end do
            k1 = 1
            do k = 2, this%n + 1, 1
                sum_d = 0d0
                i_s = this%ig(k1)
                i_e = this%ig(k)
                do m = i_s, i_e - 1, 1
                    sum_l = 0d0
                    sum_u = 0d0
                    j_s = this%ig(this%jg(m))
                    j_e = this%ig(this%jg(m)+1)
                    do i = i_s, m - 1, 1
                        do j = j_s, j_e - 1, 1
                            sum_l = sum_l + this%ll(i) * this%uu(j)
                            sum_u = sum_u + this%ll(j) * this%uu(i)
                            j_s = j_s + 1
                        end do
                    end do
                    this%ll(m) = this%ll(m) - sum_l
                    this%uu(m) = (this%uu(m) - sum_u) / this%ld(this%jg(m))
                    sum_d = sum_d + this%ll(m) * this%uu(m)
                end do
                this%ld(k1) = this%ld(k1) - sum_d
                k1 = k1 + 1
            end do
        end subroutine

        function dot_prod(this, a, b)
            implicit none
            class(gmres) :: this
            double precision :: a(*), b(*), dot_prod
            integer :: i
            dot_prod = 0d0
            do i = 1, this%n, 1
                dot_prod = dot_prod + a(i) * b(i)
            end do
        end function

        subroutine mull_a(this, f, x)
            implicit none
            class(gmres) :: this
            double precision :: f(*), x(*), v_el
            integer :: i, j, k, k1
            do i = 1, this%n, 1
                v_el = f(i)
                x(i) = this%di(i) * v_el
                k1 = this%ig(i + 1)
                do k = this%ig(i), k1 - 1, 1
                    j = this%jg(k)
                    x(i) = x(i) + this%gl(k) * f(j)
                    x(j) = x(j) + this%gu(k) * v_el
                end do
            end do
        end subroutine

        subroutine solve_l(this, f, x)
            implicit none
            class(gmres) :: this
            double precision :: f(*), x(*), sum
            integer :: i, k, k1
            k1 = 1
            do k = 2, this%n + 1, 1
                sum = 0d0
                do i = this%ig(k1), this%ig(k) - 1, 1
                    sum = sum + this%ll(i) * x(this%jg(i))
                end do
                x(k1) = (f(k1) - sum) / this%ld(k1)
                k1 = k1 + 1
            end do
        end subroutine

        subroutine solve_u(this, f, x)
            implicit none
            class(gmres) :: this
            double precision :: f(*), x(*), v_el
            double precision, allocatable :: f1(:)
            integer :: i, k, k1
            allocate(f1(this%n))
            do i = 1, this%n, 1
                f1(i) = f(i)
            end do
            k1 = this%n
            do k = this%n + 1, 2, -1
                x(k1) = f1(k1) / this%ld(k1)
                v_el = x(k1)
                do i = this%ig(k1), this%ig(k) - 1, 1
                    f1(this%jg(i)) = f1(this%jg(i)) - this%uu(i) * v_el
                end do
                k1 = k1 - 1
            end do
            deallocate(f1)
        end subroutine

        subroutine mult_v_y(this, mr, res)
            implicit none
            class(gmres) :: this
            integer :: mr, k, p
            double precision :: res(*)
            do k = 1, this%n, 1
                res(k) = 0d0
                do p = 1, mr, 1
                    res(k) = res(k) + this%v(k*m+p) * this%y(p)
                end do
            end do
        end subroutine

        subroutine insert_col(this, j, x)
            implicit none
            class(gmres) :: this
            integer :: i, j
            double precision :: x(*)
            do i = 1, this%n, 1
                this%v(i*m+j) = x(i)
            end do
        end subroutine

        subroutine calc_h_w(this, i, j)
            implicit none
            class(gmres) :: this
            integer :: i, j, ind, k
            ind = i*m+j
            this%h(ind) = 0d0
            do k = 1, this%n, 1
                this%h(ind) = this%h(ind) + this%w(k) * this%v(k*m+i)
            end do
            do k = 1, this%n, 1
                this%w(k) = this%w(k) - this%h(ind) * this%v(k*m+i)
            end do
        end subroutine

        subroutine calc_hth(this, mr)
            implicit none
            class(gmres) :: this
            integer :: mr, i, j, k, idx
            this%h2 = 0d0
            do i = 1, mr, 1
                do j = 1, mr, 1
                    idx = i * m + j
                    do k = 1, mr + 1, 1
                        this%h2(idx) = this%h2(idx) + this%h(k * m + i) * this%h(k * m + j)
                    end do
                end do
            end do
        end subroutine

        subroutine gauss(this, matr, vector, mr)
            implicit none
            class(gmres) :: this
            integer :: mr, i, j, k, ind_di, ind_maxstr
            double precision :: matr(:), vector(:), temp, max
            do i = 1, mr, 1
                ind_di = i * (m + 1)
                ind_maxstr = i
                max = dabs(matr(ind_di))
                do j = 2, mr - i, 1
                    if(dabs(matr(ind_di)) .gt. max) then
                        max = dabs(matr(ind_di + j * m))
                        ind_maxstr = i + j
                    end if
                end do
                do j = i, mr, 1
                    temp = matr(i * m + j)
                    matr(i * m + j) = matr(ind_maxstr * m + j)
                    matr(ind_maxstr * m + j) = temp
                end do
                temp = vector(i)
                vector(i) = vector(ind_maxstr)
                vector(ind_maxstr) = temp
                do j = 2, mr - i, 1
                    matr(ind_di + j) = matr(ind_di + j) / matr(ind_di)
                end do
                vector(i) = vector(i) / matr(ind_di)
                matr(ind_di) = 1d0
                do j = i+1, mr, 1
                    do k = i+1, mr, 1
                        matr(j*m+k) = matr(j*m+k) - matr(i*m+k) * matr(j*m+i)
                    end do
                    vector(j) = vector(j) - matr(j*m+i) * vector(i)
                    matr(j*m+i) = 0d0
                end do
            end do
            do i = mr, 1, -1
                do j = mr, i+1, -1
                    vector(i) = vector(i) - matr(i*m+j) * vector(j)
                end do
            end do
        end subroutine

        subroutine solve(this, solution, its)
            implicit none
            class(gmres) :: this
            double precision, pointer :: solution(:)
            double precision :: rp_norm, r_norm, h_last
            double precision, allocatable :: t(:), p(:)
            integer :: its, iter, newm, flag, i, j
            rp_norm = dsqrt(dot_prod(this, this%rp, this%rp))
            allocate(solution(this%n))
            solution = 0d0
            allocate(p(this%n))
            allocate(t(this%n))
            allocate(this%w(this%n))
            allocate(this%y(m))
            allocate(this%v(m * this%n))
            allocate(this%h(m * (m + 1)))
            allocate(this%h2(m * m))
            newm = m
            iter = 0
            ! вычисляем r0
            call this%mull_a(solution, t) ! t=A*x
            this%w = this%rp - t
            call this%solve_l(this%w, p) ! L*p=f-Ax
            r_norm = dsqrt(dot_prod(this, p, p)) ! p = r0
            do while(iter .lt. max_iter .and. r_norm / rp_norm .gt. eps)
                flag = 0
                p = p / r_norm
                call this%insert_col(1, p)
                do j = 1, newm, 1
                    call this%solve_u(p, t) ! U*t=p
                    call this%mull_a(t, p) ! p=At
                    call this%solve_l(p, this%w) ! L*w=A*t
                    do i = 1, j, 1
                        call this%calc_h_w(i, j)
                    end do
                    this%h(j*m+j) = dsqrt(dot_prod(this, this%w, this%w))
                    h_last = this%h(j*m+j)
                    if(dabs(h_last) .gt. eps**2 .and. j .ne. newm)then
                        p(i) = this%w(i) / h_last
                        call this%insert_col(j+1, p)
                    end if
                    if(dabs(h_last) .lt. eps**2)then
                        newm = j
                        flag = 1
                    end if
                end do
                if(flag .ne. 0)then
                    this%y = 0d0
                    this%y(1) = r_norm
                    call this%gauss(this%h, this%y, newm)
                else
                    this%y = this%h * r_norm
                    call this%calc_hth(newm)
                    call this%gauss(this%h2, this%y, newm)
                end if
                call this%mult_v_y(newm, t) ! t=Vy
                call this%solve_u(t, p) ! U*p=t
                solution = solution + p ! x+=U(-1)*V*y
                call this%mull_a(solution, t) ! t=Ax
                r_norm = dsqrt(dot_prod(this, p, p)) ! p=r
                iter = iter + 1
            end do
            its = iter
            deallocate(p)
            deallocate(t)
            deallocate(this%w)
            deallocate(this%y)
            deallocate(this%v)
            deallocate(this%h)
            deallocate(this%h2)
        end subroutine

    end module
