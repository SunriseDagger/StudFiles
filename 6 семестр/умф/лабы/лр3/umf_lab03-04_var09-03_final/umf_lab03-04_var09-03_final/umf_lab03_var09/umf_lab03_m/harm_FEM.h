#pragma once

#include <stdio.h>
#include <string>
//#include <windows.h> //��� ������� �������

#include "additional_classes.h"

//��������
#include "solver_LOS.h"
#include "solver_LU.h"
#include "solver_BCGStab.h"
#include "solver_GMRES.h"

using namespace std;

#if defined _WIN32 || defined _WIN64
#include <windows.h>
#else
#include <sys/time.h>
#endif

class timer
{
private:
#if defined _WIN32 || defined _WIN64
    LARGE_INTEGER b_start, b_stop, freq;
#else
    struct timeval tv1, tv2, dtv;
    struct timezone tz;
#endif
public:
    timer()
    {
#if defined _WIN32 || defined _WIN64
        QueryPerformanceFrequency(&freq);
#endif
    }
    ~timer() {};
    void start()
    {
#if defined _WIN32 || defined _WIN64
        QueryPerformanceCounter(&b_start);
#else
        gettimeofday(&tv1, &tz);
#endif
    }
    double stop()
    {
#if defined _WIN32 || defined _WIN64
        QueryPerformanceCounter(&b_stop);
        return (double)(b_stop.QuadPart - b_start.QuadPart) * 1000.0/
               (double)(freq.QuadPart);
#else
        gettimeofday(&tv2, &tz);
        dtv.tv_sec = tv2.tv_sec - tv1.tv_sec;
        dtv.tv_usec = tv2.tv_usec - tv1.tv_usec;
        if(dtv.tv_usec < 0)
        {
            dtv.tv_sec--;
            dtv.tv_usec += 1000000;
        }
        return (double)dtv.tv_sec * 1000.0 + (double)dtv.tv_usec / 1000.0;
#endif
    }
};

typedef double(*func)(double, double, double);

//������� ������������� ������ � ������� ���. ��� ��������� - ����
//solver_type - ��� �������� ����

template <class solver_type> class harm_FEM
{
public:

    void transf_grid(string file_cords, string file_elements, string file_faces); //��������������� ����� � ������ �� ������� ���������� � ������
    void init(string file_cords, string file_elements, string file_faces); //���� ������
    void set_coefs(func set_lambda, func set_sigma, func set_epsilon); //���� ����. ���������
    void set_rp(func set_f_sin, func set_f_cos); // ���� ������ ����� �������
    void set_w(double s_w); //���� ������� ���������
    void form_matrix(); //������������ ������� ����
    void solve(); //������� ����
    void out_rez(string file_name); //����� ����������� � ���� file_name
    void out_diff(string file_name); //����� ����������� � ����

    void set_sol(func us, func uc);

private:

    int elements_n; //���������� �����
    int nodes_n; //���������� ���������
    int faces_fir_n, faces_sec_n, faces_thi_n; //���������� ������ � 1, 2 � 3 �������� ��������������

    node* nodes; //������ �����
    FE* elements; //������ ���������

    face *faces_sec, *faces_thi; //������� ��� ������� ������
    double *faces_fir; //�������� ������ �������
    int *faces_fir_node; //���� ������ �������

    double u_betta_s(double x, double y, double z, int face_n); //���������� �������� ������� �������� ����, ��� ������
    double u_betta_c(double x, double y, double z, int face_n); //���������� �������� ������� �������� ����, ��� ��������
    double* betta;

    double tetta_s(double x, double y, double z, int face_n); ////���������� �������� ������� ������� ����, ��� ������
    double tetta_c(double x, double y, double z, int face_n); ////���������� �������� ������� ������� ����, ��� ��������


    func lambda, sigma, epsilon; //������������ ���������
    func f_sin, f_cos; //������ ����� ���������
    double w; //������� ���������

    solver_type solver; //�������� ����
    int solver_iters; //���������� �������� �� ������� ���� ������ ����
    SLAE_port_gen port_gen; //��������� �������� ����

    double time; //����� ������� ����

    //�������, ��� �������� ����
    int *ig, *jg;
    int SLAE_el_n; //���������� ��������� � ����(��������� � jg, gl, gu)
    double *gl, *gu, *di;
    double *right_part; //������ �����

    double* solution; //�������
    func u_sin, u_cos;


    void form_gmass(); //������������ �������� ig � jg
    int find_el_pos(int i, int j); //���������� ��������� �������� � �������
    void form_loc(double **A_loc, double *b_loc, int el_n); //������������ ��������� ������� ��� �������� el_n
    void gen_loc_thi(double **A_loc, double *b_loc, int face_n); //������������ ��������� ������� � ������� ��� ������ �������, ��� ����� face_n
    void gen_loc_sec(double *b_loc, int face_n); //������������ ���������� ������� ��� ������ �������, ��� ����� face_n


    //���������� ����������� ��������, ��� ������� ��������
    int mu(int i);
    int nu(int i);
    int v(int i);

};

