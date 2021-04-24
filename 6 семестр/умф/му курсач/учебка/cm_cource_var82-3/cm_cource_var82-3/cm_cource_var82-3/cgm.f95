#if __GFORTRAN__ == 1 && __GNUC__ == 4 && __GNUC_MINOR__ < 5
#define class type
#endif
    module cgm_module
        implicit none
        integer,parameter :: maxiter=1000 ! максимальное количество итераций
        double precision,parameter :: eps=1d-7 ! величина эпсилон

        type :: slae ! слау
            integer :: n
            integer,allocatable :: ig(:),jg(:)
            double precision,allocatable :: di(:),gg(:),f(:),x(:)
        contains
            procedure :: calc_cgm ! решение СЛАУ методом МСГ
            procedure :: alloc_all ! выделение памяти под СЛАУ
            procedure :: dealloc_all ! освобождение памяти
            procedure,private :: do_holessky_decompose ! выполнение разложения Холесского
            procedure,private :: do_solve_slae ! решение СЛАУ для матрицы с разложением Холесского
            procedure,private :: mult_mtrx_vctr ! умножение матрицы на вектор
            procedure,private :: mult_scalar ! вычисление скалярного произведения
            procedure,private :: residual ! вычисление невязки
            procedure,private :: norm_2 ! вычисление квадрата нормы вектора
        end type

    contains

        subroutine alloc_all(this,gg_size) ! выделение памяти под СЛАУ
            implicit none
            class(slae) :: this
            integer :: gg_size
            allocate(this%ig(this%n+1))
            allocate(this%jg(gg_size))
            allocate(this%di(this%n))
            allocate(this%gg(gg_size))
            allocate(this%f(this%n))
            allocate(this%x(this%n))
        end subroutine

        subroutine dealloc_all(this) ! освобождение памяти
            implicit none
            class(slae) :: this
            deallocate(this%x)
            deallocate(this%f)
            deallocate(this%gg)
            deallocate(this%di)
            deallocate(this%jg)
            deallocate(this%ig)
        end subroutine

        subroutine calc_cgm(this) ! решение СЛАУ методом МСГ
            implicit none
            class(slae) :: this
            integer :: iter,i
            double precision :: rrk,rrk1,alfa,beta
            double precision,allocatable :: r(:),z(:),y(:),m_di(:),m_au(:)
            allocate(r(this%n))
            allocate(z(this%n))
            allocate(y(this%n))
            allocate(m_di(this%n))
            allocate(m_au(this%ig(this%n+1)-1))
            do i=1,this%n
                this%x(i)=0d0
            end do
            call this%do_holessky_decompose(m_di,m_au)
            call this%mult_mtrx_vctr(this%x,r)
            do i=1,this%n
                r(i)=this%f(i)-r(i)
            end do
            call this%do_solve_slae(m_di,m_au,r,z)
            rrk=this%mult_scalar(z,r)
            do iter=1,maxiter
                if(mod(iter,10).eq.0)then
                    call this%mult_mtrx_vctr(this%x,r)
                    r=this%f-r
                end if
                !if(mod(iter,100).eq.0) print*,iter,this%residual(r)
                if(this%residual(r).le.eps)then
                    call this%mult_mtrx_vctr(this%x,r)
                    r=this%f-r
                    if(dsqrt(this%norm_2(r)).gt.eps)goto 100
                    !print*,'Solution found! Congratulations!'
                    !print*,iter,dsqrt(this%norm_2(r))
                    goto 200
                end if
100             continue
                call this%mult_mtrx_vctr(z,y)
                rrk1=rrk
                alfa=rrk1/this%mult_scalar(y,z)
                this%x=this%x+alfa*z
                r=r-alfa*y
                call this%do_solve_slae(m_di,m_au,r,y)
                rrk=this%mult_scalar(y,r)
                beta=rrk/rrk1
                z=y+beta*z
            end do
            !print*,'Solution can`t found! Breaking...'
            !print*,iter,dsqrt(this%norm_2(r))
