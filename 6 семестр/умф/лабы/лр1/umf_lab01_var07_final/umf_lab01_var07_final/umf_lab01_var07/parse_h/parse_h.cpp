#include <stdio.h>
#include <vector>

using namespace std;

struct fin
{
    double x,y,val;
};

struct fout
{
    double x,y,val1,val2,val4;
};

int main()
{
    vector<fin> h2,h4;
    vector<fout> hfout;
    fin b;
    fout c;
    FILE *fh1=fopen("h1.txt","r");
    FILE *fh2=fopen("h2.txt","r");
    FILE *fh4=fopen("h4.txt","r");
    while(!feof(fh1)){
        fscanf(fh1," %lf %lf %lf ",&(c.x), &(c.y), &(c.val1));
        hfout.push_back(c);
    }
    while(!feof(fh2)){
        fscanf(fh2," %lf %lf %lf ",&(b.x), &(b.y), &(b.val));
        h2.push_back(b);
    }
    while(!feof(fh4)){
        fscanf(fh4," %lf %lf %lf ",&(b.x), &(b.y), &(b.val));
        h4.push_back(b);
    }
    fclose(fh1);
    fclose(fh2);
    fclose(fh4);
    for(int i=0;i<h2.size();i++)
    {
        for(int j=0;j<hfout.size();j++)
        {
            if(hfout[j].x == h2[i].x && hfout[j].y == h2[i].y)
            {
                hfout[j].val2=h2[i].val;
            }
        }
    }
    for(int i=0;i<h4.size();i++)
    {
        for(int j=0;j<hfout.size();j++)
        {
            if(hfout[j].x == h4[i].x && hfout[j].y == h4[i].y)
            {
                hfout[j].val4=h4[i].val;
            }
        }
    }
    FILE *f=fopen("res.txt","w");
    for(int i=0;i<hfout.size();i++)
    {
        fprintf(f,"%27.16E\t%27.16E\t%27.16E\t%27.16E\t%27.16E\n",hfout[i].x,hfout[i].y,hfout[i].val1,hfout[i].val2,hfout[i].val4);
    }
    fclose(f);
    return 0;
}