// ====================================================================================================================
// ====================================================================================================================
// ====================================================================================================================
// ============================================= ���������� ===========================================================
// ====================================================================================================================
// ====================================================================================================================

template <typename solver_type> void harm_FEM<solver_type>::init(string file_cords, string file_elements, string file_faces)
{
    FILE* inp_cord = fopen(file_cords.c_str(), "r");

    //���� ��������� ������
    fscanf(inp_cord, "%d", &nodes_n);
    nodes = new node [nodes_n];

    for(int i = 0; i < nodes_n; i++)
        fscanf(inp_cord, "%lf %lf %lf", &nodes[i].x, &nodes[i].y, &nodes[i].z);
    fclose(inp_cord);

    FILE* inp_els = fopen(file_elements.c_str(), "r");

    //���� ���������
    fscanf(inp_els, "%d", &elements_n);
    elements = new FE [elements_n];

    for(int i = 0; i < elements_n; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            fscanf(inp_els, "%d", &elements[i].node_n[j]);
        }
    }

    fclose(inp_els);

    FILE* inp_faces = fopen(file_faces.c_str(), "r");

    //���� ������ �������
    fscanf(inp_faces, "%d", &faces_fir_n);

    faces_fir = new double [faces_fir_n];
    faces_fir_node = new int [faces_fir_n];

    for(int i = 0; i < faces_fir_n; i++)
    {
        fscanf(inp_faces, "%d %lf", &faces_fir_node[i], &faces_fir[i]);
    }

    //���� ������ �������
    fscanf(inp_faces, "%d", &faces_sec_n);

    faces_sec = new face [faces_sec_n];

    for(int i = 0; i < faces_sec_n; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            fscanf(inp_faces, "%d", &faces_sec[i].node_n[j]);
        }
    }

    //���� ������� �������
    fscanf(inp_faces, "%d", &faces_thi_n);

    faces_thi = new face [faces_thi_n];

    for(int i = 0; i < faces_thi_n; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            fscanf(inp_faces, "%d", &faces_thi[i].node_n[j]);
        }
    }

    fclose(inp_faces);
}
template <typename solver_type> void harm_FEM<solver_type>::out_diff(string file_name)
{
    FILE* out_f = fopen(file_name.c_str(), "w");

    double diff1 = 0; //����� �����������
    double u_norm = 0; //�����
    double diff_s = 0, diff_c = 0;
    double u_ns = 0, u_nc = 0;


    for(int i = 0; i < nodes_n/2; i++)
    {
        double x = nodes[2*i].x; //���������� �����
        double y = nodes[2*i].y; //���������� �����
        double z = nodes[2*i].z; //���������� �����

        double us = u_sin(x,y,z);
        double uc = u_cos(x,y,z);
        diff_s += (us - solution[2*i])*(us - solution[2*i]);
        diff_c += (uc - solution[2*i+1])*(uc - solution[2*i+1]);

        u_ns += us*us;
        u_nc += uc*uc;

    }

    diff1 = diff_s + diff_c;
    u_norm = u_ns + u_nc;

    fprintf(out_f, "Total:\t%.3e\n", sqrt(diff1/u_norm));
    fprintf(out_f, "Sin:\t%.3e\n", sqrt(diff_s/u_ns));
    fprintf(out_f, "Cos:\t%.3e\n", sqrt(diff_c/u_nc));
    fprintf(out_f, "Iters:\t%d\n", solver_iters);
    fprintf(out_f, "Time:\t%lf\n", time);


    fclose(out_f);

}

