#include "harm_FEM.h"
#include "grid_gen_cube.h"
#include <set>

using namespace std;

void trans_face(string file_cord, string file_face); // �������������� ��� ������ �������

namespace test0{

	double w1 = 1.0;
	double lambda1(double x, double y, double z){ return 2;}
	double sigma1(double x, double y, double z) {return 1;}
	double epsilon1(double x, double y, double z) {return 0.01;}

	double us1(double x, double y, double z) { return x+y+z; }
	double uc1(double x, double y, double z) { return x-y-z; }

	double fs1(double x, double y, double z){ return -w1*w1*epsilon1(x,y,z)*us1(x,y,z) - w1*sigma1(x,y,z)*uc1(x,y,z);}
	double fc1(double x, double y, double z){ return -w1*w1*epsilon1(x,y,z)*uc1(x,y,z) + w1*sigma1(x,y,z)*us1(x,y,z);}

}

namespace test1{

	double w1 = 1E5;
	double lambda1(double x, double y, double z){ return 4E3;}
	double sigma1(double x, double y, double z) {return 1E8;}
	double epsilon1(double x, double y, double z) {return 5E-11;}

	double us1(double x, double y, double z) { return exp(-x-y); }
	double uc1(double x, double y, double z) { return exp(-y-z); }

	double fs1(double x, double y, double z){ return -w1*w1*epsilon1(x,y,z)*us1(x,y,z) - w1*sigma1(x,y,z)*uc1(x,y,z) - 2*lambda1(x,y,z)*exp(-x-y);}
	double fc1(double x, double y, double z){ return -w1*w1*epsilon1(x,y,z)*uc1(x,y,z) + w1*sigma1(x,y,z)*us1(x,y,z) - 2*lambda1(x,y,z)*exp(-y-z);}

}

namespace test2{

	double w1 = 1E-4;
	double lambda1(double x, double y, double z){ return 4E3;}
	double sigma1(double x, double y, double z) {return 1E3;}
	double epsilon1(double x, double y, double z) {return 5E-11;}

	double us1(double x, double y, double z) { return 5*exp(-x-y); }
	double uc1(double x, double y, double z) { return 3*exp(-y-z); }

	double fs1(double x, double y, double z){ return -w1*w1*epsilon1(x,y,z)*us1(x,y,z) - w1*sigma1(x,y,z)*uc1(x,y,z) - 10*lambda1(x,y,z)*exp(-x-y);}
	double fc1(double x, double y, double z){ return -w1*w1*epsilon1(x,y,z)*uc1(x,y,z) + w1*sigma1(x,y,z)*us1(x,y,z) - 6*lambda1(x,y,z)*exp(-y-z);}

}

namespace test3
{

    double w1 = 1.0;
    double lambda1(double x, double y, double z)
    {
        return 1;
    }
    double sigma1(double x, double y, double z)
    {
        return 1;
    }
    double epsilon1(double x, double y, double z)
    {
        return 0.1;
    }

    double us1(double x, double y, double z)
    {
        return x+y+z;
    }
    double uc1(double x, double y, double z)
    {
        return x-y-z;
    }

    double fs1(double x, double y, double z)
    {
        return -w1*w1*epsilon1(x,y,z)*us1(x,y,z) - w1*sigma1(x,y,z)*uc1(x,y,z);
    }
    double fc1(double x, double y, double z)
    {
        return -w1*w1*epsilon1(x,y,z)*uc1(x,y,z) + w1*sigma1(x,y,z)*us1(x,y,z);
    }
};

namespace test4
{

    double w1 = 1e-2;
    double lambda1(double x, double y, double z)
    {
        return 1e4;
    }
    double sigma1(double x, double y, double z)
    {
        return 10e3;
    }
    double epsilon1(double x, double y, double z)
    {
        return 1e-11;
    }

    double us1(double x, double y, double z)
    {
        return 2.0*exp(x+y+z);
    }
    double uc1(double x, double y, double z)
    {
        return 3.0*exp(x-y-z);
    }

    double fs1(double x, double y, double z)
    {
        return -lambda1(x,y,z)*3.0*us1(x,y,z) -w1*w1*epsilon1(x,y,z)*us1(x,y,z) - w1*sigma1(x,y,z)*uc1(x,y,z);
    }
    double fc1(double x, double y, double z)
    {
        return -lambda1(x,y,z)*3.0*uc1(x,y,z) -w1*w1*epsilon1(x,y,z)*uc1(x,y,z) + w1*sigma1(x,y,z)*us1(x,y,z);
    }
}

using namespace test4;

