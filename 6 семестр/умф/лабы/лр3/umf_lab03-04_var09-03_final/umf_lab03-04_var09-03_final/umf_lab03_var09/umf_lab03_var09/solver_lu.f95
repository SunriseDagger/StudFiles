#if __GFORTRAN__ == 1 && __GNUC__ == 4 && __GNUC_MINOR__ < 5
#define class type
#endif
    module solver_lu
        implicit none

        type :: lu
            integer, private :: n ! Размерность СЛАУ
            ! Основные массивы
            integer, pointer, private :: ig(:)
            double precision, pointer, private :: gu(:), gl(:), di(:)
            double precision, pointer, private :: rp(:)
        contains
            procedure :: init   ! инициализация
            procedure :: set_rp ! Установка правой части
            procedure :: solve  ! Получение решения и количества итераций
            procedure, private :: dec ! Простроение LU-разложения
        end type

    contains

        subroutine init(this, s_ig, s_jg, s_gu, s_gl, s_di, s_n)
            implicit none
            class(lu) :: this
            integer, pointer :: s_ig(:), s_jg(:)
            double precision, pointer :: s_gu(:), s_gl(:), s_di(:)
            integer :: s_n, i, j, k, total_n, j_s, j_e, column, s_point
            this%n = s_n
            allocate(this%di(this%n))
            allocate(this%ig(this%n+1))
            ! Перенос диагонали
            do i = 1, this%n, 1
                this%di(i) = s_di(i)
            end do
            ! Формируем новый массив ig
            this%ig(1) = 1
            do i = 2, this%n+1, 1
                k = s_ig(i) - s_ig(i-1)
                if(k .gt. 0)then
                    total_n = i - s_jg(s_ig(i-1))
                    this%ig(i) = this%ig(i-1) + total_n
                else
                    this%ig(i) = this%ig(i-1)
                end if
            end do
            allocate(this%gu(this%ig(this%n+1)))
            allocate(this%gl(this%ig(this%n+1)))
            ! Формируем новые gl и gu
            do i = 1, this%n, 1
                j_s = this%ig(i)
                j_e = this%ig(i+1)
                column = i - (j_e - j_s)
                s_point = s_ig(i)
                do j = j_s, j_e-1, 1
                    if(column .eq. s_jg(s_point))then
                        this%gu(j) = s_gu(s_point)
                        this%gl(j) = s_gl(s_point)
                        s_point = s_point + 1
                    else
                        this%gu(j) = 0d0
                        this%gl(j) = 0d0
                    end if
                    column = column + 1
                end do
            end do
            call this%dec()
        end subroutine

        subroutine set_rp(this, s_rp)
            implicit none
            class(lu) :: this
            double precision, pointer :: s_rp(:)
            integer :: i
            allocate(this%rp(this%n))
            do i = 1, this%n, 1
                this%rp(i) = s_rp(i)
            end do
        end subroutine

        subroutine dec(this)
            implicit none
            class(lu) :: this
            integer :: i, i0, i1, j, j0, j1, m, mi, mj, kol_i, kol_j, kol_r
            double precision :: sd, sl, su
            do i = 1, this%n, 1
                i0 = this%ig(i)
                i1 = this%ig(i+1)
                j = i - (i1 - i0)
                sd = 0d0
                do m = i0, i1 - 1, 1
                    sl = 0d0
                    su = 0d0
                    j0 = this%ig(j)
                    j1 = this%ig(j+1)
                    mi = j0
                    mj = j0
                    kol_i = m - i0
                    kol_j = j1 - j0
                    kol_r = kol_i - kol_j
                    if(kol_r .lt. 0)then
                        mj = mj - kol_r
                    else
                        mi = mi + kol_r
                    end if
                    do mi = mi, m-1, 1
                        sl = sl + this%gl(mi) * this%gu(mj)
                        su = su + this%gu(mi) * this%gl(mj)
                        mj = mj + 1
                    end do
                    this%gl(m) = this%gl(m) - sl
                    this%gu(m) = (this%gu(m) - su) / this%di(j)
                    sd = sd + this%gl(m) * this%gu(m)
                    j = j + 1
                end do
                this%di(i) = this%di(i) - sd
            end do
        end subroutine

        subroutine solve(this, solution, its)
            implicit none
            class(lu) :: this
            double precision, pointer :: solution(:)
            integer :: its, i, j, j_start, j_end, vect_iter
            double precision :: sum
            allocate(solution(this%n))
            ! Прямой ход
            do i = 1, this%n, 1
                sum = 0d0
                j_start = this%ig(i)
                j_end = this%ig(i+1)
                vect_iter = i - (j_end - j_start)
                do j = j_start, j_end-1, 1
                    sum = sum + this%gl(j) * this%rp(vect_iter)
                    vect_iter = vect_iter + 1
                end do
                this%rp(i) = (this%rp(i) - sum) / this%di(i)
            end do
            ! Обраный ход
            do i = this%n, 1, -1
                j_start = this%ig(i)
                j_end = this%ig(i+1)
                vect_iter = i - (j_end - j_start)
                do j = j_start, j_end-1, 1
                    this%rp(vect_iter) = this%rp(vect_iter) - this%gu(j) * this%rp(i)
                    vect_iter = vect_iter + 1
                end do
            end do

            do i = 1, this%n, 1
                solution(i) = this%rp(i)
            end do
            deallocate(this%ig)
            deallocate(this%gu)
            deallocate(this%gl)
            deallocate(this%di)
            deallocate(this%rp)
            its = 1
        end subroutine

    end module