template <typename solver_type> void harm_FEM<solver_type>::set_coefs(func set_lambda, func set_sigma, func set_epsilon)
{
    lambda = set_lambda;
    sigma = set_sigma;
    epsilon = set_epsilon;
}
template <typename solver_type> void harm_FEM<solver_type>::set_rp(func set_f_sin, func set_f_cos)
{
    f_sin = set_f_sin;
    f_cos = set_f_cos;
}
template <typename solver_type> void harm_FEM<solver_type>::set_w(double s_w)
{
    w = s_w;
}

template <typename solver_type> void harm_FEM<solver_type>::solve()
{

    class timer t;
    //������ ������ �������
    t.start();

    solver.init(ig, jg, gu, gl, di, nodes_n);
    solver.set_rp(right_part);
    solver.solve(solution, solver_iters);

    //����� ������ �������
    time = t.stop(); //���������� ������� �������*/
}

template <typename solver_type> void harm_FEM<solver_type>::form_gmass()
{
    port_gen.init(nodes_n);

    //�� ��������
    ig = new int [nodes_n+1];

    for(int i = 0; i < elements_n; i++)
        port_gen.add_el(elements[i]);

    port_gen.gen(ig, jg, SLAE_el_n); //�������� �������

    gl = new double [SLAE_el_n];
    gu = new double [SLAE_el_n];
    di = new double [nodes_n];

    right_part = new double [nodes_n];
    solution = new double [nodes_n];

    //���������
    for(int i = 0; i < SLAE_el_n; i++)
        gl[i] = gu[i] = 0;

    for(int i = 0; i < nodes_n; i++)
        di[i] = right_part[i] = solution[i] = 0;

    port_gen.~SLAE_port_gen();
}

