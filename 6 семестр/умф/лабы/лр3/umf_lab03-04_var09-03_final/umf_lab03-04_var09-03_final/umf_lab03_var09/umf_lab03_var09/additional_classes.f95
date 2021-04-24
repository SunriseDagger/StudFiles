#if __GFORTRAN__ == 1 && __GNUC__ == 4 && __GNUC_MINOR__ < 5
#define class type
#endif
    module addition_classes
        implicit none

        ! Узел
        type :: node
            double precision :: x, y, z
            integer :: number
        end type

        ! Двойной куб
        type :: fe
            integer :: node_n(16)
            integer :: number
            integer :: area
        end type

        ! Грань
        type :: face
            integer :: node_n(8)
            integer :: number
            integer :: area
        end type

        ! элемент списка для геренации портрета СЛАУ
        type :: gen_l_el
            integer :: value
            type(gen_l_el), pointer :: next=>null()
        end type

        type :: slae_port_list
            type(gen_l_el), pointer, private :: begin => null()
            type(gen_l_el), pointer, private :: end => null()
            type(gen_l_el), pointer, private :: cash => null()
            integer, private :: l_size = 0, m_size, number_of_line
        contains
            !procedure :: construct_l
            procedure :: destruct_l
            procedure :: add_el_l ! Добавление элемента
            procedure :: init_l
            procedure :: set_num ! Установить номер линии
            !procedure :: size
            procedure :: size_before
            procedure :: take_and_next
            procedure :: cash_off
            procedure :: get_m_size
            procedure, private :: exclude_last_el
            procedure, private :: add_l
        end type

        type :: slae_port_gen
            integer, private :: n
            type(slae_port_list), allocatable :: lists(:)
        contains
            !procedure :: construct_g
            procedure :: destruct_g
            procedure :: init_g
            procedure :: add_el_g
            procedure :: gen
        end type

    contains

        !subroutine construct_l(this)
        !    implicit none
        !    class(slae_port_list) :: this
        !    call this%init_l()
        !end subroutine

        subroutine init_l(this)
            implicit none
            class(slae_port_list) :: this
            nullify(this%begin)
            nullify(this%end)
            nullify(this%cash)
            this%l_size = 0
        end subroutine

        subroutine set_num(this, s_num)
            implicit none
            class(slae_port_list) :: this
            integer :: s_num
            this%number_of_line = s_num
        end subroutine

        subroutine destruct_l(this)
            implicit none
            class(slae_port_list) :: this
            nullify(this%cash)
            do while(associated(this%begin))
                call this%exclude_last_el()
            end do
        end subroutine

        subroutine add_el_l(this, el_a)
            implicit none
            class(slae_port_list) :: this
            class(fe) :: el_a
            integer :: i
            do i = 1, 16
                call this%add_l(el_a%node_n(i))
            end do
        end subroutine

        subroutine cash_off(this)
            implicit none
            class(slae_port_list) :: this
            this%cash = this%begin
        end subroutine

        function take_and_next(this)
            implicit none
            class(slae_port_list) :: this
            integer :: take_and_next
            take_and_next = this%cash%value
            this%cash => this%cash%next
        end function

        function get_m_size(this)
            implicit none
            class(slae_port_list) :: this
            integer :: get_m_size
            get_m_size = this%m_size
        end function

        subroutine add_l(this, val)
            implicit none
            class(slae_port_list) :: this
            integer :: val
            class(gen_l_el), pointer :: add_el
            if(val .le. this%number_of_line) then
                allocate(add_el)
                add_el%value = val
                if(.not.associated(this%begin)) then
                    this%begin => add_el
                    nullify(this%begin%next)
                    this%end => this%begin
                    this%cash => this%begin
                else
                    if(val .lt. this%begin%value) then
                        add_el%next => this%begin
                        this%begin => add_el
                        this%cash => this%begin
                    else
                        if(val .gt. this%end%value) then
                            nullify(add_el%next)
                            this%end%next => add_el
                            this%end => this%end%next
                        else
                            this%cash => this%begin
                            do while(associated(this%cash%next) .and. val .gt. this%cash%next%value)
                                this%cash => this%cash%next
                            end do
                            if(associated(this%cash%next) .and. this%cash%next%value .ne. val .and. this%cash%value .ne. val) then
                                add_el%next => this%cash%next
                                this%cash%next => add_el
                            end if
                        end if
                    end if
                end if
                this%l_size = this%l_size +1
                nullify(add_el)
            end if
        end subroutine

        subroutine exclude_last_el(this)
            implicit none
            class(slae_port_list) :: this
            if(associated(this%begin, this%end)) then
                this%cash => this%begin
                do while (.not.associated(this%cash%next, this%end))
                    this%cash => this%cash%next
                end do
                deallocate(this%cash%next)
                nullify(this%cash%next)
                this%end => this%cash
            else
                deallocate(this%begin)
                nullify(this%begin)
                nullify(this%end)
                nullify(this%cash)
            end if
        end subroutine

        function size_before(this, n)
            implicit none
            class(slae_port_list) :: this
            integer :: size_before, n, tmp_s = 0
            this%cash => this%begin
            if(associated(this%begin)) then
                if(this%begin%value .lt. n) tmp_s = tmp_s + 1
                do while (associated(this%cash%next) .and. this%cash%next%value .lt. n)
                    tmp_s = tmp_s + 1
                    this%cash => this%cash%next
                end do
            end if
            this%m_size = tmp_s
            size_before = tmp_s
        end function

        subroutine destruct_g(this)
            implicit none
            class(slae_port_gen), INTENT(inout) :: this
            deallocate(this%lists)
        end subroutine

        subroutine init_g(this, nodes_n)
            implicit none
            class(slae_port_gen) :: this
            integer :: nodes_n, i
            this%n = nodes_n
            allocate(this%lists(nodes_n))
            do i = 1, nodes_n, 1
                call this%lists(i)%set_num(i)
            end do
        end subroutine

        subroutine add_el_g(this, el_a)
            implicit none
            class(slae_port_gen) :: this
            class(fe) :: el_a
            integer :: i
            do i = 1, 16, 1
                call this%lists(el_a%node_n(i))%add_el_l(el_a)
            end do
        end subroutine

        subroutine gen(this, gi, gj, m)
            implicit none
            class(slae_port_gen) :: this
            integer :: gi(this%n + 1), m, i, j, shift, m1 = 0, iters_m
            integer, pointer :: gj(:)
            m = 1
            gi(1) = 1;
            do i = 1, this%n, 1
                gi(i) = m
                m = m + this%lists(i)%size_before(i)
            end do
            gi(this%n + 1) = m
            allocate(gj(m))
            do i = 1, this%n, 1
                shift = m1
                iters_m = this%lists(i)%get_m_size()
                call this%lists(i)%cash_off()
                do j = 1, iters_m, 1
                    gj(shift + j) = this%lists(i)%take_and_next()
                end do
                m1 = m1 + iters_m
            end do

        end subroutine

    end module
