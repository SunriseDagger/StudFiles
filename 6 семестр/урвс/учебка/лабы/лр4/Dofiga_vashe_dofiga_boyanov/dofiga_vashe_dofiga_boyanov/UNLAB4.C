#include<sys/stat.h>
//#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

void RisTab1(void);
void RisTab2(void);
void Prob(int, int);


char tab[4]={"    "};
FILE *pol, *res;
char *mas1 [] = { "descriptor", "   name   ","   disk   ","  N index ",
				  "   type   ", "   prava  ","  svyazi  "," vladelec ",
				  "   group  ", "   size   "};
char *mas2 [] = { "       access time       ","       rewrite time    ",
				  "   change status time    " };
char *mas3 [] = { "   name   ", "ssylka TOF","access mod", "loc. point"};


struct stat statb;



main()
{

//  int i, k;
  remove("result");
  pol=fopen("makefile","a+t");

  tab[0]='i';
  RisTab2();
  RisTab1();
  tab[1]='o';
  RisTab2();
  RisTab1();
  tab[2]='e';
  RisTab2();
  RisTab1();
  tab[3]='p';
  RisTab2();
  RisTab1();

  tab[0]=' ';
  RisTab2();
  RisTab1();

  tab[0]='p';
  RisTab2();
  RisTab1();

  tab[3]=' ';
  RisTab2();
  RisTab1();


  fclose(pol);

exit(0);//  getch();
}



// ***************************************************************
// *                 Рисование таблицы открытых файлов           *
// ***************************************************************


void RisTab1( void)
{
  char c;
  char string[10];
  int i,j,j1, k, kolsim, ndig;
  int dec, sign;
  res = fopen("result","at");


/*-----------Цикл для стандарта------------*/

  for(j=0;j<=9;j++)
	{
	  fwrite(mas1[j],10,1,res);
	  fwrite("│",1,1,res);
	}
	for(j=0;j<=2;j++)
	{
	  fwrite(mas2[j],25,1,res);
	  fwrite("│",1,1,res);
	}
	fwrite("\n",1,1,res);
/*-----------------------------------------*/

  for(i=0;i<=3;i++)
  {
	 if(tab[i]!=' ')
	 {
		if(tab[i]=='p')
		{  k=i;
			i=fileno(pol);
		} else k=i;

		for(j=0;j<=9;j++)
		fwrite("──────────┼",11,1,res);
		for(j=0;j<=2;j++)
			fwrite("─────────────────────────┼",26,1,res);
		fwrite("\n",1,1,res);
		fstat(i, &statb);
/*------------------Дискриптор----------------*/
		sprintf(string,"%d  ",k);
		fwrite(string,3,1,res);
		Prob(3,10);
/*------------------Имя файла-----------------*/
		switch(tab[k])
		{ case 'i': {fwrite("stdin",5,1,res);kolsim=5; break;}
		  case 'o': {fwrite("stdout",6,1,res);kolsim=6; break;}
		  case 'e': {fwrite("stderr",6,1,res);kolsim=6; break;}
		  case 'p': {fwrite("makefile",8,1,res); kolsim=8; break;}
		}
	
		Prob(kolsim,10);

/*----------Номер устройства------------------*/
		sprintf(string,"%d       ",statb.st_dev);
		fwrite(string,8,1,res);
		Prob(8,10);
/*---------------номер индекса----------------*/
		sprintf(string,"%d       ",statb.st_ino);
		fwrite(string,8,1,res);
		Prob(8,10);
/*------------------тип файла-----------------*/
//string = ecvt(i, 10, &dec, &sign);
//	fwrite(string,dec,1,res);
		Prob(0,10);
/*-----------------права доступа--------------*/
//	string = ecvt(i, 10, &dec, &sign);
//	fwrite(string,dec,1,res);
		Prob(0,10);
/*-----------------число связей--------------*/
		sprintf(string,"%d  ",statb.st_nlink);
		fwrite(string,3,1,res);
		Prob(3,10);
/*--------------владелец----------------------*/
		sprintf(string,"%d       ",statb.st_uid);
		fwrite(string,5,1,res);
		Prob(5,10);
/*-----------------группа---------------------*/
		sprintf(string,"%d       ",statb.st_gid);
		fwrite(string,5,1,res);
		Prob(5,10);
/*-----------------размер файла--------------*/
		sprintf(string,"%d        ",statb.st_size);
		fwrite(string,8,1,res);
		Prob(8,10);
/*-------время посл. доступа-----------------*/
		fwrite(ctime(&statb.st_atime),24,1,res);
		Prob(24,25);
/*-----------посл. внесение изменений--------*/
		fwrite(ctime(&statb.st_mtime),24,1,res);
		Prob(24,25);
/*-----------время изменения статуса---------*/
		fwrite(ctime(&statb.st_ctime),24,1,res);
		Prob(24,25);
/*-------------------------------------------*/

		fwrite("\n",1,1,res);
		i=k;
	 }
	 else
	 {
	   for(j=0;j<=9;j++)
		fwrite("──────────┼",11,1,res);
		for(j=0;j<=2;j++)
			fwrite("─────────────────────────┼",26,1,res);
		fwrite("\n",1,1,res);

	   sprintf(string,"%d   ",i);
	   fwrite(string,3,1,res);
	   Prob(3,10);
	   for(j=0;j<9;j++)
		 Prob(0,10);

	   for(j1=0;j1<=2;j1++)
		   Prob(0,25);
	   fwrite("\n",1,1,res);

	 }


  }
/*-------------------------------------------*/
	 for(j=0;j<=9;j++)
		fwrite("──────────┴",11,1,res);
		for(j=0;j<=2;j++)
			fwrite("─────────────────────────┴",26,1,res);
	 fwrite("\n\n",1,1,res);
	 fwrite("\n\n",1,1,res);

	 fclose(res);

}