template <typename solver_type> void harm_FEM<solver_type>::form_matrix()
{
    double **A_loc, *b_loc; //��������� ������� � ������ ������ �����
    b_loc = new double [16];
    A_loc = new double* [16];

    for(int i = 0; i < 16; i++)
        A_loc[i] = new double [16];

    form_gmass(); //��������� �������

    int cur_row; //������� ������
    int pos; //������� � gu � gl

    //��������� �������� ����

    for(int k = 0; k < elements_n; k++)
    {
        form_loc(A_loc, b_loc, k); //�������� ��������� �������

        for(int i = 0; i < 16; i++)
        {
            cur_row = elements[k].node_n[i]; //���������� ������� ������
            for(int j = 0 ; j < i ; j++)
            {
                if(cur_row > elements[k].node_n[j])  //���� �������� ����������� � ������
                {
                    pos = find_el_pos(cur_row, elements[k].node_n[j]); //������� ������� � gu � gl
                    gl[pos] += A_loc[i][j];
                    gu[pos] += A_loc[j][i];
                }
                else
                {
                    pos = find_el_pos(elements[k].node_n[j], cur_row); //������� ������� � gu � gl
                    gu[pos] += A_loc[i][j];
                    gl[pos] += A_loc[j][i];
                }
            }

            di[cur_row] += A_loc[i][i];
            right_part[cur_row] += b_loc[i];
        }
    }

    // ���� ������� ������� �������

    for(int k = 0; k < faces_thi_n; k++)
    {
        gen_loc_thi(A_loc, b_loc, k);

        for(int i = 0; i < 8; i++)
        {
            cur_row = faces_thi[k].node_n[i];
            for(int j = 0 ; j < i ; j++)
            {
                if(cur_row > faces_thi[k].node_n[j])  //���� �������� ����������� � ������
                {
                    pos = find_el_pos(cur_row, faces_thi[k].node_n[j]); //������� ������� � gu � gl
                    gl[pos] += A_loc[i][j];
                    gu[pos] += A_loc[j][i];
                }
                else
                {
                    pos = find_el_pos(faces_thi[k].node_n[j], cur_row); //������� ������� � gu � gl
                    gu[pos] += A_loc[i][j];
                    gl[pos] += A_loc[j][i];
                }
            }

            di[cur_row] += A_loc[i][i];
            right_part[cur_row] += b_loc[i];
        }
    }

    //���� ������ ������� �������

    for(int k = 0; k < faces_sec_n; k++)
    {
        gen_loc_sec(b_loc, k);

        for(int i = 0; i < 8; i++)
            right_part[faces_sec[k].node_n[i]] += b_loc[i];
    }

    //���� ������ ������� �������

    for(int k = 0; k < faces_fir_n; k++)
    {
        cur_row = faces_fir_node[k]; //����, � ������� ������� �������

        double val = faces_fir[k]; //�������� ��������

        di[cur_row] = 1;
        right_part[cur_row] = val;

        //�������� ������� ����� �������
        int i_s = ig[cur_row], i_e = ig[cur_row+1];
        for(int i = i_s; i < i_e; i++)
        {
            right_part[jg[i]] -= gu[i]*val;
            gl[i] = 0;
            gu[i] = 0;
        }
        //�������� ������ ����� �������
        for(int p = cur_row + 1; p < nodes_n; p++)
        {
            int i_s = ig[p], i_e = ig[p+1];
            for(int i = i_s; i < i_e; i++)
            {
                if(jg[i] == cur_row)  //�������� - ��� �� �������
                {
                    right_part[p] -= gl[i]*val;
                    gl[i] = 0;
                    gu[i] = 0;
                }
            }
        }
    }
}

