#include "../inc/core.h"

void WGS84ToGuass(double latitude,double longitude,double *x,double *y)
{
    double p =206264.8062470963;     //p表示206265
    double B;  //纬度
    double L;  //经度
    double L0;  //中央经度
    double c=6399593.625803977;
    double e2=0.006739496742227;
    double c0 = 6367449.145928;
    double c2 = -32144.479876;
    double c4 = 135.366946;
    double c6 = -0.709481;
    double c8 = 0.004061;
    double N,l,t2,n2,l0;   //n2是η的平方
    double a1,a2,b1,b2,dx,dy,cx,cy; //r为该点的平面子午线收敛角
    double X;   //X是子午线弧长
    double zonewide = 3;
    double prjno;

    L0 = longitude;
    if (zonewide == 6)
    {
        prjno = trunc(L / zonewide) + 1;
        L0 = prjno * zonewide - 3;
    }
    else
    {
        prjno = trunc((L0 - 1.5) / 3) + 1;
        L0 = prjno * 3;
    }
    B = latitude;
    L = longitude;
    l = (L-L0)*3600.0;         //经度秒差
    B = B*IPI;            //纬度纯度转换为弧度
    double tcos = cos(B);
    double tsin = sin(B);
    double ttan = tan(B);
    N = c/sqrt(1+e2*pow(tcos,2));
    X = c0*B+(c2*tcos+c4*pow(tcos,3)+c6*pow(tcos,5)+c8*pow(tcos,7))*tsin;
    t2 = pow(ttan,2);    //t的平方
    n2 = e2*pow(tcos,2);
    l0 = l*l*0.00000001;
    a1 = N*tsin*tcos*0.0011752215269548;
    a2 = N*tsin*pow(tcos,3)*(5-t2+9*n2+4*n2*n2)*0.00000023019093957;
    b1 = N*tcos/p;
    b2 = N*pow(tcos,3)*(1-t2+n2)*0.000000001899211582;
    dx = N*tsin*pow(tcos,5)*(61-58*t2+t2*t2)*pow(l,6)/(720*pow(p,6));
    dy = N*pow(tcos,5)*(5-18*t2+t2*t2+14*n2-58*n2*t2)*pow(l,5)/(120*pow(p,5));
    cx = X+l0*(a1+a2*l0)+dx;
    cy = l*(b1+b2*l0)+dy;
    *y = cx;
    *x = cy+500000;
    if(latitude == 90)
        *x = 500000;
}

void CovertBLToXY(double latitude,double longitude,double centerLongitude,double *cx,double *cy,double *cr)
{
    double PI=3.14159265358979;
    double p =206264.8062470963;     //p表示206265
    double B;  //纬度
    double L;  //经度
    double L0;  //中央经度
    double c,e2Power;
    //  a=6378137;
    c=6399593.625803977;
    e2Power=0.006739496742227;

    double  c0,c2,c4,c6,c8;
    c0=c*(1-0.75*e2Power+45*e2Power*e2Power/64-175*pow(e2Power,3)/256+11025*pow(e2Power,4)/16384);
    c2=c0-c;
    c4=c*(15*e2Power*e2Power/32-175*pow(e2Power,3)/384+3675*pow(e2Power,4)/8192);
    c6=c*(-35*pow(e2Power,3)/96+735*pow(e2Power,4)/2048);
    c8=c*315*pow(e2Power,4)/1024;

    double N,l,t_s,n2,l0;   //n2是η的平方
    double a1,a2,b1,b2,dx,dy,x,y,r; //r为该点的平面子午线收敛角
    double X;   //X是子午线弧长

    B=latitude;
    L=longitude;
    L0=centerLongitude;
    l=(L-L0)*3600.0;         //经度秒差
    B=B*PI/180.0;            //纬度纯度转换为弧度

    N=c/sqrt(1+e2Power*cos(B)*cos(B));
    X=c0*B+(c2*cos(B)+c4*pow(cos(B),3)+c6*pow(cos(B),5)+c8*pow(cos(B),7))*sin(B);

    t_s=tan(B)*tan(B);    //t的平方
    n2=e2Power*cos(B)*cos(B);
    l0=l*l*pow(10.0,-8);

    a1=N*sin(B)*cos(B)*pow(10.0,8)/(2*pow(p,2));
    a2=N*sin(B)*pow(cos(B),3)*(5-t_s+9*n2+4*n2*n2)*pow(10.0,16)/(24*pow(p,4));
    b1=N*cos(B)/p;
    b2=N*pow(cos(B),3)*(1-t_s+n2)*pow(10.0,8)/(6*pow(p,3));
    dx=N*sin(B)*pow(cos(B),5)*(61-58*t_s+t_s*t_s)*pow(l,6)/(720*pow(p,6));
    dy=N*pow(cos(B),5)*(5-18*t_s+t_s*t_s+14*n2-58*n2*t_s)*pow(l,5)/(120*pow(p,5));
    x=X+l0*(a1+a2*l0)+dx;
    y=l*(b1+b2*l0)+dy;
    *cy=x;
    *cx=y+500000;

    if(latitude==90)
        *cx=500000;
    r=sin(B)*l+sin(B)*cos(B)*cos(B)*pow(l,3)*(1+3*n2+2*n2*n2)/3/p/p+sin(B)*pow(cos(B),4)*pow(l,5)*(2-t_s)/15/p/p/p/p;
    //this->R=r/3600.0;  //r为秒值,转换为纯度
    *cr=r/3600.0;
}

