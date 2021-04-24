#if __GFORTRAN__ == 1 && __GNUC__ == 4 && __GNUC_MINOR__ < 5
#define class type
#endif
    module timer_module
        implicit none

        type :: timer_
            double precision, private :: t_start, t_stop
            logical, private :: is_started = .false.
        contains
            procedure :: start_
            procedure :: stop_
            procedure, private :: gettime_
        end type

    contains

        function gettime_(this)
            implicit none
            class(timer_) :: this
            integer*4 :: ival(8)
            double precision :: gettime_
            call date_and_time(values=ival)
            gettime_ = dble(ival(8)) * 0.001d0 + &
                       dble(ival(7)) + &
                       dble(ival(6)) * 60.0d0 + &
                       dble(ival(5)) * 3600.0d0
            gettime_ = gettime_*1.0d3
        end function

        subroutine start_(this)
            implicit none
            class(timer_) :: this
            this%t_start = gettime_(this)
            this%is_started = .true.
        end subroutine

        function stop_(this)
            implicit none
            class(timer_) :: this
            double precision :: stop_
            if(this%is_started) then
                this%t_stop = gettime_(this)
                stop_ = this%t_start - this%t_stop
                this%is_started = .false.
            else
                stop_ = 0d0
            end if
        end function

    end module
