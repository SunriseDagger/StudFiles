#if __GFORTRAN__ == 1 && __GNUC__ == 4 && __GNUC_MINOR__ < 5
#define class type
#endif
    module param_module
        double precision, parameter :: derivative = 1d-8
        double precision, parameter :: t_start = 0d0, t_delta = 0.0125d0, t_end = 1d0
        double precision, parameter :: x_start = 0d0, x_delta = 1d0, x_end = 10d0
        double precision, parameter :: y_start = 0d0, y_delta = 1d0, y_end = 10d0
        double precision, parameter :: z_start = 0d0, z_delta = 1d0, z_end = 10d0
        double precision, parameter :: lambda_grid = 1d0

    contains

        function u_(x, y, z, t)
            implicit none
            double precision :: x, y, z, t, u_
            !u_ = x**2 + t**2 + y**2 + z**2
            !u_ = x**2 + y**2 + z**2 + t
            !u_ = 2d0*x + y*t + z - 2d0 + t + 3d0*t**2
            !u_ = 2d0*x + y*t + z**2 - 2d0 + t + 3d0*t**2 + x**2*t**2
            u_ = sin(x + y + z + t**2)
        end function

        function f_(x, y, z, t) ! функция правой части уравнения
            implicit none
            double precision :: x, y, z, t, f_
            !f_ = -6d0 + sigma_(x, y, z, t) * 2d0 * t
            !f_ = -6d0 + sigma_(x, y, z, t)
            !f_ = sigma_(x, y, z, t) * (y + 1d0 + 6d0*t)
            !f_ = sigma_(x, y, z, t) * (y + 1d0 + 6d0*t + 2d0*t*x**2) - 2 - 2d0*t**2
            f_ = sigma_(x, y, z, t) * 2d0 * t * cos(t**2 + x + y + z) + 3d0*sin(t**2 + x + y + z)
        end function

        function sigma_(x, y, z, t)
            implicit none
            double precision :: x, y, z, t, sigma_
            sigma_ = 1d5
        end function

        function grad_u_(x, y, z, t)
            implicit none
            double precision :: x, y, z, t
            double precision :: grad_u_(4)
            !grad_u_(1) = (u_(x + derivative, y, z, t) - u_(x - derivative, y, z, t)) / (2d0 * derivative)
            !grad_u_(2) = (u_(x, y + derivative, z, t) - u_(x, y - derivative, z, t)) / (2d0 * derivative)
            !grad_u_(3) = (u_(x, y, z + derivative, t) - u_(x, y, z - derivative, t)) / (2d0 * derivative)
            !grad_u_(4) = (u_(x, y, z, t + derivative) - u_(x, y, z, t - derivative)) / (2d0 * derivative)
            grad_u_(1) = 2d0*x
            grad_u_(2) = 2d0*y
            grad_u_(3) = 2d0*z
            grad_u_(4) = 2d0*t
        end function

    end module
