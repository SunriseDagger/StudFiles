    module slae_gauss_module
        implicit none

        type slae
            double precision,allocatable :: a(:,:),x(:),f(:)
            integer :: n
        contains
            procedure :: calc_gauss
            procedure :: alloc_all_gauss
            procedure :: dealloc_all_gauss
        end type

    contains

        subroutine alloc_all_gauss(this)
            implicit none
            type(slae) :: this
            allocate(this%a(this%n,this%n))
            allocate(this%f(this%n))
            allocate(this%x(this%n))
        end subroutine

        subroutine dealloc_all_gauss(this)
            implicit none
            type(slae) :: this
            deallocate(this%a)
            deallocate(this%f)
            deallocate(this%x)
        end subroutine

        subroutine calc_gauss(this,status_)
            implicit none
            type(slae) :: this
            integer :: i,j,k,i_max
            integer(1) :: status_
            double precision :: max_,s
            status_=0
            do i=1,this%n
                this%x(i)=0d0
            end do
            do k=1,this%n-1
                max_=0d0
                do i=k,this%n
                    if(max_.lt.dabs(this%a(i,k)))then
                        i_max=i
                        max_=dabs(this%a(i,k))
                    end if
                end do
                do j=1,this%n
                    call r_swap(this%a(k,j),this%a(i_max,j))
                end do
                call r_swap(this%f(k),this%f(i_max))
                s=this%a(k,k)
                do j=k,this%n
                    this%a(k,j)=this%a(k,j)/s
                end do
                this%f(k)=this%f(k)/s
                this%a(k,k)=1d0
                do i=k+1,this%n
                    s=this%a(i,k)
                    do j=k,this%n
                        this%a(i,j)=this%a(i,j)-s*this%a(k,j)
                    end do
                    this%f(i)=this%f(i)-s*this%f(k)
                end do
            end do
            s=this%a(this%n,this%n)
            do j=this%n,this%n
                this%a(this%n,j)=this%a(this%n,j)/s
            end do
            this%x(this%n)=this%f(this%n)/s
            if(this%x(this%n).ne.this%x(this%n).or.dabs(this%x(this%n)).gt.1.7d308)then
                status_=1
                return
            end if
            do i=this%n-1,1,-1
                s=0d0
                do j=i+1,this%n
                    s=s+this%a(i,j)*this%x(j)
                end do
                this%x(i)=this%f(i)-s
            end do
        end subroutine

        subroutine r_swap(a,b)
            implicit none
            double precision :: a,b,c
            c=a
            a=b
            b=c
        end subroutine

    end module

    program tmpgauss
        use slae_gauss_module
        implicit none
        type(slae) :: a
        integer :: i,j
        integer(1) :: stat_
        a%n=21
        call a%alloc_all_gauss()
        open(10,file='../tmpmatr.txt',status='old')
        do i=1,a%n
            read(10,*) (a%a(i,j), j=1,a%n),a%f(i)
        end do
        call a%calc_gauss(stat_)
        do i=1,a%n
            print*,a%x(i)
        end do
        call a%dealloc_all_gauss()
    end program
