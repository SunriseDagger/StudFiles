    ! Finite difference method
    module module_fdm
        implicit none

        type :: area_g
            double precision,allocatable :: mesh_x(:),mesh_y(:)
            integer :: num_x,num_y,num_g_x,num_g_y,num_all
            double precision :: lambda_,gamma_,bound_corner(5)
        contains
            procedure :: input_g
        end type

        type :: slae
            double precision,allocatable :: di(:),du1(:),du2(:),dl1(:),dl2(:),df(:),x(:)
            integer :: shift1,shift2,dshift,n,maxiter=10000
            double precision :: eps=1d-15,def_x=1d0,omega=1.0d0
        contains
            procedure :: calc_gauss_seidel
            procedure,private :: norm
        end type

        type :: fdm
            type(area_g) :: area_g
            type(slae) :: slae
        contains
            procedure :: f
            procedure :: gettypeofb
            procedure :: getvalueofb
            procedure :: getslae
            procedure :: dealloc
            procedure :: write_
        end type

    contains

        function f(this,x,y)
            implicit none
            type(fdm) :: this
            double precision :: f,x,y
            !# Test 1,2,3
            !f=2d0*x+2d0*y
            !# Test 4
            !f=2d0*x**2+2d0*y**2-4d0
            !# Test 5
            !f=2d0*x**3+2d0*y**3-6d0*x-6d0*y
            !# Test 6
            !f=2d0*x**4+2d0*y**4-12d0*x**2-12d0*y**2
            !# Test 7
            f=9d0*cos(2d0*x+2d0*y)
        end function

        ! ----------
        ! |   6    |
        ! |        | 5
        ! | 2   ----
        ! |     |   4
        ! |  1  | 3
        ! -------
        function gettypeofb(this,numb)
            implicit none
            type(fdm) :: this
            integer :: numb,gettypeofb
            !# Test 1,3,4,5,6,7
            if(numb.eq.1.or.numb.eq.2.or.numb.eq.3.or.numb.eq.4.or.numb.eq.5.or.numb.eq.6) then
                gettypeofb=1
            end if
            !# Test 2
            !if(numb.eq.1.or.numb.eq.3.or.numb.eq.5.or.numb.eq.6) then
            !    gettypeofb=1
            !else
            !    gettypeofb=2
            !end if
        end function

        function getvalueofb(this,numb,x,y)
            implicit none
            type(fdm) :: this
            integer :: numb
            double precision :: getvalueofb,x,y
            !# Test 1,3
            !getvalueofb=x+y
            !# Test 2
            !if(numb.eq.1.or.numb.eq.3.or.numb.eq.5.or.numb.eq.6) then
            !    getvalueofb=x+y
            !else if(numb.eq.2) then
            !    getvalueofb=1d0
            !else
            !    getvalueofb=1d0
            !end if
            !# Test 4
            !if(numb.eq.1.or.numb.eq.3.or.numb.eq.5.or.numb.eq.6) then
            !    getvalueofb=x**2+y**2
            !else if(numb.eq.2) then
            !    getvalueofb=2d0*x
            !else
            !    getvalueofb=2d0*y
            !end if
            !# Test 4
            !getvalueofb=x**2+y**2
            !# Test 5
            !getvalueofb=x**3+y**3
            !# Test 6
            !getvalueofb=x**4+y**4
            !# Test 7
            getvalueofb=cos(2d0*x+2d0*y)
        end function

        subroutine input_g(this)
            implicit none
            type(area_g) :: this
            integer :: i
            open(10,file='../area.txt',status='old')
            read(10,*) this%num_x,this%num_y
            allocate(this%mesh_x(this%num_x))
            allocate(this%mesh_y(this%num_y))
            read(10,*) (this%mesh_x(i), i=1,this%num_x)
            read(10,*) (this%mesh_y(i), i=1,this%num_y)
            read(10,*) this%num_g_x,this%num_g_y
            read(10,*) this%lambda_,this%gamma_
            close(10)
            this%num_all=this%num_g_x*(this%num_g_y-1)+(this%num_y-this%num_g_y+1)*this%num_x
            open(20,file='../bcorner.txt',status='old')
            read(20,*) (this%bound_corner(i), i=1,5)
            close(20)
        end subroutine

        subroutine getslae(this)
            implicit none
            type(fdm) :: this
            integer :: i,j,t
            double precision :: hx1,hx2,hy1,hy2
            this%slae%n=this%area_g%num_all
            allocate(this%slae%di(this%slae%n))
            allocate(this%slae%du1(this%slae%n))
            allocate(this%slae%du2(this%slae%n))
            allocate(this%slae%dl1(this%slae%n))
            allocate(this%slae%dl2(this%slae%n))
            allocate(this%slae%df(this%slae%n))
            this%slae%di=0d0
            this%slae%du1=0d0
            this%slae%du2=0d0
            this%slae%dl1=0d0
            this%slae%dl2=0d0
            this%slae%df=0d0
            this%slae%shift1=this%area_g%num_g_x
            this%slae%shift2=this%area_g%num_x

            ! нижняя плоскость "Г"
            this%slae%di(1)=1d0
            this%slae%df(1)=this%area_g%bound_corner(1)
            hy1=dabs(this%area_g%mesh_y(1)-this%area_g%mesh_y(2))
            do i=2,this%area_g%num_g_x-1
                this%slae%df(i)=this%getvalueofb(1,this%area_g%mesh_x(i),this%area_g%mesh_y(1))
                if(this%gettypeofb(1).eq.2) then
                    this%slae%di(i)=-this%area_g%lambda_/hy1
                    this%slae%du2(i)=this%area_g%lambda_/hy1
                else
                    this%slae%di(i)=1d0
                end if
            end do
            t=this%area_g%num_g_x
            this%slae%di(t)=1d0
            this%slae%df(t)=this%area_g%bound_corner(2)
            t=t+1

            ! ножка "Г"
            do i=2,this%area_g%num_g_y-1
                hy1=dabs(this%area_g%mesh_y(i)-this%area_g%mesh_y(i-1))
                hy2=dabs(this%area_g%mesh_y(i+1)-this%area_g%mesh_y(i))
                this%slae%df(t)=this%getvalueofb(2,this%area_g%mesh_x(1),this%area_g%mesh_y(i))
                if(this%gettypeofb(2).eq.2) then
                    hx1=dabs(this%area_g%mesh_x(1)-this%area_g%mesh_x(2))
                    this%slae%di(t)=-this%area_g%lambda_/hx1
                    this%slae%du1(t)=this%area_g%lambda_/hx1
                else
                    this%slae%di(t)=1d0
                end if
                t=t+1

                do j=2,this%area_g%num_g_x-1
                    hx1=dabs(this%area_g%mesh_x(j)-this%area_g%mesh_x(j-1))
                    hx2=dabs(this%area_g%mesh_x(j+1)-this%area_g%mesh_x(j))
                    this%slae%df(t)=this%f(this%area_g%mesh_x(j),this%area_g%mesh_y(i))
                    this%slae%dl1(t-1)=-2d0*this%area_g%lambda_/(hx1*(hx2+hx1))
                    this%slae%dl2(t-this%slae%shift1)=-2d0*this%area_g%lambda_/(hy1*(hy2+hy1))
                    this%slae%du1(t)=-2d0*this%area_g%lambda_/(hx2*(hx2+hx1))
                    this%slae%du2(t)=-2d0*this%area_g%lambda_/(hy2*(hy2+hy1))
                    this%slae%di(t)=(2d0/(hx1*hx2)+2d0/(hy1*hy2))*this%area_g%lambda_+this%area_g%gamma_
                    t=t+1
                end do

                this%slae%df(t)=this%getvalueofb(3,this%area_g%mesh_x(this%area_g%num_g_x),this%area_g%mesh_y(i))
                if(this%gettypeofb(3).eq.2) then
                    hx1=dabs(this%area_g%mesh_x(this%area_g%num_g_x)-this%area_g%mesh_x(this%area_g%num_g_x-1))
                    this%slae%di(t)=this%area_g%lambda_/hx1
                    this%slae%dl1(t-1)=-this%area_g%lambda_/hx1
                else
                    this%slae%di(t)=1d0
                end if
                t=t+1
            end do

            this%slae%dshift=t

            ! Между ножкой и шляпкой
            hy1=dabs(this%area_g%mesh_y(this%area_g%num_g_y)-this%area_g%mesh_y(this%area_g%num_g_y-1))
            hy2=dabs(this%area_g%mesh_y(this%area_g%num_g_y+1)-this%area_g%mesh_y(this%area_g%num_g_y))
            this%slae%df(t)=this%getvalueofb(2,this%area_g%mesh_x(1),this%area_g%mesh_y(this%area_g%num_g_y))
            if(this%gettypeofb(2).eq.2) then
                hx1=dabs(this%area_g%mesh_x(1)-this%area_g%mesh_x(2))
                this%slae%di(t)=-this%area_g%lambda_/hx1
                this%slae%du1(t)=this%area_g%lambda_/hx1
            else
                this%slae%di(t)=1d0
            end if
            t=t+1

            do j=2,this%area_g%num_g_x
                hx1=dabs(this%area_g%mesh_x(j)-this%area_g%mesh_x(j-1))
                hx2=dabs(this%area_g%mesh_x(j+1)-this%area_g%mesh_x(j))
                this%slae%df(t)=this%f(this%area_g%mesh_x(j),this%area_g%mesh_y(this%area_g%num_g_y))
                this%slae%dl1(t-1)=-2d0*this%area_g%lambda_/(hx1*(hx2+hx1))
                this%slae%dl2(t-this%slae%shift1)=-2d0*this%area_g%lambda_/(hy1*(hy2+hy1))
                this%slae%du1(t)=-2d0*this%area_g%lambda_/(hx2*(hx2+hx1))
                this%slae%du2(t)=-2d0*this%area_g%lambda_/(hy2*(hy2+hy1))
                this%slae%di(t)=(2d0/(hx1*hx2)+2d0/(hy1*hy2))*this%area_g%lambda_+this%area_g%gamma_
                t=t+1
            end do

            do j=this%area_g%num_g_x+1,this%area_g%num_x-1
                this%slae%df(t)=this%getvalueofb(4,this%area_g%mesh_x(j),this%area_g%mesh_y(this%area_g%num_g_x))
                if(this%gettypeofb(4).eq.2) then
                    this%slae%di(t)=-this%area_g%lambda_/hy2
                    this%slae%du2(t)=this%area_g%lambda_/hy2
                else
                    this%slae%di(t)=1d0
                end if
                t=t+1
            end do

            this%slae%df(t)=this%area_g%bound_corner(3)
            this%slae%di(t)=1d0
            t=t+1

            ! шляпка "Г"
            do i=this%area_g%num_g_y+1,this%area_g%num_y-1
                hy1=dabs(this%area_g%mesh_y(i)-this%area_g%mesh_y(i-1))
                hy2=dabs(this%area_g%mesh_y(i+1)-this%area_g%mesh_y(i))
                this%slae%df(t)=this%getvalueofb(2,this%area_g%mesh_x(1),this%area_g%mesh_y(i))
                if(this%gettypeofb(2).eq.2) then
                    hx1=dabs(this%area_g%mesh_x(1)-this%area_g%mesh_x(2))
                    this%slae%di(t)=-this%area_g%lambda_/hx1
                    this%slae%du1(t)=this%area_g%lambda_/hx1
                else
                    this%slae%di(t)=1d0
                end if
                t=t+1

                do j=2,this%area_g%num_x-1
                    hx1=dabs(this%area_g%mesh_x(j)-this%area_g%mesh_x(j-1))
                    hx2=dabs(this%area_g%mesh_x(j+1)-this%area_g%mesh_x(j))
                    this%slae%df(t)=this%f(this%area_g%mesh_x(j),this%area_g%mesh_y(i))
                    this%slae%dl1(t-1)=-2d0*this%area_g%lambda_/(hx1*(hx2+hx1))
                    this%slae%dl2(t-this%slae%shift1)=-2d0*this%area_g%lambda_/(hy1*(hy2+hy1))
                    this%slae%du1(t)=-2d0*this%area_g%lambda_/(hx2*(hx2+hx1))
                    this%slae%du2(t)=-2d0*this%area_g%lambda_/(hy2*(hy2+hy1))
                    this%slae%di(t)=(2d0/(hx1*hx2)+2d0/(hy1*hy2))*this%area_g%lambda_+this%area_g%gamma_
                    t=t+1
                end do

                this%slae%df(t)=this%getvalueofb(5,this%area_g%mesh_x(this%area_g%num_x),this%area_g%mesh_y(i))
                if(this%gettypeofb(5).eq.2) then
                    hx1=dabs(this%area_g%mesh_x(this%area_g%num_x)-this%area_g%mesh_x(this%area_g%num_x-1))
                    this%slae%di(t)=this%area_g%lambda_/hx1
                    this%slae%dl1(t-1)=-this%area_g%lambda_/hx1
                else
                    this%slae%di(t)=1d0
                end if
                t=t+1
            end do

            ! верхняя граница шляпки "Г"
            this%slae%df(t)=this%area_g%bound_corner(4)
            this%slae%di(t)=1d0
            t=t+1
            hy1=dabs(this%area_g%mesh_y(this%area_g%num_y)-this%area_g%mesh_y(this%area_g%num_y-1))
            do i=2,this%area_g%num_x-1
                this%slae%df(t)=this%getvalueofb(6,this%area_g%mesh_x(i),this%area_g%mesh_y(this%area_g%num_y))
                if(this%gettypeofb(6).eq.2) then
                    this%slae%di(t)=this%area_g%lambda_/hy1
                    this%slae%dl2(t-this%slae%shift1)=-this%area_g%lambda_/hy1
                else
                    this%slae%di(t)=1d0
                end if
                t=t+1
            end do
            this%slae%di(t)=1d0
            this%slae%df(t)=this%area_g%bound_corner(5)
        end subroutine

        function norm(this,x)
            implicit none
            type(slae) :: this
            double precision :: x(*),norm
            integer :: i
            norm=0d0
            do i=1,this%n
                norm=norm+x(i)**2
            end do
        end function

        subroutine calc_gauss_seidel(this)
            implicit none
            type(slae) :: this
            integer :: i,iter
            double precision :: sum_,residual,res_fax,res_f
            allocate(this%x(this%n))
            this%x=this%def_x
            res_f=this%norm(this%df)
            do iter=1,this%maxiter
                res_fax=0d0
                do i=1,this%n
                    sum_=this%di(i)*this%x(i)
                    if(i.le.this%n-1) then
                        sum_=sum_+this%du1(i)*this%x(i+1)
                    end if
                    if(i.ge.2) then
                        sum_=sum_+this%dl1(i-1)*this%x(i-1)
                    end if
                    if(i.le.this%dshift) then
                        sum_=sum_+this%du2(i)*this%x(this%shift1+i)
                    else if(i.le.this%n-this%shift2) then
                        sum_=sum_+this%du2(i)*this%x(this%shift2+i)
                    end if
                    if(i.ge.this%shift1+this%dshift+1) then
                        sum_=sum_+this%dl2(i-this%shift1)*this%x(i-this%shift2)
                    else if(i.ge.1+this%shift1) then
                        sum_=sum_+this%dl2(i-this%shift1)*this%x(i-this%shift1)
                    end if
                    res_fax=res_fax+(this%df(i)-sum_)**2
                    this%x(i)=this%x(i)+this%omega/this%di(i)*(this%df(i)-sum_)
                end do
                residual=dsqrt(res_fax/res_f)
                if(mod(iter,10).eq.0) write(*,fmt='( a5 i5 a7 e9.2)') 'Iter=',iter,' Resid=',residual
                if(residual.le.this%eps) goto 100
            end do
