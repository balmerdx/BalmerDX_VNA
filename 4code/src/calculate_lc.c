#include "calculate_lc.h"
#include "dac.h"
#include <math.h>

void calculateLC(complexf Zx, float F, bool is_serial,
                 float* L, float* C, bool* isC, float* Rout)
{
    *C = 0;
    *L = 0;
    *isC = false;
    *Rout = 0;

    float Cmax = 1e-2f;
    if(is_serial)
    {
        *isC = false;
        *L = cimagf(Zx)/(2*pi*F);
        *Rout = crealf(Zx);

        if(cimagf(Zx)<-1e-10f)
        {
            *isC = true;
            *C = -1/(2*pi*F*cimagf(Zx));
        } else
        {
            *C = 0;
        }

        //если сопротивление маленькое и индуктивность немного отрицательная, то таки считаем что это ошибка калибрации
        if(cabsf(Zx)<2 && *L<0 && *L>-1e-9f)
        {
            *isC = false;
        }
    } else
    { //parallel
        *isC = true;
        complexf Yx = 1.0f/Zx;
        *C = cimagf(Yx)/(2*pi*F);
        if(*C>Cmax)
            *C = Cmax;
        if(*C<-Cmax)
            *C = -Cmax;

        if(fabsf(crealf(Yx))>-1e-9f)
        {
            *Rout = 1/crealf(Yx);
        } else
        {
            *Rout = 1e9f;
        }

        if(cimagf(Yx)<-1e-10f)
        {
            *isC = false;
            *L = -1/(2*pi*F*cimagf(Yx));
        } else
        {
            *L = 0;
        }

        //если сопротивление большое и емкость немного отрицательная, то таки считаем что это ошибка калибрации
        if(cabsf(Zx)>1e5f && *C<0 && *C>-5e-15f)
            *isC = true;
    }
}
