#if __GFORTRAN__ == 1 && __GNUC__ == 4 && __GNUC_MINOR__ < 5
#define class type
#endif
    ! Модуль, реализующий двусвязный список элементов типа "integer".
    module list_module
        implicit none
        ! Константа, задающая значение элемента списка по умолчанию
        integer,parameter,private :: def_elem_value=0

        ! Тип данных - элемент списка
        type,private :: list_elem
            integer :: elem=def_elem_value              ! Значение элемента списка
            type(list_elem),pointer :: next_=>null()    ! Указатель на следующий элемент списка
            type(list_elem),pointer :: prev_=>null()    ! Указатель на предыдущий элемент списка
        end type

        ! Тип данных - список
        type :: list
            type(list_elem),pointer,private :: head_=>null()    ! Указатель на начало списка
            type(list_elem),pointer,private :: end_=>null()     ! Указатель на конец списка
            integer,private :: size_=0                          ! Число элементов в списке
        contains
            procedure :: add            ! Процедура добавления элемента в список
            procedure :: del            ! Процедура удаления элемента(ов) из списка
            procedure :: get            ! Функция, возвращающая значение элемента списка
            procedure :: sort           ! Процедура сортировки списка по возрастанию
            procedure :: list_size      ! Функция, возвращающая число всех элементов в списке
            procedure :: elem_count     ! Функция, возвращающая число заданных элементов в списке
            procedure :: debug_print    ! Процедура, выводящая на экран содержимое списка
        end type

    contains

        ! Процедура добавления элемента в список.
        ! elem - значение добавляемого элемента
        ! index_ - номер (индекс), после которого будет вставлен ноый элемент (необязательный
        ! параметр). В случае неуказания параметра index_ или указания в нем некорректного
        ! значения, добавление производится в конец списка.
        subroutine add(this,elem,index_)
            implicit none
            class(list) :: this
            integer :: elem,i
            integer,optional :: index_
            type(list_elem),pointer :: tmpptr,curr
            allocate(tmpptr)
            tmpptr%elem=elem
            if(present(index_).and.index_.ge.0.and.index_.lt.this%size_)then
                if(index_.eq.0) then
                    nullify(tmpptr%prev_)
                    if(associated(this%head_))then
                        tmpptr%next_=>this%head_
                        this%head_%prev_=>tmpptr
                        this%head_=>tmpptr
                    else
                        nullify(tmpptr%next_)
                        this%head_=>tmpptr
                        this%end_=>tmpptr
                    end if
                else
                    if(index_.lt.this%size_/2)then
                        i=1
                        curr=>this%head_
                        do while(i.lt.index_)
                            curr=>curr%next_
                            i=i+1
                        end do
                    else
                        i=this%size_
                        curr=>this%end_
                        do while(i.gt.index_)
                            curr=>curr%prev_
                            i=i-1
                        end do
                    end if
                    tmpptr%prev_=>curr
                    if(associated(curr%next_))then
                        tmpptr%next_=>curr%next_
                        curr%next_%prev_=>tmpptr
                    else
                        nullify(tmpptr%next_)
                    end if
                    curr%next_=>tmpptr
                end if
            else
                nullify(tmpptr%next_)
                if(associated(this%end_)) then
                    tmpptr%prev_=>this%end_
                    this%end_%next_=>tmpptr
                else
                    nullify(tmpptr%prev_)
                    this%end_=>tmpptr
                    this%head_=>tmpptr
                end if
                this%end_=>tmpptr
            end if
            this%size_=this%size_+1
        end subroutine

        ! Функция, возвращающая число всех элементов в списке.
        function list_size(this)
            implicit none
            class(list) :: this
            integer :: list_size
            list_size=this%size_
        end function

        ! Процедура, выводящая на экран содержимое списка.
        subroutine debug_print(this)
            implicit none
            class(list) :: this
            type(list_elem),pointer :: tmpptr
            tmpptr=>this%head_
            do while(associated(tmpptr))
                if(associated(tmpptr%next_))then
                    write(*, '( i5" <=>"$ )') , tmpptr%elem
                else
                    write(*,fmt='(i5)') tmpptr%elem
                end if
                tmpptr=>tmpptr%next_
            end do
        end subroutine

        ! Процедура удаления элемента(ов) из списка.
        ! index_ - номер (индекс) элемента, который будет удален (необязательный параметр).
        ! В случае некорректного задания значения index_ удаление не производится.
        ! В случае неуказания параметра index_ из списка будут удалены все элементы.
        subroutine del(this,index_)
            implicit none
            class(list) :: this
            type(list_elem),pointer :: curr
            integer,optional :: index_
            integer :: i
            if(present(index_))then
                if(index_.eq.0) then
                    this%head_=>this%head_%next_
                    deallocate(this%head_%prev_)
                    nullify(this%head_%prev_)
                    this%size_=this%size_-1
                else if(index_.eq.this%size_)then
                    this%end_=>this%end_%prev_
                    deallocate(this%end_%next_)
                    nullify(this%end_%next_)
                    this%size_=this%size_-1
                else if(index_.gt.0.and.index_.lt.this%size_)then
                    if(index_.lt.this%size_/2)then
                        i=1
                        curr=>this%head_
                        do while(i.lt.index_)
                            curr=>curr%next_
                            i=i+1
                        end do
                    else
                        i=this%size_
                        curr=>this%end_
                        do while(i.gt.index_)
                            curr=>curr%prev_
                            i=i-1
                        end do
                    end if
                    curr%prev_%next_=>curr%next_
                    curr%next_%prev_=>curr%prev_
                    deallocate(curr)
                    this%size_=this%size_-1
                end if
            else
                curr=>this%head_
                do while(associated(curr%next_))
                    curr=>curr%next_
                    deallocate(curr%prev_)
                end do
                deallocate(curr)
                nullify(this%head_)
                nullify(this%end_)
                this%size_=0
            end if
        end subroutine

        ! Функция, возвращающая число заданных элементов в списке.
        ! elem - искомый элемент.
        function elem_count(this,elem)
            implicit none
            class(list) :: this
            type(list_elem),pointer :: curr
            integer :: elem,elem_count
            elem_count=0
            curr=>this%head_
            do while(associated(curr))
                if(curr%elem.eq.elem) elem_count=elem_count+1
                curr=>curr%next_
            end do
        end function

        ! Функция, возвращающая значение элемента списка.
        ! index_ - номер (индекс) искомого элемента.
        function get(this,index_)
            implicit none
            class(list) :: this
            type(list_elem),pointer :: curr
            integer :: index_,get,i
            get=def_elem_value
            if(index_.gt.0.and.index_.lt.this%size_)then
                if(index_.lt.this%size_/2)then
                    i=1
                    curr=>this%head_
                    do while(i.lt.index_)
                        curr=>curr%next_
                        i=i+1
                    end do
                else
                    i=this%size_
                    curr=>this%end_
                    do while(i.gt.index_)
                        curr=>curr%prev_
                        i=i-1
                    end do
                end if
                get=curr%elem
            end if
        end function

        ! Процедура сортировки списка по возрастанию.
        ! Сортировка производится алгоритмом "Сортировки вставками" ("Insertion Sort").
        subroutine sort(this)
            class(list) :: this
            type(list_elem),pointer :: curr,begin,tmpptr
            logical :: flag
            if(this%size_.gt.1)then
                begin=>this%head_%next_
                do while(associated(begin))
                    curr=>begin
                    flag=.false.
                    do while(associated(curr%prev_).and.begin%elem.lt.curr%prev_%elem)
                        curr=>curr%prev_
                        flag=.true.
                    end do
                    tmpptr=>begin
                    if(.not.associated(begin%next_)) this%end_=>begin
                    begin=>begin%next_
                    if(flag)then
                        if(associated(tmpptr%next_)) tmpptr%next_%prev_=>tmpptr%prev_
                        tmpptr%prev_%next_=>tmpptr%next_
                        if(associated(curr%prev_)) then
                            curr%prev_%next_=>tmpptr
                            tmpptr%prev_=>curr%prev_
                        else
                            nullify(tmpptr%prev_)
                            this%head_=>tmpptr
                        end if
                        tmpptr%next_=>curr
                        curr%prev_=>tmpptr
                    end if
                end do
            end if
        end subroutine

    end module