200         continue
            deallocate(r)
            deallocate(z)
            deallocate(y)
            deallocate(m_di)
            deallocate(m_au)
        end subroutine

        subroutine do_holessky_decompose(this,m_di,m_au) ! выполнение разложения Холесского
            implicit none
            class(slae) :: this
            double precision :: m_di(*),m_au(*),sum_,sum_2
            integer :: i,num,index1,index2,index_k,index_l,max_k,max_l
            do i=1,this%n
                num=this%ig(i+1)-this%ig(i)
                sum_2=0d0
                do index1=this%ig(i),this%ig(i+1)-1
                    sum_=0d0
                    index2=this%jg(index1)
                    index_k=this%ig(i)
                    index_l=this%ig(index2)
                    max_k=index1-1
                    max_l=this%ig(index2+1)-1
                    do while(index_k.le.max_k.and.index_l.le.max_l)
                        if(this%jg(index_k).eq.this%jg(index_l))then
                            sum_=sum_+m_au(index_k)*m_au(index_l)
                            index_k=index_k+1
                            index_l=index_l+1
                        end if
                        if(this%jg(index_k).gt.this%jg(index_l)) index_l=index_l+1
                        if(this%jg(index_k).lt.this%jg(index_l)) index_k=index_k+1
                    end do
                    m_au(index1)=(this%gg(index1)-sum_)/m_di(index2)
                    sum_2=sum_2+m_au(index1)**2
                end do
                m_di(i)=dsqrt(this%di(i)-sum_2)
            end do
        end subroutine

        subroutine do_solve_slae(this,di,au,f,x) ! решение СЛАУ для матрицы с разложением Холесского
            implicit none
            class(slae) :: this
            double precision :: di(*),au(*),f(*),x(*),sum_
            integer :: i,j,index1
            do i=1,this%n
                sum_=0d0
                index1=this%ig(i+1)-1
                do j=this%ig(i),index1
                    sum_=sum_+au(j)*x(this%jg(j))
                end do
                x(i)=(f(i)-sum_)/di(i)
            end do
            do i=this%n,1,-1
                x(i)=x(i)/di(i)
                index1=this%ig(i+1)-1
                do j=this%ig(i),index1
                    x(this%jg(j))=x(this%jg(j))-au(j)*x(i)
                end do
            end do
        end subroutine

        subroutine mult_mtrx_vctr(this,vctr,rslt) ! умножение матрицы на вектор
            implicit none
            class(slae) :: this
            integer :: num,i,j
            double precision :: vctr(*),rslt(*)
            do i=1,this%n
                rslt(i)=this%di(i)*vctr(i)
            end do
            do i=2,this%n
                num=this%ig(i+1)-this%ig(i)
                do j=1,num
                    rslt(i)=rslt(i)+this%gg(this%ig(i)+j-1)*vctr(this%jg(this%ig(i)+j-1))
                    rslt(this%jg(this%ig(i)+j-1))=rslt(this%jg(this%ig(i)+j-1))+this%gg(this%ig(i)+j-1)*vctr(i)
                end do
            end do
        end subroutine

        function mult_scalar(this,x,y) ! вычисление скалярного произведения (x,y)
            implicit none
            class(slae) :: this
            double precision :: x(*),y(*),mult_scalar
            integer :: i
            mult_scalar=0d0
            do i=1,this%n
                mult_scalar=mult_scalar+x(i)*y(i)
            end do
        end function

        function residual(this,r) ! вычисление невязки
            implicit none
            class(slae) :: this
            double precision :: residual,r(*)
            residual=dsqrt(this%norm_2(r)/this%norm_2(this%f))
        end function

        function norm_2(this,x) ! вычисление квадрата нормы вектора x
            implicit none
            class(slae) :: this
            integer :: i
            double precision :: x(*),norm_2
            norm_2=0d0
            do i=1,this%n
                norm_2=norm_2+x(i)**2
            end do
        end function
    end module
