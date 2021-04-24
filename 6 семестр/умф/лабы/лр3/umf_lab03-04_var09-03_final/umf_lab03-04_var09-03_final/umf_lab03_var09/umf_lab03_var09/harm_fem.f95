#if __GFORTRAN__ == 1 && __GNUC__ == 4 && __GNUC_MINOR__ < 5
#define class type
#endif
#define SOLVERTYPE gmres
#define SOLVERMODULE solver_gmres
    module harm_fem_
        use addition_classes
        use SOLVERMODULE
        use timer_module
        implicit none

        type :: harm_fem
            integer, private :: elements_n  ! Количество узлов
            integer, private :: nodes_n     ! Количество элементов
            integer, private :: faces_fir_n, faces_sec_n, faces_thi_n ! Количество граней с 1, 2 и 3 краевыми соответственно
            type(node), private, allocatable :: nodes(:)    ! Массив узлов
            type(fe), private, allocatable :: elements(:)   ! Массив элементов
            type(face), private, allocatable :: faces_sec(:), faces_thi(:) ! Массивы для краевых граней
            double precision, private, allocatable :: faces_fir(:)  ! Значения первых краевых
            integer, private, allocatable :: faces_fir_node(:)      ! Узлы первых краевых

            double precision, private, allocatable :: betta(:)
            type(SOLVERTYPE), private :: solver      ! Решатель СЛАУ
            integer, private :: solver_iters         ! количество итераций за которое было решено СЛАУ
            type(slae_port_gen), private :: port_gen ! Генератор портрета СЛАУ
            double precision, private :: time        ! Время решения СЛАУ
            ! Массивы, для хранения СЛАУ
            integer, pointer, private :: ig(:), jg(:)
            integer, private :: slae_el_n ! Количество элементов в СЛАУ(элементов в jg, gl, gu)
            double precision, pointer, private :: gl(:), gu(:), di(:)
            double precision, pointer, private :: right_part(:) ! правая часть
            double precision, pointer, private :: solution(:)   ! Решение
        contains
            procedure :: transf_grid    ! преобразовывает сетку в файлах из обычной кубической в нужную
            procedure :: init_hf        ! Ввод данных
            procedure :: form_matrix    ! Формирование матрицы СЛАУ
            procedure :: solve_         ! Решение СЛАУ
            procedure :: out_rez        ! Вывод резульатата в файл file_name
            procedure :: out_diff       ! Вывод погрешности в файл
            procedure, private :: u_betta_s     ! Вычисление краевого условия третьего рода, для синуса
            procedure, private :: u_betta_c     ! Вычисление краевого условия третьего рода, для косинуса
            procedure, private :: tetta_s       ! Вычисление краевого условия второго рода, для синуса
            procedure, private :: tetta_c       ! Вычисление краевого условия второго рода, для косинуса
            procedure, private :: lambda        ! коэффициенты уравнения
            procedure, private :: sigma
            procedure, private :: hi
            procedure, private :: f_sin         ! правая часть уравнения
            procedure, private :: f_cos
            procedure, private :: w             ! Частота колебаний
            procedure, public :: u_sin
            procedure, public :: u_cos
            procedure, private :: form_gmass    ! Формирование массивов ig и jg
            procedure, private :: find_el_pos   ! Определяет положение элемента в матрице
            procedure, private :: form_loc      ! Формирование локальной матрицы для элемента el_n
            procedure, private :: gen_loc_thi   ! Формирование локальной матрицы и вектора для третих краевых, для грани face_n
            procedure, private :: gen_loc_sec   ! Формирование локального вектора для вторых краевых, для грани face_n
            ! Вычисление специальных индексов, для матрицы жескости
            procedure, private :: mu
            procedure, private :: nu
            procedure, private :: v
        end type

    contains

        function w(this)
            implicit none
            class(harm_fem) :: this
            double precision :: w
            w = 1d-2
        end function

        function lambda(this, x, y, z)
            implicit none
            class(harm_fem) :: this
            double precision :: x, y, z, lambda
            lambda = 1d4
        end function

        function sigma(this, x, y, z)
            implicit none
            class(harm_fem) :: this
            double precision :: x, y, z, sigma
            sigma = 1d4
        end function

        function hi(this, x, y, z)
            implicit none
            class(harm_fem) :: this
            double precision :: x, y, z, hi
            hi = 1d-11
        end function

        function u_sin(this, x, y, z)
            implicit none
            class(harm_fem) :: this
            double precision :: x, y, z, u_sin
            u_sin = 2d0*exp(x+y+z)
        end function

        function u_cos(this, x, y, z)
            implicit none
            class(harm_fem) :: this
            double precision :: x, y, z, u_cos
            u_cos = 3d0*exp(x-y-z)
        end function

        function f_sin(this, x, y, z)
            implicit none
            class(harm_fem) :: this
            double precision :: x, y, z, f_sin
            f_sin = - this%lambda(x,y,z) * 3d0 * this%u_sin(x,y,z) &
                    - this%w()**2 * this%hi(x,y,z) * this%u_sin(x,y,z) &
                    - this%w() * this%sigma(x,y,z) * this%u_cos(x,y,z)
        end function

        function f_cos(this, x, y, z)
            implicit none
            class(harm_fem) :: this
            double precision :: x, y, z, f_cos
            f_cos = - this%lambda(x,y,z) * 3d0 * this%u_cos(x,y,z) &
                    - this%w()**2 * this%hi(x,y,z) * this%u_cos(x,y,z) &
                    + this%w() * this%sigma(x,y,z) * this%u_sin(x,y,z)
        end function

        function u_betta_s(this, x, y, z, face_n)
            implicit none
            class(harm_fem) :: this
            double precision :: x, y, z, u_betta_s
            integer :: face_n
            u_betta_s = 0d0
        end function

        function u_betta_c(this, x, y, z, face_n)
            implicit none
            class(harm_fem) :: this
            double precision :: x, y, z, u_betta_c
            integer :: face_n
            u_betta_c = 0d0
        end function

        function tetta_s(this, x, y, z, face_n)
            implicit none
            class(harm_fem) :: this
            double precision :: x, y, z, tetta_s
            integer :: face_n
            tetta_s = 0d0
        end function

        function tetta_c(this, x, y, z, face_n)
            implicit none
            class(harm_fem) :: this
            double precision :: x, y, z, tetta_c
            integer :: face_n
            tetta_c = 0d0
        end function

        subroutine init_hf(this, file_cords, file_elements, file_faces)
            implicit none
            class(harm_fem) :: this
            character(len=255) :: file_cords, file_elements, file_faces
            integer :: i, j
            open(10, file=file_cords, status='old')
            ! Ввод координат вершин
            read(10,*) this%nodes_n
            allocate(this%nodes(this%nodes_n))
            do i = 1, this%nodes_n
                read(10,*) this%nodes(i)%x, this%nodes(i)%y, this%nodes(i)%z
            end do
            close(10)
            open(10, file=file_elements, status='old')
            ! Ввод элементов
            read(10,*) this%elements_n
            allocate(this%elements(this%elements_n))
            do i = 1, this%elements_n, 1
                do j = 1, 16, 1
                    read(10,*) this%elements(i)%node_n(j)
                end do
            end do
            close(10)
            open(10, file=file_faces, status='old')
            ! Ввод первых краевых
            read(10,*) this%faces_fir_n
            allocate(this%faces_fir(this%faces_fir_n))
            allocate(this%faces_fir_node(this%faces_fir_n))
            do i = 1, this%faces_fir_n, 1
                read(10,*) this%faces_fir_node(i), this%faces_fir(i)
            end do
            ! Ввод вторых краевых
            read(10,*) this%faces_sec_n
            allocate(this%faces_sec(this%faces_sec_n))
            do i = 1, this%faces_fir_n, 1
                do j = 1, 8, 1
                    read(10,*) this%faces_sec(i)%node_n(j)
                end do
            end do
            ! Ввод третьих краевых
            read(10,*) this%faces_thi_n
            allocate(this%faces_sec(this%faces_thi_n))
            do i = 1, this%faces_thi_n, 1
                do j = 1, 8, 1
                    read(10,*) this%faces_thi(i)%node_n(j)
                end do
            end do
            close(10)
        end subroutine

        subroutine out_diff(this, file_name)
            implicit none
            class(harm_fem) :: this
            character*255 :: file_name
            double precision :: diff1 = 0d0 ! Общая погрешность
            double precision :: u_norm = 0d0 ! Общая
            double precision :: diff_s = 0d0, diff_c = 0d0, u_ns = 0d0, u_nc = 0d0
            double precision :: x, y, z, us, uc
            integer :: i
            open(10, file=file_name, status='unknown')
            do i = 1, this%nodes_n / 2, 1
                x = this%nodes(2*i)%x ! координаты точки
                y = this%nodes(2*i)%y ! координаты точки
                z = this%nodes(2*i)%z ! координаты точки
                us = this%u_sin(x,y,z)
                uc = this%u_cos(x,y,z)
                diff_s = diff_s + (us - this%solution(2*i))*(us - this%solution(2*i))
                diff_c = diff_c + (uc - this%solution(2*i+1))*(uc - this%solution(2*i+1))
                u_ns = u_ns + us**2
                u_nc = u_nc + uc**2
            end do
            diff1 = diff_s + diff_c
            u_norm = u_ns + u_nc
            write(10, fmt='( a8e10.3 )'), 'Total:', dsqrt(diff1/u_norm)
            write(10, fmt='( a8e10.3 )'), 'Sin:', dsqrt(diff_s/u_ns)
            write(10, fmt='( a8e10.3 )'), 'Cos:', dsqrt(diff_c/u_nc)
            write(10, fmt='( a8i10 )'), 'Iters:', this%solver_iters
            write(10, fmt='( a8e10.3 )'), 'Time:', this%time
            close(10)
        end subroutine

        subroutine solve_(this)
            implicit none
            class(harm_fem) :: this
            class(timer_) :: t
            ! Начало замера времени
            call t%start_
            call this%solver%init(this%ig, this%jg, this%gu, this%gl, this%di, this%nodes_n)
            call this%solver%set_rp(this%right_part)
            call this%solver%solve(this%solution, this%solver_iters)
            ! Конец замера времени
            this%time = t%stop_()
        end subroutine

        subroutine form_gmass(this)
            implicit none
            class(harm_fem) :: this
            integer :: i
            call this%port_gen%init_g(this%nodes_n)
            ! Вс собираем
            allocate(this%ig(this%nodes_n + 1))
            do i = 1, this%elements_n, 1
                call this%port_gen%add_el_g(this%elements(i))
            end do
            call this%port_gen%gen(this%ig, this%jg, this%slae_el_n) ! получаем портрет
            allocate(this%gl(this%slae_el_n))
            allocate(this%gu(this%slae_el_n))
            allocate(this%di(this%nodes_n))
            allocate(this%right_part(this%nodes_n))
            allocate(this%solution(this%nodes_n))
            ! Обнуление
            this%gl = 0d0
            this%gu = 0d0
            this%di = 0d0
            this%right_part = 0d0
            this%solution = 0d0
            call this%port_gen%destruct_g()
        end subroutine

        subroutine form_matrix(this)
            implicit none
            class(harm_fem) :: this
            double precision :: a_loc(16,16), b_loc(16) ! локальные матрица и вектор правой части
            integer :: cur_row ! текущая строка
            integer :: pos ! Позиция в gu и gl
            integer :: k, i, j, i_s, i_e
            double precision :: val
            call this%form_gmass() ! формируем массивы
            ! Генерация основной СЛАУ
            do k = 1, this%elements_n, 1
                call this%form_loc(a_loc, b_loc, k) ! Получаем локальные матрицы
                do i = 1, 16, 1
                    cur_row = this%elements(k)%node_n(i) ! Определяем элемент строки
                    do j = 1, i - 1, 1
                        if(cur_row .gt. this%elements(k)%node_n(j)) then ! Если элементы содержаться в строке
                            pos = this%find_el_pos(cur_row, this%elements(k)%node_n(j)) ! Находим позицию в gu и gl
                            this%gl(pos) = this%gl(pos) + a_loc(i, j)
                            this%gu(pos) = this%gu(pos) + a_loc(j, i)
                        else
                            pos = this%find_el_pos(this%elements(k)%node_n(j), cur_row) ! Находим позицию в gu и gl
                            this%gl(pos) = this%gl(pos) + a_loc(i, j)
                            this%gu(pos) = this%gu(pos) + a_loc(j, i)
                        end if
                    end do
                    this%di(cur_row) = this%di(cur_row) + a_loc(i, i)
                    this%right_part(cur_row) = this%right_part(cur_row) + b_loc(i)
                end do
            end do
            ! Учёт третьих краевых условий
            do k = 1, this%faces_thi_n, 1
                call this%gen_loc_thi(a_loc, b_loc, k)
                do i = 1, 8, 1
                    cur_row = this%faces_thi(k)%node_n(i)
                    do j = 1, i-1, 1
                        if(cur_row .gt. this%faces_thi(k)%node_n(j)) then ! Если элементы содержаться в строке
                            pos = this%find_el_pos(cur_row, this%faces_thi(k)%node_n(j)) ! Находим позицию в gu и gl
                            this%gl(pos) = this%gl(pos) + a_loc(i, j)
                            this%gu(pos) = this%gu(pos) + a_loc(j, i)
                        else
                            pos = this%find_el_pos(this%faces_thi(k)%node_n(j), cur_row) ! Находим позицию в gu и gl
                            this%gl(pos) = this%gl(pos) + a_loc(i, j)
                            this%gu(pos) = this%gu(pos) + a_loc(j, i)
                        end if
                    end do
                    this%di(cur_row) = this%di(cur_row) + a_loc(i, i)
                    this%right_part(cur_row) = this%right_part(cur_row) + b_loc(i)
                end do
            end do
            ! Учёт вторых краевых условий
            do k = 1, this%faces_sec_n, 1
                call this%gen_loc_sec(b_loc, k)
                do i = 1, 8, 1
                    this%right_part(this%faces_sec(k)%node_n) = &
                        this%right_part(this%faces_sec(k)%node_n) + b_loc(i)
                end do
            end do
            ! Учёт первых краевых условий
            do k = 1, this%faces_fir_n, 1
                cur_row = this%faces_fir_node(k) ! Узел, в котором заданно краевое
                val = this%faces_fir(k) ! Получаем значение
                this%di(cur_row) = 1d0
                this%right_part(cur_row) = val
                ! Обнуляем верхную часть столбца
                i_s = this%ig(cur_row)
                i_e = this%ig(cur_row + 1)
                do i = i_s, i_e - 1, 1
                    this%right_part(this%jg(i)) = &
                        this%right_part(this%jg(i)) - this%gu(i) * val
                    this%gl(i) = 0d0
                    this%gu(i) = 0d0
                end do
                ! обнуляем нижную часть столбца
                do j = cur_row + 1, this%nodes_n, 1
                    i_s = this%ig(j)
                    i_e = this%ig(j + 1)
                    do i = i_s, i_e - 1, 1
                        if(this%jg(i) .eq. cur_row) then ! Проверка - тот ли столбец
                            this%right_part(j) = &
                                this%right_part(j) - this%gl(i) * val
                            this%gl(i) = 0d0
                            this%gu(i) = 0d0
                        end if
                    end do
                end do
            end do
        end subroutine

        subroutine form_loc(this, a_loc, b_loc, el_n)
            implicit none
            class(harm_fem) :: this
            double precision :: a_loc(16,16), b_loc(16)
            integer :: el_n
            ! Матрицы жескости и массы для обычного МКЭ
            double precision :: g(8,8), m(8,8)
            double precision :: hx, hy, hz
            double precision :: g1(2,2)=reshape(source=(/1d0,-1d0,-1d0,1d0/),shape=(/2,2/)) ! Дополнительная матрица
            double precision :: m1(2,2)=reshape(source=(/1d0/3d0,1d0/6d0,1d0/6d0,1d0/3d0/),shape=(/2,2/)) ! Ещё одна дополнительная матрица
            double precision :: lambda_aver = 0 ! Усреднённое лямбда
            double precision :: sigma_aver = 0 ! Усреднённое сигма
            double precision :: hi_aver = 0 ! Усреднённое эпсилон
            double precision :: val_f_sin(8), val_f_cos(8), b_sin(8), b_cos(8)
            double precision :: vec2(16), vals(16), diff(16)
            integer :: i, j
            double precision :: x, y, z
            hx = dabs(this%nodes(this%elements(el_n)%node_n(3))%x - &
                      this%nodes(this%elements(el_n)%node_n(0))%x)
            hy = dabs(this%nodes(this%elements(el_n)%node_n(5))%y - &
                      this%nodes(this%elements(el_n)%node_n(0))%y)
            hz = dabs(this%nodes(this%elements(el_n)%node_n(9))%z - &
                      this%nodes(this%elements(el_n)%node_n(0))%z)
            ! Получаем средние значения
            do i = 1, 8, 1
                x = this%nodes(this%elements(el_n)%node_n(2*i))%x ! координаты точки
                y = this%nodes(this%elements(el_n)%node_n(2*i))%y ! координаты точки
                z = this%nodes(this%elements(el_n)%node_n(2*i))%z ! координаты точки
                lambda_aver = lambda_aver + this%lambda(x, y, z)
                sigma_aver = sigma_aver + this%sigma(x, y, z)
                hi_aver = hi_aver + this%hi(x, y, z)
            end do
            lambda_aver = lambda_aver / 8d0
            sigma_aver = sigma_aver / 8d0
            hi_aver = hi_aver / 8d0
            ! Получение матриц G и M
            do i = 1, 8, 1
                do j = 1, 8, 1
                    g(i, j) = hy * hz * g1(this%mu(i), this%mu(j)) * m1(this%nu(i), this%nu(j)) * m1(this%v(i), this%v(j)) / hx + &
                               hx * hz * m1(this%mu(i), this%mu(j)) * g1(this%nu(i), this%nu(j)) * m1(this%v(i), this%v(j)) / hy
                    g(i, j) = g(i, j) + &
                               hx * hy * m1(this%mu(i), this%mu(j)) * m1(this%nu(i), this%nu(j)) * g1(this%v(i), this%v(j)) / hz
                    g(i, j) = g(i, j) * lambda_aver
            m(i, j) = hx*hy*hz * m1(this%mu(i), this%mu(j)) * m1(this%nu(i), this%nu(j)) * m1(this%v(i), this%v(j))
                end do
            end do
            ! Собираем локальную матрицу, как блочную
            do i = 1, 8, 1
                do j = 1, 8, 1
                    a_loc(2 * i + 1, 2 * j + 1) = g(i, j) - this%w()**2 * hi_aver * m(i, j)
                    a_loc(2 * i, 2 * j) = g(i, j) - this%w()**2 * hi_aver * m(i, j)
                    a_loc(2 * i, 2 * j + 1) = - this%w() * sigma_aver * m(i, j)
                    a_loc(2 * i + 1, 2 * j) = this%w() * sigma_aver * m(i, j)
                end do
            end do
            ! Теперь начинаем собрать локлаьную правую часть
            ! Вычисляем значения
            do i = 1, 8, 1
                x = this%nodes(this%elements(el_n)%node_n(2 * i))%x ! координаты точки
                y = this%nodes(this%elements(el_n)%node_n(2 * i))%y ! координаты точки
                z = this%nodes(this%elements(el_n)%node_n(2 * i))%z ! координаты точки
                val_f_sin(i) = this%f_sin(x, y, z)
                val_f_cos(i) = this%f_cos(x, y, z)
            end do
            ! Вычисляем подвекторы правой части
            do i = 1, 8, 1
                b_sin(i) = 0d0
                b_cos(i) = 0d0
                do j = 1, 8, 1
                    b_sin(i) = b_sin(i) + m(i, j) * val_f_sin(j)
                    b_cos(i) = b_cos(i) + m(i, j) * val_f_cos(j)
                end do
            end do
            ! Соединяем два вектора в один
            do i = 1, 8, 1
                b_loc(2 * i) = b_sin(i)
                b_loc(2 * i + 1) = b_cos(i)
            end do
            do i = 1, 8, 1
                x = this%nodes(this%elements(el_n)%node_n(2 * i))%x ! координаты точки
                y = this%nodes(this%elements(el_n)%node_n(2 * i))%y ! координаты точки
                z = this%nodes(this%elements(el_n)%node_n(2 * i))%z ! координаты точки
                vals(2 * i) = this%u_sin(x, y, z)
                vals(2 * i + 1) = this%u_cos(x, y, z)
            end do
            do i = 1, 16, 1
                vec2(i) = 0
                do j = 1, 16, 1
                    vec2(i) = vec2(i) + a_loc(i, j) * vals(j)
                end do
            end do
            do i = 1, 16, 1
                diff(i) = b_loc(i) - vec2(i)
            end do
        end subroutine

        function mu(this, i)
            implicit none
            class(harm_fem) :: this
            integer :: i, mu
            mu = mod(i, 2)
        end function

        function nu(this, i)
            implicit none
            class(harm_fem) :: this
            integer :: i, nu
            nu = mod(i / 2, 2)
        end function

        function v(this, i)
            implicit none
            class(harm_fem) :: this
            integer :: i, v
            v = mod(i / 4, 2)
        end function

        subroutine gen_loc_thi(this, a_loc, b_loc, face_n)
            implicit none
            class(harm_fem) :: this
            double precision :: a_loc(16,16), b_loc(16)
            integer :: face_n, i, j
            double precision :: hx, hy, loc_betta, x, y, z
            ! Матрица массы для обычного МКЭ
            double precision :: m1(4,4) = reshape(source=(/&
                4d0, 2d0, 2d0, 1d0, &
                2d0, 4d0, 1d0, 2d0, &
                2d0, 1d0, 4d0, 2d0, &
                1d0, 2d0, 2d0, 4d0/),shape=(/4,4/))
            double precision :: b_both(8) ! Вектор значений
            ! Определяем оринтацию грани
            if(this%nodes(this%faces_thi(face_n)%node_n(3))%x .eq. &
               this%nodes(this%faces_thi(face_n)%node_n(1))%x) then ! Если в плоскости yOz
                hx = dabs(this%nodes(this%faces_thi(face_n)%node_n(3))%y - &
                          this%nodes(this%faces_thi(face_n)%node_n(1))%y)
                hy = dabs(this%nodes(this%faces_thi(face_n)%node_n(5))%z - &
                          this%nodes(this%faces_thi(face_n)%node_n(1))%z)
            else
                if(this%nodes(this%faces_thi(face_n)%node_n(3))%y .eq. &
                   this%nodes(this%faces_thi(face_n)%node_n(1))%y) then ! Если в плоскости xOz
                    hx = dabs(this%nodes(this%faces_thi(face_n)%node_n(3))%x - &
                              this%nodes(this%faces_thi(face_n)%node_n(1))%x)
                    hy = dabs(this%nodes(this%faces_thi(face_n)%node_n(5))%z - &
                              this%nodes(this%faces_thi(face_n)%node_n(1))%z)
                else ! Если в плоскости xOy
                    hx = dabs(this%nodes(this%faces_thi(face_n)%node_n(3))%x - &
                              this%nodes(this%faces_thi(face_n)%node_n(1))%x)
                    hy = dabs(this%nodes(this%faces_thi(face_n)%node_n(5))%y - &
                              this%nodes(this%faces_thi(face_n)%node_n(1))%y)
                end if
            end if
            loc_betta = this%betta(this%faces_thi(face_n)%area)
            ! Формируем матрицу массы
            do i = 1, 4, 1
                do j = 1, 4, 1
                    a_loc(2*i, 2*i) = loc_betta * hx * hy * m1(i, i) / 36d0
                    a_loc(2*j+1, 2*j+1) = loc_betta * hx * hy * m1(i, i) / 36d0
                    a_loc(2*i+1, 2*j) = 0d0
                    a_loc(2*i, 2*j+1) = 0d0
                end do
            end do
            ! Формируем правую часть
            do i = 1, 4, 1
                x = this%nodes(this%elements(face_n)%node_n(2 * i))%x ! координата x
                y = this%nodes(this%elements(face_n)%node_n(2 * i))%y ! координата y
                z = this%nodes(this%elements(face_n)%node_n(2 * i))%z ! координата z
                b_both(2*i) = this%u_betta_s(x, y, z, face_n)
                b_both(2*i+1) = this%u_betta_c(x, y, z, face_n)
            end do
            ! Получаем правую часть
            do i = 1, 8, 1
                b_loc(i) = 0d0
                do j = 1, 8, 1
                    b_loc(i) = b_loc(i) + a_loc(i, j) * b_both(j)
                end do
            end do
        end subroutine

        subroutine gen_loc_sec(this, b_loc, face_n)
            implicit none
            class(harm_fem) :: this
            double precision :: b_loc(16), a_loc(8, 8)
            integer :: face_n, i, j
            double precision :: hx, hy, x, y, z
            double precision :: b_both(8) ! Вектор значений
            ! Матрица массы для обычного МКЭ
            double precision :: m1(4,4) = reshape(source=(/&
                4d0, 2d0, 2d0, 1d0, &
                2d0, 4d0, 1d0, 2d0, &
                2d0, 1d0, 4d0, 2d0, &
                1d0, 2d0, 2d0, 4d0/),shape=(/4,4/))
            ! Определяем оринтацию грани
            if(this%nodes(this%faces_thi(face_n)%node_n(3))%x .eq. &
               this%nodes(this%faces_thi(face_n)%node_n(1))%x) then ! Если в плоскости yOz
                hx = dabs(this%nodes(this%faces_thi(face_n)%node_n(3))%y - &
                          this%nodes(this%faces_thi(face_n)%node_n(1))%y)
                hy = dabs(this%nodes(this%faces_thi(face_n)%node_n(5))%z - &
                          this%nodes(this%faces_thi(face_n)%node_n(1))%z)
            else
                if(this%nodes(this%faces_thi(face_n)%node_n(3))%y .eq. &
                   this%nodes(this%faces_thi(face_n)%node_n(1))%y) then ! Если в плоскости xOz
                    hx = dabs(this%nodes(this%faces_thi(face_n)%node_n(3))%x - &
                              this%nodes(this%faces_thi(face_n)%node_n(1))%x)
                    hy = dabs(this%nodes(this%faces_thi(face_n)%node_n(5))%z - &
                              this%nodes(this%faces_thi(face_n)%node_n(1))%z)
                else ! Если в плоскости xOy
                    hx = dabs(this%nodes(this%faces_thi(face_n)%node_n(3))%x - &
                              this%nodes(this%faces_thi(face_n)%node_n(1))%x)
                    hy = dabs(this%nodes(this%faces_thi(face_n)%node_n(5))%y - &
                              this%nodes(this%faces_thi(face_n)%node_n(1))%y)
                end if
            end if
            ! Формируем матрицу массы
            do i = 1, 4, 1
                do j = 1, 4, 1
                    a_loc(2*i, 2*i) = hx * hy * m1(i, i) / 36d0
                    a_loc(2*j+1, 2*j+1) = hx * hy * m1(i, i) / 36d0
                    a_loc(2*i+1, 2*j) = 0d0
                    a_loc(2*i, 2*j+1) = 0d0
                end do
            end do
            ! Формируем правую часть
            do i = 1, 4, 1
                x = this%nodes(this%elements(face_n)%node_n(2 * i))%x ! координата x
                y = this%nodes(this%elements(face_n)%node_n(2 * i))%y ! координата y
                z = this%nodes(this%elements(face_n)%node_n(2 * i))%z ! координата z
                b_both(2*i) = this%tetta_s(x, y, z, face_n)
                b_both(2*i+1) = this%tetta_c(x, y, z, face_n)
            end do
            ! Получаем правую часть
            do i = 1, 8, 1
                b_loc(i) = 0d0
                do j = 1, 8, 1
                    b_loc(i) = b_loc(i) + a_loc(i, j) * b_both(j)
                end do
            end do
        end subroutine

        function find_el_pos(this, i, j)
            implicit none
            class(harm_fem) :: this
            integer :: i, j, find_el_pos, k_s, k_e, k
            logical :: find = .false.
            k_s = this%ig(i)
            k_e = this%ig(i+1)
            k = k_s
            do while(k .lt. k_e .and. .not. find)
                if(this%jg(k) .eq. j) then
                    find_el_pos = k
                    find = .true.
                end if
                k = k + 1
            end do
        end function

        subroutine out_rez(this, file_name)
            implicit none
            class(harm_fem) :: this
            character*255 :: file_name
            integer :: i
            open(10, file=file_name, status='unknown')
            do i = 1, this%nodes_n, 1
                write(10, fmt='( i10e27.16 )'), i, this%solution(i)
            end do
            close(10)
        end subroutine

        subroutine transf_grid(this, file_cords, file_elements, file_faces)
            implicit none
            class(harm_fem) :: this
            character*255 :: file_cords, file_elements, file_faces
            integer :: n, i, j, k
            double precision :: x, y, z
            ! Преобразование файла координат
            open(10, file=file_cords, status='old') ! Файл исходной сетки
            open(20, file=file_cords(1:index(file_cords, ' ') - 1) // 'tr', status='unknown') ! Файл модифицированной сетки
            read(10, *) n
            write(20, fmt='( i10 )') 2 * n
            do i = 1, n, 1 ! считываем координаты
                read(10, *) x, y, z
                write(20, fmt='( 3e27.16 )') x, y, z
                write(20, fmt='( 3e27.16 )') x, y, z
            end do
            close(10)
            close(20)
            ! Преобразование файла элементов
            open(10, file=file_elements, status='old') ! Файл исходной сетки
            open(20, file=file_elements(1:index(file_elements, ' ') - 1) // 'tr', status='unknown') ! Файл модифицированной сетки
            read(10, *) n
            write(20, fmt='( i10 )') n
            do i = 1, n, 1
                do j = 1, 8, 1
                    read(10, *) k
                    write(20, fmt='( i10/i10 )') 2*k, 2*k+1
                end do
            end do
            close(10)
            close(20)
            ! Преобразование файла граней
            open(10, file=file_faces, status='old') ! Файл исходной сетки
            open(20, file=file_faces(1:index(file_faces, ' ') - 1) // 'tr', status='unknown') ! Файл модифицированной сетки
            read(10, *) n
            write(20, fmt='( i10 )') n
            do i = 1, n, 1
                do j = 1, 4, 1
                    read(10, *) k
                    write(20, fmt='( i10/i10 )') 2*k, 2*k+1
                end do
            end do
            close(10)
            close(20)
        end subroutine

    end module