template <typename solver_type> void harm_FEM<solver_type>::form_loc(double **A_loc, double *b_loc, int el_n)
{
    //������� �������� � ����� ��� �������� ���
    double G[8][8], M[8][8];
    double hx = fabs(nodes[elements[el_n].node_n[2]].x - nodes[elements[el_n].node_n[0]].x); //��� �� x
    double hy = fabs(nodes[elements[el_n].node_n[4]].y - nodes[elements[el_n].node_n[0]].y); //��� �� y
    double hz = fabs(nodes[elements[el_n].node_n[8]].z - nodes[elements[el_n].node_n[0]].z); //��� �� z

    double G1[2][2] = {{1.0, -1.0}, {-1.0, 1.0}}; // �������������� �������
    double M1[2][2] = {{1.0/3.0, 1.0/6.0}, {1.0/6.0, 1.0/3.0}}; // ��� ���� �������������� �������

    double lambda_aver = 0; //���������� ������
    double sigma_aver = 0; //���������� �����
    double epsilon_aver = 0; //���������� �������


    //�������� ������� ��������
    for(int i = 0; i < 8; i++)
    {
        double x = nodes[elements[el_n].node_n[2*i]].x; //���������� �����
        double y = nodes[elements[el_n].node_n[2*i]].y; //���������� �����
        double z = nodes[elements[el_n].node_n[2*i]].z; //���������� �����
        lambda_aver += lambda(x,y,z);
        sigma_aver += sigma(x,y,z);
        epsilon_aver += epsilon(x, y, z);
    }
    lambda_aver /= 8.0;
    sigma_aver /= 8.0;
    epsilon_aver /= 8.0;

    //��������� ������ G � M
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            G[i][j] = hy*hz * G1[mu(i)][mu(j)] * M1[nu(i)][nu(j)] * M1[v(i)][v(j)] / hx;
            G[i][j] += hx*hz * M1[mu(i)][mu(j)] * G1[nu(i)][nu(j)] * M1[v(i)][v(j)] / hy;
            G[i][j] += hx*hy * M1[mu(i)][mu(j)] * M1[nu(i)][nu(j)] * G1[v(i)][v(j)] / hz;

            G[i][j] *= lambda_aver;

            M[i][j] = hx*hy*hz * M1[mu(i)][mu(j)] * M1[nu(i)][nu(j)] * M1[v(i)][v(j)];
        }
    }

    //�������� ��������� �������, ��� �������
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            A_loc[2*i+1][2*j+1] = A_loc[2*i][2*j] = G[i][j] - w*w*epsilon_aver*M[i][j];
            A_loc[2*i][2*j+1] = -w*sigma_aver*M[i][j];
            A_loc[2*i+1][2*j] = w*sigma_aver*M[i][j];
        }
    }

    //������ �������� ������� ��������� ������ �����
    double val_f_sin[8], val_f_cos[8], b_sin[8], b_cos[8];

    //��������� ��������
    for(int i = 0; i < 8; i++)
    {
        double x = nodes[elements[el_n].node_n[2*i]].x; //���������� �����
        double y = nodes[elements[el_n].node_n[2*i]].y; //���������� �����
        double z = nodes[elements[el_n].node_n[2*i]].z; //���������� �����

        val_f_sin[i] = f_sin(x,y,z);
        val_f_cos[i] = f_cos(x,y,z);
    }
    //��������� ���������� ������ �����
    for(int i = 0; i < 8; i++)
    {
        b_sin[i] = b_cos[i] = 0;
        for(int j = 0; j < 8; j++)
        {
            b_sin[i] += M[i][j]*val_f_sin[j];
            b_cos[i] += M[i][j]*val_f_cos[j];
        }
    }

    //��������� ��� ������� � ����
    for(int i = 0; i < 8; i++)
    {
        b_loc[2*i] = b_sin[i];
        b_loc[2*i+1] = b_cos[i];
    }

    double vec2[16], vals[16];

    for(int i = 0; i < 8; i++)
    {
        double x = nodes[elements[el_n].node_n[2*i]].x; //���������� �����
        double y = nodes[elements[el_n].node_n[2*i]].y; //���������� �����
        double z = nodes[elements[el_n].node_n[2*i]].z; //���������� �����

        vals[2*i] = u_sin(x,y,z);
        vals[2*i+1] = u_cos(x,y,z);
    }

    for(int i = 0; i < 16; i++)
    {
        vec2[i] = 0;
        for(int j = 0; j < 16; j++)
            vec2[i] += A_loc[i][j]*vals[j];
    }
    double diff[16];
    for(int i = 0; i < 16; i++)
        diff[i] = b_loc[i] - vec2[i];

}

template <typename solver_type> int harm_FEM<solver_type>::mu(int i)
{
    return i % 2;
}
template <typename solver_type> int harm_FEM<solver_type>::nu(int i)
{
    return (i/2) % 2;
}
template <typename solver_type> int harm_FEM<solver_type>::v(int i)
{
    return (i/4) % 2;
}

