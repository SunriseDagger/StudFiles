#if __GFORTRAN__ == 1 && __GNUC__ == 4 && __GNUC_MINOR__ < 5
#define class type
#endif
    module solver_los
        implicit none
        ! Параметры решателя
        integer, parameter, private :: max_iter = 100000
        double precision, parameter, private :: eps = 1d-16

        type :: los
            integer, private :: n ! Размерность СЛАУ
            ! Основные массивы
            integer, pointer, private :: ig(:), jg(:)
            double precision, pointer, private :: gu(:), gl(:), di(:)
            double precision, pointer, private :: rp(:)
            ! Массивы для преобславливателя
            double precision, allocatable, private :: uu(:), ll(:), ld(:)
        contains
            procedure, private :: precond  ! Вычисление матриц L и U
            procedure, private :: dot_prod ! скалярное произведение
            procedure, private :: mull_a   ! x = Af
            procedure, private :: solve_l  ! Lx = f, прямой ход
            procedure, private :: solve_u  ! Ux = f, обратный ход
            procedure :: init   ! инициализация
            procedure :: set_rp ! Установка правой части
            procedure :: solve  ! Получение решения и количества итераций
        end type

    contains

        subroutine init(this, s_ig, s_jg, s_gu, s_gl, s_di, s_n)
            implicit none
            class(los) :: this
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
            class(los) :: this
            double precision, pointer :: s_rp(:)
            this%rp => s_rp
        end subroutine

        subroutine precond(this)
            implicit none
            class(los) :: this
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
            class(los) :: this
            double precision :: a(*), b(*), dot_prod
            integer :: i
            dot_prod = 0d0
            do i = 1, this%n, 1
                dot_prod = dot_prod + a(i) * b(i)
            end do
        end function

        subroutine mull_a(this, f, x)
            implicit none
            class(los) :: this
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
            class(los) :: this
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
            class(los) :: this
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

        subroutine solve(this, solution, its)
            implicit none
            class(los) :: this
            double precision, pointer :: solution(:)
            double precision :: rp_norm, discr, dot1, alpha, betta
            double precision, allocatable :: x0(:), r(:), z(:), p(:), s(:), t(:)
            integer :: its, end_cycle = 0, iter
            ! Норма правой части, для выхода
            rp_norm = dsqrt(dot_prod(this, this%rp, this%rp))
            ! Начинаем решение
            allocate(x0(this%n)) ! Приближение
            x0 = 0d0
            allocate(solution(this%n))
            allocate(r(this%n)) ! Вектор невязки
            allocate(z(this%n))
            allocate(p(this%n))
            allocate(s(this%n)) ! Вспомогательный вектор
            allocate(t(this%n)) ! Вспомогательный вектор
            ! r0 = L^(-1) * (f - Ax0)
            call this%mull_a(x0, s)
            s = this%rp - s
            call this%solve_l(s, r)
            ! z0 = U^(-1)r0
            call this%solve_u(r,z)
            ! p0 = L^(-1)Az0
            call this%mull_a(z, s)
            call this%solve_l(s, p)
            iter = 0
            do while(iter .lt. max_iter .and. end_cycle .eq. 0)
                discr = dsqrt(dot_prod(this, r, r)) ! Абсолютная невязка
                if(discr / rp_norm .gt. eps)then ! Проверка условия выхода
                    dot1 = dot_prod(this, p, p) ! (p[k-1], p[k-1])
                    alpha = dot_prod(this, p ,r) / dot1 ! a = (p[k-1], r[k-1]) / (p[k-1], p[k-1])
                    x0 = x0 + alpha * z ! x[k] = x[k-1] + a*z[k-1]
                    r = r - alpha * p ! r[k] = r[k-1] - a*p[k-1]
                    ! betta = -(p[k-1], L^(-1)*A*U^(-1)r[k]) / (p[k-1], p[k-1])
                    call this%solve_u(r, s) ! s = U^(-1)r[k]
                    call this%mull_a(s, t)
                    call this%solve_l(t, t);
                    betta = - dot_prod(this, p, t) / dot1
                    z = s + betta * z ! z[k] = U^(-1)r[k] + b*z[k-1]
                    p = t + betta * p ! p[k] = L^(-1)*A*U^(-1)r[k] + b*p[k-1]
                    if(mod(iter, this%n) .eq. 0) then ! Обновление метода
                        ! r0 = L^(-1) * (f - Ax0)
                        call this%mull_a(x0, s)
                        s = this%rp - s
                        call this%solve_l(s, r)
                        ! z0 = U^(-1)r0
                        call this%solve_u(r, z)
                        ! p0 = L^(-1)Az0
                        call this%mull_a(z, s)
                        call this%solve_l(s, p)
                    end if
                else
                    end_cycle = 1
                end if
                iter = iter + 1
            end do
            solution = x0
            its = iter
            ! И отчишаем память
            deallocate(x0)
            deallocate(p)
            deallocate(r)
            deallocate(z)
            deallocate(s)
            deallocate(t)
        end subroutine

    end module