// ***************************************************************
// *                 Рисование пробелов                          *
// ***************************************************************

void Prob(int d1, int d2)
{
  int i;
  for(i=0;i<d2-d1;i++)
	fwrite(" ",1,1,res);
  fwrite("│",1,1,res);

}


// ***************************************************************
// *                 Рисование таблицы файлов                    *
// ***************************************************************

void RisTab2(void)
{

  char c;
  char string[10];
  int i,j,j1, k, kolsim, ndig;
  int dec, sign;
  res = fopen("result","at");


/*-----------Цикл для стандарта------------*/

  for(j=0;j<=3;j++)
	{
	  fwrite(mas3[j],10,1,res);
	  fwrite("│",1,1,res);
	}
	fwrite("\n",1,1,res);
/*-----------------------------------------*/
  for(i=0;i<=3;i++)
  {
	 if(tab[i]!=' ')
	 {
		if(tab[i]=='p')
		{  k=i;
			i=fileno(pol);
		} else k=i;

		for(j=0;j<=3;j++)
		fwrite("──────────┼",11,1,res);
		fwrite("\n",1,1,res);

/*------------------Имя файла-----------------*/
		switch(tab[k])
		{ case 'i': {fwrite("stdin",5,1,res);kolsim=5; break;}
		  case 'o': {fwrite("stdout",6,1,res);kolsim=6; break;}
		  case 'e': {fwrite("stderr",6,1,res);kolsim=6; break;}
		  case 'p': {fwrite("makefile",8,1,res); kolsim=8; break;}
		}
		Prob(kolsim,10);

/*------------------Дискриптор----------------*/
		sprintf(string,"%d  ",k+1);
		fwrite(string,3,1,res);
		Prob(3,10);

/*----------тип доступа------------------------*/
	switch(tab[k])
		{ case 'i': {fwrite("read",4,1,res);kolsim=4; break;}
		  case 'o': {fwrite("write",5,1,res);kolsim=5; break;}
		  case 'e': {fwrite("read/write",10,1,res);kolsim=10; break;}
		  case 'p': {fwrite("read/write",10,1,res);  kolsim=10; break;}
		}

		Prob(kolsim,10);
/*---------------точка чтения/записи----------*/
//		string = ecvt(statb.st_ino, 10, &dec, &sign);
//		fwrite(string,dec,1,res);
		Prob(0,10);
/*--------------------------------------------*/

		fwrite("\n",1,1,res);
		i=k;
	 }
	 else
	 {
	   for(j=0;j<=3;j++)
		fwrite("──────────┼",11,1,res);
		fwrite("\n",1,1,res);

	   for(j=0;j<=3;j++)
		 Prob(0,10);

	   fwrite("\n",1,1,res);

	 }
  }
/*-------------------------------------------*/
	 for(j=0;j<=3;j++)
		fwrite("──────────┴",11,1,res);
	 fwrite("\n\n",1,1,res);
	 fwrite("\n\n",1,1,res);

	 fclose(res);



}