template <typename solver_type> void harm_FEM<solver_type>::gen_loc_thi(double **A_loc, double *b_loc, int face_n)
{
    double hx;
    double hy;

    //���������� ��������� �����
    if(nodes[faces_thi[face_n].node_n[2]].x == nodes[faces_thi[face_n].node_n[0]].x)  //���� � ��������� yOz
    {
        hx = fabs(nodes[faces_thi[face_n].node_n[2]].y - nodes[faces_thi[face_n].node_n[0]].y);
        hy = fabs(nodes[faces_thi[face_n].node_n[4]].z - nodes[faces_thi[face_n].node_n[0]].z);
    }
    else
    {
        if(nodes[faces_thi[face_n].node_n[2]].y == nodes[faces_thi[face_n].node_n[0]].y)  //���� � ��������� xOz
        {
            hx = fabs(nodes[faces_thi[face_n].node_n[2]].x - nodes[faces_thi[face_n].node_n[0]].x);
            hy = fabs(nodes[faces_thi[face_n].node_n[4]].z - nodes[faces_thi[face_n].node_n[0]].z);
        }
        else  //���� � ��������� xOy
        {
            hx = fabs(nodes[faces_thi[face_n].node_n[2]].x - nodes[faces_thi[face_n].node_n[0]].x);
            hy = fabs(nodes[faces_thi[face_n].node_n[4]].y - nodes[faces_thi[face_n].node_n[0]].y);
        }
    }

    //������� ����� ��� �������� ���
    double M1[4][4] = {{4, 2, 2, 1}, {2, 4, 1, 2}, {2, 1, 4 ,2}, {1, 2, 2, 4}};

    double loc_betta = betta[faces_thi[face_n].area];

    //��������� ������� �����
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            A_loc[2*i][2*i] = A_loc[2*j+1][2*j+1] = loc_betta*hx*hy*M1[i][i]/36.0;
            A_loc[2*i+1][2*j] = A_loc[2*i][2*j+1] = 0;
        }
    }

    //��������� ������ �����
    double b_both[8]; //������ ��������
    for(int i = 0; i < 4; i++)
    {
        double x = nodes[faces_thi[face_n].node_n[2*i]].x; //���������� x
        double y = nodes[faces_thi[face_n].node_n[2*i]].y; //���������� y
        double z = nodes[faces_thi[face_n].node_n[2*i]].z; //���������� z
        b_both[2*i] = u_betta_s(x, y, z, face_n);
        b_both[2*i+1] = u_betta_c(x, y, z, face_n);
    }

    //�������� ������ �����
    for(int i = 0; i < 8; i++)
    {
        b_loc[i] = 0;
        for(int j = 0; j < 8; j++)
        {
            b_loc[i] += A_loc[i][j]*b_both[j];
        }
    }

}



template <typename solver_type> void harm_FEM<solver_type>::gen_loc_sec(double *b_loc, int face_n)
{

    double hx;
    double hy;

    double A_loc[8][8]; //������� ��������

    //���������� ��������� �����
    if(nodes[faces_thi[face_n].node_n[2]].x == nodes[faces_thi[face_n].node_n[0]].x)  //���� � ��������� yOz
    {
        hx = fabs(nodes[faces_thi[face_n].node_n[2]].y - nodes[faces_thi[face_n].node_n[0]].y);
        hy = fabs(nodes[faces_thi[face_n].node_n[4]].z - nodes[faces_thi[face_n].node_n[0]].z);
    }
    else
    {
        if(nodes[faces_thi[face_n].node_n[2]].y == nodes[faces_thi[face_n].node_n[0]].y)  //���� � ��������� xOz
        {
            hx = fabs(nodes[faces_thi[face_n].node_n[2]].x - nodes[faces_thi[face_n].node_n[0]].x);
            hy = fabs(nodes[faces_thi[face_n].node_n[4]].z - nodes[faces_thi[face_n].node_n[0]].z);
        }
        else  //���� � ��������� xOy
        {
            hx = fabs(nodes[faces_thi[face_n].node_n[2]].x - nodes[faces_thi[face_n].node_n[0]].x);
            hy = fabs(nodes[faces_thi[face_n].node_n[4]].y - nodes[faces_thi[face_n].node_n[0]].y);
        }
    }

    //������� ����� ��� �������� ���
    double M1[4][4] = {{4, 2, 2, 1}, {2, 4, 1, 2}, {2, 1, 4 ,2}, {1, 2, 2, 4}};

    //��������� ������� �����
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            A_loc[2*i][2*i] = A_loc[2*j+1][2*j+1] = hx*hy*M1[i][i]/36.0;
            A_loc[2*i+1][2*j] = A_loc[2*i][2*j+1] = 0;
        }
    }

    //��������� ������ �����
    double b_both[8]; //������ ��������
    for(int i = 0; i < 4; i++)
    {
        double x = nodes[faces_thi[face_n].node_n[2*i]].x; //���������� x
        double y = nodes[faces_thi[face_n].node_n[2*i]].y; //���������� y
        double z = nodes[faces_thi[face_n].node_n[2*i]].z; //���������� z
        b_both[2*i] = tetta_s(x, y, z, face_n);
        b_both[2*i+1] = tetta_c(x, y, z, face_n);
    }

    //�������� ������ �����
    for(int i = 0; i < 8; i++)
    {
        b_loc[i] = 0;
        for(int j = 0; j < 8; j++)
        {
            b_loc[i] += A_loc[i][j]*b_both[j];
        }
    }
}