100         continue
            write(*,fmt='( a5 i5 a7 e9.2)') 'Iter=',iter,' Resid=',residual
        end subroutine

        subroutine dealloc(this)
            implicit none
            type(fdm) :: this
            deallocate(this%area_g%mesh_x)
            deallocate(this%area_g%mesh_y)
            deallocate(this%slae%di)
            deallocate(this%slae%du1)
            deallocate(this%slae%du2)
            deallocate(this%slae%dl1)
            deallocate(this%slae%dl2)
            deallocate(this%slae%df)
            deallocate(this%slae%x)
        end subroutine

        subroutine write_(this)
            implicit none
            type(fdm) :: this
            integer :: i,j,k=1
            open(30,file='../output.txt',status='unknown')
            do i=1,this%area_g%num_g_y-1
                do j=1,this%area_g%num_g_x
                    write(30,fmt='( 3e27.16 )') this%area_g%mesh_x(j),this%area_g%mesh_y(i),this%slae%x(k)
                    k=k+1
                end do
            end do
            do i=this%area_g%num_g_y,this%area_g%num_y
                do j=1,this%area_g%num_x
                    write(30,fmt='( 3e27.16 )') this%area_g%mesh_x(j),this%area_g%mesh_y(i),this%slae%x(k)
                    k=k+1
                end do
            end do
            close(30)
        end subroutine

    end module

    program prog_main
        use module_fdm
        implicit none
        type(fdm) :: a
        integer :: i,j,l

        !double precision :: tmpa(100,100)
        !tmpa=0d0

        call a%area_g%input_g()
        call a%getslae()
        call a%slae%calc_gauss_seidel()
        call a%write_()

        l=a%slae%n-a%area_g%num_x+1
        do i=a%area_g%num_y,a%area_g%num_g_y,-1
            do j=1,a%area_g%num_x
                write(*,fmt='( f8.3 $)') a%slae%x(l)
                l=l+1
            end do
            print*,''
            l=l-2*a%area_g%num_x
        end do
        l=l+a%slae%shift1-1
        do i=a%area_g%num_g_y-1,1,-1
            do j=1,a%area_g%num_g_x
                write(*,fmt='( f8.3 $)') a%slae%x(l)
                l=l+1
            end do
            print*,''
            l=l-2*a%area_g%num_g_x
        end do

        !do i=1,a%slae%n
        !    tmpa(i,i)=a%slae%di(i)
        !    if(i.le.a%slae%n-1) then
        !        tmpa(i,i+1)=a%slae%du1(i)
        !    end if
        !    if(i.ge.2) then
        !        tmpa(i,i-1)=a%slae%dl1(i-1)
        !    end if
        !    if(i.le.a%slae%dshift) then
        !        tmpa(i,a%slae%shift1+i)=a%slae%du2(i)
        !    else if(i.le.a%slae%n-a%slae%shift2) then
        !        tmpa(i,a%slae%shift2+i)=a%slae%du2(i)
        !    end if
        !    if(i.ge.a%slae%shift1+a%slae%dshift+1) then
        !        tmpa(i,i-a%slae%shift2)=a%slae%dl2(i-a%slae%shift1)
        !    else if(i.ge.1+a%slae%shift1) then
        !        tmpa(i,i-a%slae%shift1)=a%slae%dl2(i-a%slae%shift1)
        !    end if
        !end do

        !open(100,file='../tmpmatr.txt',status='unknown')
        !do i=1,a%slae%n
        !    write(100,fmt='( 100f7.3 )') (tmpa(i,j), j=1,a%slae%n), a%slae%df(i)
        !end do
        !write(100,*) ''
        !write(100,*) ''
        !close(100)


        call a%dealloc()
    end program