int main(){
	harm_FEM<LOS> our_meth_LOS;
	harm_FEM<solver_LU> our_meth_LU;
	//harm_FEM<BCGStab> our_meth_BCGS;
	harm_FEM<GMRES> our_meth_GMRES;

	//=========== ��������� ����� ===========

	//����� ��� �������� �������� �����
	string file_cords = "cords";
	string file_els = "els";
	string file_face = "face";

	//���������� ������ ����� �� ��������� ����
	double ax = 0.0, ay = 0.0, az = 0.0;
	//���������� ����� ����� �� ��������� ����
	double bx = 4.0, by = 4.0, bz = 4.0;

	double h = 0.025;
	double k = 1.0001;
	double k1 = sqrt(k);
	double h1 = h/(1+k1);


	//����������� ���� �� ����
	double hx = /*0.1*/0.2, hy = /*0.015*/0.2, hz = /*0.2*/0.2;
	//������������ �������� �� ����
	double kx = k, ky = k, kz = k;

	//��������������� ����������
	gen_point st(ax, ay, az), en(bx, by, bz);

	printf("Grid gen\n");

	//����������, �������� �����
	grid_gen_cube::generate_reg_grid_FEM(st, en, hx, hy, hz, file_cords, file_els, file_face);

	printf("Grid init\n");

	// ������������� �����
	our_meth_LOS.transf_grid(file_cords, file_els, file_face);
	trans_face(file_cords+"tr",  file_face+"tr");

	/*printf("LOS\n");

	//������ LOS
	our_meth_LOS.init(file_cords+"tr", file_els+"tr", file_face+"tr1");
	our_meth_LOS.set_sol(us1, uc1);
	our_meth_LOS.set_coefs(lambda1, sigma1, epsilon1);
	our_meth_LOS.set_w(w1);
	our_meth_LOS.set_rp(fs1, fc1);

	our_meth_LOS.form_matrix();
	our_meth_LOS.solve();

	our_meth_LOS.out_rez("sol_LOS.txt");
	our_meth_LOS.out_diff("diff_LOS.txt");

	printf("LU\n");

	//������ LU
	our_meth_LU.init(file_cords+"tr", file_els+"tr", file_face+"tr1");
	our_meth_LU.set_sol(us1, uc1);
	our_meth_LU.set_coefs(lambda1, sigma1, epsilon1);
	our_meth_LU.set_w(w1);
	our_meth_LU.set_rp(fs1, fc1);

	our_meth_LU.form_matrix();
	our_meth_LU.solve();

	our_meth_LU.out_rez("sol_LU.txt");
	our_meth_LU.out_diff("diff_LU.txt");

	/*printf("BCGStab\n");

	//������ BCGStab
	our_meth_BCGS.init(file_cords+"tr", file_els+"tr", file_face+"tr1");
	our_meth_BCGS.set_sol(us1, uc1);
	our_meth_BCGS.set_coefs(lambda1, sigma1, epsilon1);
	our_meth_BCGS.set_w(w1);
	our_meth_BCGS.set_rp(fs1, fc1);

	our_meth_BCGS.form_matrix();
	our_meth_BCGS.solve();

	our_meth_BCGS.out_rez("sol_BCGStab.txt");
	our_meth_BCGS.out_diff("diff_BCGStab.txt");*/


    printf("GMRES\n");

	//������ GMRES
	our_meth_GMRES.init(file_cords+"tr", file_els+"tr", file_face+"tr1");
	our_meth_GMRES.set_sol(us1, uc1);
	our_meth_GMRES.set_coefs(lambda1, sigma1, epsilon1);
	our_meth_GMRES.set_w(w1);
	our_meth_GMRES.set_rp(fs1, fc1);

	our_meth_GMRES.form_matrix();
	our_meth_GMRES.solve();

	our_meth_GMRES.out_rez("sol_GMRES.txt");
	our_meth_GMRES.out_diff("diff_GMRES.txt");


	return 0;
}

void trans_face(string file_cord, string file_face){
	FILE* cords = fopen(file_cord.c_str(), "r");
	int n;
	fscanf(cords, "%d", &n);

	node* nodes = new node [n];

	for(int i = 0; i < n; i++)
		fscanf(cords, "%lf %lf %lf", &nodes[i].x, &nodes[i].y, &nodes[i].z);

	fclose(cords);

	FILE* faces = fopen(file_face.c_str(), "r");
	int m;
	fscanf(faces, "%d", &m);
	set<int> face_node;

	for(int i = 0; i < m; i++){
		for(int j = 0; j < 8; j++){
			int k;
			fscanf(faces, "%d", &k);
			face_node.insert(k);
		}
	}
	fclose(faces);

	faces = fopen((file_face+"1").c_str(), "w");
	//�������� ��� ��������� �������, �������� ����������� ����

	fprintf(faces, "%d\n", face_node.size());
for(set<int>::iterator it = face_node.begin(); it != face_node.end(); it++){
		int k = *it;
		double x = nodes[k].x;
		double y = nodes[k].y;
		double z = nodes[k].z;
		if(k%2 == 0)
			fprintf(faces, "%d\t%.15lf\n", k , us1(x,y,z));
		else
			fprintf(faces, "%d\t%.15lf\n", k , uc1(x,y,z));
	}

	fprintf(faces, "%d\n%d", 0, 0);

	fclose(faces);

	face_node.clear();
	delete[] nodes;

}