template <typename solver_type> int harm_FEM<solver_type>::find_el_pos(int i, int j)
{
    int k_s = ig[i], k_e = ig[i+1];
    int cur;
    bool find = false;
    for(int k = k_s; k < k_e && !find; k++)
    {
        if(jg[k] == j)
        {
            cur = k;
            find = true;
        }
    }
    return cur;
}


template <typename solver_type> void harm_FEM<solver_type>::transf_grid(string file_cords, string file_elements, string file_faces)
{

    //�������������� ����� ���������
    FILE* cords_f_in = fopen(file_cords.c_str(), "r"); //���� �������� �����
    FILE* cords_f_out = fopen((file_cords + "tr").c_str(), "w"); //���� ���������������� �����

    int n; //����������
    fscanf(cords_f_in, "%d", &n);
    fprintf(cords_f_out, "%d\n", 2*n);


    for(int i = 0; i < n; i++)
    {
        double x, y, z; //��������� ����������
        fscanf(cords_f_in, "%lf %lf %lf",&x, &y, &z);
        fprintf(cords_f_out, "%.15lf\t%.15lf\t%.15lf\n", x, y, z);
        fprintf(cords_f_out, "%.15lf\t%.15lf\t%.15lf\n", x, y, z);
    }

    fclose(cords_f_in);
    fclose(cords_f_out);

    //�������������� ����� ���������

    FILE* els_f_in = fopen(file_elements.c_str(), "r"); //���� �������� �����
    FILE* els_f_out = fopen((file_elements+"tr").c_str(), "w"); //���� ���������������� �����

    fscanf(els_f_in, "%d", &n);
    fprintf(els_f_out, "%d\n", n);

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            int k; //������� �������
            fscanf(els_f_in, "%d", &k);
            fprintf(els_f_out, "%d %d ", 2*k, 2*k+1);
        }
        fprintf(els_f_out, "\n");
    }

    fclose(els_f_in);
    fclose(els_f_out);

    //�������������� ����� ������

    FILE* face_f_in = fopen(file_faces.c_str(), "r"); //���� �������� �����
    FILE* face_f_out = fopen((file_faces+"tr").c_str(), "w"); //���� ���������������� �����

    fscanf(face_f_in, "%d", &n);
    fprintf(face_f_out, "%d\n", n);

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            int k; //������� �������
            fscanf(face_f_in, "%d", &k);
            fprintf(face_f_out, "%d %d ", 2*k, 2*k+1);
        }
        fprintf(face_f_out, "\n");
    }

    fclose(face_f_in);
    fclose(face_f_out);
}

template <typename solver_type> double harm_FEM<solver_type>::u_betta_s(double x, double y, double z, int face_n)
{
    double val; //�������� ��������

    return val;
}

template <typename solver_type> double harm_FEM<solver_type>::u_betta_c(double x, double y, double z, int face_n)
{
    double val; //�������� ��������

    return val;
}

template <typename solver_type> double harm_FEM<solver_type>::tetta_s(double x, double y, double z, int face_n)
{
    double val; //�������� ��������

    return val;
}

template <typename solver_type> double harm_FEM<solver_type>::tetta_c(double x, double y, double z, int face_n)
{
    double val; //�������� ��������

    return val;
}
template <typename solver_type> void harm_FEM<solver_type>::out_rez(string file_name)
{
    FILE* out_f = fopen(file_name.c_str(), "w");
    for(int i = 0; i < nodes_n; i++)
        fprintf(out_f, "%d\t%.15lf\n", i, solution[i]);

    fclose(out_f);
}


template <typename solver_type> void harm_FEM<solver_type>::set_sol(func us, func uc)
{
    u_sin = us;
    u_cos = uc;
}
